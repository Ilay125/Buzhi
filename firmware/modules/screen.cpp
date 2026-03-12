#include "Screen.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "config.h"
#include <cmath>

Screen::Screen(spi_inst_t* spi,
               uint pin_sck,
               uint pin_mosi,
               uint pin_dc,
               uint pin_cs,
               uint pin_rst) {
    this->spi = spi;
    this->pin_sck = pin_sck;
    this->pin_mosi = pin_mosi;
    this->pin_dc = pin_dc;
    this->pin_cs = pin_cs;
    this->pin_rst = pin_rst;

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            this->framebuffer[i][j] = 0x0000; // Initialize framebuffer to black
        }
    }

    this->init();
}

void Screen::write_cmd(uint8_t cmd)
{
    gpio_put(this->pin_dc,0);
    gpio_put(this->pin_cs,0);
    spi_write_blocking(this->spi,&cmd,1);
    gpio_put(this->pin_cs,1);
}

void Screen::write_data(const uint8_t* data,size_t len)
{
    gpio_put(this->pin_dc,1);
    gpio_put(this->pin_cs,0);
    spi_write_blocking(this->spi,data,len);
    gpio_put(this->pin_cs,1);
}

void Screen::reset()
{
    gpio_put(this->pin_rst,1);
    sleep_ms(50);
    gpio_put(this->pin_rst,0);
    sleep_ms(50);
    gpio_put(this->pin_rst,1);
    sleep_ms(120);
}

void Screen::set_window(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{
    uint8_t data[4];

    write_cmd(0x2A);
    data[0]=x0>>8; data[1]=x0;
    data[2]=x1>>8; data[3]=x1;
    write_data(data,4);

    write_cmd(0x2B);
    data[0]=y0>>8; data[1]=y0;
    data[2]=y1>>8; data[3]=y1;
    write_data(data,4);

    write_cmd(0x2C);
}

void Screen::init()
{
    stdio_init_all();
    spi_init(this->spi, 60000000);

    gpio_set_function(this->pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(this->pin_mosi, GPIO_FUNC_SPI);

    gpio_init(this->pin_dc);
    gpio_set_dir(this->pin_dc, GPIO_OUT);

    gpio_init(this->pin_cs);
    gpio_set_dir(this->pin_cs, GPIO_OUT);

    gpio_init(this->pin_rst);
    gpio_set_dir(this->pin_rst, GPIO_OUT);

    gpio_put(this->pin_cs,1);

    reset();

    uint8_t d;

    write_cmd(0xEF);

    write_cmd(0xEB);
    d = 0x14;
    write_data(&d,1);

    write_cmd(0xFE);
    write_cmd(0xEF);

    write_cmd(0xEB);
    d = 0x14;
    write_data(&d,1);

    write_cmd(0x84);
    d = 0x40;
    write_data(&d,1);

    write_cmd(0x85);
    d = 0xFF;
    write_data(&d,1);

    write_cmd(0x86);
    d = 0xFF;
    write_data(&d,1);

    write_cmd(0x87);
    d = 0xFF;
    write_data(&d,1);

    write_cmd(0x88);
    d = 0x0A;
    write_data(&d,1);

    write_cmd(0x89);
    d = 0x21;
    write_data(&d,1);

    write_cmd(0x8A);
    d = 0x00;
    write_data(&d,1);

    write_cmd(0x8B);
    d = 0x80;
    write_data(&d,1);

    write_cmd(0x8C);
    d = 0x01;
    write_data(&d,1);

    write_cmd(0x8D);
    d = 0x01;
    write_data(&d,1);

    write_cmd(0x8E);
    d = 0xFF;
    write_data(&d,1);

    write_cmd(0x8F);
    d = 0xFF;
    write_data(&d,1);

    write_cmd(0xB6);
    d = 0x00;
    write_data(&d,1);

    write_cmd(0x36);   // MADCTL
    d = 0x08;
    write_data(&d,1);

    write_cmd(0x3A);   // RGB565
    d = 0x05;
    write_data(&d,1);

    write_cmd(0x90);
    uint8_t data90[4] = {0x08, 0x08, 0x08, 0x08};
    write_data(data90,4);

    write_cmd(0xBD);
    d = 0x06;
    write_data(&d,1);

    write_cmd(0xBC);
    d = 0x00;
    write_data(&d,1);

    write_cmd(0xFF);
    uint8_t dataFF[3] = {0x60, 0x01, 0x04};
    write_data(dataFF,3);

    write_cmd(0xC3);
    d = 0x13;
    write_data(&d,1);

    write_cmd(0xC4);
    d = 0x13;
    write_data(&d,1);

    write_cmd(0xC9);
    d = 0x22;
    write_data(&d,1);

    write_cmd(0xBE);
    d = 0x11;
    write_data(&d,1);

    write_cmd(0xE1);
    uint8_t dataE1[2] = {0x10, 0x0E};
    write_data(dataE1,2);

    write_cmd(0xDF);
    uint8_t dataDF[3] = {0x21, 0x0c, 0x02};
    write_data(dataDF,3);

    write_cmd(0xF0);
    uint8_t dataF0[6] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
    write_data(dataF0,6);

    write_cmd(0xF1);
    uint8_t dataF1[6] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
    write_data(dataF1,6);

    write_cmd(0xF2);
    uint8_t dataF2[6] = {0x45, 0x09, 0x08, 0x08, 0x26, 0x2A};
    write_data(dataF2,6);

    write_cmd(0xF3);
    uint8_t dataF3[6] = {0x43, 0x70, 0x72, 0x36, 0x37, 0x6F};
    write_data(dataF3,6);

    write_cmd(0xED);
    uint8_t dataED[2] = {0x1B, 0x0B};
    write_data(dataED,2);

    write_cmd(0xAE);
    d = 0x77;
    write_data(&d,1);

    write_cmd(0xCD);
    d = 0x63;
    write_data(&d,1);

    write_cmd(0x70);
    uint8_t data70[9] = {0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03};
    write_data(data70,9);

    write_cmd(0xE8);
    d = 0x34;
    write_data(&d,1);

    write_cmd(0x62);
    uint8_t data62[12] = {0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70};
    write_data(data62,12);

    write_cmd(0x63);
    uint8_t data63[12] = {0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70};
    write_data(data63,12);

    write_cmd(0x64);
    uint8_t data64[7] = {0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07};
    write_data(data64,7);

    write_cmd(0x66);
    uint8_t data66[11] = {0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00};
    write_data(data66,11);

    write_cmd(0x67);
    uint8_t data67[10] = {0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98};
    write_data(data67,10);

    write_cmd(0x74);
    uint8_t data74[7] = {0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00};
    write_data(data74,7);

    write_cmd(0x98);
    uint8_t data98[2] = {0x3e, 0x07};
    write_data(data98,2);

    write_cmd(0x35);
    write_cmd(0x21);

    write_cmd(0x11);   // sleep out
    sleep_ms(120);

    write_cmd(0x29);   // display on
    sleep_ms(20);
}

void Screen::set_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    // Bounds check
    if(x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }

    this->framebuffer[y][x] = color;
}

void Screen::fill(uint16_t color)
{
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            set_pixel(j, i, color);
        }
    }
}

void Screen::fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            set_pixel(x + j, y + i, color);
        }
    }
}

void Screen::draw_circle(int x0, int y0, int r, uint16_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        set_pixel(x0 + x, y0 + y, color);
        set_pixel(x0 + y, y0 + x, color);
        set_pixel(x0 - y, y0 + x, color);
        set_pixel(x0 - x, y0 + y, color);
        set_pixel(x0 - x, y0 - y, color);
        set_pixel(x0 - y, y0 - x, color);
        set_pixel(x0 + y, y0 - x, color);
        set_pixel(x0 + x, y0 - y, color);

        y += 1;
        if (err <= 0)
        {
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void Screen::fill_circle(int x0, int y0, int r, uint16_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        // Draw horizontal lines for each octant
        fill_rect(x0 - x, y0 + y, 2*x+1, 1, color);
        fill_rect(x0 - x, y0 - y, 2*x+1, 1, color);
        fill_rect(x0 - y, y0 + x, 2*y+1, 1, color);
        fill_rect(x0 - y, y0 - x, 2*y+1, 1, color);

        y += 1;
        if (err <= 0)
        {
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

void Screen::fill_ellipse(int x0, int y0, int rx, int ry, uint16_t color)
{
    for (int y = -ry; y <= ry; y++) {
        int x_span = (int)(rx * std::sqrt(1 - (y*y) / (float)(ry*ry)));
        fill_rect(x0 - x_span, y0 + y, 2*x_span + 1, 1, color);
    }
}

void Screen::fill_arc(int x0, int y0, int r, int width, float start_angle, float end_angle, uint16_t color)
{
    // 0 will be the top and go CLOCKWISE - write in note where the clkockwise is defined

    for (float angle = start_angle; angle < end_angle; angle += 0.01) {
        int x_outer = x0 - (int)(r * std::sin(angle));
        int y_outer = y0 - (int)(r * std::cos(angle));
        int x_inner = x0 - (int)((r - width) * std::sin(angle));
        int y_inner = y0 - (int)((r - width) * std::cos(angle));

        // Draw line from inner to outer point
        int dx = x_outer - x_inner;
        int dy = y_outer - y_inner;
        int steps = std::max(std::abs(dx), std::abs(dy));
        for (int i = 0; i <= steps; i++) {
            int x = x_inner + i * dx / steps;
            int y = y_inner + i * dy / steps;
            set_pixel(x, y, color);
        }
    }  
}

void Screen::draw_digit(int x, int y, int digit, uint16_t color, int scale)
{
    if(digit < 0 || digit > 10) return; // 10 = %

    for(int row = 0; row < 7; row++)
    {
        for(int col = 0; col < 5; col++)
        {
            // No flip at all
            if(font_digits[digit][row][4 - col])
            {
                fill_rect(x + col*scale, y + row*scale, scale, scale, color);
            }
        }
    }
}

void Screen::draw_number(int x, int y, int number, uint16_t color, int scale)
{
    if(number < 0) number = 0;
    if(number > 999) number = 999;

    // Extract digits into array
    int digits[3] = {0,0,0};
    int num_digits = 0;
    int n = number;
    do {
        digits[num_digits++] = n % 10;
        n /= 10;
    } while(n > 0);

    // Draw digits left-to-right (most significant first)
    int cursor_x = x;
    for(int i = num_digits - 1; i >= 0; i--)
    {
        draw_digit(cursor_x, y, digits[i], color, scale);
        cursor_x -= 5*scale + scale; // scaled width + spacing
    }
}

// draw_percentage
void Screen::draw_percentage(int x, int y, int value, uint16_t color, int scale)
{
    draw_number(x, y, value, color, scale);

    // Count digits
    int num_digits = 0;
    int n = value;
    do {
        num_digits++;
        n /= 10;
    } while(n > 0);

    // Place % sign after last digit
    int percent_x = x - num_digits * (5*scale + scale);
    draw_digit(percent_x, y, 10, color, scale); // 10 = %
}

void Screen::update()
{
    set_window(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

    uint8_t buf[SCREEN_WIDTH * 2];
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            uint16_t p = framebuffer[i][j];
            buf[j*2]   = p >> 8;    // hi byte
            buf[j*2+1] = p & 0xFF;  // lo byte
        }
        write_data(buf, SCREEN_WIDTH * 2);
    }
}
