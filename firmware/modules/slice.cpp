#include "slice.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

Slice::Slice(int num, int clkdiv, int wrap, bool is_enabled) {
    this->slice_num = num;
    this->clkdiv = clkdiv;
    this->wrap = wrap;
    this->is_enabled = is_enabled;

    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, is_enabled);
}

Slice::Slice(int num, int freq, bool is_enabled) {
    int pico_f = clock_get_hz(clk_sys);

    // we must create clock divider that counts microsecs - freq will be 1MHz

    int clkdiv = pico_f / 1000000;

    // create the qrap for a desired frequency
    int wrap = pico_f / (clkdiv * freq);

    this->slice_num = num;
    this->clkdiv = clkdiv;
    this->wrap = wrap;
    this->is_enabled = is_enabled;

    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, is_enabled);
}

void Slice::set_wrap(int wrap) {
    this->wrap = wrap;
    pwm_set_wrap(slice_num, wrap);
}

void Slice::set_enabled(bool state) {
    this->is_enabled = state;
    pwm_set_enabled(slice_num, is_enabled);
}

void Slice::set_clkdiv(int clkdiv) {
    this->clkdiv = clkdiv;
    pwm_set_clkdiv(slice_num, clkdiv);
}

int Slice::get_num() {
    return this->slice_num;
}

int Slice::get_clkdiv() {
    return this->clkdiv;
}

int Slice::get_wrap() {
    return this->wrap;
}

bool Slice::get_enabled() {
    return this->is_enabled;
}