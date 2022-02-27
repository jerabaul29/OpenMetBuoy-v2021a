#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Arduino.h"

#include "etl.h"
#include "etl/vector.h"

#include "params.h"

struct DataEntry{
    float accel_down;
};

void print_data_entry(DataEntry const & crrt_entry);

// for now, this is just a stupid thin wrapper around the etl::vector; but this may become more sophisticated later on if use FRAM...
class DataManager{
    public:
        void clear(void);
        void push_back_entry(DataEntry const & data_entry);
        size_t size(void) const;
        DataEntry const & get_entry(size_t index) const;
        void print_information(void) const;
        static constexpr size_t max_data_size {total_number_of_samples + 100};  // 20 minutes, at 10 Hz, with a bit of margin
    private:
        etl::vector<DataEntry, max_data_size> etl_vector_data;
};

extern DataManager board_data_manager;

#endif
