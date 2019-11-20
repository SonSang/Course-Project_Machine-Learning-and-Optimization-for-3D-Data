#include "shader.hpp"
#include "io.hpp"
#include <iostream>
#include <SDL2/SDL_opengles2.h>

namespace AF {
    using uint = unsigned int;

    // Ctor
    shader::shader(const std::string &vert_filename, const std::string &frag_filename) {
        set_program(vert_filename, frag_filename);
    }
    shader::~shader() {
        glDetachShader(get_program(), get_vertex_shader());
		glDetachShader(get_program(), get_fragment_shader());
		glDeleteShader(get_vertex_shader());
		glDeleteShader(get_fragment_shader());
		glDeleteProgram(get_program());
    }

    // Create ( Compile )
    uint shader::create_vertex_shader(const std::string &filename) {
        int
            success;
        char
            infoLog[512];
        GLuint
            id;
        std::string
            shader = IO::read_text(filename);
        const GLchar
            *glshader = shader.c_str();
        id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(id, 1, &glshader, NULL);
        glCompileShader(id);

        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            throw(std::runtime_error(   std::string("[SHADER ERROR] : Vertex shader compilation failed\n") +
                                        std::string("[COMPILATION ERROR MESSAGE] : \n") + 
                                        std::string(infoLog)));
        }
        return id;
    }
    uint shader::create_fragment_shader(const std::string &filename) {
        int
            success;
        char
            infoLog[512];
        GLuint
            id;
        std::string
            shader = IO::read_text(filename);
        const GLchar
            *glshader = shader.c_str();
        id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(id, 1, &glshader, NULL);
        glCompileShader(id);

        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            throw(std::runtime_error(   std::string("[SHADER ERROR] : Fragment shader compilation failed\n") +
                                        std::string("[COMPILATION ERROR MESSAGE] : \n") + 
                                        std::string(infoLog)));
        }
        return id;
    }

    // Set ( Create and set )
    void shader::set_vertex_shader(const std::string &filename) {
        this->vertex_shader = create_vertex_shader(filename);
    }
    void shader::set_fragment_shader(const std::string &filename) {
        this->fragment_shader = create_fragment_shader(filename);
    }
    void shader::set_program(const std::string &vert_filename, const std::string &frag_filename) {
        try {
            set_vertex_shader(vert_filename);
            set_fragment_shader(frag_filename);
            GLuint
                p = glCreateProgram();
            glAttachShader(p, get_vertex_shader());
            glAttachShader(p, get_fragment_shader());
            glLinkProgram(p);
            int
                success;
            char
                infoLog[512];
            glGetProgramiv(p, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(p, 512, NULL, infoLog);
                throw(std::runtime_error(   std::string("[SHADER ERROR] : Shader program linkage failed\n") +
                                            std::string("[LINKAGE ERROR MESSAGE] : \n") + 
                                            std::string(infoLog)));
            }
            program = p;
        } catch(std::exception &e) {
            std::cout<<"Shader failed "<<std::endl<<e.what()<<std::endl;
        }       
    }

    // Get
    uint shader::get_program() const noexcept {
        return this->program;
    }
    uint shader::get_vertex_shader() const noexcept {
        return this->vertex_shader;
    }
    uint shader::get_fragment_shader() const noexcept {
        return this->fragment_shader;
    }

    // Matrix settings.
    void shader::set_uniform_model_matrix(const transform &TR) const {
        float mat[16];
        TR.to_array(mat, true);

        uint id = get_uniform_location("model");

        enable();
        glUniformMatrix4fv(id, 1, GL_FALSE, mat);
        disable();
    }
    void shader::set_uniform_view_matrix(const transform &TR) const {
        float mat[16];
        TR.to_array(mat, true);

        uint id = get_uniform_location("view");

        enable();
        glUniformMatrix4fv(id, 1, GL_FALSE, mat);
        disable();
    }
    void shader::set_uniform_view_matrix(const mat4d &M) const {
        float mat[16];
        M.to_array(mat, true);

        uint id = get_uniform_location("view");

        enable();
        glUniformMatrix4fv(id, 1, GL_FALSE, mat);
        disable();
    }
    void shader::set_uniform_projection_matrix(const mat4d &M) const {
        float mat[16];
        M.to_array(mat, true);

        uint id = get_uniform_location("projection");

        enable();
        glUniformMatrix4fv(id, 1, GL_FALSE, mat);
        disable();
    }

    // Get shader var location
    int shader::get_attribute_location(const std::string &varname) const {
        return glGetAttribLocation(get_program(), varname.c_str());
    }
    int shader::get_uniform_location(const std::string &varname) const {
        return glGetUniformLocation(get_program(), varname.c_str());
    }

    // Usage
    void shader::enable() const {
		glUseProgram(get_program());
    }
    void shader::disable() {
        glUseProgram(0);
    }
}