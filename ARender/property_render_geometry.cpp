#include "property_render_geometry.hpp"

#include <iostream>
namespace AF {
    using uint = unsigned int;
    // rmesh3
    template<>
    void property_render_geometry<rmesh3>::build_BO() {
		// 1. Create Vertex Array Object
        GLuint vao;
        glGenVertexArraysOES(1, &vao);
        glBindVertexArrayOES(vao);

        // 2. Create a Vertex Buffer Object and copy the vertex data to it
        GLuint vbo[2];  // 0 for Position, 1 for normal.
        glGenBuffers(2, vbo);

        const auto &geometry = get_geometry_c();
        int size = (int)geometry.get_vertices_c().size();

        GLfloat *vertices = new GLfloat[3 * size];
        GLfloat *normals = new GLfloat[3 * size];
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < 3; j++) {
                vertices[3 * i + j] = (float)geometry.get_vertices_c()[i][j];
                normals[3 * i + j] = (float)geometry.get_normals_c()[i][j];
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

        size = (int)geometry.get_faces_c().size();

        uint *faces = new uint[3 * size];
        uint *edges = new uint[6 * size];
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < 3; j++)
                faces[3 * i + j] = geometry.get_faces_c()[i][j];
            edges[6 * i + 0] = geometry.get_faces_c()[i][0];
            edges[6 * i + 1] = geometry.get_faces_c()[i][1];
            edges[6 * i + 2] = geometry.get_faces_c()[i][1];
            edges[6 * i + 3] = geometry.get_faces_c()[i][2];
            edges[6 * i + 4] = geometry.get_faces_c()[i][2];
            edges[6 * i + 5] = geometry.get_faces_c()[i][0];
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

        glBindVertexArrayOES(0);

        // 4. Set BO info.
        BO.VAO = vao;
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
    template<>
    void property_render_geometry<rmesh3>::render() const noexcept {
		get_shader_c().enable();
        
        // Render by mode.
        if(C.M == C.PHONG)
            glUniform1i(get_shader_c().get_uniform_location("phong"), true);
        else 
            glUniform1i(get_shader_c().get_uniform_location("phong"), false);
        
        glBindVertexArrayOES(BO.VAO);
        if(C.M != C.WIREFRAME) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO.EBO.at(0));
            glDrawElements(GL_TRIANGLES, BO.EBO_size.at(0), GL_UNSIGNED_INT, 0);
        }
        if(C.M != C.PHONG) {    // When use phong shader, do not render edges.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO.EBO.at(1));
            glDrawElements(GL_LINES, BO.EBO_size.at(1), GL_UNSIGNED_INT, 0);
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArrayOES(0);
        shader::disable();
	}
    template<>
    void property_render_geometry<rmesh3>::render_ui() {
        return;
    }
    
}