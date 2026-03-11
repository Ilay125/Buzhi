#include "motor.h"


Motor::Motor(int step_pin, int dir_pin, int enable_pin, double angle0) {
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->enable_pin = enable_pin;

    gpio_init(this->step_pin);
    gpio_init(this->dir_pin);
    gpio_init(this->enable_pin);

    gpio_set_dir(this->step_pin, GPIO_OUT);
    gpio_set_dir(this->dir_pin, GPIO_OUT);
    gpio_set_dir(this->enable_pin, GPIO_OUT);

    this->home(angle0);
}

void Motor::home(double angle0) {
    this->angle0 = angle0;
    this->steps = 0;
}

int Motor::get_steps() {
    return this->steps;
}

double Motor::get_angle() {
    return this->angle0 + this->steps * STEP_TO_DEG;
} 

int Motor::get_dir_pin() {
    return this->dir_pin;
}

void Motor::set_steps(int steps) {
    this->steps = steps;
}

void Motor::add_steps(int steps, Dir dir) {
    this->steps += steps * dir;
}

void Motor::enable() {
    gpio_put(this->enable_pin, 0);
}

void Motor::disable() {
    gpio_put(this->enable_pin, 1);
}

Dir Motor::calc_dir(double new_angle) {
    double curr_angle = this->get_angle();
    return (new_angle > curr_angle) ? clockwise : counterclockwise;
}

int Motor::calc_steps_num(double new_angle) {
    double curr_angle = this->get_angle();
    double angle_diff = fabs(new_angle - curr_angle);

    //printf("curr_angle: %f, new_angle: %f, angle_diff: %f\n", curr_angle, new_angle, angle_diff);
    return (int)(angle_diff / STEP_TO_DEG);
}

void Motor::calc_speeds(double max_speed, int steps1, int steps2, double& speed1, double& speed2) {
    int max_steps = std::max(steps1, steps2);
    double t1 = (double)steps1 / max_steps;
    double t2 = (double)steps2 / max_steps;

    speed1 = max_speed * t1;
    speed2 = max_speed * t2;

    //printf("t1: %f, t2: %f, speed1: %f, speed2: %f\n", t1, t2, speed1, speed2);
}

double velocity_profile(double curr_t, double max_t, double max_speed) {
    double start_slope = max_speed / (T1*max_t);
    double end_slope = max_speed / (T2*max_t - max_t);

    if (curr_t < T1*max_t) {
        return start_slope * curr_t;
    }

    if (curr_t > T2*max_t) {
        return max_speed + (curr_t - T2*max_t) * end_slope;
    }

    return max_speed;
}


void Motor::move(int steps, Dir dir, double speed) {
    gpio_put(this->dir_pin, dir == clockwise ? 1 : 0);

    double step_delay = (STEP_TO_DEG) / speed; // seconds per step
    uint32_t delay_us = (uint32_t)(step_delay * 1e6); // microseconds per step

    //printf("starting move");

    sleep_us(20);

    for (int i = 0; i < steps; i++) {
        gpio_put(this->step_pin, 1);
        busy_wait_us_32(delay_us);
        gpio_put(this->step_pin, 0);
        busy_wait_us_32(delay_us);

        //this->add_steps(1, dir);
    }
}