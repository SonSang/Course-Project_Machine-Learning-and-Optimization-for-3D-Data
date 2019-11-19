#ifndef __AF_RENDER_GEOMETRY_H__
#define __AF_RENDER_GEOMETRY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AMath/math.hpp"
#include "../AGeometry/geometry.hpp"

namespace AF {
    class rmesh3 : public mesh3 {
    private:
        std::vector<vec2d> textures;    // Texture coordinates for each vertices.
    public:
        void set_textures(const std::vector<vec2d> &textures) noexcept;
        const std::vector<vec2d>& get_textures_c() const noexcept;
        std::vector<vec2d>& get_textures() noexcept;

        void build_obj(const std::string &path);    // Build by [ .obj ] file.
        void clear() noexcept;
    };
}

#endif