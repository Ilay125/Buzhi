#ifndef __DEBUG__
#define __DEBUG__

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "config.h"

#ifdef DEBUG
    #include "pico/cyw43_arch.h"
    #define DEBUG_LED CYW43_WL_GPIO_LED_PIN
#else
    #define DEBUG_LED 25
#endif


class Debug {
    bool state;
    public:
        /**
         * @fn Debug
         * @brief Construct a new Debug object.
         */
        Debug();

        /**
         * @fn set
         * @brief Set the debug LED state.
         * @param state true to turn on, false to turn off.
         */
        void set(bool state);

        /**
         * @fn get
         * @brief Get the current state of the debug LED.
         * @return true if on, false if off.
         */
        bool get();

        /**
         * @fn toggle
         * @brief Toggle the state of the debug LED.
         */
        void toggle();

        /**
         * @fn blink
         * @brief Blink the debug LED a specified number of times with a given delay.
         * @param times Number of times to blink.
         * @param delay_ms Delay in milliseconds between on and off states.
         */
        void blink(int times, int delay_ms);
};

#endif