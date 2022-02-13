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

// for the print_hex family of functins:
// 1 byte is represented by 2 hex chars
// so u8 ie 1 byte is 0x + 1*2 + 1 hex chars ie 5 chars
// so u16 ie 2 bytes is 0x + 2*2 + 1 hex chars ie 7 chars
// so u32 ie 4 bytes is 0x + 2*4 + 1 hex chars ie 11 chars
// so u64 ie 8 bytes is 0x + 2*8 + 1 hex chars ie 19 chars
// we round to closest number of bytes that is a multiple of 4 as we are on either 32 or 64 bits MCU

// print arrays of uints

void print_hex_u8s(uint8_t *data, size_t length)
{
    constexpr size_t tmp_size {8};
    char tmp[tmp_size];
    for (int i = 0; i < length; i++)
    {
        snprintf(tmp, tmp_size, "0x%.2X", data[i]);
        SERIAL_USB->print(tmp);
        SERIAL_USB->print(" ");
    }
}

void print_hex_u16s(uint16_t *data, size_t length)
{
    constexpr size_t tmp_size {8};
    char tmp[tmp_size];
    for (int i = 0; i < length; i++)
    {
        snprintf(tmp, tmp_size, "0x%.4X", data[i]);
        SERIAL_USB->print(tmp);
        SERIAL_USB->print(" ");
    }
}

void print_hex_u32s(uint32_t *data, size_t length)
{
    constexpr size_t tmp_size {12};
    char tmp[tmp_size];
    for (int i = 0; i < length; i++)
    {
        snprintf(tmp, tmp_size, "0x%.8X", data[i]);
        SERIAL_USB->print(tmp);
        SERIAL_USB->print(" ");
    }
}

void print_hex_u64s(uint64_t *data, size_t length)
{
    constexpr size_t tmp_size {20};
    char tmp[tmp_size];
    for (int i = 0; i < length; i++)
    {
        // workaround uint64_t issues with the snprintf provided
        snprintf(tmp, tmp_size, "0x%.8X", static_cast<uint32_t>(data[i] >> 32));
        SERIAL_USB->print(tmp);
        snprintf(tmp, tmp_size, "%.8X", static_cast<uint32_t>(data[i]));
        SERIAL_USB->print(tmp);
        SERIAL_USB->print(" ");
    }
}

// printing a single uint

void print_hex_u8(uint8_t data)
{
    constexpr size_t tmp_size {8};
    char tmp[tmp_size];
    snprintf(tmp, tmp_size, "0x%.2X", data);
    SERIAL_USB->print(tmp);
}

void print_hex_u16(uint16_t data)
{
    constexpr size_t tmp_size {8};
    char tmp[tmp_size];
    snprintf(tmp, tmp_size, "0x%.4X", data);
    SERIAL_USB->print(tmp);
}

void print_hex_u32(uint32_t data)
{
    constexpr size_t tmp_size {12};
    char tmp[tmp_size];
    snprintf(tmp, tmp_size, "0x%.8X", data);
    SERIAL_USB->print(tmp);
}

void print_hex_u64(uint64_t data)
{
    constexpr size_t tmp_size {20};
    char tmp[tmp_size];

    // workaround uint64_t issues with the snprintf provided
    snprintf(tmp, tmp_size, "0x%.8X", static_cast<uint32_t>(data >> 32));
    SERIAL_USB->print(tmp);
    snprintf(tmp, tmp_size, "%.8X", static_cast<uint32_t>(data));
    SERIAL_USB->print(tmp);
    SERIAL_USB->print(" ");
}
