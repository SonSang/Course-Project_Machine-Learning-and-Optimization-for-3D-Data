#include "render_geometry.hpp"
#include "../Dependencies/OBJ-Loader/Source/OBJ_Loader.h"

namespace AF {
    // rmesh3
    // Textures
    void rmesh3::set_textures(const std::vector<vec2d> &textures) noexcept {
        this->textures = textures;
    }
    const std::vector<vec2d>& rmesh3::get_textures_c() const noexcept {
        return this->textures;
    }
    std::vector<vec2d>& rmesh3::get_textures() noexcept {
        return this->textures;
    }
    void rmesh3::operator=(const mesh3 &M) noexcept {
        clear();
        this->set_vertices(M.get_vertices_c());
        this->set_normals(M.get_normals_c());
        this->set_faces(M.get_faces_c());
    }
    void rmesh3::clear() noexcept {
        mesh3::clear();
        textures.clear();
    }

    // Build
    vec3d convert(const objl::Vector3 &ov) {
        return vec3d(ov.X, ov.Y, ov.Z);
    }
    vec2d convert(const objl::Vector2 &ov) {
        return vec2d(ov.X, ov.Y);
    }
    void rmesh3::build_obj(const std::string &path) {
        objl::Loader loader;
        if(!loader.LoadFile(path)) {
            std::cerr<<"Error in reading .obj file at : "<<path<<std::endl;
            return;
        }
        
        clear();
        // Set vertex, normal and texture info.
        for(auto it = loader.LoadedVertices.begin(); it != loader.LoadedVertices.end(); it++) {
            this->vertices.push_back(convert(it->Position));
            this->normals.push_back(convert(it->Normal));
            this->textures.push_back(convert(it->TextureCoordinate));
        }

        // Set face info.
        for(auto it = loader.LoadedIndices.begin(); it != loader.LoadedIndices.end(); ) {
            face nface;
            nface[0] = (*it++);
            nface[1] = (*it++);
            nface[2] = (*it++);
            this->faces.push_back(nface);
        }
    }
}