#include "timer.hpp"

#include <iostream>
#include <fstream>
#include <chrono>

std::string name;
std::chrono::system_clock::time_point start;
std::chrono::system_clock::time_point end;

namespace AF {
    void timer::start(const std::string &n) {
        name = n;
        ::start = std::chrono::system_clock::now();
    }
    void timer::end() {
        ::end = std::chrono::system_clock::now();
    }
    void timer::print() {
        std::chrono::duration<double> sec = ::end - ::start;
        std::cout << "[" << name << "] : " << sec.count() << "(sec), " << sec.count() * 1e+3 << "(msec)" << std::endl;
    }
    void timer::end_print() {
        end();
        print();
    }

    double timer::get_time() {
        auto now = std::chrono::system_clock::now();
        return (double)(now.time_since_epoch().count()) * 1e+9;
    }

    double timer::get_elapsed_time() {
        std::chrono::duration<double> sec = ::end - ::start;
        return sec.count();
        //return (::end) - (::start);
    }

    // stat_time
    void stat_timer::clear_time() {
        times.clear();
    }
    void stat_timer::add_time(double t) {
        times.push_back(t);
    }
    void stat_timer::save_time(const std::string &filename) {
        std::ofstream out(filename);
        if (out.is_open()) {
            for (auto it = times.begin(); it != times.end(); it++) {
                out << *it;
                out << std::endl;
            }
        }
        out.close();
    }
    int stat_timer::get_size() {
        return (int)times.size();
    }
    void stat_timer::start() {
        this->tbeg = timer::get_time();
    }
    void stat_timer::end() {
        this->tend = timer::get_time();
    }
    void stat_timer::add_elapsed_time() {
        add_time(get_elapsed_time());
    }
    double stat_timer::get_elapsed_time() {
        return this->tend - this->tbeg;
    }
}