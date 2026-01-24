#ifndef __CONFIG__
#define __CONFIG__

// CHANGE 1 IF USING PICO 2W FOR DEBUG SIM
#define DEBUG 0

/* GPIO PINS */

// MOTORS


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
#define Y_OFFSET 9.0 // [cm]

#endif