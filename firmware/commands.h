#ifndef __COMMANDS__
#define __COMMANDS__

#include "modules/motor.h"
#include "modules/servo.h"
#include <vector>

struct point_data {
    Dir dir;
    int steps;
    double speed;
    bool is_move;
};

class Command {
    public:
        virtual int draw(Motor &m1, Motor &m2, std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB) = 0;
        virtual void print_command() = 0;
        virtual ~Command() = default;
};

class Move: public Command {
    double x0;
    double y0;

    public:
        Move(double x0, double y0);
        int draw(Motor &m1, Motor &m2, std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB);
        void print_command();
};

class Line: public Command {
    double x0;
    double y0;
    double x1;
    double y1;

    public:
        Line(double x0, double y0, double x1, double y1);
        int draw(Motor &m1, Motor &m2, std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB);
        void print_command();
};

class Cubic: public Command {
    double x0;
    double y0;
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;

    public:
        Cubic(double x0, double y0, double x1, double y1,
             double x2, double y2, double x3, double y3);
        int draw(Motor &m1, Motor &m2, std::vector<point_data*>& pointsA, std::vector<point_data*>& pointsB);
        void print_command();
};

#endif