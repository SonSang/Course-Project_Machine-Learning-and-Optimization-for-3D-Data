#ifndef __AF_PROPERTY_RENDER_H__
#define __AF_PROPERTY_RENDER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "property.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "material.hpp"
#include <vector>
#include <functional>

#define USE_DISPLAY_LIST_DEFAULT false

namespace AF {
    // This property gives an object information how to render it.
    // User can modify selected object's apperance with this property.
    class property_render : public property {
    public:
        // Buffer objects.
        struct buffer_object {
            using uint = unsigned int;
            uint VAO;
            std::vector<uint> VBO;  // Multiple VBO can occur. e.g) Position, Normal, Color...
            std::vector<uint> EBO;  // Multiple EBO can occur. e.g) Face, Edge, ...
            std::vector<uint> EBO_size;  // # of indices.
        };
    private:
        std::string     name;
        bool	    	valid = true;
    protected:
        // Rendering options.
        buffer_object   BO;
        shader          SH;
        texture2D       TEX;
        material        MAT;
    public:
        property_render() = default;
        property_render(const std::string &name_);

        const std::string& get_name() const noexcept;
        void set_name(const std::string &name);
        bool is_valid() const noexcept;
        void set_valid(bool v);

        void set_texture2D(const texture2D &TEX);
        texture2D& get_texture2D() noexcept;
        const texture2D& get_texture2D_c() const noexcept;

        void set_material(const material &MAT);
        material& get_material() noexcept;
        const material& get_material_c() const noexcept;

        void build_shader(const std::string &vert_path, const std::string &frag_path);
        void set_shader(const shader &SH);
        shader& get_shader() noexcept;
        const shader& get_shader_c() const noexcept;

        void set_BO(const buffer_object &BO);
        buffer_object get_BO();
        
        virtual void build_BO();
        virtual void render() const noexcept;
        virtual void render_ui();
    };
}

#endif