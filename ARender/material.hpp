#ifndef __AF_MATERIAL_H__
#define __AF_MATERIAL_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "color.hpp"

namespace AF {
    class material {
    private:
        color emmision;
        color ambient;
        color diffuse;
        color specular;
        double shininess;
    public:
        void set_emmision(const color &c);
        void set_ambient(const color &c);
        void set_diffuse(const color &c);
        void set_specular(const color &c);
        void set_shininess(double sh);

        color get_emmision() const noexcept;
        color get_ambient() const noexcept;
        color get_diffuse() const noexcept;
        color get_specular() const noexcept;
        double get_shininess() const noexcept;
    };
}

#endif