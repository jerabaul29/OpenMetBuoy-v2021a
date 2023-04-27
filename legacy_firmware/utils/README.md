Some examples of utils to get the binary messages from Rock7 and process them. Relies on Bash and Python3. These are only coarse examples, you will need to adapt to your needs.

## Workflow v1: using custom scripts

- to run the full update automatically, run: ```bash script_regenerate_all.sh```. Note that there are 3 options to get the raw data:
  - apirequest : will need a file ```username.secret``` with content ```USERNAME="YOUR_USERNAME"``` and a file ```password.secret``` with content ```PASSWORD="YOUR_PASSWORD"```, substituting with the correct values (be careful to not share publicly this information! In particular, be careful if you have an open repository and you use git / github / gitlab / similar). The URL encoding is done in the script, there is not need to URL encode the username and password. If you need to request specific data, adapt the URL request part in ```filterDeviceAssignmentId=&filterDirection=&filterDateFrom=22%2FJul%2F2022+00%3A00%3A00&filterDateTo=30%2FJun%2F2033``` as needed.
  - download : this assumes that you have downloaded by hand the CSV file you need, and that it is in the ```~/Downloads``` folder
  - all.csv : this assumes that you already have a file ```all.csv``` containing the Rock7 data, in the current folder.

- you can also run parts of the workflow separately:
  - to generate the dict of data, use the ```script_all_messages_to_dict.py```
  - to plot the drift, use the ```script_plot_trajectories.py```
  - to plot the spectra, use the ```script_plot_spectra.py```

- to run periodically the data acquisition, use the ```script_run_periodically.sh```

- the ```params.py``` file contains all the parameters needed to perform the processing (list of instruments and deployment times, start / end times, geographical extent, etc).

- the ```*_cookie_*.sh``` scripts show examples of how to locally generate some cookie for sharing with colleagues.

- in addition, we provide script examples for i) packing the .pkl data dict into netCDF-CF files, ii) displaying the data from the created netCDF file.

#### using in a conda env

All of this can be run from a conda env:

- To create the environment:

```
conda env create -f OMBdata_env.yml
```

- To update if yml-file has changed:

```
conda env update --file OMBdata_env.yml --prune 
```

- To use the environment:

```
conda activate OMBdata
```

## Workflow v2: using trajan (TRAJectory ANalysis)

The project trajan: https://github.com/OpenDrift/trajan aims at providing tooling to analyze trajectory data out of the box as an extension to the xarray ecosystem. This is still under development, but already provides nice support for the OMB. In case of questions / requests specifically about trajan, report directly on the issue tracker system there. This workflow leverages trajan functionalities, in particular the OpenMetBuoy CSV reader built in, for more information, see:

- example of trajan for OMB: https://opendrift.github.io/trajan/gallery/example_omb_csv_reader.html#sphx-glr-gallery-example-omb-csv-reader-py
- API reference: https://opendrift.github.io/trajan/autoapi/trajan/readers/omb/index.html
- source code: https://github.com/OpenDrift/trajan/blob/main/trajan/readers/omb.py

To run this workflow, you will need to use trajan, xarray, and the dependencies around; recommended installation is ```conda -c conda-forge install trajan``` or ```pip install trajan```, see https://opendrift.github.io/trajan/index.html#installation and related for more information.

Workflow to use trajan method:

- To download the CSV file, run the trajan OMB decoder on it, and generate the netCDF-CF file: ```bash ./script_trajan_processing.sh``` (edit the URL request part ```filterDeviceAssignmentId=&filterDirection=&filterDateFrom=22%2FJul%2F2022+00%3A00%3A00&filterDateTo=30%2FJun%2F2033``` as necessary)

- To look at the trajectories, use trajan directly on the generated NetCDF-CF file generated; for example:

```python
(trajan_omb) ~/Desktop/Current/trajan_omb_illustration> python3
Python 3.11.3 | packaged by conda-forge | (main, Apr  6 2023, 08:57:19) [GCC 11.3.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> import matplotlib.pyplot as plt
>>> import xarray as xr
>>> import trajan as _
>>> ds = xr.open_dataset('all.nc')
>>> ds.traj.plot()
[<matplotlib.lines.Line2D object at 0x7fa5e9b8d510>, <matplotlib.lines.Line2D object at 0x7fa5e9b8d7d0>, <matplotlib.lines.Line2D object at 0x7fa5e9b8da10>, <matplotlib.lines.Line2D object at 0x7fa5e9b8df10>, <matplotlib.lines.Line2D object at 0x7fa5e9b8e350>, <matplotlib.lines.Line2D object at 0x7fa5e9b8ec50>, <matplotlib.lines.Line2D object at 0x7fa5e9b8f0d0>, <matplotlib.lines.Line2D object at 0x7fa5e9b2d990>]
>>> plt.show()
>>> speed = ds.traj.speed()
>>> print(f'Max speed {speed.max().values} m/s')
Max speed 5.461420219286076 m/s
```

See https://opendrift.github.io/trajan/gallery/example_drifters.html#sphx-glr-gallery-example-drifters-py and related for more examples of use of trajan.

