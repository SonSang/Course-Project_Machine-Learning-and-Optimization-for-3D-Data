#include "geometry.hpp"
#include <iostream>

namespace AF {
    // mesh3
    void mesh3::set_vertices(const std::vector<vec3d> &vertices) noexcept {
        this->vertices = vertices;
    }
    void mesh3::set_normals(const std::vector<vec3d> &normals) noexcept {
        this->normals = normals;
    }
    void mesh3::set_faces(const std::vector<face> &faces) noexcept {
        this->faces = faces;
    }

    const std::vector<vec3d>& mesh3::get_vertices_c() const noexcept {
        return vertices;
    }
    const std::vector<vec3d>& mesh3::get_normals_c() const noexcept {
        return normals;
    }
    const std::vector<mesh3::face>& mesh3::get_faces_c() const noexcept {
        return faces;
    }

    std::vector<vec3d>& mesh3::get_vertices() noexcept {
        return vertices;
    }
    std::vector<vec3d>& mesh3::get_normals() noexcept {
        return normals;
    }
    std::vector<mesh3::face>& mesh3::get_faces() noexcept {
        return faces;
    }
    void mesh3::clear() noexcept {
        this->vertices.clear();
        this->normals.clear();
        this->faces.clear();
    }

    // Triangle
    void triangle::set_vertices(const std::array<vec3d, 3> &vertices) noexcept {
        this->vertices = vertices;
    }
    void triangle::set_vertices(const vec3d &a, const vec3d &b, const vec3d &c) noexcept {
        vertices[0] = a;
        vertices[1] = b;
        vertices[2] = c;
    }
    void triangle::set_vertex(int i, const vec3d &v) {
        vertices.at(i) = v;
    }
    const std::array<vec3d, 3>& triangle::get_vertices_c() const noexcept {
        return vertices;
    }
    std::array<vec3d, 3>& triangle::get_vertices() noexcept {
        return vertices;
    }
    const vec3d& triangle::get_vertex_c(int i) const {
        return vertices.at(i);
    }
    vec3d& triangle::get_vertex(int i) {
        return vertices.at(i);
    }

    // Sphere
    void sphere::set_center(const vec3d &C) {
        this->center = C;
    }
    vec3d& sphere::get_center() noexcept {
        return this->center;
    }
    const vec3d& sphere::get_center() const noexcept {
        return this->center;
    }

    void sphere::set_radius(double radius) {
        this->radius = radius;
    }
    double sphere::get_radius() const noexcept {
        return this->radius;
    }
    // Build sphere mesh from a single triangle, which is itself rough approximation of a side of a sphere.
    static void build_sphere_mesh_triangle(const triangle &base, std::vector<triangle> &result, int degree) {
        if(degree == 0)
            return;
        triangle t0, t1, t2, t3;
        vec3d
            mid01 = (base.get_vertex_c(0) + base.get_vertex_c(1)) * 0.5, 
            mid12 = (base.get_vertex_c(1) + base.get_vertex_c(2)) * 0.5, 
            mid20 = (base.get_vertex_c(2) + base.get_vertex_c(0)) * 0.5;
        t0.set_vertices(base.get_vertex_c(0), mid01, mid20);
        t1.set_vertices(base.get_vertex_c(1), mid12, mid01);
        t2.set_vertices(mid01, mid12, mid20);
        t3.set_vertices(base.get_vertex_c(2), mid20, mid12);
        if(degree-- == 1) {
            result.push_back(t0);
            result.push_back(t1);
            result.push_back(t2);
            result.push_back(t3);
        }
        build_sphere_mesh_triangle(t0, result, degree);
        build_sphere_mesh_triangle(t1, result, degree);
        build_sphere_mesh_triangle(t2, result, degree);
        build_sphere_mesh_triangle(t3, result, degree);
    }
    mesh3 sphere::build_mesh3() const noexcept {
        int 
            degree = 4;
        vec3d
            xrad(radius, 0, 0),
            yrad(0, radius, 0),
            zrad(0, 0, radius),
            npole = center + yrad,
            spole = center - yrad,
            waist0 = center + zrad,
            waist1 = center + xrad,
            waist2 = center - zrad,
            waist3 = center - xrad;
        triangle
            base[8];
        base[0].set_vertices(npole, waist0, waist1);
        base[1].set_vertices(npole, waist1, waist2);
        base[2].set_vertices(npole, waist2, waist3);
        base[3].set_vertices(npole, waist3, waist0);
        base[4].set_vertices(spole, waist1, waist0);
        base[5].set_vertices(spole, waist2, waist1);
        base[6].set_vertices(spole, waist3, waist2);
        base[7].set_vertices(spole, waist0, waist3);
        std::vector<triangle>
            Ts;
        for(int i = 0; i < 8; i++) 
            build_sphere_mesh_triangle(base[i], Ts, degree);
        for(auto it = Ts.begin(); it != Ts.end(); it++) {
            triangle &T = *it;
            vec3d V = T.get_vertex(0) - center;
            V.normalize();
            T.set_vertex(0, V * radius + center);

            V = T.get_vertex(1) - center;
            V.normalize();
            T.set_vertex(1, V * radius + center);

            V = T.get_vertex(2) - center;
            V.normalize();
            T.set_vertex(2, V * radius + center);
        }

        mesh3 ret;
        int id = 0;
        for(auto it = Ts.begin(); it != Ts.end(); it++) {
            mesh3::face nf;
            ret.get_vertices().push_back(it->get_vertex(0));
            ret.get_normals().push_back(it->get_vertex(0) - center);
            nf[0] = id++;

            ret.get_vertices().push_back(it->get_vertex(1));
            ret.get_normals().push_back(it->get_vertex(1) - center);
            nf[1] = id++;

            ret.get_vertices().push_back(it->get_vertex(2));
            ret.get_normals().push_back(it->get_vertex(2) - center);
            nf[2] = id++;

            ret.get_faces().push_back(nf);
        }

        return ret;
    }
}