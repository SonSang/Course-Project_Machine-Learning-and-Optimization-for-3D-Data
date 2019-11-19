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
}