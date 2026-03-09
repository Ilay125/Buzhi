#ifndef __COMMANDQ__
#define __COMMANDQ__

#include <queue>
#include "commands.h"
#include "modules/motor.h"
#include "modules/servo.h"
#include <vector>
#include "pico/stdlib.h"


class CommandQueue {
    

    public:
    //fix this later
        std::queue<Command*> queue;
        int size;

        CommandQueue();
        void add_command(Command* c);
        void parse(std::vector<uint8_t> data, int size); // replace char with uint8_t
        void gen_draw_vec(Motor &m1, Motor &m2,
                          std::vector<point_data*>& pointsA, 
                          std::vector<point_data*>& pointsB);
        void print_commands();

};

#endif