- buffering: both the GNSS and wave spectra packets are buffered using a deque (double ended queue). The newest messages are always transmitted first.

- wake up at fixes UTC times

- GNSS:
-- several fixes
-- timeout

- wave spectra:
-- IMU
--- 9-dof at 833kHz (accel, gyro)
--- for now, not magnetometer
--- low pass filter or 9-dof to 100 Hz
--- Kalman filter at 100 Hz
--- low pass filter of Kalman output
--- vertical acceleration at 10Hz
-- processing
--- measuremetns for a bit over 20 minutes of wave vertical acceleration
--- welch method by:
---- segments of 2048 points at 10Hz, with 75% overlap
---- take rfft, and compute the energy at each frequency
---- average the energy over the segments to compute the welch spectrum, which is some amplitude square of spectra
-- packaging
--- keep only the relevant bins
--- package into uint16 by 1) computing max value of spectrum on the selected bins, 2) use it to normalize between 0 and 65000
-> so the wave spectrum transmitted is the "Welch energy spectrum (quadratic) of the vertical acceleration, over relevant bins"
TODO: double check the normalization convention

