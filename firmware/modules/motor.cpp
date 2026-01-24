#include "motor.h"


Motor::Motor(int step_pin, int dir_pin, double angle0) {
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->angle0 = angle0;
}

int Motor::get_steps() {
    return this->steps;
}

double Motor::get_angle() {
    return this->angle0 + this->steps / STEP_TO_DEG;
} 

int Motor::get_dir_pin() {
    return this->dir_pin;
}

void Motor::set_steps(int steps) {
    this->steps = steps;
}

void Motor::add_steps(int steps, Dir dir) {
    this->steps += steps * dir;
}

double velocity_profile(double curr_t, double max_t, double max_speed) {
    double start_slope = max_speed / (T1*max_t);
    double end_slope = max_speed / (T2*max_t - max_t);

    if (curr_t < T1*max_t) {
        return start_slope * curr_t;
    }

    if (curr_t > T2*max_t) {
        return max_speed + (curr_t - T2*max_t) * end_slope;
    }

    return max_speed;
}

bool pio_done(PIO pio, uint sm) {
    return pio_sm_is_tx_fifo_empty(pio, sm) && pio_sm_is_exec_stalled(pio, sm);
}

void move_motors(Motor &m1, Motor &m2, double s1, double s2,
                double dist, PIO pio, uint sA, uint sB) {

    // Time both motor will finih their motion
    double t_total = dist / SPEED;

    double delta_t1 = fabs(s1 - m1.get_angle());
    double delta_t2 = fabs(s2 - m2.get_angle());

 

    Dir dir1 = s1 - m1.get_angle() < 0 ? clockwise :counterclockwise; // Right motor
    Dir dir2 = s2 - m2.get_angle() < 0 ? clockwise :counterclockwise; // Left motor

    gpio_put(m1.get_dir_pin(), dir1 == clockwise ? 1 : 0);
    gpio_put(m2.get_dir_pin(), dir2 == clockwise ? 1 : 0);

    double const_v1 = delta_t1 / t_total;
    double const_v2 = delta_t2 / t_total;

    // The ratio beteen the avg "const" to max speed.
    double ratio_avg_max =  2  / (1 + T2 - T1);

    double step_t = t_total / T_RES;
    double freq = clock_get_hz(clk_sys) / PIO_CLKDIV;

    for (double t = 0; t < t_total; t+=step_t) {
        
        double v1 = velocity_profile(t, t_total, ratio_avg_max*const_v1);
        double v2 = velocity_profile(t, t_total, ratio_avg_max*const_v2);

        int p1 = round(fabs(v1 * step_t / STEP_TO_DEG));
        int p2 = round(fabs(v2 * step_t / STEP_TO_DEG));

        // Check if queue is empty before running next
        while (!pio_done(pio, sA) || !pio_done(pio, sB)) {
            tight_loop_contents();
        }
        
        if (p1 > 0 && v1 > 1e-6) {
            int f1 = round((freq * STEP_TO_DEG) / (2.0 * v1));
            pio_sm_put_blocking(pio, sA, p1);
            pio_sm_put_blocking(pio, sA, f1);
        }

        if (p2 > 0 && v2 > 1e-6) {
            int f2 = round((freq * STEP_TO_DEG) / (2.0 * v2));
            pio_sm_put_blocking(pio, sB, p2);
            pio_sm_put_blocking(pio, sB, f2);
        }
    }
}