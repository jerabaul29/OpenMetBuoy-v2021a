Some examples of utils to get the binary messages from Rock7 and process them. Relies on Bash and Python3. These are only coarse examples, you will need to adapt to your needs.

- to run the full update automatically, run: ```bash script_regenerate_all.sh```. Note that there are 3 options to get the raw data:
  - apirequest : will need a file ```username.secret``` with content ```USERNAME="YOUR_USERNAME"``` and a file ```password.secret``` with content ```PASSWORD="YOUR_PASSWORD"```, substituting with the correct values (be careful to not share publicly this information! In particular, be careful if you have an open repository and you use git / github / gitlab / similar). The URL encoding is done in the script, there is not need to URL encode the username and password.
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
