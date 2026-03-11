#include "nfc_utils.h"
#include <vector>
#include "pico/stdlib.h"

bool extract_only_data_raw_commands(
    const std::vector<uint8_t>& raw,
    std::vector<uint8_t>& out
) {
    out.clear();

    auto is_cmd = [](uint8_t b) {
        return b=='M' || b=='L' || b=='C' || b=='Z';
    };
    auto need = [](uint8_t cmd) -> int {
        switch (cmd) {
            case 'M': return 2;
            case 'L': return 2;
            case 'C': return 6;
            case 'Z': return 0;
            default:  return -1;
        }
    };

    // 1) find first command
    size_t i = 0;
    while (i < raw.size() && !is_cmd(raw[i])) i++;
    if (i >= raw.size()) return false;

    // 2) parse command stream
    while (i < raw.size()) {
        uint8_t cmd = raw[i];

        // Optional explicit terminator: stop if you *know* you write 0x00 at end.
        // If you're saying "it doesn't return all data", you probably SHOULD NOT
        // treat 0x00 as terminator. So it's commented out.
        // if (cmd == 0x00) break;

        if (!is_cmd(cmd)) break;

        int n = need(cmd);
        if (n < 0) break;

        // include command byte
        out.push_back(cmd);
        i++;

        // include its coords bytes
        if (i + (size_t)n > raw.size()) break; // truncated read
        for (int k = 0; k < n; k++) {
            out.push_back(raw[i + k]);
        }
        i += (size_t)n;

        // loop expects next byte to be a command
    }

    return !out.empty();
}