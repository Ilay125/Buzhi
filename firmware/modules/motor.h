#ifndef __MOTOR__
#define __MOTOR__

enum Dir {
    clockwise = 1,
    counterclockwise = -1
};


class Motor {
    int steps;
    int step_pin;
    int dir_pin;
    double angle0;

    public:
    Motor(int step_pin, int dir_pin, double angle0);

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
    void Motor::set_steps(int steps);

    /**
     * @name add_steps.
     * @brief adds the relative num of steps from home given the direction moving.
     * @param[in] steps the steps.
     * @param[in] dir the dircetion the motor moves
     */
    void Motor::add_steps(int steps, Dir dir);

    int Motor::get_dir_pin();
};

/**
 * @name move_motors
 * @brief Moves two motors simultaniously using PIO.
 * @param[in] m1 Reference to right motor.
 * @param[in] m2 Reference to left motor.
 * @param[in] s1 Angle for right motor.
 * @param[in] s2 angle for left motor.
 * @param[in] pio ...
 */
void move_motors(Motor &m1, Motor &m2, double s1, double s2, double dist, PIO pio, uint sA, uint sB);

#endif