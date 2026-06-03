#ifndef STAT_PROCESSING
#define STAT_PROCESSING

#include "Arduino.h"

#include "etl.h"
#include "etl/vector.h"

#include "math.h"

#ifndef STAT_PROCESSING_VERBOSE
  #define STAT_PROCESSING_VERBOSE 0
#endif

float float_mean_filter(etl::ivector<float> const & vec_in);

float float_sigma_filter(etl::ivector<float> const & vec_in, float n_sigma=2.0);

// a n-sigma filter, i.e. compute the mean based on only the values of the array that are with n standard deviations of the mean
// we do it here taking great care to not overflow and keep as high accuracy as possible; this is in case we for example use some integral types
// and need high accuracy on these (for example, working with lat and lon data represented as int or long)
template <typename T>
T accurate_sigma_filter(etl::ivector<T> const & vec_in, double n_sigma=2.0, bool verbose=false){
  static_assert(std::is_signed<T>::value, "signed values only; we rely on signed arithmetics to compute the mean while avoiding overflows");

  // we need to use a n_sigma that is large enough to make sure we have at least a sample; 1.0 should be enough, see 
  // https://stats.stackexchange.com/questions/524011/how-many-standard-deviations-around-the-mean-do-i-need-to-guarantee-covering-at/524014#524014
  // for a bit of margin, take 1.5
  // is n_sigma too low? Increase it
  if (n_sigma < 1.5){
    n_sigma = 1.5;
    #if STAT_PROCESSING_VERBOSE
      Serial.println(F("we were using unsafe small n_sigma; set it to 1.5"));
    #endif
  }

  // do we have a vec of length 0? if this is the case, return default value 0
  if (vec_in.size() == 0){
    #if STAT_PROCESSING_VERBOSE
      Serial.println(F("empty vector, return 0"));
    #endif
    return 0;
  }

  // are all points equal? If this is the case, no need to compute anything
  T first_elem = vec_in[0];
  bool all_equal = true;
  for (int ind=0; ind<vec_in.size(); ind++){
    if (first_elem != vec_in[ind]){
      all_equal = false;
      break;
    }
  }

  // all elements are equal, just return the first element
  if (all_equal){
    #if STAT_PROCESSING_VERBOSE
      Serial.println(F("all elements equal, use the first element"));
    #endif
    return first_elem;
  }

  // we now know that we have a vector that is non empty and that not all elements there are equal; can apply a n-sigma
  // filtering safely, as long as no overflow... We take great care to 1) not overflow 2) get as high accuracy as possible

  // start by finding out which elements should be kept for calculating the mean; this is all done in double to avoid overflows
    
  double vec_len_as_double = static_cast<double>(vec_in.size());

  // calculate the double mean
  double double_mean {0.0};
  for (int ind=0; ind<vec_in.size(); ind++){
    double crrt_value = static_cast<double>(vec_in[ind]);
    double_mean += crrt_value / vec_len_as_double; 
  }

  #if STAT_PROCESSING_VERBOSE
    Serial.print(F("double_mean = ")); Serial.println(double_mean);
  #endif

  // calculate the double rms
  double double_rms {0.0};
  for (int ind=0; ind<vec_in.size(); ind++){
    double crrt_value = static_cast<double>(vec_in[ind]);
    double_rms += (crrt_value - double_mean) * (crrt_value - double_mean) / vec_len_as_double;
  }

  #if STAT_PROCESSING_VERBOSE
    Serial.print(F("double_rms = ")); Serial.println(double_rms);
  #endif

  // calculate the double std
  double double_std {0};
  double_std = sqrt(double_rms);

  // calculate the max distance
  double double_max_distance = n_sigma * double_std; 

  #if STAT_PROCESSING_VERBOSE
    Serial.print(F("double_std = ")); Serial.println(double_std);
    Serial.print(F("double_max_distance = ")); Serial.println(double_max_distance);
  #endif

  // find out the coarse mean with only the points that should be used for calculating
  // compute in double first and divide after, to avoid overflows
  double double_coarse_mean {0.0};
  int nbr_of_valid_points {0};

  for (int ind=0; ind<vec_in.size(); ind++){
    double crrt_value = static_cast<double>(vec_in[ind]);
    if (fabs(crrt_value - double_mean) <= double_max_distance){
      double_coarse_mean += crrt_value;
      nbr_of_valid_points ++;
    }
  }

  double_coarse_mean /= static_cast<double>(nbr_of_valid_points);

  T coarse_mean = static_cast<T>(double_coarse_mean);

  #if STAT_PROCESSING_VERBOSE
    Serial.print(F("nbr_of_valid_points ")); Serial.println(nbr_of_valid_points);
    Serial.print(F("coarse_mean = ")); Serial.println(coarse_mean);
  #endif

  // find out the fine mean (relative to the coarse mean)
  T fine_mean {0};

  for (int ind=0; ind<vec_in.size(); ind++){
    double crrt_value = static_cast<double>(vec_in[ind]);
    if (fabs(crrt_value - double_mean) < double_max_distance){
      fine_mean += vec_in[ind] - coarse_mean;
    }
  }

  fine_mean /= static_cast<T>(nbr_of_valid_points);

  #if STAT_PROCESSING_VERBOSE
    Serial.print(F("fine_mean = ")); Serial.println(fine_mean);
  #endif

  return (coarse_mean + fine_mean);
}

// TODO: change to modern looping
// TODO: provide also the std in the same way

#endif

