#ifndef __MOTOR__
#define __MOTOR__

#include "config.h"
#include "hardware/gpio.h"
#include <cmath>
#include "hardware/clocks.h"
#include "pico/time.h"
#include "stdlib.h"
#include <cstdio>

enum Dir {
    clockwise = 1,
    counterclockwise = -1
};

class Motor {
    int steps;
    int step_pin;
    int dir_pin;
    int enable_pin;
    double angle0;

    public:
    Motor(int step_pin, int dir_pin, int enable_pin, double angle0);

    /**
     * @name get_steps.
     * @brief gets the relative num of steps from home.
     * @return steps.
     */
    int get_steps();

    /**
     * @name get_angle.
     * @brief Calculates the angle of the motor based on R/L motor and step size.
     * @return angle
     */
    double get_angle();

    /**
     * @name set_steps.
     * @brief sets the relative num of steps from home.
     * @param[in] steps the steps.
     */
    void set_steps(int steps);

    /**
     * @name add_steps.
     * @brief adds the relative num of steps from home given the direction moving.
     * @param[in] steps the steps.
     * @param[in] dir the dircetion the motor moves
     */
    void add_steps(int steps, Dir dir);

    void enable();
    void disable();

    void move(int steps, Dir dir, double speed);

    int get_dir_pin();
};

#endif