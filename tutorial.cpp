#include <exception>
#include <functional>

//#define __EMSCRIPTEN__ 1
#ifdef __EMSCRIPTEN__

#include "Dependencies/Emscripten/emsdk/upstream/emscripten/system/include/emscripten.h"
#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

#else

#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengles2.h>

#endif

#include "ARender/shader.hpp"

#include <iostream>

std::function<void()> loop;
void main_loop() { loop(); }

int main(int argc, char** argv)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr<<"[ SDL_Init ] failed : %s"<<SDL_GetError()<<std::endl;
        exit(-1);
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window *window = SDL_CreateWindow("Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window, SDL_TRUE);

    // Returns the OpenGL context associated with [ window ] or NULL on error.
    SDL_GLContext glc = SDL_GL_CreateContext(window);   
    if(glc == nullptr) {
        std::cerr<<"[ SDL_GL_CreateContext ] failed : %s"<<SDL_GetError()<<std::endl;
        exit(-1);
    }

    // Disable v-sync.
    if(SDL_GL_SetSwapInterval(0) < 0) 
        std::cerr<<"[ SDL_GL_SetSwapInterval ] failed : %s"<<SDL_GetError()<<std::endl;
     
    // Returns a valid rendering context or NULL if there was an error.
    SDL_Renderer *rdr = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if(rdr == nullptr) {
        std::cerr<<"[ SDL_CreateRenderer ] failed : %s"<<SDL_GetError()<<std::endl;
        exit(-1);
    }

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArraysOES(1, &vao);
    glBindVertexArrayOES(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Shader
    AF::shader shader;
    try {
        shader.set_program("./Shader/simple-vert.glsl", "./Shader/simple-frag.glsl");
    } catch(const std::exception &e) {
        std::cout<<e.what()<<std::endl;
        exit(-1);
    }
    

    // Specify the layout of the vertex data
    GLint posAttrib = shader.get_attribute_location("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    loop = [&]
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) 
                std::terminate();
        }

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw a triangle from the 3 vertices
        shader.enable();                        // Must call this! ( Or have just white result... )
        glDrawArrays(GL_TRIANGLES, 0, 3);
        shader.disable();

        SDL_GL_SwapWindow(window);
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(true) 
        main_loop();
#endif

    return 0;
}