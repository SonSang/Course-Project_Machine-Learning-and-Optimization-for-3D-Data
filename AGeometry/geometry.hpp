#ifndef __AF_GEOMETRY_H__
#define __AF_GEOMETRY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AMath/math.hpp"
#include <array>
#include <vector>

namespace AF {
    // Data structures to represent geometries.
    class geometry {
    };

    // Triangular mesh.
    class mesh3 : public geometry {
    public:
        using face = std::array<unsigned int, 3>;
    protected:
        std::vector<vec3d> vertices;
        std::vector<vec3d> normals;     // We sometimes need normal vectors for geometric computation. e.g) Smoothing
        std::vector<face> faces;
    public:
        mesh3() = default;
        
        void set_vertices(const std::vector<vec3d> &vertices) noexcept;
        void set_normals(const std::vector<vec3d> &normals) noexcept;
        void set_faces(const std::vector<face> &faces) noexcept;

        const std::vector<vec3d>& get_vertices_c() const noexcept;
        const std::vector<vec3d>& get_normals_c() const noexcept;
        const std::vector<face>& get_faces_c() const noexcept;

        std::vector<vec3d>& get_vertices() noexcept;
        std::vector<vec3d>& get_normals() noexcept;
        std::vector<face>& get_faces() noexcept;

        //void build_obj(const std::string &path);    // Build by [ .obj ] file.
        void clear() noexcept;
    };

    // Triangle.
    class triangle : public geometry {
    private:
        std::array<vec3d, 3> vertices;
    public:
        void set_vertices(const std::array<vec3d, 3> &vertices) noexcept;
        void set_vertices(const vec3d &a, const vec3d &b, const vec3d &c) noexcept;
        void set_vertex(int i, const vec3d &v);

        const std::array<vec3d, 3>& get_vertices_c() const noexcept;
        std::array<vec3d, 3>& get_vertices() noexcept;
        const vec3d& get_vertex_c(int i) const;
        vec3d& get_vertex(int i);
    };

    // Sphere.
    class sphere : public geometry {
    private:
        vec3d center;
        double radius;
    public:
        void set_center(const vec3d &C);
        vec3d& get_center() noexcept;
        const vec3d& get_center() const noexcept;

        void set_radius(double radius);
        double get_radius() const noexcept;
        
        mesh3 build_mesh3() const noexcept;
    };
}

#endif