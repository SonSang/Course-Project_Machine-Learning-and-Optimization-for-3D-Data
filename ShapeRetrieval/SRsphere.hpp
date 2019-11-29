#ifndef __SR_SPHERE_H__
#define __SR_SPHERE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AGeometry/geometry.hpp"

namespace AF {
    class SRsphere : public sphere {
    public:
        // Does this sphere contain [ other ] sphere ?
        bool contain(const SRsphere &other);

        // Compute volume of this sphere.
        double volume() const noexcept;

        // Create a bigger sphere that encompasses both spheres [ a ] and [ b ].
        static SRsphere merge(const SRsphere &a, const SRsphere &b);

        // Do these spheres overlap ? (offset == 0)
        static bool overlap(const SRsphere &a, const SRsphere &b);

        // How far apart two spheres located ?
        static double offset(const SRsphere &a, const SRsphere &b);

        // Remove subset of this sphere that is also part of [ other ] sphere. 
        // It does not conduct precise computation : just approximate.
        void subtract(const SRsphere &other);
    };
}

#endif