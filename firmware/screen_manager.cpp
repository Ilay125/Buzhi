#include "screen_manager.h"
#include "config.h"
#include <cmath>
#include "pico/rand.h"

Screen_Manager::Screen_Manager(Screen* screen) {
    this->screen = screen;
    this->state = RST;
    this->last_update_time = 0;
    this->looking_duration = 0;
    this->drawing_progress = 0.0;

    this->update();
}

void Screen_Manager::set_state(Screen_State new_state) {
    this->state = new_state;
    this->last_update_time = 0; // Reset timer to show new state immediately
    this->looking_duration = 0;
    this->drawing_progress = 0.0;
    this->update();
}

void Screen_Manager::set_drawing_progress(double progress) {
    this->drawing_progress = progress;
    if (this->state == DRAWING) {
        drawing_frame(this->screen);
    }
}

void Screen_Manager::update() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    switch (this->state) {
        case LOOKING_FOR_CARD:
            if (current_time - this->last_update_time >= this->looking_duration) {
                looking_for_card_frame(this->screen);
                this->last_update_time = current_time;
            }
            break;
        case DRAWING:
            drawing_frame(this->screen);
            break;
        default:
            rst_screen(this->screen);
            break;
    }
}

void Screen_Manager::rst_screen(Screen* screen) {
    screen->fill(COLOR_BLACK);
    screen->update();
}

void Screen_Manager::looking_for_card_frame(Screen* screen) {   
    int eye_rad = 50;

    screen->fill(COLOR_WHITE);
    int eye_margin = 50;
    int eye_x = get_rand_32() % (SCREEN_WIDTH - eye_rad*2 - eye_margin) + eye_rad;
    int eye_y = get_rand_32() % (SCREEN_HEIGHT - eye_rad*2 - eye_margin) + eye_rad;
    
    int dx = eye_x - SCREEN_WIDTH / 2;
    int dy = eye_y - SCREEN_HEIGHT / 2;
    
    double theta = atan2(dy, dx);
    double dist_from_center = sqrt(dx*dx + dy*dy);

    screen->fill_circle(eye_x, eye_y, eye_rad, COLOR_BLACK);
    
    int offset = 15;
    int pupil_x = eye_x + (int)(offset * cos(theta));
    int pupil_y = eye_y + (int)(offset * sin(theta));

    screen->fill_circle(pupil_x, pupil_y, 15, COLOR_WHITE);

    screen->update();

    this->looking_duration = 1000 + (get_rand_32() % 1000); // Random delay between 1-2 seconds
    
}

void Screen_Manager::drawing_frame(Screen* screen) {
    screen->fill(COLOR_BLACK);

    screen->fill_arc(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 110, 20, 0, 2*M_PI*this->drawing_progress, COLOR_CYAN);

    int percentage = (int)(this->drawing_progress * 100); 
    int scale = 5; 
    // Count digits 
    int num_digits = (percentage >= 100) ? 3 : (percentage >= 10) ? 2 : 1; 

    // Each digit: 5 pixels wide, spacing = 1 pixel 
    int digit_width = 5 * scale; 
    int spacing = 1 * scale; 

    int text_y = SCREEN_HEIGHT/2 - (int)(3.5*scale);

    // I dont know why it works - but it is :)
    int text_x = SCREEN_WIDTH/2 + ((num_digits + 1) * digit_width + (num_digits) * spacing) / 2 - 20;

    // Centered horizontally 
    screen->draw_percentage(text_x, text_y, percentage, COLOR_CYAN, scale);
    screen->update();
}