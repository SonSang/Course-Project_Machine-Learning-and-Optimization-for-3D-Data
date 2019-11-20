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

#include "ARender/scene_manager.hpp"
#include "ARender/property_render_geometry.hpp"
#include <iostream>

std::function<void()> loop;
void main_loop() { loop(); }

int wnd_width = 640, wnd_height = 480;

AF::camera          MC;     // Main Camera
AF::scene_manager   SM;     // Scene Manager

void init_camera() {
    MC.set_position(0, 0, 0);
    MC.update_view();
    MC.update_proj(wnd_width, wnd_height);
}

void resize(int width, int height) {
    wnd_width = width;
    wnd_height = height;
    glViewport(0, 0, wnd_width, wnd_height);
    MC.update_proj(wnd_width, wnd_height);
}

void import_model() {
    std::shared_ptr<AF::object>
		cobj = std::make_shared<AF::object>("Model");

    SM.get_object_manager().add_object(cobj);
    
    std::shared_ptr<AF::property_render_geometry<AF::rmesh3>>
        ptr = std::make_shared<AF::property_render_geometry<AF::rmesh3>>();
    ptr->get_geometry().build_obj("./Assets/Greek_Vase/Greek_Vase_3.obj");
    ptr->build_shader("./Shader/render_geometry-vert.glsl", "./Shader/render_geometry-frag.glsl");  //  Always have to set shader before BO.
    ptr->build_BO();
    ptr->get_config().M = ptr->get_config().WIREFRAME;
    SM.add_object_property(cobj->get_id(), ptr);
}

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

    SDL_Window *window = SDL_CreateWindow("Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, wnd_width, wnd_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
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

    init_camera();
    import_model();

    loop = [&]
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_WINDOWEVENT) {
                switch(e.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    resize(e.window.data1, e.window.data2);
                    break;
                }
            }
            else {
                switch(e.type) {
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == 119) {   // W
                        MC.move_back_forth(1);
                    }
                    else if(e.key.keysym.sym == 115) {   // S
                        MC.move_back_forth(-1);
                    }
                    else if(e.key.keysym.sym == 97) {   // A
                        MC.move_left_right(-1);
                    }
                    else if(e.key.keysym.sym == 100) {   // D
                        MC.move_left_right(1);
                    }
                    break;
                case SDL_QUIT:
                    std::terminate();
                    break;
                }
            }
        }

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SM.render(MC);        

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