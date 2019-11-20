#ifndef __AF_LIGHT_H__
#define __AF_LIGHT_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AMath/math.hpp"
#include "color.hpp"

namespace AF {
    class light {
    private:
        color ambient;
        color diffuse;
        color specular;
    public:
        light() = default;

        void set_ambient(double r, double g, double b);
        void set_diffuse(double r, double g, double b);
        void set_specular(double r, double g, double b);

        void set_ambient(const color &v);
        void set_diffuse(const color &v);
        void set_specular(const color &v);

        color get_ambient() const noexcept;
        color get_diffuse() const noexcept;
        color get_specular() const noexcept;

        // static void add_light(const light *light_) noexcept;
    };

    class light_point : public light {
    private:
        vec3d position;
    public:
        light_point() = default;

        void set_position(double x, double y, double z);
        void set_position(const vec3d &v);
        vec3d get_position() const noexcept;
    };

    class light_tube : public light {
    private:
        vec3d beg;
        vec3d end;
    public:
        light_tube() = default;

        void set_beg(double x, double y, double z);
        void set_end(double x, double y, double z);

        void set_beg(const vec3d &v);
        void set_end(const vec3d &v);

        vec3d get_beg() const noexcept;
        vec3d get_end() const noexcept;

        // Reference : http://dx.doi.org/10.1109/38.124286
        // calculate coefficients for rendering this light.
        // @ coef[0] = <beg, beg>
        // @ coef[1] = 2<beg, end-beg>
        // @ coef[2] = <end-beg, end-beg>
        void get_diffuse_coef(double coef[3]);
    };
}

#endif