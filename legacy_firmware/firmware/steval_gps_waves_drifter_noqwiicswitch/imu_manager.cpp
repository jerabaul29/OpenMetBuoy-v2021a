#include "imu_manager.h"

Adafruit_ISM330DHCX ism330dhcx;
//Adafruit_LIS3MDL lis3mdl;
Adafruit_NXPSensorFusion Kalman_filter;
IMU_Manager board_imu_manger;

bool IMU_Manager::start_IMU(){
  Serial.println(F("start the IMU"));
  wdt.restart();

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  enableBurstMode();
  delay(100);

  ArtemisWire.begin();
  delay(100);
  ArtemisWire.setClock(1000000);
  delay(100);
  wdt.restart();
  Serial.println(F("started ArtemisWire"));

  Serial.println(F("start qwiic switch"));
  if (qwiic_switch.begin(ArtemisWire) == false){
      Serial.println(F("Qwiic Power Switch not detected at default I2C address. Please check wiring. Freezing."));
      while (true){;}
  }
  turn_qwiic_switch_off();
  delay(500);
  wdt.restart();

   // TODO: instead of hang, return false...
  turn_qwiic_switch_on();

  // configure the power switch
  turn_qwiic_switch_on();
  qwiic_switch.isolationOff();
  delay(500);
  wdt.restart();

  Serial.println("Adafruit ISM330DHCX start!");
  while (true){
    if (!ism330dhcx.begin_I2C(LSM6DS_I2CADDR_DEFAULT,
                     &ArtemisWire, 0)) {
      Serial.println("Failed to find ISM330DHCX chip, will try again...");
      delay(500);
    }
    else{
      break;
    }
  }
  Serial.println("ISM330DHCX Found!");
  wdt.restart();

  /*
  Serial.println(F("Adafruit LIS3MDL start!"));
  while (true){
    if (! lis3mdl.begin_I2C(LIS3MDL_I2CADDR_DEFAULT,
                     &ArtemisWire)) {
      Serial.println("Failed to find LIS3MDL chip, will try again...");
      delay(500);
    }
    else{
      break;
    }
  }
  Serial.println("LIS3MDL Found!");
  wdt.restart();
  */

   set_sensors_parameters();
   delay(100);
   print_sensors_information();
   delay(100);
   wdt.restart();

  // initialize the filter
  Kalman_filter.begin(update_frequency_Kalman_Hz);
  wdt.restart();

  time_last_accel_gyro_reading_us = micros();
  /*
  time_last_mag_reading_us = micros();
  */
  time_last_Kalman_update_us = micros();
  time_last_IMU_update_us = micros();

  // let a bit of time for the filter to converge; 10 s at 100Hz is 100 updates
  for (int i=0; i<100; i++){
   get_new_reading(dummy_inout, dummy_inout, dummy_inout, dummy_inout, dummy_inout, dummy_inout);
   wdt.restart();
  }

  return true;
}

bool IMU_Manager::stop_IMU(){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

   turn_qwiic_switch_off();
   delay(100);

   disableBurstMode();
   delay(100);

  Serial.println(F("power down ArtemisWire"));
  ArtemisWire.end();
  wdt.restart();
  delay(100);

   return true;
}

void IMU_Manager::set_sensors_parameters(void){
   wdt.restart();
  // set the accel gyro properties
  ism330dhcx.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  ism330dhcx.setGyroRange(LSM6DS_GYRO_RANGE_125_DPS);
  ism330dhcx.setAccelDataRate(LSM6DS_RATE_833_HZ);
  ism330dhcx.setGyroDataRate(LSM6DS_RATE_833_HZ);
  ism330dhcx.configInt1(false, false, true); // accelerometer DRDY on INT1
  ism330dhcx.configInt2(false, true, false); // gyro DRDY on INT2
  delay(500);
  wdt.restart();

  /*
  // set the magnetometer properties
  lis3mdl.setPerformanceMode(LIS3MDL_HIGHMODE);
  lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
  lis3mdl.setDataRate(LIS3MDL_DATARATE_560_HZ);
  lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
  lis3mdl.setIntThreshold(500);
  lis3mdl.configInterrupt(false, false, true, // enable z axis
                          true, // polarity
                          false, // don't latch
                          true); // enabled!
  */
  delay(500);
  wdt.restart();
}

void IMU_Manager::print_sensors_information(void){
   wdt.restart();
  // print accel gyro info
  Serial.println(F("accel gyro info"));
  Serial.print("Accelerometer range set to: ");
  switch (ism330dhcx.getAccelRange()) {
    case LSM6DS_ACCEL_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case LSM6DS_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DS_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DS_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
   wdt.restart();

  Serial.print("Gyro range set to: ");
  switch (ism330dhcx.getGyroRange()) {
    case LSM6DS_GYRO_RANGE_125_DPS:
    Serial.println("125 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  case ISM330DHCX_GYRO_RANGE_4000_DPS:
    Serial.println("4000 degrees/s");
    break;
  }
   wdt.restart();

  Serial.print("Accelerometer data rate set to: ");
  switch (ism330dhcx.getAccelDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }
   wdt.restart();

  Serial.print("Gyro data rate set to: ");
  switch (ism330dhcx.getGyroDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }
   wdt.restart();

  /*
  // print magneto info
  Serial.println(F("print magnetometer info"));

  Serial.print("Performance mode set to: ");
  switch (lis3mdl.getPerformanceMode()) {
    case LIS3MDL_LOWPOWERMODE: Serial.println("Low"); break;
    case LIS3MDL_MEDIUMMODE: Serial.println("Medium"); break;
    case LIS3MDL_HIGHMODE: Serial.println("High"); break;
    case LIS3MDL_ULTRAHIGHMODE: Serial.println("Ultra-High"); break;
  }
   wdt.restart();

  Serial.print("Operation mode set to: ");
  // Single shot mode will complete conversion and go into power down
  switch (lis3mdl.getOperationMode()) {
    case LIS3MDL_CONTINUOUSMODE: Serial.println("Continuous"); break;
    case LIS3MDL_SINGLEMODE: Serial.println("Single mode"); break;
    case LIS3MDL_POWERDOWNMODE: Serial.println("Power-down"); break;
  }
   wdt.restart();

  Serial.print("Data rate set to: ");
  switch (lis3mdl.getDataRate()) {
    case LIS3MDL_DATARATE_0_625_HZ: Serial.println("0.625 Hz"); break;
    case LIS3MDL_DATARATE_1_25_HZ: Serial.println("1.25 Hz"); break;
    case LIS3MDL_DATARATE_2_5_HZ: Serial.println("2.5 Hz"); break;
    case LIS3MDL_DATARATE_5_HZ: Serial.println("5 Hz"); break;
    case LIS3MDL_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3MDL_DATARATE_20_HZ: Serial.println("20 Hz"); break;
    case LIS3MDL_DATARATE_40_HZ: Serial.println("40 Hz"); break;
    case LIS3MDL_DATARATE_80_HZ: Serial.println("80 Hz"); break;
    case LIS3MDL_DATARATE_155_HZ: Serial.println("155 Hz"); break;
    case LIS3MDL_DATARATE_300_HZ: Serial.println("300 Hz"); break;
    case LIS3MDL_DATARATE_560_HZ: Serial.println("560 Hz"); break;
    case LIS3MDL_DATARATE_1000_HZ: Serial.println("1000 Hz"); break;
  }
   wdt.restart();

  Serial.print("Range set to: ");
  switch (lis3mdl.getRange()) {
    case LIS3MDL_RANGE_4_GAUSS: Serial.println("+-4 gauss"); break;
    case LIS3MDL_RANGE_8_GAUSS: Serial.println("+-8 gauss"); break;
    case LIS3MDL_RANGE_12_GAUSS: Serial.println("+-12 gauss"); break;
    case LIS3MDL_RANGE_16_GAUSS: Serial.println("+-16 gauss"); break;
  }
   wdt.restart();
   */
}

bool IMU_Manager::update_accumulate_Kalman(void){
   // clear measurement accus
   accu_acc_x.clear();
   accu_acc_y.clear();
   accu_acc_z.clear();

   accu_gyr_x.clear();
   accu_gyr_y.clear();
   accu_gyr_z.clear();

   accu_mag_x.clear();
   accu_mag_y.clear();
   accu_mag_z.clear();

   if (micros() - time_last_Kalman_update_us > nbr_micros_between_Kalman_update){
      Serial.print(F("W behind UAC by ")); Serial.println(micros() - time_last_Kalman_update_us - nbr_micros_between_Kalman_update);
   }

   if (micros() - time_last_accel_gyro_reading_us > 1.7 * nbr_micros_between_accel_gyro_readings){
      Serial.print(F("W behind ACC by ")); Serial.println(micros() - time_last_accel_gyro_reading_us - nbr_micros_between_accel_gyro_readings);
   }

   /*
   if (micros() - time_last_mag_reading_us > 1.7 * nbr_micros_between_mag_readings){
      Serial.print(F("W behind MAG by ")); Serial.println(micros() - time_last_mag_reading_us - nbr_micros_between_mag_readings);
   }
   */

   // perform as many measurements as possible while it is time
   while (micros() - time_last_Kalman_update_us < nbr_micros_between_Kalman_update){
      // if time to read acc / gyr, do it
      if (micros() - time_last_accel_gyro_reading_us > nbr_micros_between_accel_gyro_readings){
         time_last_accel_gyro_reading_us += nbr_micros_between_accel_gyro_readings;

        // NOTE: if there are issues with the stability of the reading, it would be possible to:
        // 1 - have a counter of "number of consecutive bad readings"
        // 2 - if more than 5 consecutive bad readings, return false; while less than 5, try again
        // looks like the readings are stable though, so probably no need to implement this workaround.
         if (!ism330dhcx.getEvent(&accel, &gyro, &temp)){
           Serial.println(F("ERROR: unable to read from acc / gyr"));
           return false;
         }

         accu_acc_x.push_back(accel.acceleration.x);
         accu_acc_y.push_back(accel.acceleration.y);
         accu_acc_z.push_back(accel.acceleration.z);

         accu_gyr_x.push_back(gyro.gyro.x);
         accu_gyr_y.push_back(gyro.gyro.y);
         accu_gyr_z.push_back(gyro.gyro.z);

         stat_nbr_accel_gyro_readings++;
      }

      /*
      // if time to read mag, do it
      if (micros() - time_last_mag_reading_us > nbr_micros_between_mag_readings){
         time_last_mag_reading_us += nbr_micros_between_mag_readings;

         lis3mdl.getEvent(&mag);

         accu_mag_x.push_back(mag.magnetic.x);
         accu_mag_y.push_back(mag.magnetic.y);
         accu_mag_z.push_back(mag.magnetic.z);

         stat_nbr_mag_readings++;
      }
      */

   }
   time_last_Kalman_update_us += nbr_micros_between_Kalman_update;

   // enableBurstMode();

   // compute the means of the measurements, using a n-sigma filter
   acc_x = float_mean_filter(accu_acc_x);
   acc_y = float_mean_filter(accu_acc_y);
   acc_z = float_mean_filter(accu_acc_z);

   gyr_x = float_mean_filter(accu_gyr_x) * SENSORS_RADS_TO_DPS;
   gyr_y = float_mean_filter(accu_gyr_y) * SENSORS_RADS_TO_DPS;
   gyr_z = float_mean_filter(accu_gyr_z) * SENSORS_RADS_TO_DPS;

   /*
   mag_x = float_mean_filter(accu_mag_x);
   mag_y = float_mean_filter(accu_mag_y);
   mag_z = float_mean_filter(accu_mag_z);
   */
  
   // unsigned long crrt_micros;
   // crrt_micros = micros();

   // feed the means of the measurements to the Kalman filter
   // in case the magnometer is calibrated, we can get direction information
   if (imu_use_magnetometer){
      Kalman_filter.update(gyr_x, gyr_y, gyr_z,
                           acc_x, acc_y, acc_z,
                           mag_x, mag_y, mag_z);
   }
   // in case the magnometer is not calibrated, the magnometer does more harm than good, consider switching off then!
   else{
      Kalman_filter.update(gyr_x, gyr_y, gyr_z,
                           acc_x, acc_y, acc_z,
                           0.0f, 0.0f, 0.0f);
   }

   // Serial.print(F("K took ")); Serial.println(micros() - crrt_micros);
   // crrt_micros = micros();

   // get the Kalman filter information, and apply rotation to get NED
   roll_ = Kalman_filter.getRoll();
   pitch = Kalman_filter.getPitch();
   yaw__ = Kalman_filter.getYaw();

   Kalman_filter.getQuaternion(&qr, &qi, &qj, &qk);

   vec3_setter(&accel_raw, acc_x, acc_y, acc_z);
   quat_setter(&quat_rotation, qr, qi, qj, qk);
   rotate_by_quat_R(&accel_raw, &quat_rotation, &accel_NED);

   // Serial.print(F("Q took ")); Serial.println(micros() - crrt_micros);

   // accumulate Kalman measurements
   accu_acc_N.push_back(accel_NED.i);
   accu_acc_E.push_back(accel_NED.j);
   accu_acc_D.push_back(accel_NED.k);

   accu_yaw__.push_back(yaw__);
   accu_pitch.push_back(pitch);
   accu_roll_.push_back(roll_);

   stat_nbr_kalman_updates++;

   //disableBurstMode();

   return true;
}

bool IMU_Manager::get_new_reading(float & acc_N_inout, float & acc_E_inout, float & acc_D_inout,
                   float & yaw___inout,   float & pitch_inout, float & roll__inout
                   ){
   if (blink_when_use_IMU){
     if (counter_nbr_cycles_LED_off == 0){
       digitalWrite(LED, LOW);
     }
     if (counter_nbr_cycles_LED_off >= number_update_between_blink){
       counter_nbr_cycles_LED_off = -1;
       digitalWrite(LED, HIGH);
     }
   }
   counter_nbr_cycles_LED_off += 1;

   // clear the Kalman output accus
   accu_acc_N.clear();
   accu_acc_E.clear();
   accu_acc_D.clear();

   accu_yaw__.clear();
   accu_pitch.clear();
   accu_roll_.clear();

   stat_nbr_accel_gyro_readings = 0;
   stat_nbr_mag_readings = 0;
   stat_nbr_kalman_updates = 0;

   if (micros() - time_last_IMU_update_us > 1.2 * nbr_micros_between_IMU_update){
      Serial.print(F("W: behind GNR by ")); Serial.println(micros() - time_last_IMU_update_us - nbr_micros_between_IMU_update);
   }

   // perform as many kalman updates as possible while it is time
   while (micros() - time_last_IMU_update_us < nbr_micros_between_IMU_update){
      if(!update_accumulate_Kalman()){
        Serial.println(F("ERROR cannot get new IMU Kalman reading"));
        return false;
      }
   }
   time_last_IMU_update_us += nbr_micros_between_IMU_update;

   // enableBurstMode();

   // filter all outputs
   acc_N_inout = float_mean_filter(accu_acc_N);
   acc_E_inout = float_mean_filter(accu_acc_E);
   acc_D_inout = float_mean_filter(accu_acc_D);

   yaw___inout = float_mean_filter(accu_yaw__);
   pitch_inout = float_mean_filter(accu_pitch);
   roll__inout = float_mean_filter(accu_roll_);

   // print stats
   /*
   Serial.println("stats IMU");
   Serial.print(F("nbr reads | acc gyro ")); Serial.print(stat_nbr_accel_gyro_readings);
   Serial.print(F(" | mag ")); Serial.print(stat_nbr_mag_readings);
   Serial.print(F(" | K updates ")); Serial.println(stat_nbr_kalman_updates);
   */

   // disableBurstMode();

   wdt.restart();

   return true;
}
