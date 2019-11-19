#ifndef __AF_COLOR_H__
#define __AF_COLOR_H__

#ifdef _MSC_VER
#pragma once
#endif

#include <array>

namespace AF {
    class color : public std::array<double, 4> {
	public:
		color() = default;
		color(double r, double g, double b, double a = 1.0);
	
    	void set(double r, double g, double b, double a = 1.0);
		double get(int i) const;
		static color random(bool opacity_random = false);
	};
}

#endif