#ifndef __AF_SHADER_H__
#define __AF_SHADER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AMath/math.hpp"
#include <string>

namespace AF {
    class shader {
    private:
        using uint = unsigned int;
        uint program;
        uint vertex_shader;
        uint fragment_shader;

    public:
        shader() = default;
        shader(const std::string &vert_filename, const std::string &frag_filename);
        ~shader();

        static uint create_vertex_shader(const std::string &filename);
        static uint create_fragment_shader(const std::string &filename);

        void set_vertex_shader(const std::string &filename);
        void set_fragment_shader(const std::string &filename);
        void set_program(const std::string &vert_filename, const std::string &frag_filename);

        uint get_program() const noexcept;
        uint get_vertex_shader() const noexcept;
        uint get_fragment_shader() const noexcept;

        void set_uniform_model_matrix(const transform &TR) const;
        void set_uniform_view_matrix(const transform &TR) const;
        void set_uniform_view_matrix(const mat4d &M) const;
        void set_uniform_projection_matrix(const mat4d &M) const;

        int get_attribute_location(const std::string &varname) const;
        int get_uniform_location(const std::string &varname) const;

        void enable() const;
        static void disable();
    };
}

#endif