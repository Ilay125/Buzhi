#ifndef __SCREEN__
#define __SCREEN__

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "config.h"

// Colors
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F
#define COLOR_YELLOW 0xFFE0
#define COLOR_CYAN 0x07FF
#define COLOR_MAGENTA 0xF81F

class Screen {

    spi_inst_t* spi;

    int pin_sck;
    int pin_mosi;
    int pin_dc;
    int pin_cs;
    int pin_rst;

    uint16_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    void init();

    void write_cmd(uint8_t cmd);
    void write_data(const uint8_t* data, size_t len);
    void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

    public:
        Screen(spi_inst_t* spi,
            uint pin_sck,
            uint pin_mosi,
            uint pin_dc,
            uint pin_cs,
            uint pin_rst);

        void fill(uint16_t color);

        void set_pixel(uint16_t x, uint16_t y, uint16_t color);

        // Rectangles
        void fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

        // Circle outline
        void draw_circle(int x0, int y0, int r, uint16_t color);

        // Filled circle
        void fill_circle(int x0, int y0, int r, uint16_t color);

        // Filled elipse
        void fill_ellipse(int x0, int y0, int rx, int ry, uint16_t color);

        void fill_arc(int x0, int y0, int r, int width, float start_angle, float end_angle, uint16_t color);

        void draw_digit(int x, int y, int digit, uint16_t color, int scale);
        void draw_number(int x, int y, int number, uint16_t color, int scale);
        void draw_percentage(int x, int y, int value, uint16_t color, int scale);

        void reset();

        void update();

};


// BITMAPS
// 5x7 pixel font: 0-9 and %
const bool font_digits[11][7][5] = {
    // 0
    {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,1,1},
        {1,0,1,0,1},
        {1,1,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    },
    // 1
    {
        {0,0,1,0,0},
        {0,1,1,0,0},
        {1,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {0,0,1,0,0},
        {1,1,1,1,1}
    },
    // 2
    {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1}
    },
    // 3
    {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {0,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1}
    },
    // 4
    {
        {1,0,0,1,0},
        {1,0,0,1,0},
        {1,0,0,1,0},
        {1,1,1,1,1},
        {0,0,0,1,0},
        {0,0,0,1,0},
        {0,0,0,1,0}
    },
    // 5
    {
        {1,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,1}
    },
    // 6
    {
        {0,1,1,1,1},
        {1,0,0,0,0},
        {1,0,0,0,0},
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    },
    // 7
    {
        {1,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,1,0},
        {0,0,1,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0},
        {0,1,0,0,0}
    },
    // 8
    {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,0}
    },
    // 9
    {
        {0,1,1,1,0},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {0,1,1,1,1},
        {0,0,0,0,1},
        {0,0,0,0,1},
        {1,1,1,1,0}
    },
    // %
    {
        {1,0,0,0,1},
        {1,0,0,1,0},
        {0,0,1,0,0},
        {0,1,0,0,0},
        {1,0,0,1,0},
        {1,0,0,0,1},
        {0,0,0,0,0}
    }
};

#endif