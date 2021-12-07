#include "utils/print_utils.h"

void print_uint64(uint64_t to_print){
    clear_utils_char_buffer();

    // I have tried with snprintf and associates, seem to not work with uint64_t
    // even using the special macros etc. Write my simple implementation by hand
    // then!

    size_t crrt_index {0};

    while (crrt_index < utils_char_buffer_size){
        utils_char_buffer[crrt_index] = static_cast<char>(to_print % 10 + 48);
        to_print /= 10;
        if (to_print == 0){
            break;
        }
        else {
            crrt_index += 1;
        }
    }

    while (true){
        SERIAL_USB.print(utils_char_buffer[crrt_index]);
        if (crrt_index == 0){
            break;
        }
        else {
            crrt_index -= 1;
        }
    }

    clear_utils_char_buffer();
}

//////////////////////////////////////////////////////////////////////////////////////////
// utils char buffer

void clear_utils_char_buffer(void){
    for (size_t i=0; i<utils_char_buffer_size; i++){
        utils_char_buffer[i] = '\0';
    }
}

char utils_char_buffer[utils_char_buffer_size];
