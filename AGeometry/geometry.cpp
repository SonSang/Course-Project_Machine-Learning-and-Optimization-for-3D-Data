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

    std::set<vec3d> mesh3::get_vertex_set() const noexcept {
        std::set<vec3d> ret;
        for(auto it = vertices.begin(); it != vertices.end(); it++) 
            ret.insert(*it);
        return ret;
    }
    
    box mesh3::build_bounding_box() const {
        box ret;
        for(auto it = vertices.begin(); it != vertices.end(); it++)
            ret.insert(*it);
        return ret;
    }
    void mesh3::scale(double size) {
        box BB = build_bounding_box();
        double DL = BB.diagonal_length();
        double ratio = size / DL;
        for(auto it = vertices.begin(); it != vertices.end(); it++)
            *it = (*it) * ratio;
    }
    void mesh3::scale_norm() {
        scale(1);
    }

    void mesh3::compute_normals() {
        normals.clear();
        normals.resize(vertices.size());
        for(auto it = faces.begin(); it != faces.end(); it++) {
            const vec3d 
                &a = vertices.at((*it)[0]),
                &b = vertices.at((*it)[1]),
                &c = vertices.at((*it)[2]);
            triangle T;
            T.set_vertices(a, b, c);
            vec3d
                N = T.normal();
            normals.at((*it)[0]) = N;
            normals.at((*it)[1]) = N;
            normals.at((*it)[2]) = N;
        }
    }

    void mesh3::reverse_normals() {
        for(auto it = normals.begin(); it != normals.end(); it++) {
            *it = (*it) * -1;
        }
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
    vec3d triangle::normal(bool ccw) const {
        vec3d
            a_b = vertices[0] - vertices[1],
            c_b = vertices[2] - vertices[1],
            norm = (ccw ? c_b.cross(a_b) : a_b.cross(c_b));
        norm.normalize();
        return norm;
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
            degree = 3;
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

    // box
    void box::set_min_x(double xmin) {
        min[0] = xmin;
    }
    void box::set_min_y(double ymin) {
        min[1] = ymin;
    }
    void box::set_min_z(double zmin) {
        min[2] = zmin;
    }
    void box::set_max_x(double xmax) {
        max[0] = xmax;
    }
    void box::set_max_y(double ymax) {
        max[1] = ymax;
    }
    void box::set_max_z(double zmax) {
        max[2] = zmax;
    }
    void box::insert(const vec3d &v) {
        if(v[0] < min[0]) set_min_x(v[0]);
        if(v[1] < min[1]) set_min_y(v[1]);
        if(v[2] < min[2]) set_min_z(v[2]);
        
        if(v[0] > max[0]) set_max_x(v[0]);
        if(v[1] > max[1]) set_max_y(v[1]);
        if(v[2] > max[2]) set_max_z(v[2]);
    }

    double box::get_min_x() const noexcept {
        return min[0];
    }
    double box::get_min_y() const noexcept {
        return min[1];
    }
    double box::get_min_z() const noexcept {
        return min[2];
    }
    double box::get_max_x() const noexcept {
        return max[0];
    }
    double box::get_max_y() const noexcept {
        return max[1];
    }
    double box::get_max_z() const noexcept {
        return max[2];
    }
    double box::diagonal_length() const noexcept {
        return (max - min).len();
    }
    mesh3 box::build_mesh3() const noexcept {
        mesh3 ret;

        std::vector<vec3d> &vertices = ret.get_vertices();
        std::vector<vec3d> &normals = ret.get_normals();
        // 0, 1, 2
        vertices.push_back(vec3d(min[0], min[1], min[2]));
        normals.push_back(vec3d(0, -1, 0));
        vertices.push_back(vec3d(min[0], min[1], min[2]));
        normals.push_back(vec3d(-1, 0, 0));
        vertices.push_back(vec3d(min[0], min[1], min[2]));
        normals.push_back(vec3d(0, 0, -1));

        // 3, 4, 5
        vertices.push_back(vec3d(max[0], min[1], min[2]));
        normals.push_back(vec3d(0, -1, 0));
        vertices.push_back(vec3d(max[0], min[1], min[2]));
        normals.push_back(vec3d(1, 0, 0));
        vertices.push_back(vec3d(max[0], min[1], min[2]));
        normals.push_back(vec3d(0, 0, -1));

        // 6, 7, 8
        vertices.push_back(vec3d(max[0], min[1], max[2]));
        normals.push_back(vec3d(0, -1, 0));
        vertices.push_back(vec3d(max[0], min[1], max[2]));
        normals.push_back(vec3d(1, 0, 0));
        vertices.push_back(vec3d(max[0], min[1], max[2]));
        normals.push_back(vec3d(0, 0, 1));

        // 9, 10, 11
        vertices.push_back(vec3d(min[0], min[1], max[2]));
        normals.push_back(vec3d(0, -1, 0));
        vertices.push_back(vec3d(min[0], min[1], max[2]));
        normals.push_back(vec3d(-1, 0, 0));
        vertices.push_back(vec3d(min[0], min[1], max[2]));
        normals.push_back(vec3d(0, 0, 1));

        // 12, 13, 14
        vertices.push_back(vec3d(min[0], max[1], min[2]));
        normals.push_back(vec3d(0, 1, 0));
        vertices.push_back(vec3d(min[0], max[1], min[2]));
        normals.push_back(vec3d(-1, 0, 0));
        vertices.push_back(vec3d(min[0], max[1], min[2]));
        normals.push_back(vec3d(0, 0, -1));

        // 15, 16, 17
        vertices.push_back(vec3d(max[0], max[1], min[2]));
        normals.push_back(vec3d(0, 1, 0));
        vertices.push_back(vec3d(max[0], max[1], min[2]));
        normals.push_back(vec3d(1, 0, 0));
        vertices.push_back(vec3d(max[0], max[1], min[2]));
        normals.push_back(vec3d(0, 0, -1));

        // 18, 19, 20
        vertices.push_back(vec3d(max[0], max[1], max[2]));
        normals.push_back(vec3d(0, 1, 0));
        vertices.push_back(vec3d(max[0], max[1], max[2]));
        normals.push_back(vec3d(1, 0, 0));
        vertices.push_back(vec3d(max[0], max[1], max[2]));
        normals.push_back(vec3d(0, 0, 1));

        // 21, 22, 23
        vertices.push_back(vec3d(min[0], max[1], max[2]));
        normals.push_back(vec3d(0, 1, 0));
        vertices.push_back(vec3d(min[0], max[1], max[2]));
        normals.push_back(vec3d(-1, 0, 0));
        vertices.push_back(vec3d(min[0], max[1], max[2]));
        normals.push_back(vec3d(0, 0, 1));

        // Faces
        std::vector<mesh3::face> &faces = ret.get_faces();
        mesh3::face front[2];
        front[0][0] = 0; front[0][1] = 3; front[0][2] = 9;
        front[1][0] = 9; front[1][1] = 3; front[1][2] = 6;
        faces.push_back(front[0]);
        faces.push_back(front[1]);

        mesh3::face back[2];
        back[0][0] = 21; back[0][1] = 15; back[0][2] = 12;
        back[1][0] = 21; back[1][1] = 18; back[1][2] = 15;
        faces.push_back(back[0]);
        faces.push_back(back[1]);

        mesh3::face left[2];
        left[0][0] = 10; left[0][1] = 13; left[0][2] = 1;
        left[1][0] = 22; left[1][1] = 13; left[1][2] = 10;
        faces.push_back(left[0]);
        faces.push_back(left[1]);

        mesh3::face right[2];
        right[0][0] = 7; right[0][1] = 4; right[0][2] = 16;
        right[1][0] = 19; right[1][1] = 7; right[1][2] = 16;
        faces.push_back(right[0]);
        faces.push_back(right[1]);

        mesh3::face up[2];
        up[0][0] = 23; up[0][1] = 11; up[0][2] = 8;
        up[1][0] = 23; up[1][1] = 8; up[1][2] = 20;
        faces.push_back(up[0]);
        faces.push_back(up[1]);

        mesh3::face down[2];
        down[0][0] = 2; down[0][1] = 5; down[0][2] = 14;
        down[1][0] = 5; down[1][1] = 17; down[1][2] = 14;
        faces.push_back(down[0]);
        faces.push_back(down[1]);

        return ret;
    }
}