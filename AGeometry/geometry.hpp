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

    class box;

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

        box build_bounding_box() const;   // Build bounding box that encompasses all the points in this mesh.

        void scale(double size);            // Using bounding box, change scale of this mesh to [ size ].
        void scale_norm();                  // scale(1)

        virtual void compute_normals();             // Compute and set normals according to current vertex-face relationship.
        void reverse_normals();             // Reverse current normals.

        //void build_obj(const std::string &path);    // Build by [ .obj ] file.
        virtual void clear() noexcept;
    };

    // // General mesh
    // class mesh : public mesh3 {
    // public:
    //     using face4 = std::array<unsigned int, 4>;
    // protected:
    //     std::vector<face4> faces4;
    // public:
    //     void set_faces4(const std::vector<face4> &faces) noexcept;
    //     const std::vector<face4>& get_faces4_c() const noexcept;
    //     std::vector<face4>& get_faces4() noexcept;

    //     virtual void compute_normals();
    //     virtual void clear() noexcept;
    // };

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

        // @ccw : Assume vertices are defined in couter clockwise order.
        vec3d normal(bool ccw = true) const;
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

    // Box (AABB).
    class box : public geometry {
    private:
        vec3d min = {1e+10, 1e+10, 1e+10};
        vec3d max = {-1e+10, -1e+10, -1e+10};
    public:
        void set_min_x(double xmin);
        void set_min_y(double ymin);
        void set_min_z(double zmin);
        void set_max_x(double xmax);
        void set_max_y(double ymax);
        void set_max_z(double zmax);
        void insert(const vec3d &v);    // Update [ min ], [ max ] by a vector [ v ].

        double get_min_x() const noexcept;
        double get_min_y() const noexcept;
        double get_min_z() const noexcept;
        double get_max_x() const noexcept;
        double get_max_y() const noexcept;
        double get_max_z() const noexcept;

        double diagonal_length() const noexcept;    // distance between min, max.

        mesh3 build_mesh3() const noexcept;
    };
}

#endif