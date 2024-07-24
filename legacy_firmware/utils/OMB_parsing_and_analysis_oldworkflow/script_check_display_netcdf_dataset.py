"""
From the nc4 file:
- plot trajectories
- plot spectra
- plot SWH Hs, Tp Tz, and similar from the spectrum and check they agree

Looking at these plots allows to double check the quality and content of the nc files.
This can also be used as an example for how to read and use the data.
"""

from datetime import timedelta
from datetime import datetime

import os
import time

from icecream import ic

import netCDF4 as nc4

import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import matplotlib.colors as mcolors

import math

import numpy as np

from scipy import integrate

# ------------------------------------------------------------------------------------------

# in case mix of pure drifter and drifter and waves instruments, only choose a set of instruments
# to show their spectra
list_instruments_use_spectra = [
    "LIST_OF_INSTRUMENTS_WITH_SPECTRUM",
]

number_of_instruments_use_spectra = len(list_instruments_use_spectra)

# the x axis limits of the spectra displayed
date_start_ice_spectra = datetime(2022, 3, 27, 0, 0, 0)
date_end_ice_spectra = datetime(2022, 5, 23, 0, 0, 0)

# what netCDF file to print
input_file = "some_file.nc"

# ------------------------------------------------------------------------------------------
print("***** Put the interpreter in UTC, to make sure no TZ issues")
os.environ["TZ"] = "UTC"
time.tzset()

# ------------------------------------------------------------------------------------------
print("***** Configure icecream")
ic.configureOutput(prefix='', outputFunction=print)

# ------------------------------------------------------------------------------------------
print("***** configure matplotlib")
plt.rcParams.update({'font.size': 14})
list_colors = list(mcolors.TABLEAU_COLORS)
list_colors.append("k")
list_colors.append("w")

# ------------------------------------------------------------------------------------------
print("***** load the netcdf data file")

verbose_load = False

dict_extracted_data = {}

time_start = datetime.fromtimestamp(2e10)
time_end = datetime.fromtimestamp(0)

with nc4.Dataset(input_file, "r", format="NETCDF4") as nc4_fh:
    number_of_frequency_bins = nc4_fh.dimensions["frequency"].size
    if verbose_load:
        ic(number_of_frequency_bins)
    frequencies = nc4_fh["frequency"][:].data

    number_of_trajectories = nc4_fh.dimensions["trajectory"].size
    if verbose_load:
        ic(number_of_trajectories)

    for ind_trajectory in range(number_of_trajectories):
        if verbose_load:
            print("** new instrument **")

        # get the instrument id
        crrt_instrument_id = ""
        array_chars = nc4_fh["trajectory_id"][ind_trajectory, :]
        for crrt_char in array_chars:
            if np.ma.is_masked(crrt_char):
                break
            crrt_instrument_id += crrt_char.decode('ascii')
            if verbose_load:
                ic(crrt_instrument_id)

        # read the data content and organize it into
        # dicts {time: (spectrum, swh, hs, tp, tz0)} and {time: (lat, lon)}
        crrt_dict_time_pos = {}
        dict_time_wave = {}

        array_kind = nc4_fh["message_kind"][ind_trajectory, :]
        array_lat = nc4_fh["lat"][ind_trajectory, :]
        array_lon = nc4_fh["lon"][ind_trajectory, :]
        array_spectrum = nc4_fh["wave_spectrum"][ind_trajectory, :, :]
        array_swh = nc4_fh["swh"][ind_trajectory, :]
        array_hs = nc4_fh["hs"][ind_trajectory, :]
        array_tp = nc4_fh["tp"][ind_trajectory, :]
        array_tz0 = nc4_fh["tz0"][ind_trajectory, :]

        if verbose_load:
            ic(array_swh)

        for crrt_index, crrt_kind in enumerate(array_kind):
            if verbose_load:
                print("**")

            if np.ma.is_masked(crrt_kind):
                break
            crrt_kind = crrt_kind.decode('ascii')
            if verbose_load:
                ic(crrt_kind)

            crrt_time = nc4_fh["time"][ind_trajectory, crrt_index].data
            crrt_datetime = datetime.fromtimestamp(float(crrt_time))
            if verbose_load:
                ic(crrt_datetime)

            if crrt_kind == "G":
                crrt_position = (float(array_lat[crrt_index]), float(array_lon[crrt_index]))
                if verbose_load:
                    ic(crrt_position)
                crrt_dict_time_pos[crrt_datetime] = crrt_position
            elif crrt_kind == "W":
                crrt_dict_wave = {}
                crrt_dict_wave["spectrum"] = array_spectrum[crrt_index, :].data
                crrt_dict_wave["hs"] = float(array_hs[crrt_index])
                crrt_dict_wave["tp"] = float(array_tp[crrt_index])
                if verbose_load:
                    ic(crrt_dict_wave)
                dict_time_wave[crrt_datetime] = crrt_dict_wave
            elif crrt_kind == "N":
                continue
            else:
                raise RuntimeError("unknown kind {}".format(crrt_kind))

            time_end = max(time_end, crrt_datetime)
            time_start = min(time_start, crrt_datetime)

        dict_extracted_data[crrt_instrument_id] = {}
        dict_extracted_data[crrt_instrument_id]["time_pos"] = crrt_dict_time_pos
        dict_extracted_data[crrt_instrument_id]["time_wave"] = dict_time_wave

# ------------------------------------------------------------------------------------------
# order the instruments with "higher lat higher up" in the plots
list_keys = list(dict_extracted_data.keys())
list_lats = []
for crrt_key in list_keys:
    crrt_dict_time_pos = dict_extracted_data[crrt_key]["time_pos"]
    crrt_list_times = sorted(list(crrt_dict_time_pos.keys()))
    crrt_lat = dict_extracted_data[crrt_key]["time_pos"][crrt_list_times[1]][0]
    list_lats.append(crrt_lat)
list_keys_ordered_by_first_lat = [crrt_key for _, crrt_key in sorted(zip(list_lats, list_keys))]
list_keys_ordered_by_first_lat.reverse()

# ------------------------------------------------------------------------------------------
# plot the data for checking consistency
print("***** plot the data")

# ------------------------------------------------------------------------------------------
# plot the positions on a 2D plot (no map)
# we do not use cartopy or similar so that this runs fast
print("** plot positions on simple 2D plot")

plt.figure()

for crrt_index, crrt_instrument_id in enumerate(list_keys_ordered_by_first_lat):
    crrt_dict_time_pos = dict_extracted_data[crrt_instrument_id]["time_pos"]
    crrt_list_times = sorted(list(crrt_dict_time_pos.keys()))
    crrt_list_pos = [crrt_dict_time_pos[crrt_time] for crrt_time in crrt_list_times]
    crrt_list_lat = [crrt_pos[0] for crrt_pos in crrt_list_pos]
    crrt_list_lon = [crrt_pos[1] for crrt_pos in crrt_list_pos]
    plt.plot(crrt_list_lon, crrt_list_lat, label="{}".format(crrt_instrument_id), marker="*", color=list_colors[crrt_index])

plt.legend()

plt.xlabel("lon [DD]")
plt.ylabel("lat [DD]")
plt.tight_layout()
plt.savefig("trajectories.pdf")

# ------------------------------------------------------------------------------------------
# plot the spectra
print("** plot the spectra as spectrograms")

# what is the max duration between 2 consecutive spectrograms over which
# we consider that there has been data loss and we need to introduce a couple
# of NaN spectra to display it as blank
max_duration_between_spectrograms = timedelta(hours=10)
max_valid_half_duration = timedelta(hours=5)

fig, axes = plt.subplots(nrows=number_of_instruments_use_spectra, ncols=1)

time_start_md = mdates.date2num(date_start_ice_spectra)
time_end_md = mdates.date2num(date_end_ice_spectra)

ordered_list_instruments_use_spectra = []
for crrt_instrument in list_keys_ordered_by_first_lat:
    if crrt_instrument in list_instruments_use_spectra:
        ordered_list_instruments_use_spectra.append(crrt_instrument)

# one instrument spectrum at a time
for crrt_index, crrt_logger_ID in enumerate(ordered_list_instruments_use_spectra):
    plt.subplot(number_of_instruments_use_spectra, 1, crrt_index + 1)

    # get the data to plot
    crrt_datetimes = sorted(list(dict_extracted_data[crrt_logger_ID]["time_wave"].keys()))
    crrt_spectra = [dict_extracted_data[crrt_logger_ID]["time_wave"][crrt_key]["spectrum"] for crrt_key in crrt_datetimes]

    crrt_spectra_logged = np.log10(np.array(crrt_spectra) + 1e-20)

    # add some NaN spectra if too large gaps between consecutive transmissions
    crrt_datetimes_holed = [crrt_datetimes[0]]
    crrt_spectra_logged_holed = [crrt_spectra_logged[0]]

    previous_datetime = crrt_datetimes[0]
    for crrt_datetime_entry, crrt_spectra_logged_entry in zip(crrt_datetimes, crrt_spectra_logged):
        if crrt_datetime_entry - previous_datetime >= max_duration_between_spectrograms:
            crrt_datetimes_holed.append(previous_datetime + max_valid_half_duration)
            crrt_datetimes_holed.append(crrt_datetime_entry - max_valid_half_duration)
            crrt_spectra_logged_holed.append(np.nan * np.ones(np.shape(crrt_spectra_logged_entry)))
            crrt_spectra_logged_holed.append(np.nan * np.ones(np.shape(crrt_spectra_logged_entry)))
        crrt_datetimes_holed.append(crrt_datetime_entry)
        crrt_spectra_logged_holed.append(crrt_spectra_logged_entry)
        previous_datetime = crrt_datetime_entry

    crrt_spectra_logged_holed = np.array(crrt_spectra_logged_holed)

    # we ignore the first spectrum: corrupted by deployment
    pclr = plt.pcolor(crrt_datetimes_holed[1:], frequencies, np.transpose(crrt_spectra_logged_holed)[:, 1:], vmin=-3.0, vmax=1.0)

    # only set the ticks property once
    if crrt_index < number_of_instruments_use_spectra-1:
        plt.xticks([])
    plt.xticks(rotation=30)

    # use consistent Y label and X limits
    plt.ylabel("f [Hz]\n(instr. {})".format(crrt_logger_ID))
    plt.xlim([time_start_md, time_end_md])

fig.subplots_adjust(right=0.8)
cbar_ax = fig.add_axes([0.85, 0.15, 0.02, 0.7])
cbar = fig.colorbar(pclr, cax=cbar_ax)
cbar.set_label('log$_{10}$(S) [m$^2$/Hz]')
plt.savefig("spectra.pdf")

# ------------------------------------------------------------------------------------------
# plot the SWH, Hs, Tp, Tz0
print("** plot the wave statistics")

fig, (ax1, ax2) = plt.subplots(nrows=2, ncols=1)

max_SWH = 0


def compute_wave_spectrum_moments_stats(dict_wave_data):
    """Computing the spectral moments and spectral wave properties from
    the wave spectrum for all time entries in the input wave data dict."""
    for crrt_entry in dict_wave_data.keys():
        dict_wave_data[crrt_entry]["M0"] = integrate.trapz(dict_wave_data[crrt_entry]["spectrum"], x=frequencies)
        dict_wave_data[crrt_entry]["M1"] = integrate.trapz(dict_wave_data[crrt_entry]["spectrum"] * frequencies, x=frequencies)
        dict_wave_data[crrt_entry]["M2"] = integrate.trapz(dict_wave_data[crrt_entry]["spectrum"] * (frequencies**2), x=frequencies)
        dict_wave_data[crrt_entry]["M3"] = integrate.trapz(dict_wave_data[crrt_entry]["spectrum"] * (frequencies**3), x=frequencies)
        dict_wave_data[crrt_entry]["M4"] = integrate.trapz(dict_wave_data[crrt_entry]["spectrum"] * (frequencies**3), x=frequencies)
        dict_wave_data[crrt_entry]["Hs_proc"] = 4.0 * math.sqrt(dict_wave_data[crrt_entry]["M0"])
        dict_wave_data[crrt_entry]["Tz0_proc"] = math.sqrt(dict_wave_data[crrt_entry]["M0"] / dict_wave_data[crrt_entry]["M2"])


# perform the plotting of SWH and period properties for each instrument
# show the different methods for each quantity to allow to cross check validity
for crrt_index, crrt_key in enumerate(ordered_list_instruments_use_spectra):
    crrt_color_index = list_keys_ordered_by_first_lat.index(crrt_key)
    crrt_color = list_colors[crrt_color_index]

    crrt_dict_time_waves = dict_extracted_data[crrt_key]["time_wave"]
    crrt_list_datetimes = sorted(list(crrt_dict_time_waves.keys()))
    compute_wave_spectrum_moments_stats(crrt_dict_time_waves)

    crrt_list_Hs = [crrt_dict_time_waves[crrt_datetime]["hs"] for crrt_datetime in crrt_list_datetimes]
    crrt_list_Hs_proc = [crrt_dict_time_waves[crrt_datetime]["Hs_proc"] for crrt_datetime in crrt_list_datetimes]
    max_SWH = max(max_SWH, max(crrt_list_Hs))

    crrt_list_Tp = [crrt_dict_time_waves[crrt_datetime]["tp"] for crrt_datetime in crrt_list_datetimes]
    crrt_list_Tz0_proc = [crrt_dict_time_waves[crrt_datetime]["Tz0_proc"] for crrt_datetime in crrt_list_datetimes]

    # plot on the top subplot: SWH properties
    # is it time to set the axis, legend, etc, properties?
    if crrt_index == len(ordered_list_instruments_use_spectra)-1:
        ax1.plot(crrt_list_datetimes, crrt_list_Hs, color=crrt_color, linestyle="-.", marker="*", label="Hs")
        ax1.plot(crrt_list_datetimes, crrt_list_Hs_proc, color=crrt_color, linestyle=":", marker="o", label="Hs_proc")

        ax1.axis(xmin=time_start_md, xmax=time_end_md)
        ax1.axis(ymin=0.01, ymax=max_SWH)
        ax1.set_xticks([])
        ax1.legend(loc="upper left", ncol=3, fontsize=12)
        ax1.set_ylabel("SWH (m)")
    else:
        ax1.plot(crrt_list_datetimes, crrt_list_Hs, color=crrt_color, linestyle="-.", marker="*")
        ax1.plot(crrt_list_datetimes, crrt_list_Hs_proc, color=crrt_color, linestyle=":", marker="o")

    # plot on the bottom subplot: wave period properties
    # is it time to set the axis, legend, etc, properties?
    if crrt_index == len(ordered_list_instruments_use_spectra)-1:
        ax2.plot(crrt_list_datetimes, crrt_list_Tp, color=crrt_color, linestyle="--", marker="+", label="Tp {}".format(crrt_key))
        ax2.plot(crrt_list_datetimes, crrt_list_Tz0_proc, color=crrt_color, linestyle=":", marker="o", label="Tz0_proc")

        ax2.axis(ymin=-0.05, ymax=23.0)
        ax2.axis(xmin=date_start_ice_spectra, xmax=date_end_ice_spectra)
        ax2.set_xticklabels(ax2.get_xticks(), rotation=30)
        formatter = mdates.DateFormatter("%Y-%m-%d")
        ax2.xaxis.set_major_formatter(formatter)
        ax2.set_ylabel("WP (s)")
        ax2.legend(ncol=3, loc="upper left", fontsize=12)
    else:
        ax2.plot(crrt_list_datetimes, crrt_list_Tp, color=crrt_color, linestyle="--", marker="+", label="Tp {}".format(crrt_key))
        ax2.plot(crrt_list_datetimes, crrt_list_Tz0_proc, color=crrt_color, linestyle=":", marker="o")

    plt.tight_layout()

plt.savefig("wave_statistics.pdf")

# show all figures at the same time, to be able to compare...
plt.show()
