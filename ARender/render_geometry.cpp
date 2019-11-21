#include "render_geometry.hpp"
#include "obj_loader.hpp"

#include <iostream>

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
    void rmesh3::build_obj(const std::string &path) {
        obj_loader loader;
        std::cout<<"Loading .OBJ file : "<<path<<"..."<<std::endl;
        loader.load(path);
        std::cout<<"Loaded .OBJ file : "<<path<<std::endl;
        
        clear();
        // Set vertex, normal and texture info.
        using uint = unsigned int;
        int osize = loader.size();
        for(int i = 0; i < osize; i++) {
            const auto& object = loader.get_object_c(i);
            int gsize = object.size();
            for(int j = 0; j < gsize; j++) {
                const auto &group = object.get_group_c(j);
                int fsize = group.size();
                for(int k = 0; k < fsize; k++) {
                    const obj_loader::face &F = group.get_face_c(k);

                    if(F.size() == 3) {
                        face nf;
                        this->vertices.push_back(F.get_vertex_c(0).get_position_c());
                        this->normals.push_back(F.get_vertex_c(0).get_normal_c());
                        nf[0] = (uint)vertices.size() - 1;
                        this->vertices.push_back(F.get_vertex_c(1).get_position_c());
                        this->normals.push_back(F.get_vertex_c(1).get_normal_c());
                        nf[1] = (uint)vertices.size() - 1;
                        this->vertices.push_back(F.get_vertex_c(2).get_position_c());
                        this->normals.push_back(F.get_vertex_c(2).get_normal_c());
                        nf[2] = (uint)vertices.size() - 1;
                        this->faces.push_back(nf);
                    }
                    else if(F.size() == 4) {
                        face nf[2];
                        this->vertices.push_back(F.get_vertex_c(0).get_position_c());
                        this->normals.push_back(F.get_vertex_c(0).get_normal_c());
                        nf[0][0] = (uint)vertices.size() - 1;
                        this->vertices.push_back(F.get_vertex_c(1).get_position_c());
                        this->normals.push_back(F.get_vertex_c(1).get_normal_c());
                        nf[0][1] = (uint)vertices.size() - 1;
                        this->vertices.push_back(F.get_vertex_c(2).get_position_c());
                        this->normals.push_back(F.get_vertex_c(2).get_normal_c());
                        nf[0][2] = (uint)vertices.size() - 1;

                        this->vertices.push_back(F.get_vertex_c(0).get_position_c());
                        this->normals.push_back(F.get_vertex_c(0).get_normal_c());
                        nf[1][0] = (uint)vertices.size() - 1;
                        this->vertices.push_back(F.get_vertex_c(2).get_position_c());
                        this->normals.push_back(F.get_vertex_c(2).get_normal_c());
                        nf[1][1] = (uint)vertices.size() - 1;
                        this->vertices.push_back(F.get_vertex_c(3).get_position_c());
                        this->normals.push_back(F.get_vertex_c(3).get_normal_c());
                        nf[1][2] = (uint)vertices.size() - 1;
                        this->faces.push_back(nf[0]);
                        this->faces.push_back(nf[1]);
                    }
                    
                   
                }
            }
        }
    }

    // rmesh
    // void rmesh::set_textures(const std::vector<vec2d> &textures) noexcept {
    //     this->textures = textures;
    // }
    // const std::vector<vec2d>& rmesh::get_textures_c() const noexcept {
    //     return this->textures;
    // }
    // std::vector<vec2d>& rmesh::get_textures() noexcept {
    //     return this->textures;
    // }

    // void rmesh::operator=(const mesh &M) noexcept {
    //     clear();
    //     this->set_vertices(M.get_vertices_c());
    //     this->set_normals(M.get_normals_c());
    //     this->set_faces(M.get_faces_c());
    //     this->set_faces4(M.get_faces4_c());
    // }

    // void rmesh::build_obj(const std::string &path) {
    //     obj_loader loader;
    //     std::cout<<"Loading .OBJ file : "<<path<<"..."<<std::endl;
    //     loader.load(path);
    //     std::cout<<"Loaded .OBJ file : "<<path<<std::endl;
        
    //     clear();
    //     // Set vertex, normal and texture info.
    //     int osize = loader.size();
    //     for(int i = 0; i < osize; i++) {
    //         const auto& object = loader.get_object_c(i);
    //         int gsize = object.size();
    //         for(int j = 0; j < gsize; j++) {
    //             const auto &group = object.get_group_c(j);
    //             int fsize = group.size();
    //             for(int k = 0; k < fsize; k++) {
    //                 const obj_loader::face &F = group.get_face_c(k);
                    
    //                 if(F.size() == 3) {
    //                     face nf;
    //                     this->vertices.push_back(F.get_vertex_c(0).get_position_c());
    //                     this->normals.push_back(F.get_vertex_c(0).get_normal_c());
    //                     nf[0] = (uint)vertices.size() - 1;
    //                     this->vertices.push_back(F.get_vertex_c(1).get_position_c());
    //                     this->normals.push_back(F.get_vertex_c(1).get_normal_c());
    //                     nf[1] = (uint)vertices.size() - 1;
    //                     this->vertices.push_back(F.get_vertex_c(2).get_position_c());
    //                     this->normals.push_back(F.get_vertex_c(2).get_normal_c());
    //                     nf[2] = (uint)vertices.size() - 1;
    //                     this->faces.push_back(nf);
    //                 }
    //                 else if(F.size() == 4) {
    //                     face4 nf;
    //                     this->vertices.push_back(F.get_vertex_c(0).get_position_c());
    //                     this->normals.push_back(F.get_vertex_c(0).get_normal_c());
    //                     nf[0] = (uint)vertices.size() - 1;
    //                     this->vertices.push_back(F.get_vertex_c(1).get_position_c());
    //                     this->normals.push_back(F.get_vertex_c(1).get_normal_c());
    //                     nf[1] = (uint)vertices.size() - 1;
    //                     this->vertices.push_back(F.get_vertex_c(2).get_position_c());
    //                     this->normals.push_back(F.get_vertex_c(2).get_normal_c());
    //                     nf[2] = (uint)vertices.size() - 1;
    //                     this->vertices.push_back(F.get_vertex_c(3).get_position_c());
    //                     this->normals.push_back(F.get_vertex_c(3).get_normal_c());
    //                     nf[3] = (uint)vertices.size() - 1;
    //                     this->faces4.push_back(nf);
    //                 }
    //             }
    //         }
    //     }
    // }
    // void rmesh::clear() noexcept {
    //     mesh::clear();
    //     textures.clear();
    // }
}