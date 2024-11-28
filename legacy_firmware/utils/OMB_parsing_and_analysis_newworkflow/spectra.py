"""
Tools to help plot spectra.
"""

import logging
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import xarray as xr
from typing import Union, Tuple
import datetime
from pathlib import Path

logger = logging.getLogger(__name__)


def plot_trajan_spectra(
        xr_trajan_in: xr.DataArray,
        tuple_date_start_end: Union[None, Tuple[datetime.datetime, datetime.datetime]] = None,
        tuple_vrange_pcolor: Union[None, Tuple[float, float]] = None,
        plt_show: bool = True,
        fignamesave: Union[None, str, Path] = None,
        field: str = "processed_elevation_energy_spectrum",
        nseconds_gap: float = 6 * 3600):
    """Plot the wave spectra information from a trajan compatible xarray.
    All spectra are plotted as a vertical array of subfigures, with a common
    horizontal time basis.

    Args:
        `xr_trajan_in`:
            input xarray following the trajan conventions
        `tuple_date_start_end`: can be either:
            - None to plot over the full datetime range available across trajectories
            - a tuple of datetimes (datetime_start, datetime_end) to set the time axis limits explicitely
        `tuple_vrange_pcolor`: can be either:
            - None to use the default log range [-3.0, 1.0]
            - a tuple of float to set the log range explicitely
        `plt_show`: bool flag
            Whether to show the figure or not
        `fignamesave`: can be either:
            - None to not save the figure
            - a valid filename string or path ending in .png or .pdf to save the figure to disk
        `field`: string
            the xr_trajan_in field to use as the 1D spectrum data;
            by default, this is the "processed_elevation_energy_spectrum", but
            one could also set "elevation_energy_spectrum" to plot also the bins
            with low frequency noise.
        `nseconds_gap`: float
            Number of seconds between 2 consecutive
            spectra for one instrument above which we consider that there is a
            data loss that should be filled with NaN. This is to avoid "stretching"
            neighboring spectra over long times if an instrument gets offline.
    """

    if tuple_date_start_end is not None:
        date_start = tuple_date_start_end[0]
        date_end = tuple_date_start_end[1]
    else:
        # we need a common date start end to share the bottom time axis
        list_date_start = []
        list_date_end = []

        for ind, _ in enumerate(xr_trajan_in["trajectory"]):
            crrt_spectra_times = xr_trajan_in.isel(
                trajectory=ind)["time_waves_imu"].to_numpy()
            list_date_start.append(np.nanmin(crrt_spectra_times))
            list_date_end.append(np.nanmax(crrt_spectra_times))

        list_date_start = [x for x in list_date_start if np.isfinite(x)]
        list_date_end = [x for x in list_date_end if np.isfinite(x)]
        date_start = min(list_date_start)
        date_end = max(list_date_end)

    date_start_md = mdates.date2num(date_start)
    date_end_md = mdates.date2num(date_end)

    logger.debug(f"{tuple_date_start_end = }")
    logger.debug(f"{date_start_md = }")
    logger.debug(f"{date_end_md = }")

    if tuple_vrange_pcolor is None:
        vmin_pcolor = -3.0
        vmax_pcolor = 1.0
    else:
        vmin_pcolor = tuple_vrange_pcolor[0]
        vmax_pcolor = tuple_vrange_pcolor[1]

    logger.debug(f"{tuple_vrange_pcolor = }")
    logger.debug(f"{vmin_pcolor = }")
    logger.debug(f"{vmax_pcolor = }")

    logger.debug(f"{plt_show = }")
    logger.debug(f"{fignamesave = }")

    fig, axes = plt.subplots(nrows=len(xr_trajan_in.trajectory), ncols=1)

    spectra_frequencies = xr_trajan_in["frequencies_waves_imu"].to_numpy()

    for ind, crrt_instrument in enumerate(xr_trajan_in["trajectory"]):
        crrt_instrument = str(crrt_instrument.data)
        logger.debug(f"{crrt_instrument = }")

        plt.subplot(len(xr_trajan_in.trajectory), 1, ind+1)

        try:
            crrt_spectra = xr_trajan_in.isel(trajectory=ind)[
                field].to_numpy()
            crrt_spectra_times = xr_trajan_in.isel(
                trajectory=ind)["time_waves_imu"].to_numpy()

            list_datetimes = []
            list_spectra = []

            # avoid streching at the left
            list_datetimes.append(
                crrt_spectra_times[0] - np.timedelta64(2, 'm'))
            list_spectra.append(np.full(len(spectra_frequencies), np.nan))

            for crrt_spectra_ind in range(1, crrt_spectra.shape[0], 1):
                if np.isnan(crrt_spectra_times[crrt_spectra_ind]):
                    continue

                # if a gap with more than nseconds_gap seconds, fill with NaNs
                # to avoid stretching neighbors over missing data
                seconds_after_previous = float(
                    crrt_spectra_times[crrt_spectra_ind] - crrt_spectra_times[crrt_spectra_ind-1]) / 1e9
                if seconds_after_previous > nseconds_gap:
                    logger.debug(
                        f"spectrum index {crrt_spectra_ind} is {seconds_after_previous} seconds \
                        after the previous one; insert nan spectra in between to avoid stretching")
                    list_datetimes.append(
                        crrt_spectra_times[crrt_spectra_ind-1] + np.timedelta64(2, 'h'))
                    list_spectra.append(
                        np.full(len(spectra_frequencies), np.nan))
                    list_datetimes.append(
                        crrt_spectra_times[crrt_spectra_ind] - np.timedelta64(2, 'h'))
                    list_spectra.append(
                        np.full(len(spectra_frequencies), np.nan))

                list_spectra.append(crrt_spectra[crrt_spectra_ind, :])
                list_datetimes.append(crrt_spectra_times[crrt_spectra_ind])

            # avoid stretching at the right
            last_datetime = list_datetimes[-1]
            list_datetimes.append(last_datetime + np.timedelta64(2, 'm'))
            list_spectra.append(np.full(len(spectra_frequencies), np.nan))

            pclr = plt.pcolor(list_datetimes, spectra_frequencies, np.log10(
                np.transpose(np.array(list_spectra))), vmin=vmin_pcolor, vmax=vmax_pcolor)

        except Exception as e:
            logger.error(
                f"isse with instrument {crrt_instrument}: received exception {e}")

        plt.xlim([date_start_md, date_end_md])
        plt.ylim([0.05, 0.25])

        if ind < len(xr_trajan_in.trajectory)-1:
            plt.xticks([])
        else:
            plt.xticks(rotation=30)

        plt.ylabel("f [Hz]\n({})".format(
            crrt_instrument), rotation=90, ha='right')

    plt.subplots_adjust(right=0.8)
    cbar_ax = fig.add_axes([0.85, 0.15, 0.02, 0.7])
    cbar = fig.colorbar(pclr, cax=cbar_ax)
    cbar.set_label('log$_{10}$(S) [m$^2$/Hz]')

    # plt.tight_layout()

    if fignamesave is not None:
        plt.savefig(fignamesave)

    if plt_show:
        plt.show()
