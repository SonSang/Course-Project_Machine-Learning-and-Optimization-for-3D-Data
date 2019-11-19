#include "color.hpp"
#include "../AMath/random.hpp"

namespace AF {
    color::color(double r, double g, double b, double a) {
        set(r, g, b, a);
    }
	void color::set(double r, double g, double b, double a) {
        (*this)[0] = r;
        (*this)[1] = g;
        (*this)[2] = b;
        (*this)[3] = a;
    }
	double color::get(int i) const {
        return (*this).at(i);
    }
	color color::random(bool opacity_random) {
        color ret(random::double_(0, 1), random::double_(0, 1), random::double_(0, 1), (opacity_random ? random::double_(0, 1) : 1.0));
		return ret;
    }
}