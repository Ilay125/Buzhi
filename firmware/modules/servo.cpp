#include <cmath>
#include "servo.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define MIN_PULSE_WIDTH_PER 0.025
#define MAX_PULSE_WIDTH_PER 0.125

#define SLOW_INCREMENT 1

Servo::Servo(int pin, int f) {
    this->pin = pin;
    this->angle = 0;
    this->freq = f;
    
    gpio_set_function(this->pin, GPIO_FUNC_PWM);

    int slice_num = pwm_gpio_to_slice_num(this->pin);
    this->channel = pwm_gpio_to_channel(this->pin);

    this->slice = new Slice(slice_num, this->freq, true);
}

int Servo::get_pin() {
    return this->pin;
}

int Servo::get_angle() {
    return this->angle;
}

void Servo::_set_angle_fast(int ang) {
    this->angle = ang;

    int max_pulse_width = this->slice->get_wrap() * MAX_PULSE_WIDTH_PER;
    int min_pulse_width = this->slice->get_wrap() * MIN_PULSE_WIDTH_PER;

    int rel_time = (this->angle * (max_pulse_width - min_pulse_width)) / 180;
    int pulse_width = min_pulse_width + rel_time;

    pwm_set_chan_level(this->slice->get_num(), this->channel, pulse_width);
}

void Servo::_set_angle_slow(int ang) {
    int current = this->angle;

    int step = (current < ang) ? SLOW_INCREMENT : -SLOW_INCREMENT;

    while (abs(ang - current) > SLOW_INCREMENT) {
        current += step;
        this->_set_angle_fast(current);
        sleep_ms(10);  // adjust for speed
    }

    // Sets final angle exactly
    this->_set_angle_fast(ang);

    this->angle = ang;
}

void Servo::set_angle(int ang, bool fast) {
    if (ang < 0) {
        ang = 0;
    } else if (ang > 180) {
        ang = 180;
    }

    if (fast) {
        this->_set_angle_fast(ang);
    } else {
        this->_set_angle_slow(ang);
    }

    this->angle = ang;
}

void Servo::add_angle(int ang, bool fast) {
    this->set_angle(this->angle + ang, fast);
}

Servo::~Servo() {
    delete this->slice;
}