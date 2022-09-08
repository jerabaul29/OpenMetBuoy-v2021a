#include "data_manager.h"

void print_data_entry(const DataEntry &crrt_entry){
    Serial.print(F("acc_D: ")); Serial.println(crrt_entry.accel_down);
}

void DataManager::clear(void){
    etl_vector_data.clear();
}

void DataManager::push_back_entry(const DataEntry &data_entry){
    etl_vector_data.push_back(data_entry);
}

size_t DataManager::size(void) const {
    return etl_vector_data.size();
}

DataEntry const & DataManager::get_entry(size_t index) const {
    return etl_vector_data[index];
}

void DataManager::print_information(void) const{
    Serial.print("data manager size: "); Serial.print(size()); Serial.print(F(" | total capacity ")); Serial.println(etl_vector_data.capacity());
    Serial.println(F("START CONTENT DATA MANAGER"));
    for (size_t i=0; i<size(); i++){
        print_data_entry(get_entry(i));
    }
    Serial.println(F("END CONTENT DATA MANAGER"));
}

DataManager board_data_manager;
