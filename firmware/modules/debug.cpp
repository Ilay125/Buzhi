#include "debug.h"
#include "pico/stdlib.h"


#if defined(PICO_CYW43_SUPPORTED)
    Debug::Debug() { 
        this->state = false;
        cyw43_arch_gpio_put(DEBUG_LED, this->state);
    }

    void Debug::set(bool state) {
        this->state = state;
        cyw43_arch_gpio_put(DEBUG_LED, this->state);
    }

#else
    Debug::Debug() { 
        gpio_init(DEBUG_LED);
        gpio_set_dir(DEBUG_LED, GPIO_OUT);
        this->state = false;
        gpio_put(DEBUG_LED, this->state);
    }

    void Debug::set(bool state) {
        this->state = state;
        gpio_put(DEBUG_LED, this->state);
    }
#endif


bool Debug::get() {
    return this->state;
}

void Debug::toggle() {
    this->set(!this->state);
}

void Debug::blink(int times, int delay_ms) {
    for (int i = 0; i < times; i++) {
        this->toggle();
        sleep_ms(delay_ms);
        this->toggle();
        sleep_ms(delay_ms);
    }
}