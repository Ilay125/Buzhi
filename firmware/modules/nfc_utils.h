#ifndef __NFC_UTILS__
#define __NFC_UTILS__

#include <vector>
#include "pico/stdlib.h"
#include <cstdint>


bool extract_only_data_raw_commands(
    const std::vector<uint8_t>& raw,
    std::vector<uint8_t>& out
);

#endif