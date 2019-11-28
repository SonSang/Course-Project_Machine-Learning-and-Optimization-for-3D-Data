#ifndef __AF_OBJ_LOADER_H__
#define __AF_OBJ_LOADER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AMath/math.hpp"

#include <array>
#include <vector>
#include <string>
#include <set>

namespace AF {
    class obj_loader {
    public:
        class vertex {
        private:
            vec3d position;
            vec3d normal;
            vec2d texture;
        public:
            void set_position(const vec3d &v);
            void set_normal(const vec3d &n);
            void set_texture(const vec2d &t);

            vec3d& get_position() noexcept;
            vec3d& get_normal() noexcept;
            vec2d& get_texture() noexcept;

            const vec3d& get_position_c() const noexcept;
            const vec3d& get_normal_c() const noexcept;
        };

        class face {
        private:
            std::vector<vertex> vertexlist;
        public:
            void add_vertex(const vertex &v);
            vertex& get_vertex(size_t i);
            const vertex& get_vertex_c(size_t i) const;
            bool is_empty() const noexcept;
            size_t size() const noexcept;

            void set_normal();
        };

        class group {
        private:
            std::string name;
            std::vector<face> facelist;
        public:
            void set_name(const std::string &name);
            const std::string& get_name() const noexcept;

            void add_face(const face &f);
            face& get_face(size_t i);
            const face& get_face_c(size_t i) const;
            bool is_empty() const noexcept;
            size_t size() const noexcept;
        };

        class object {
        private:
            std::string name;
            std::vector<group> grouplist;
        public:
            void set_name(const std::string &name);
            const std::string& get_name() const noexcept;

            void add_group(const group &g);
            group& get_group(size_t i);
            const group& get_group_c(size_t i) const;
            bool is_empty() const noexcept;
            size_t size() const noexcept;
        };

    private:
        std::vector<object> objectlist;

        void start_object(std::string &line);
        void start_group(std::string &line);

        void parse_vertex(std::string &line, std::vector<vec3d> &vlist);
        void parse_normal(std::string &line, std::vector<vec3d> &nlist);
        void parse_texture(std::string &line, std::vector<vec2d> &tlist);

        void parse_face(std::string &line,
            const std::vector<vec3d> &vlist,
            const std::vector<vec3d> &nlist,
            const std::vector<vec2d> &tlist);

        std::set<std::set<int>> uface; // Used to prevent duplicate faces.
    public:
        obj_loader() = default;
        obj_loader(const std::string &filename);

        void load(const std::string &filename);

        void add_object(const object &o);
        object& get_object(size_t i);
        const object& get_object_c(size_t i) const;
                
        bool is_empty() const noexcept;
        size_t size() const noexcept;

        void get_vertex_list(std::vector<vertex> &vlist) const noexcept;
        void get_vertex_pos_list(std::vector<vec3d> &vlist) const noexcept;

        void clear();

        void set_normal();	// set normal vector of each face.
    };
}

#endif