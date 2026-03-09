#include "commandQ.h"

#include "pico/stdlib.h"
#include <cstdio>

CommandQueue::CommandQueue() {
    this->size = 0;
}

void CommandQueue::add_command(Command* c) {
    this->queue.push(c);
    this->size++;
}

void CommandQueue::gen_draw_vec(Motor &m1, Motor &m2, 
                                std::vector<point_data*>& pointsA, 
                                std::vector<point_data*>& pointsB) {
    while (!this->queue.empty()) {
        Command* c = queue.front();
        queue.pop();
        c->print_command();
        c->draw(m1, m2, pointsA, pointsB);
        delete c;
        //sleep_ms(200);
    }
}

void CommandQueue::print_commands() {
    std::queue<Command*> temp_queue = this->queue;
    while (!temp_queue.empty()) {
        Command* c = temp_queue.front();
        temp_queue.pop();
        c->print_command();
    }
}


void CommandQueue::parse(std::vector<uint8_t> data, int size) {
    int idx = 0;
    double start_x = 0;
    double start_y = 0;
    double curr_x = 0;
    double curr_y = 0;

    while (idx < size) {
        Command* c = nullptr;

        switch(data.at(idx)) {
            case 0: {
                //std::cout << "success" << std::endl;
                return;
            }

            case 'M': {
                start_x = data.at(idx + 1) / 10.0;
                start_y = data.at(idx + 2) / 10.0;

                curr_x = start_x;
                curr_y = start_y;

                c = new Move(start_x, start_y);

                idx += 3;
                break;
            }

            case 'L': {
                double x1 = data.at(idx + 1) / 10.0;
                double y1 = data.at(idx + 2) / 10.0;

                c = new Line(curr_x, curr_y, x1, y1);

                curr_x = x1;
                curr_y = y1;

                idx += 3;
                break;
            }

            case 'C': {
                double x1 = data.at(idx + 1) / 10.0;
                double y1 = data.at(idx + 2) / 10.0;
                double x2 = data.at(idx + 3) / 10.0;
                double y2 = data.at(idx + 4) / 10.0;
                double x3 = data.at(idx + 5) / 10.0;
                double y3 = data.at(idx + 6) / 10.0;
                c = new Cubic(curr_x, curr_y, x1, y1, x2, y2, x3, y3);

                curr_x = x3;
                curr_y = y3;

                idx += 7;
                break;
            }
            case 'Z': {
                c = new Line(curr_x, curr_y, start_x, start_y);

                idx += 1;
                break;
            }

            default:
                printf("Unknown command: %c at index %d\n", data.at(idx), idx);
                return; 

        }

        if (c) {
            this->add_command(c);
        }
          
    }

}