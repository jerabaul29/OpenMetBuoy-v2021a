import math
import pickle as pkl
import matplotlib.pyplot as plt
import datetime
import numpy as np
import matplotlib.dates as mdates
import matplotlib.colors as mcolors

from utils import sliding_filter_nsigma

import os
import time

# ------------------------------------------------------------------------------------------
print("***** Put the interpreter in UTC, to make sure no TZ issues")
os.environ["TZ"] = "UTC"
time.tzset()

# ------------------------------------------------------------------------------------------
print("***** configure matplotlib")
plt.rcParams.update({'font.size': 14})
list_colors = list(mcolors.TABLEAU_COLORS)
list_colors.append("w")
list_colors.append("k")

with open("./dict_all_data.pkl", "rb") as fh:
    dict_data_each_logger = pkl.load(fh)

list_instruments_with_spectra = dict_data_each_logger.keys()

show_spectrum = "normalized_elevation"

fig, axes = plt.subplots(nrows=len(list_instruments_with_spectra), ncols=1)

for ind, crrt_instrument in enumerate(list_instruments_with_spectra):

    val = 100 * len(list_instruments_with_spectra) + 10 + ind + 1
    plt.subplot(val)

    list_spectra_data = dict_data_each_logger[crrt_instrument]["spectra"]

    list_frequencies = list_spectra_data[0].list_frequencies

    list_datetimes = [list_spectra_data[0].datetime_fix]
    list_spectra = [list_spectra_data[0].list_elevation_energies]
    shape_spectrum = (len(list_spectra[0]), )

    for crrt_entry in list_spectra_data[1:]:
        if (crrt_entry.datetime_fix - list_datetimes[-1] > datetime.timedelta(hours=6)):
            list_datetimes.append(list_datetimes[-1] + datetime.timedelta(hours=2))
            list_datetimes.append(crrt_entry.datetime_fix - datetime.timedelta(hours=2))
            list_spectra.append(np.full(shape_spectrum, np.nan))
            list_spectra.append(np.full(shape_spectrum, np.nan))

        list_datetimes.append(crrt_entry.datetime_fix)

        if show_spectrum == "normalized_elevation":
            crrt_spectrum = crrt_entry.list_elevation_energies
            list_spectra.append(crrt_spectrum)

    pclr = plt.pcolor(list_datetimes, list_frequencies, np.log10(np.transpose(np.array(list_spectra))))
    # pclr = plt.pcolor(list_datetimes, list_frequencies, np.log10(np.transpose(np.array(list_spectra))), vmin=vmin_pcolor, vmax=vmax_pcolor)
    # plt.xlim([date_start_md, date_end_md])
    plt.ylim([0.05, 0.30])

    if ind < len(list_instruments_with_spectra)-1:
        plt.xticks([])
    plt.xticks(rotation=30)

    plt.ylabel("f [Hz]\n({})".format(crrt_instrument))

plt.tight_layout()

plt.subplots_adjust(right=0.8)
cbar_ax = fig.add_axes([0.85, 0.15, 0.02, 0.7])
cbar = fig.colorbar(pclr, cax=cbar_ax)
if show_spectrum == "normalized_elevation":
    cbar.set_label('log$_{10}$(S) [m$^2$/Hz]')

plt.savefig("spectra_" + show_spectrum + ".png")

plt.show()

fig, axes = plt.subplots(1, ncols=2)

for ind, crrt_instrument in enumerate(list_instruments_with_spectra):
    crrt_color = list_colors[ind]

    list_spectra_data = dict_data_each_logger[crrt_instrument]["spectra"]

    list_datetimes = [crrt_data.datetime_fix for crrt_data in list_spectra_data]
    list_swh = [crrt_data.Hs for crrt_data in list_spectra_data]
    list_tp = [crrt_data.Tz for crrt_data in list_spectra_data]
    list_spectra = [crrt_data.list_elevation_energies for crrt_data in list_spectra_data]
    list_frequencies = [crrt_data.list_frequencies for crrt_data in list_spectra_data]

    def compute_spectral_moment(list_frequencies, list_elevation_energies, order):
        list_to_integrate = [
            math.pow(crrt_freq, order) * crrt_energy for (crrt_freq, crrt_energy) in zip(list_frequencies, list_elevation_energies)
        ]
        moment = np.trapz(list_to_integrate, list_frequencies)
        return moment

    list_processed_swh = []
    list_processed_tp = []

    for crrt_entry in list_spectra_data:
        m0 = compute_spectral_moment(crrt_entry.list_frequencies, crrt_entry.list_elevation_energies, 0)
        m2 = compute_spectral_moment(crrt_entry.list_frequencies, crrt_entry.list_elevation_energies, 2)
        m4 = compute_spectral_moment(crrt_entry.list_frequencies, crrt_entry.list_elevation_energies, 4)

        list_processed_tp.append(math.sqrt(m2/m0))
        list_processed_swh.append(4.0 * math.sqrt(m0))

    plt.subplot(121)
    plt.plot(list_datetimes, sliding_filter_nsigma(np.array(list_swh), nsigma=2.0), color=crrt_color, marker=".", label="swh {}".format(crrt_instrument))
    plt.plot(list_datetimes, sliding_filter_nsigma(0.001 + np.array(list_processed_swh), nsigma=2.0), marker="o", color=crrt_color, label="swh processed")
    plt.ylabel("[m]")
    
    plt.subplot(122)
    plt.plot(list_datetimes, sliding_filter_nsigma(np.array(list_tp), nsigma=2.0), color=crrt_color, label="fp", marker="+")
    plt.plot(list_datetimes, sliding_filter_nsigma(0.001 + np.array(list_processed_tp), nsigma=2.0), color=crrt_color, marker="*", label="fp processed")
    plt.ylabel("[Hz]")

plt.subplot(121)
plt.legend(loc="lower left")
plt.xticks(rotation=30)

plt.subplot(122)
plt.legend(loc="lower left")
plt.xticks(rotation=30)

plt.tight_layout()

plt.savefig("swh_tp.png")
plt.show()

list_sliding_averaged_tz = list(sliding_filter_nsigma(np.array(list_tp), nsigma=2.0))
list_sliding_averaged_swh = list(sliding_filter_nsigma(np.array(list_swh), nsigma=2.0))

print()
print()
print("---------- DUMP ALL WAVE INFO START ----------")
print("format: timestamp, tz[Hz], swh[m]")
for crrt_datetime, crrt_tz, crrt_swh in zip(list_datetimes, list_sliding_averaged_tz, list_sliding_averaged_swh):
    print(f"{crrt_datetime.isoformat()}, {crrt_tz:4.2f}, {crrt_swh:4.2f}")
print("---------- DONE DUMP ALL WAVE INFO ----------")
print()
print()

