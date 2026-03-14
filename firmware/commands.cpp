#include "commands.h"
#include <cmath>
#include "pico/stdlib.h"
#include "config.h"
#include "modules/motor.h"
#include "kinematics.h"
#include <cstdio>
#include "modules/servo.h"
#include <vector>

double clampd(double v, double lo, double hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

double velocity_profile(double curr_t, double max_speed) {
    double real_max_speed = max_speed / (1 - 0.5*(T1 + T2));
    double start_slope = real_max_speed / T1;
    double end_slope = real_max_speed / (T2 - 1.0);
    //printf("curr t=%f => ", curr_t);
    if (curr_t < T1) {
        //printf("start speed = %f\n", start_slope * curr_t);
        return start_slope * curr_t;
    }

    if (curr_t > T2) {
        //printf("end speed = %f\n", real_max_speed + (curr_t - T2) * end_slope);
        return real_max_speed + (curr_t - T2) * end_slope;
    }

    //printf("max speed = %f\n", real_max_speed);
    return real_max_speed;
}

int move_to_xy(
    Motor &m1, Motor &m2,
    double x, double y,
    std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB,
    double curr_t, bool is_move = false) {

    printf("Moving to (%.3f, %.3f)\n", x, y);

    double s1, s2;
    int res = inverse_kin(x, y, s1, s2);
    if (res != 0) return res;

    double deltaA = s1 - m1.get_angle();
    double deltaB = s2 - m2.get_angle();

    if (!std::isfinite(deltaA) || !std::isfinite(deltaB)) {
        printf("BAD DELTA (NaN/inf)\n");
        return 1; // or handle
    }

    int stepsA = (int)std::round(fabs(deltaA / STEP_TO_DEG));
    int stepsB = (int)std::round(fabs(deltaB / STEP_TO_DEG));

    Dir dirA = (deltaA < 0) ? counterclockwise : clockwise;
    Dir dirB = (deltaB < 0) ? counterclockwise : clockwise;

    double speedA, speedB;

    if (fabs(deltaA) > fabs(deltaB)) {
        speedA = MAX_SPEED;
        speedB = speedA * (fabs(deltaB) / fabs(deltaA));
    } else {
        speedB = MAX_SPEED;
        speedA = speedB * (fabs(deltaA) / fabs(deltaB));
    }

    double adj_speedA = speedA;
    double adj_speedB = speedB;

    
    if(!is_move) {
        adj_speedA = velocity_profile(curr_t, speedA);
        adj_speedB = velocity_profile(curr_t, speedB);
    }

    //double r = std::sqrt(x * x + y * y);
    double r = y;

    int servo_angle = SERVO_UP_ANGLE;

    if (!is_move) {
        if (r < R_LOW) {
            servo_angle = SERVO_DOWN_MIN_ANGLE;
        } else if (r > R_HIGH) {
            servo_angle = SERVO_DOWN_MAX_ANGLE;
        } else {
            servo_angle = SERVO_DOWN_MID_ANGLE;
        }
    }

    point_data* pointA = new point_data{dirA, stepsA, speedA, is_move, servo_angle};
    point_data* pointB = new point_data{dirB, stepsB, speedB, is_move, servo_angle};

    //printf("dataA : steps=%d, dir=%d, speed=%f\n", pointA->steps, pointA->dir, pointA->speed);
    //printf("dataB : steps=%d, dir=%d, speed=%f\n", pointB->steps, pointB->dir, pointB->speed);

    pointsA.push_back(pointA);
    pointsB.push_back(pointB);

    // Update motor angles so next call gets current position
    m1.add_steps(stepsA, dirA);
    m2.add_steps(stepsB, dirB);

    return 0;
}


Move::Move(double x0, double y0) {
    this->x0 = x0 * X_SCALE + X_OFFSET;
    this->y0 = y0 * Y_SCALE + Y_OFFSET;
}

int Move::draw(Motor &m1, Motor &m2, std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB) {
    int res = move_to_xy(m1, m2, this->x0, this->y0, pointsA, pointsB, 0, true);
    return res;
}

void Move::print_command() {
    //std::cout << "Move: (" << this->x0 << ", " << this->y0 << ")" << std::endl;
    printf("Move: (%f, %f)\n", this->x0, this->y0);
}


Line::Line(double x0, double y0, double x1, double y1) {
    this->x0 = x0 * X_SCALE + X_OFFSET;
    this->y0 = y0 * Y_SCALE + Y_OFFSET;
    this->x1 = x1 * X_SCALE + X_OFFSET;
    this->y1 = y1 * Y_SCALE + Y_OFFSET;
}

/*
Line::Line(double x0, double y0, double x1, double y1) {
    this->x0 = x0;
    this->y0 = y0;
    this->x1 = x1;
    this->y1 = y1;
}
*/


int Line::draw(Motor &m1, Motor &m2, std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB) {

    const double dx = this->x1 - this->x0;
    const double dy = this->y1 - this->y0;
    const double dist = std::sqrt(dx*dx + dy*dy);

    // If it's basically a point, just go there once
    if (dist < 1e-9) {
        int res = move_to_xy(m1, m2, this->x1, this->y1, pointsA, pointsB, 0, true);
        return res;
    }

    // Use ceil so we never get 0 steps
    const int steps = (int)std::ceil(dist / MICROSTEP);
    const double step_x = dx / steps;
    const double step_y = dy / steps;

    double x = this->x0;
    double y = this->y0;

    for (int i = 1; i <= steps; i++) {
        x = this->x0 + step_x * i;
        y = this->y0 + step_y * i;

        int res = move_to_xy(m1, m2, x, y, pointsA, pointsB, (double)i / steps, false);
        if (res != 0) {
            printf("Inverse kinematics error in Line command.\n");
            return res;
        }
    }

    return 0;
}


void Line::print_command() {
    printf("line: (%f, %f) -> (%f, %f)\n", this->x0, this->y0, this->x1, this->y1);
}

Cubic::Cubic(double x0, double y0, double x1, double y1,
             double x2, double y2, double x3, double y3) {
    this->x0 = x0 * X_SCALE + X_OFFSET;
    this->y0 = y0 * Y_SCALE + Y_OFFSET;
    this->x1 = x1 * X_SCALE + X_OFFSET;
    this->y1 = y1 * Y_SCALE + Y_OFFSET;
    this->x2 = x2 * X_SCALE + X_OFFSET;
    this->y2 = y2 * Y_SCALE + Y_OFFSET;
    this->x3 = x3 * X_SCALE + X_OFFSET;
    this->y3 = y3 * Y_SCALE + Y_OFFSET;
}

int Cubic::draw(Motor &m1, Motor &m2, std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB) {
    // If all control points are practically in the same place, just move there once.
    const double epsilon = 1e-6;
    if (std::abs(this->x0 - this->x1) < epsilon && std::abs(this->y0 - this->y1) < epsilon &&
        std::abs(this->x1 - this->x2) < epsilon && std::abs(this->y1 - this->y2) < epsilon &&
        std::abs(this->x2 - this->x3) < epsilon && std::abs(this->y2 - this->y3) < epsilon) {
        
        int res = move_to_xy(m1, m2, this->x3, this->y3, pointsA, pointsB, 1.0, false);
        return res;
    }

    auto bezier = [this](double t, double &x, double &y) {
        double u  = 1.0 - t;
        double tt = t * t;
        double uu = u * u;

        x = (uu*u) * this->x0 + (3.0*uu*t) * this->x1 + (3.0*u*tt) * this->x2 + (tt*t) * this->x3;
        y = (uu*u) * this->y0 + (3.0*uu*t) * this->y1 + (3.0*u*tt) * this->y2 + (tt*t) * this->y3;
    };

    // B'(t) for cubic Bezier:
    auto bezier_deriv = [this](double t, double &dx, double &dy) {
        double u = 1.0 - t;

        dx = 3.0*u*u*(this->x1 - this->x0)
           + 6.0*u*t*(this->x2 - this->x1)
           + 3.0*t*t*(this->x3 - this->x2);

        dy = 3.0*u*u*(this->y1 - this->y0)
           + 6.0*u*t*(this->y2 - this->y1)
           + 3.0*t*t*(this->y3 - this->y2);
    };

    const double target_ds = MICROSTEP;
    const double t_min = 1e-5;
    const double t_max = 0.05;

    double t = 0.0;
    while (t < 1.0) {
        double dxdt, dydt;
        bezier_deriv(t, dxdt, dydt);
        double speed = std::sqrt(dxdt*dxdt + dydt*dydt);

        // If derivative is tiny, force a small step in t
        double dt = (speed > 1e-9) ? (target_ds / speed) : t_min;
        dt = clampd(dt, t_min, t_max);

        double t_next = t + dt;
        if (t_next > 1.0) t_next = 1.0;

        double x, y;
        bezier(t_next, x, y);

        int res = move_to_xy(m1, m2, x, y, pointsA, pointsB, t_next, false);
        if (res != 0) {
            printf("Inverse kinematics error in Cubic command at t=%.5f (x=%.3f,y=%.3f)\n", t_next, x, y);
            return res;
        }

        t = t_next;

    }

    // Snap to exact end (safe)
    {
        int res = move_to_xy(m1, m2, this->x3, this->y3, pointsA, pointsB, 1.0, false);
        if (res != 0) {
            printf("IK error Cubic end\n");
            return res;
        }
    }

    return 0;
}


void Cubic::print_command() {
    printf("Cubic: p0=(%f, %f) p1=(%f, %f) p2=(%f, %f) p3=(%f, %f)\n", this->x0, this->y0, this->x1, this->y1, this->x2, this->y2, this->x3, this->y3);
}