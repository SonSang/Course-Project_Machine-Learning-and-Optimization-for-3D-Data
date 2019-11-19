#ifndef __AF_PROPERTY_RENDER_GEOMETRY_H__
#define __AF_PROPERTY_RENDER_GEOMETRY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "color.hpp"
#include "property_render.hpp"
#include "render_geometry.hpp"

namespace AF {
    // Simple rapper class for geometric data structures.
    template <typename T>
    class property_render_geometry : public property_render {
        static_assert(std::is_base_of<geometry, T>::value,
            "Geometry render property must contain a subclass object of [geometry] class.");
    public:
        struct config {
        public:
            enum mode {
                SIMPLE = 0,     // Draw face and edge in single specified color.
                WIREFRAME,      // Draw edge in single speficied color.
                PHONG           // Use phong shader.
            };
            mode M;     // How to render this geometry.

            // Data used for SIMPLE & WIREFRAME mode.
            color face_color;   
            color edge_color;   

            // Data used for PHONG mode.
            // Data used for choice between MATERIAL & TEXTURE during PHONG mode.
            bool use_material = true;
        };
    private:
        T		G;
        config 	C;
    public:
        property_render_geometry() = default;
        property_render_geometry(const std::string &name) : property_render(name) {

        }
        void			set_geometry(const T &geometry) {
            this->G = geometry;
        }
        T&				get_geometry() noexcept {
            return this->G;
        }
        const T&		get_geometry_c() const noexcept {
            return this->G;
        }
        config&			get_config() noexcept {
            return this->C;
        }
        const config&	get_config_c() const noexcept {
            return C;
        }
        virtual void    build_BO();
        virtual void	render() const noexcept;
        virtual void	render_ui();
    };
}
#endif