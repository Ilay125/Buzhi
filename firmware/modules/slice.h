#ifndef __SLICE__
#define __SLICE__

class Slice {
    int slice_num;
    int clkdiv;
    int wrap;
    bool is_enabled;

    public:
        /**
         * @fn Slice
         * @brief Construct a new Slice object.
         * @param num PWM slice number (0-7).
         * @param clkdiv Clock divider (float value, e.g., 4.0).
         * @param wrap Wrap value (max count value).
         * @param is_enabled true to enable the slice, false to disable.
         */
        Slice(int num, int clkdiv, int wrap, bool is_enabled);

        /**
         * @fn Slice
         * @brief Construct a new Slice object with frequency.
         * @param num PWM slice number (0-7).
         * @param freq Frequency in Hz.
         * @param is_enabled true to enable the slice, false to disable.
         */
        Slice(int num, int freq, bool is_enabled);

        /**
         * @fn set_clkdiv
         * @brief Set the clock divider.
         * @param clkdiv Clock divider (float value, e.g., 4.0).
         */
        void set_clkdiv(int clkdiv);

        /**
         * @fn set_wrap
         * @brief Set the wrap value.
         * @param wrap Wrap value (max count value).
         */
        void set_wrap(int wrap);

        /**
         * @fn set_enabled
         * @brief Enable or disable the PWM slice.
         * @param state true to enable, false to disable.
         */
        void set_enabled(bool state);

        /**
         * @fn get_num
         * @brief Get the PWM slice number.
         * @return PWM slice number (0-7).
         */
        int get_num();

        /**
         * @fn get_clkdiv
         * @brief Get the current clock divider.
         * @return Current clock divider (float value).
         */
        int get_clkdiv();

        /**
         * @fn get_wrap
         * @brief Get the current wrap value.
         * @return Current wrap value (max count value).
         */
        int get_wrap();

        /**
         * @fn get_enabled
         * @brief Check if the PWM slice is enabled.
         * @return true if enabled, false if disabled.
         */
        bool get_enabled();
};

#endif