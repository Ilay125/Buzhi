#include "kinematics.h"
#include "config.h"
#include <cmath>

void forward_kin(double s1, double s2, double& x, double& y) {
    double s1_rad = s1 * PI / 180.0;
    double s2_rad = s2 * PI / 180.0;

    // Elbows
    double xL = -L0 + L * cos(s1_rad);
    double yL = L * sin(s1_rad);
    double xR = L0 + L * cos(s2_rad);
    double yR = L * sin(s2_rad);

    // Distance between elbows
    double dx = xR - xL;
    double dy = yR - yL;
    double d = sqrt(dx * dx + dy * dy);

    // midpoint along the line that connects elbows
    double a = d / 2;
    double h = sqrt(fmax(L * L - a * a, 0));

    double x0 = xL + a * dx / d;
    double y0 = yL + a * dy / d;

    // pick the lower intersection
    x = x0 - h * dy / d;
    y = y0 + h * dx / d;
}

int inverse_kin(double x, double y, double& s1, double& s2) {
    // distances from motors to end effector
    double d1 = sqrt((x + (L0/2))*(x + (L0/2)) + y*y);
    double d2 = sqrt((x - (L0/2))*(x - (L0/2)) + y*y);

    // reachable workspace check
    if (d1 > 2*L || d2 > 2*L) return 1;
    if (d1 < 1e-6 || d2 < 1e-6) return 1;

    // base angles
    double b1 = atan2(y, x + (L0/2)); //0
    double b2 = atan2(y, x - (L0/2)); //pi

    // law of cosines (isosceles triangle)
    double c1 = d1 / (2*L);
    double c2 = d2 / (2*L);

    if (fabs(c1) > 1.0 || fabs(c2) > 1.0) return 1;

    double a1 = acos(c1);
    double a2 = acos(c2);

    // choose ONE configuration (elbow-down)
    double theta1 = b1 - a1;
    double theta2 = b2 + a2;

    // radians → degrees
    s1 = theta1 * 180.0 / PI;
    s2 = theta2 * 180.0 / PI;

    return 0;
}