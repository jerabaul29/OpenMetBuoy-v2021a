#include "statistical_processing.h"

float float_mean_filter(etl::ivector<float> const & vec_in){
  float mean {0.0f};
  for (int ind=0; ind<vec_in.size(); ind++){
    mean += vec_in[ind];
  }
  mean /= vec_in.size();
  return mean;
}

float float_sigma_filter(etl::ivector<float> const & vec_in, float n_sigma){
  // if empty vector print error message and return 0
  if (vec_in.size() == 0){
    Serial.println(F("WARNING!!! take sigma filter of empty vec"));
    return 0.0f;
  }

  // if all the elements are equal, return the fitst one
  // this also means that the size of the vector from now on is greter than 1
  bool all_equal = true;
  for (size_t ind=0; ind<vec_in.size(); ind++){
    if (vec_in[0] != vec_in[ind]){
      all_equal = false;
      break;
    }
  }
  if (all_equal){
    Serial.println(F("WARNING!!! take sigma filter of all equal vec"));
    return (vec_in[0]);
  }

  // compute the mean and std of the vector
  float mean {0};
  float std {0};

  for (size_t ind=0; ind<vec_in.size(); ind++){
    mean += vec_in[ind];
  }
  mean /= vec_in.size();

  for (size_t ind=0; ind<vec_in.size(); ind++){
    std += (vec_in[ind] - mean) * (vec_in[ind] - mean);
  }
  std /= (vec_in.size() - 1.0f);
  std = sqrtf(std);

  // compute the mean based on the elements that are ok
  float sigma_average {0};
  int nbr_of_ok_values {0};

  bool print_vector {false};

  for (size_t ind=0; ind<vec_in.size(); ind++){
    if ( fabsf(vec_in[ind] - mean) < (n_sigma * std)){
      sigma_average += vec_in[ind];
      nbr_of_ok_values++;
    }
    else{
      Serial.print(F("outlier ")); Serial.print(vec_in[ind]);
      print_vector = true;
    }
  }
  sigma_average /= nbr_of_ok_values;

  if (print_vector){
    Serial.print(F(" from "));
    for (int ind=0; ind<vec_in.size(); ind++){
      Serial.print(vec_in[ind]); Serial.print(" ");
    }
    Serial.println();
  }

  return sigma_average;
}
