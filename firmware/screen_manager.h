#ifndef __SCREENS__
#define __SCREENS__

#include "modules/screen.h"

enum Screen_State {
    RST = 0,
    LOOKING_FOR_CARD,
    DRAWING
};

class Screen_Manager {
    Screen* screen;
    Screen_State state;
    uint32_t last_update_time;
    uint32_t looking_duration; // Duration [ms] to show looking for card frame before updating animation
    double drawing_progress; // 0.0 to 1.0 for drawing animation

    void looking_for_card_frame(Screen* screen);
    void drawing_frame(Screen* screen);
    void rst_screen(Screen* screen);

    public:
        Screen_Manager(Screen* screen);
        void set_state(Screen_State new_state);
        void update();
        void set_drawing_progress(double progress);
};

#endif