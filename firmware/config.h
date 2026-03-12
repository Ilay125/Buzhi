#ifndef __CONFIG__
#define __CONFIG__


//#define DEBUG

/* GPIO PINS */

// MOTORS
#define STEP1_PIN 2
#define DIR1_PIN 4
#define ENABLE1_PIN 6

#define STEP2_PIN 3
#define DIR2_PIN 5
#define ENABLE2_PIN 7

// SERVO
#define SERVO_PIN 15 


// NFC READER
#define RESET_PIN 16
#define MISO_PIN 20
#define MOSI_PIN 19
#define SCK_PIN 18
#define CS_PIN 21


/* KINEMATICS */
#define PI 3.14159265
#define L0 4.2 // Base length [cm]
#define L 11.0 // Arm length [cm]

// Homing
#define X0 0 // [cm]
#define Y0 15.3 // [cm]

// Recallibration of coord-system
#define X_OFFSET -5.0 // [cm] 
#define Y_OFFSET 13.0 // [cm]
#define X_SCALE 0.8
#define Y_SCALE 0.65


/* MOTOR */
// convertion constant [angle/step] - How much angle I have in one step.
const double STEP_TO_DEG = 1.8 / 8;
#define MAX_SPEED 10 // steps frequency [steps/s]
#define MICROSTEP 0.1 // [cm] distance between points for curve interpolation

// The ratio for time to accelerate at the beginning
#define T1 0.2

// The ratio for time to decelerate at the end
#define T2 0.8

#define T_RES 1000

// Servo
#define SERVO_FREQ 50 // [Hz]
#define SERVO_UP_ANGLE 100 // [Deg]
#define SERVO_DOWN_MIN_ANGLE 55 // [Deg]
#define SERVO_DOWN_MID_ANGLE 60 // [Deg]
#define SERVO_DOWN_MAX_ANGLE 60 // [Deg]

#define R_LOW 13.5// [cm] border radius for servo down position
#define R_HIGH 13 // [cm] border radius for servo down position
#endif