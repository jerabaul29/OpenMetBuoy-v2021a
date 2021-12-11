#include "print_utils.h"

void print_uint64(uint64_t to_print){
    // I have tried with snprintf and associates, seem to not work with uint64_t
    // even using the special macros etc. Write my simple implementation by hand
    // then!

    static constexpr size_t utils_char_buffer_size {24};
    char utils_char_buffer[utils_char_buffer_size] {'\0'};

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
        SERIAL_USB->print(utils_char_buffer[crrt_index]);
        if (crrt_index == 0){
            break;
        }
        else {
            crrt_index -= 1;
        }
    }

}
