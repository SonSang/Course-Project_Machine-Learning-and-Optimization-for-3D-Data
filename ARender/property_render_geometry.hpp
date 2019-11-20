#ifndef __AF_PROPERTY_RENDER_GEOMETRY_H__
#define __AF_PROPERTY_RENDER_GEOMETRY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "color.hpp"
#include "property_render.hpp"
#include "render_geometry.hpp"
#include "light.hpp"

#define GL_GLEXT_PROTOTYPES 1   // Needed to use Vertex Array Object.
#include <SDL2/SDL_opengles2.h>

namespace AF {
    // Simple rapper class for geometric data structures.
    // This class is tightly packed with shaders named [ render_geometry.glsl ].
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

        // Shader info
        // Since this class only uses [ render_geometry ] shader, we can hard code this kind of functions...
        void shader_set_material(const material &M) const { // Only 1 material is allowed.
            this->get_shader_c().enable();
            int 
                loc = get_shader_c().get_uniform_location("MM.ambient");
            glUniform3f(loc, (float)M.get_ambient()[0], (float)M.get_ambient()[1], (float)M.get_ambient()[2]);
            loc = get_shader_c().get_uniform_location("MM.emmision");
            glUniform3f(loc, (float)M.get_emmision()[0], (float)M.get_emmision()[1], (float)M.get_emmision()[2]);
            loc = get_shader_c().get_uniform_location("MM.diffuse");
            glUniform3f(loc, (float)M.get_diffuse()[0], (float)M.get_diffuse()[1], (float)M.get_diffuse()[2]);
            loc = get_shader_c().get_uniform_location("MM.specular");
            glUniform3f(loc, (float)M.get_specular()[0], (float)M.get_specular()[1], (float)M.get_specular()[2]);
            loc = get_shader_c().get_uniform_location("MM.shininess");
            glUniform1f(loc, (float)M.get_shininess());
            this->get_shader_c().disable();
        }  
        void shader_set_light_point(const light_point &L) const { // Only 1 point light is allowed.
            this->get_shader_c().enable();
            int 
                loc = get_shader_c().get_uniform_location("ML.ambient");
            glUniform3f(loc, (float)L.get_ambient()[0], (float)L.get_ambient()[1], (float)L.get_ambient()[2]);
            loc = get_shader_c().get_uniform_location("ML.diffuse");
            glUniform3f(loc, (float)L.get_diffuse()[0], (float)L.get_diffuse()[1], (float)L.get_diffuse()[2]);
            loc = get_shader_c().get_uniform_location("ML.specular");
            glUniform3f(loc, (float)L.get_specular()[0], (float)L.get_specular()[1], (float)L.get_specular()[2]);
            loc = get_shader_c().get_uniform_location("ML.position");
            glUniform3f(loc, (float)L.get_position()[0], (float)L.get_position()[1], (float)L.get_position()[2]);
            this->get_shader_c().disable();
        }   
    };
}
#endif