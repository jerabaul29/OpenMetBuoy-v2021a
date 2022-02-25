A disclaimer first:

- these firmwares work well, but they are messy; I have had very little time to improve code quality / re-factor, so things have grown up "as things went", features were added continuously, and the firmware was developed as an experiment rather than a well engineered piece of software

- the different versions of the firmware are largely copies of each other with small changes; this is of course not ideal (it violates the "dont repeat yoursel principle", but it works well enough for now, and I have no time at the moment to make things better).

The main firmware versions:

- **plain_gps_drifter**: a plain gps drifter, no wave measurements
- **standard_gps_waves_drifter**: a standard gps and waves drifter, with both drift and waves measurements
- **steval_gps_waves_drifter**: the gps and waves drifter adapted to the 6dof sensor, ie the replacement chip for when the Adafruit 9dof is not available; basically, the same code, but without the magnetometer
- **standard_gps_waves_thermistors_drifter**: a standard gps and waves and thermistor drifter, with up to 6 DS18B20 thermistors
- **two_ways_gps_waves_drifter**: the gps and waves drifter, with the 2-ways communication added to be able to change the frequency of GPS / waves measurements.

Of course, all versions of the firmware can be tuned to your needs; see in particular the **params.h** file.
