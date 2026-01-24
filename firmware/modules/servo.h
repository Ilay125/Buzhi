#ifndef __SERVO__
#define __SERVO__

#include "slice.h"

class Servo {
    int pin;
    int angle;
    Slice* slice;
    int freq;
    int channel;

    public:
        /**
         * @fn Servo
         * @brief Construct a new Servo object.
         * @param pin GPIO pin number where the servo is connected.
         * @param f Frequency in Hz. Standard servos run at 50Hz.
         */
        Servo(int pin, int f);

        /**
         * @fn _set_angle_fast
         * @brief Set the servo angle fast - might create torque.
         * @param angle Angle in degrees (0 to 180).
         */
        void _set_angle_fast(int ang);

        /**
         * @fn _set_angle_slow
         * @brief Set the servo angle slow - creates less torque.
         * @param angle Angle in degrees (0 to 180).
         */
        void _set_angle_slow(int ang);

        /**
         * @fn set_angle
         * @brief Sets the angle of the servo according to defined speed.
         * @param angle Angle in degrees (0 to 180).
         */
        void set_angle(int angle, bool fast=false);

        /**
         * @fn add_angle
         * @brief Add to the current servo angle.
         * @param angle Angle in degrees to add (can be negative).
         */
        void add_angle(int ang, bool fast=false);

        /**
         * @fn get_pin
         * @brief Get the GPIO pin number where the servo is connected.
         * @return GPIO pin number.
         */
        int get_pin();

        /**
         * @fn get_angle
         * @brief Get the current servo angle.
         * @return Current angle in degrees.
         */
        int get_angle();

        /**
         * @fn ~Servo
         * @brief Destroy the Servo object and free resources.
         */
        ~Servo();
};

#endif