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
        color diffusion;
        color specular;
        double shininess;
    public:
        
    };
}

#endif