import datetime

# the list of instruments to use and their deployment times
list_instruments_and_time = [
    ("RockBLOCK 999999", datetime.datetime(2022, 1, 1, 0, 0, 0)),
]

list_input_files = [
    "./all.csv",
]

# parameters for the extent of the time shown in trajectories
trajectories_use_full_timespan = True  # if True, the trajectories_last_time will be the last actual timestamp received, if False, trajectories_last_time will be used
trajectories_first_time = datetime.datetime(2022, 1, 1, 0, 0, 0)
trajectories_last_time = trajectories_first_time  # will only be taken into account if trajectories_use_full_timespan is False

# parameters for the cartopy plot
central_latitude = 83.0
central_longitude = 10.0
standard_parallels = (72.0, 89.0)
min_lon, max_lon, min_lat, max_lat = (-15, 20, 74.0, 85.0)

# parameters for the spectra and wave statistics displaying
date_start_spectra = datetime.datetime(2022, 1, 1, 0, 0, 0)
date_end_spectra = datetime.datetime(2022, 1, 1, 0, 0, 0)

plot_show = True  # should we actually show the plot in an interactive way?

