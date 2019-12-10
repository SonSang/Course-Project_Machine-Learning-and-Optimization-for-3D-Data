#ifndef __AF_PROPERTY_RENDER_GEOMETRY_H__
#define __AF_PROPERTY_RENDER_GEOMETRY_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "color.hpp"
#include "property_render.hpp"
#include "render_geometry.hpp"
#include "light.hpp"

#define GL_GLEXT_PROTOTYPES 1   // Needed to use Vertex Array Object.
#include <SDL2/SDL_opengles2.h>

namespace AF {
    // Simple rapper class for geometric data structures.
    // This class is tightly packed with shaders named [ render_geometry.glsl ].
    template <typename T>
    class property_render_geometry : public property_render {
        static_assert(std::is_base_of<geometry, T>::value,
            "Geometry render property must contain a subclass object of [geometry] class.");
    public:
        struct config {
        public:
            enum mode {
                SIMPLE = 0,     // Draw face and edge in single specified color.
                WIREFRAME,      // Draw edge in single speficied color.
                PHONG           // Use phong shader.
            };
            mode M;     // How to render this geometry.

            // Data used for SIMPLE & WIREFRAME mode.
            color face_color;   
            color edge_color;   

            // Data used for PHONG mode.
            // Data used for choice between MATERIAL & TEXTURE during PHONG mode.
            bool use_material = true;
        };
    private:
        T		G;
        config 	C;
    public:
        property_render_geometry() = default;
        property_render_geometry(const std::string &name) : property_render(name) {

        }
        void			set_geometry(const T &geometry) {
            this->G = geometry;
        }
        T&				get_geometry() noexcept {
            return this->G;
        }
        const T&		get_geometry_c() const noexcept {
            return this->G;
        }
        config&			get_config() noexcept {
            return this->C;
        }
        const config&	get_config_c() const noexcept {
            return C;
        }
        virtual void    build_BO();
        virtual void	render() const noexcept;
        virtual void	render_ui();

        // Shader info
        // Since this class only uses [ render_geometry ] shader, we can hard code this kind of functions...
        void shader_set_material(const material &M) const { // Only 1 material is allowed.
            this->get_shader_c().enable();
            int 
                loc = get_shader_c().get_uniform_location("MM.ambient");
            glUniform3f(loc, (float)M.get_ambient()[0], (float)M.get_ambient()[1], (float)M.get_ambient()[2]);
            loc = get_shader_c().get_uniform_location("MM.emmision");
            glUniform3f(loc, (float)M.get_emmision()[0], (float)M.get_emmision()[1], (float)M.get_emmision()[2]);
            loc = get_shader_c().get_uniform_location("MM.diffuse");
            glUniform3f(loc, (float)M.get_diffuse()[0], (float)M.get_diffuse()[1], (float)M.get_diffuse()[2]);
            loc = get_shader_c().get_uniform_location("MM.specular");
            glUniform3f(loc, (float)M.get_specular()[0], (float)M.get_specular()[1], (float)M.get_specular()[2]);
            loc = get_shader_c().get_uniform_location("MM.shininess");
            glUniform1f(loc, (float)M.get_shininess());
            this->get_shader_c().disable();
        }  
        void shader_set_light_point(const light_point &L) const { // Only 1 point light is allowed.
            this->get_shader_c().enable();
            int 
                loc = get_shader_c().get_uniform_location("ML.ambient");
            glUniform3f(loc, (float)L.get_ambient()[0], (float)L.get_ambient()[1], (float)L.get_ambient()[2]);
            loc = get_shader_c().get_uniform_location("ML.diffuse");
            glUniform3f(loc, (float)L.get_diffuse()[0], (float)L.get_diffuse()[1], (float)L.get_diffuse()[2]);
            loc = get_shader_c().get_uniform_location("ML.specular");
            glUniform3f(loc, (float)L.get_specular()[0], (float)L.get_specular()[1], (float)L.get_specular()[2]);
            loc = get_shader_c().get_uniform_location("ML.position");
            glUniform3f(loc, (float)L.get_position()[0], (float)L.get_position()[1], (float)L.get_position()[2]);
            this->get_shader_c().disable();
        }   

        // Since many geometries can be rendered with [ mesh3 ], we can just use this function
        // to build BOs or render instead of defining each characteristic function.
        void build_BO_mesh3(const mesh3 &M) {
            // 1. Create Vertex Array Object
            // GLuint vao;
            // glGenVertexArraysOES(1, &vao);
            // glBindVertexArrayOES(vao);

            // 2. Create a Vertex Buffer Object and copy the vertex data to it
            GLuint vbo[2];  // 0 for Position, 1 for normal.
            glGenBuffers(2, vbo);

            int size = (int)M.get_vertices_c().size();

            GLfloat *vertices = new GLfloat[3 * size];
            GLfloat *normals = new GLfloat[3 * size];
            for(int i = 0; i < size; i++) {
                for(int j = 0; j < 3; j++) {
                    vertices[3 * i + j] = (float)M.get_vertices_c()[i][j];
                    normals[3 * i + j] = (float)M.get_normals_c()[i][j];
                }
            }

            
            // Position
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * size, vertices, GL_STATIC_DRAW);    // @BUGFIX : sizeof(ptr) returns 8!
            int attrib = get_shader_c().get_attribute_location("position");
            glEnableVertexAttribArray(attrib);
            glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            
            // Normal
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * size, normals, GL_STATIC_DRAW);
            attrib = get_shader_c().get_attribute_location("normal");
            glEnableVertexAttribArray(attrib);
            glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            delete[] vertices;
            delete[] normals;

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // 3. Create a Element Buffer Object
            GLuint ebo[2];
            glGenBuffers(2, ebo);

            size = (int)M.get_faces_c().size();

            uint *faces = new uint[3 * size];
            uint *edges = new uint[6 * size];
            for(int i = 0; i < size; i++) {
                for(int j = 0; j < 3; j++)
                    faces[3 * i + j] = M.get_faces_c()[i][j];
                edges[6 * i + 0] = M.get_faces_c()[i][0];
                edges[6 * i + 1] = M.get_faces_c()[i][1];
                edges[6 * i + 2] = M.get_faces_c()[i][1];
                edges[6 * i + 3] = M.get_faces_c()[i][2];
                edges[6 * i + 4] = M.get_faces_c()[i][2];
                edges[6 * i + 5] = M.get_faces_c()[i][0];
            }
                
            // Face
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 3 * size, faces, GL_STATIC_DRAW);

            // Edge
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 6 * size, edges, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            delete[] faces;
            delete[] edges;

            //glBindVertexArrayOES(0);

            // 4. Set BO info.
            //BO.VAO = vao;
            BO.VBO.clear();
            BO.VBO.push_back(vbo[0]);
            BO.VBO.push_back(vbo[1]);
            BO.EBO.clear();
            BO.EBO.push_back(ebo[0]);
            BO.EBO.push_back(ebo[1]);
            BO.EBO_size.clear();
            BO.EBO_size.push_back(3 * size);
            BO.EBO_size.push_back(6 * size);
        }
        void render_mesh3() const noexcept {
            shader_set_material(this->get_material_c());

            get_shader_c().enable();
        
            // Render by mode.
            if(C.M == C.PHONG)
                glUniform1i(get_shader_c().get_uniform_location("phong"), true);
            else 
                glUniform1i(get_shader_c().get_uniform_location("phong"), false);
            
            //glBindVertexArrayOES(BO.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, BO.VBO.at(0));
            int attrib = get_shader_c().get_attribute_location("position");
            glEnableVertexAttribArray(attrib);
            glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glBindBuffer(GL_ARRAY_BUFFER, BO.VBO.at(1));
            attrib = get_shader_c().get_attribute_location("normal");
            glEnableVertexAttribArray(attrib);
            glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

            if(C.M != C.WIREFRAME) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO.EBO.at(0));
                glDrawElements(GL_TRIANGLES, BO.EBO_size.at(0), GL_UNSIGNED_INT, 0);
            }
            if(C.M != C.PHONG) {    // When use phong shader, do not render edges.
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO.EBO.at(1));
                glDrawElements(GL_LINES, BO.EBO_size.at(1), GL_UNSIGNED_INT, 0);
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            //glBindVertexArrayOES(0);
            shader::disable();
        }
    };
}
#endif