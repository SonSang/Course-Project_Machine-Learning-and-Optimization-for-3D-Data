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
#include "ARender/mouse.hpp"
#include "ARender/property_render_geometry.hpp"
#include "ARender/render_geometry.hpp"
#include "ShapeRetrieval/SRsphere_tree.hpp"
#include "ShapeRetrieval/SRcgal_interface.hpp"
#include "ShapeRetrieval/SRsearch_tree.hpp"
#include <iostream>

// Imgui
#include "Dependencies/imgui/imgui.h"
#include "Dependencies/imgui/examples/imgui_impl_sdl.h"
#include "Dependencies/imgui/examples/imgui_impl_opengl3.h"

std::function<void()> loop;
void main_loop() { loop(); }

int wnd_width = 640, wnd_height = 480;

AF::camera          MC;     // Main Camera
AF::mouse           MM;     // Main Mouse
AF::scene_manager   SM;     // Scene Manager
static bool SRmenu_on = false;  // Menu toggler

AF::SRsearch_tree   DB;     // Search DB

std::vector<std::shared_ptr<AF::object>> models;
std::vector<bool> models_select;

void init_camera() {
    MC.set_position(0, 0, 1);
    MC.update_view();
    MC.update_proj(wnd_width, wnd_height);
}

void resize(int width, int height) {
    wnd_width = width;
    wnd_height = height;
    glViewport(0, 0, wnd_width, wnd_height);
    MC.update_proj(wnd_width, wnd_height);
}

void mouse_event(const SDL_Event &e) {
    if(e.button.type == SDL_MOUSEBUTTONDOWN) {
        if(e.button.button == SDL_BUTTON_RIGHT) 
            MM.set_rpressed(true);
    }
    else if(e.button.type == SDL_MOUSEBUTTONUP) {
        if(e.button.button == SDL_BUTTON_RIGHT) {
            MM.set_rpressed(false);
            MM.set_first_mouse(true);
        }  
    }
    else if(e.button.type == SDL_MOUSEMOTION) {
        if (MM.get_rpressed()) {
			MM.move_camera(MC, e.button.x, e.button.y);	// mouse right button to change camera angle.
		}
    }
}

void window_event(const SDL_Event &e, bool &done) {
    if(e.type == SDL_WINDOWEVENT) {
        if(e.window.event == SDL_WINDOWEVENT_CLOSE)
            done = true;
        else if(e.window.event == SDL_WINDOWEVENT_RESIZED)
            resize(e.window.data1, e.window.data2);
    }
}

AF::property_render_geometry<AF::rmesh3>& get_model_mesh(int id) {
    return *(*models.at(id)->get_property<AF::property_render_geometry<AF::rmesh3>>().begin());
}

AF::SRsphere_tree& get_model_sphere_tree(int id) {
    return *(*models.at(id)->get_property<AF::SRsphere_tree>().begin());
}

void keyboard_event(const SDL_Event &e, bool &done) {
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
        else if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            done = true;
        }
        // else if(e.key.keysym.scancode == SDL_SCANCODE_F2) {     // Change render mode
        //     for(auto it = models.begin(); it != models.end(); it++) {}
        //     if(model->get_config_c().M == model->get_config_c().PHONG)
        //         model->get_config().M = model->get_config_c().WIREFRAME;
        //     else if(model->get_config_c().M == model->get_config_c().WIREFRAME)
        //         model->get_config().M = model->get_config_c().PHONG;
        // }
        // else if(e.key.keysym.scancode == SDL_SCANCODE_F3) {     // Compute normal
        //     model->get_geometry().compute_normals();
        //     model->build_BO();
        // }
        // else if(e.key.keysym.scancode == SDL_SCANCODE_F4) {     // Reverse normal
        //     model->get_geometry().reverse_normals();
        //     model->build_BO();
        // }
        else if(e.key.keysym.scancode == SDL_SCANCODE_F2) {     // Toggle menu
            SRmenu_on = !SRmenu_on;
        }
    }
}

void import_model(const std::string &path) {
    std::shared_ptr<AF::property_render_geometry<AF::rmesh3>>
        ptr = std::make_shared<AF::property_render_geometry<AF::rmesh3>>();
    std::shared_ptr<AF::SRsphere_tree>
        tptr = std::make_shared<AF::SRsphere_tree>();

    try {
        ptr->get_geometry().build_obj(path);
        //ptr->get_geometry().del_duplicate_faces();
    } catch(std::exception &e) {
        std::cout<<e.what()<<std::endl;
        return;
    }
    //ptr->get_geometry().compute_normals();
    
    std::shared_ptr<AF::object>
		cobj = std::make_shared<AF::object>(path);

    SM.get_object_manager().add_object(cobj);

    // models.push_back(ptr);
    // models_stree.push_back(tptr);
    tptr->set_valid(false);

    // ALWAYS SHADER FIRST!
    ptr->build_shader("./Shader/render_geometry-vert.glsl", "./Shader/render_geometry-frag.glsl");  //  Always have to set shader before BO.
    tptr->set_shader(ptr->get_shader_c());
    //ptr->get_geometry().scale_norm();

    ptr->build_BO();

    ptr->get_config().M = ptr->get_config().WIREFRAME;

    AF::material material;
    material.set_emmision(AF::color(0, 0, 0));
    material.set_ambient(AF::color(0.2, 0.2, 0.2));
    material.set_diffuse(AF::color(0.4, 0.4, 0.4));
    material.set_specular(AF::color(1.0, 1.0, 1.0));
    material.set_shininess(100);    
    ptr->shader_set_material(material);
    
    AF::light_point light;
    light.set_position(AF::vec3d(0, 0, 0));
    light.set_ambient(AF::color(0.2, 0.2, 0.2));
    light.set_diffuse(AF::color(1.0, 1.0, 1.0));
    light.set_specular(AF::color(0.7, 0.7, 0.7));
    ptr->shader_set_light_point(light);
    
    SM.add_object_property(cobj->get_id(), ptr);
    SM.add_object_property(cobj->get_id(), tptr);
    models.push_back(cobj);
}

void compute_normal(int id) {
    auto &mesh = get_model_mesh(id);
    mesh.get_geometry().compute_normals();
    mesh.build_BO();
}

void change_render_mode(int id) {
    auto &mesh = get_model_mesh(id);
    if(mesh.get_config_c().M == mesh.get_config_c().PHONG)
        mesh.get_config().M = mesh.get_config_c().WIREFRAME;
    else if(mesh.get_config_c().M == mesh.get_config_c().WIREFRAME)
        mesh.get_config().M = mesh.get_config_c().PHONG;
}

void unit_size(int id) {
    auto &mesh = get_model_mesh(id);
    mesh.get_geometry().scale_norm();
    mesh.build_BO();
}

void build_sphere_tree(int id) {
    std::set<AF::vec3d> pc;
    pc = get_model_mesh(id).get_geometry().get_vertex_set();
    //get_model_sphere_tree(id).build(pc);
    get_model_sphere_tree(id).build(get_model_mesh(id).get_geometry());
    get_model_sphere_tree(id).build_render();
}

bool ma_flip = true;
void build_medial_axis(int id) {
    std::set<AF::vec3d> pc;
    std::vector<AF::SRsphere> sc;
    // pc = get_model_mesh(id).get_geometry().get_vertex_set();
    // sc = AF::get_pcl_medial_axis_balls(pc, ma_flip);
    sc = AF::get_cgal_mesh_skeleton(get_model_mesh(id).get_geometry());
    get_model_sphere_tree(id).build(sc);
    get_model_sphere_tree(id).build_render();
}

void change_sphere_tree_render_mode(int id) {
    auto mode = get_model_sphere_tree(id).tree.at(0).S.get_config().M;
    int nmode;
    if(mode == get_model_sphere_tree(id).tree.at(0).S.get_config().WIREFRAME)
        nmode = 1;
    else
        nmode = 0;
    get_model_sphere_tree(id).set_render_mode(nmode);
}

void update_models_select() {
    models_select.clear();
    models_select.resize(models.size());
    for(int i = 0; i < models_select.size(); i++)
        models_select[i] = false;
}

static AF::SRsphere_tree::align_var av(0, 0, 0, 0, 0, 0);
static AF::transform atr;
void testEMD1(int level) {
    if(level > 6 || level < 1) {
        std::cout<<"Level must be lower than 7."<<std::endl;
        return;
    }
    // EMD test 1 : Simple test, for given two sphere trees and level,
    //              subtract one from each other and compute EMD.
    int aid = -1, bid = -1;
    for(int i = 0; i < models_select.size(); i++) {
        if(models_select[i])
        {
            if(aid == -1) 
                aid = i;
            else if(bid == -1) {
                bid = i;
                break;
            }
        }
    }
    if(aid == -1 || bid == -1) {
        std::cout<<"Please select two models for EMD."<<std::endl;
        return;
    }
    std::vector<AF::SRsphere> subA, subB;
    AF::SRsphere_tree copy = get_model_sphere_tree(bid);
    AF::transform copyTR = AF::SRsphere_tree::alignTR(av);
    
    for(auto it = copy.tree.begin(); it != copy.tree.end(); it++) {
        AF::SRsphere &S = it->S.get_geometry();
        S.set_center(copyTR.apply(S.get_center()));
        //S.set_radius(S.get_radius() * av.scale);
    }
    AF::SRsphere_tree::test_pseudo_emd(
        get_model_sphere_tree(aid),
        copy,
        level,
        subA, subB
    );
    double r = 0;
    for(auto it = subA.begin(); it != subA.end(); it++) {
        double t = it->get_radius();
        r += t * t * t;
    }
    for(auto it = subB.begin(); it != subB.end(); it++) {
        double t = it->get_radius();
        r += t * t * t;
    }
    r *= (4.0 / 3.0) * AF::pi;
    std::cout<<"EMD : "<<r<<std::endl;

    // Draw [ sub ].
    // std::shared_ptr<AF::object>
	// 	cobj = std::make_shared<AF::object>();

    // SM.get_object_manager().add_object(cobj);

    // AF::material material;
    // material.set_emmision(AF::color(0, 0, 0));
    // material.set_ambient(AF::color(0.2, 0.2, 0.2));
    // material.set_diffuse(AF::color(0.9, 0.4, 0.4));
    // material.set_specular(AF::color(1.0, 1.0, 1.0));
    // material.set_shininess(100);    

    // AF::material materialB;
    // materialB.set_emmision(AF::color(0, 0, 0));
    // materialB.set_ambient(AF::color(0.2, 0.2, 0.2));
    // materialB.set_diffuse(AF::color(0.4, 0.4, 0.9));
    // materialB.set_specular(AF::color(1.0, 1.0, 1.0));
    // materialB.set_shininess(100);    
    
    // AF::light_point light;
    // light.set_position(AF::vec3d(0, 0, 0));
    // light.set_ambient(AF::color(0.2, 0.2, 0.2));
    // light.set_diffuse(AF::color(1.0, 1.0, 1.0));
    // light.set_specular(AF::color(0.7, 0.7, 0.7));
    
    // for(auto it = subA.begin(); it != subA.end(); it++) {
    //     std::shared_ptr<AF::property_render_geometry<AF::rmesh3>>
    //         ptr = std::make_shared<AF::property_render_geometry<AF::rmesh3>>();
    //     ptr->build_shader("./Shader/render_geometry-vert.glsl", "./Shader/render_geometry-frag.glsl");
    //     AF::mesh3 m = it->get_mesh2();
    //     ptr->get_config().M = ptr->get_config().PHONG;
    //     ptr->build_BO_mesh3(m);
    //     ptr->shader_set_light_point(light);
    //     ptr->shader_set_material(material);
    //     SM.add_object_property(cobj->get_id(), ptr);
    // }

    // for(auto it = subB.begin(); it != subB.end(); it++) {
    //     std::shared_ptr<AF::property_render_geometry<AF::rmesh3>>
    //         ptr = std::make_shared<AF::property_render_geometry<AF::rmesh3>>();
    //     ptr->build_shader("./Shader/render_geometry-vert.glsl", "./Shader/render_geometry-frag.glsl");
    //     AF::mesh3 m = it->get_mesh2();
    //     ptr->get_config().M = ptr->get_config().PHONG;
    //     ptr->build_BO_mesh3(m);
    //     ptr->shader_set_light_point(light);
    //     ptr->shader_set_material(materialB);
    //     SM.add_object_property(cobj->get_id(), ptr);
    // }
}

void testEMD2(int level) {
    if(level > 6 || level < 1) {
        std::cout<<"Level must be lower than 7."<<std::endl;
        return;
    }
    int aid = -1, bid = -1;
    for(int i = 0; i < models_select.size(); i++) {
        if(models_select[i])
        {
            if(aid == -1) 
                aid = i;
            else if(bid == -1) {
                bid = i;
                break;
            }
        }
    }
    if(aid == -1 || bid == -1) {
        std::cout<<"Please select two models for EMD."<<std::endl;
        return;
    }
    // EMD test 2 : Simple test, for given two sphere trees and level,
    //              align the second tree to match to the first one as close as possible.
    AF::SRsphere_tree::align_var var = av;
    // AF::SRsphere_tree::align_emd(
    //     get_model_sphere_tree(aid),
    //     get_model_sphere_tree(bid),
    //     level, var);
    models.at(bid)->set_transform(AF::SRsphere_tree::alignTR(var));
    av = var;
}

void testEMD3(int level) {
     if(level > 6 || level < 1) {
        std::cout<<"Level must be lower than 7."<<std::endl;
        return;
    }
    int aid = -1, bid = -1;
    for(int i = 0; i < models_select.size(); i++) {
        if(models_select[i])
        {
            if(aid == -1) 
                aid = i;
            else if(bid == -1) {
                bid = i;
                break;
            }
        }
    }
    if(aid == -1 || bid == -1) {
        std::cout<<"Please select two models for EMD."<<std::endl;
        return;
    }
    // EMD test 3 : Simple test, for given two sphere trees and level,
    //              align the second tree to match to the first one as close as possible.
    AF::transform TR = atr;
    AF::SRsphere_tree::align_icp(
        get_model_sphere_tree(aid),
        get_model_sphere_tree(bid),
        level, TR);
    models.at(bid)->set_transform(TR);
}

void restoreEMD() {
    int aid = -1, bid = -1;
    for(int i = 0; i < models_select.size(); i++) {
        if(models_select[i])
        {
            if(aid == -1) 
                aid = i;
            else if(bid == -1) {
                bid = i;
                break;
            }
        }
    }
    if(aid == -1 || bid == -1) {
        std::cout<<"Please select two models for EMD."<<std::endl;
        return;
    }
    AF::transform I;
    I.identity();
    models.at(bid)->set_transform(I);
    av.rx = 0;
    av.ry = 0;
    av.rz = 0;
    av.tx = 0;
    av.ty = 0;
    av.tz = 0;
            //av.scale = 1;
    atr.identity();
}

void search_tree_ui_rec(int node) {
    if(node < 0 || node >= DB.tree.size())
        return;
    if(ImGui::TreeNode((void *)node, "Node %d (%d)", node, DB.tree.at(node).level)) {
        const auto &child = DB.tree.at(node).child;
        for(auto it = child.begin(); it != child.end(); it++)
            search_tree_ui_rec(*it);
        ImGui::TreePop();
    }
}

void search_tree_ui() {
    if(DB.tree.empty())
        return;
    search_tree_ui_rec(DB.root);
}

// GUI for Shape Retrieval.
void SRmenu_model();
void SRmenu_EMD();
void SRmenu_search();

void SRmenu() {
    ImGui::Begin("Shape Retrieval menu", &SRmenu_on);
    if(ImGui::CollapsingHeader("Model"))
        SRmenu_model();
    if(ImGui::CollapsingHeader("Search"))
        SRmenu_search();
    if(ImGui::CollapsingHeader("EMD"))
        SRmenu_EMD();
    ImGui::End();
}
void SRmenu_model() {
    if(ImGui::TreeNode("Imported models")) {
        int id = 0;
        for(auto it = models.begin(); it != models.end(); it++) {
            ImGui::PushID(std::distance(models.begin(), it));
            ImGui::Text((*it)->get_name().c_str());
            if(ImGui::Button("Toggle")) {
                bool v = get_model_mesh(id).is_valid();
                get_model_mesh(id).set_valid(!v);
            }
            ImGui::SameLine();
            if(ImGui::Button("Unit size"))
                unit_size(id);                
            ImGui::SameLine();
            if(ImGui::Button("Set normal")) 
                compute_normal(id);            
            ImGui::SameLine();
            if(ImGui::Button("Change render")) 
                change_render_mode(id);     
            ImGui::SameLine();
            if(ImGui::Button("Build STree"))
                build_sphere_tree(id);
            ImGui::SameLine();
            // if(ImGui::Button("Build STree(MEDIAL)"))
            //     build_medial_axis(id);
            // ImGui::SameLine();
            if(ImGui::Button("Toggle STree"))
                get_model_sphere_tree(id).set_valid(!get_model_sphere_tree(id).is_valid());
            ImGui::SameLine();
            if(ImGui::Button("STree Lower"))
                get_model_sphere_tree(id).render_nodes_child();
            ImGui::SameLine();
            if(ImGui::Button("STree upper"))            
                get_model_sphere_tree(id).render_nodes_parent();
            ImGui::SameLine();
            if(ImGui::Button("Change STree render"))            
                change_sphere_tree_render_mode(id);
            ImGui::SameLine();
            if(ImGui::Button("Flip ma normal"))            
            {
                ma_flip = !ma_flip;
                build_medial_axis(id);
            }
            ImGui::PopID();
            id++;
        }

        ImGui::TreePop();
        ImGui::Separator();
    }
    if(ImGui::TreeNode("Import model")) {
        static char path[64] = "./Assets/val/02691156/model_000081.obj";
        ImGui::InputText("path", path, 64);
        ImGui::SameLine();
        if(ImGui::Button("Import")) {
            import_model(path);
            update_models_select();
        }         

        ImGui::TreePop();
        ImGui::Separator();
    }
}
void SRmenu_EMD() {
    if(ImGui::TreeNode("Select two models")) {
        for(auto it = models.begin(); it != models.end(); it++) {
            bool b = models_select.at(std::distance(models.begin(), it));
            ImGui::Checkbox((*it)->get_name().c_str(), &b);
            models_select.at(std::distance(models.begin(), it)) = b;
        }
        static int level = 3;
        ImGui::InputInt("level", &level);
        if(ImGui::Button("Test 1 : Simple EMD")) {
            testEMD1(level);
        }
        if(ImGui::Button("Test 2 : Align EMD")) {
            testEMD2(level);
        }
        if(ImGui::Button("Test 3 : Align ICP")) {
            testEMD3(level);
        }
        if(ImGui::Button("Restore")) {
            restoreEMD();
        }
        ImGui::TreePop();
    }
}
void SRmenu_search() {
    if(ImGui::TreeNode("Select models")) {
        for(auto it = models.begin(); it != models.end(); it++) {
            if(DB.model_node_map.find((*it)->get_id()) != DB.model_node_map.end())
                continue;
            bool b = models_select.at(std::distance(models.begin(), it));
            ImGui::Checkbox((*it)->get_name().c_str(), &b);
            models_select.at(std::distance(models.begin(), it)) = b;
        }
        if(ImGui::Button("Select All")) {
            for(auto it = models_select.begin(); it != models_select.end(); it++) 
                (*it) = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Deselect All")) {
            for(auto it = models_select.begin(); it != models_select.end(); it++)
                (*it) = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Add DB")) {
            // THIS IS WHERE TREE BUILDING ALGORITHM GOES IN....
            for(int i = 0; i < models_select.size(); i++) {
                if(!models_select[i])
                    continue;
                unsigned int model_id = models.at(i)->get_id();
                if(DB.model_node_map.find(model_id) != DB.model_node_map.end())
                    continue;
                DB.add(models.at(i));
            }
        }
        ImGui::TreePop();
    }
    if(ImGui::TreeNode("Result tree")) {
        search_tree_ui();
        ImGui::TreePop();
    }
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
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);  // Anti-alising.

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

    glEnable(GL_DEPTH_TEST);

    // Setup ImGui.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, glc);
    ImGui_ImplOpenGL3_Init();

    /////// ========================================== For utility.
    import_model("./Assets/val/02691156/model_000081.obj");
    import_model("./Assets/val/02691156/model_000555.obj");

    //import_model("./Assets/Greek_Vase/Greek_Vase_3.obj");
    update_models_select();
    ////// ===========================================

    // Main loop.
    bool done = false;
    bool show_demo_window = true;
    loop = [&]
    {
        // Poll and handle events (inputs, window resize, etc.)
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            mouse_event(event);
            window_event(event, done);
            keyboard_event(event, done);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
#ifdef __EMSCRIPTEN__
#else
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        if(SRmenu_on)
            SRmenu();
#endif

        // Rendering
        ImGui::Render();
#ifdef __EMSCRIPTEN__
        SDL_GL_MakeCurrent(window, glc);
#endif
        // Clear the screen to black
        glClearColor(1.0f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SM.render(MC);        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(!done) 
        main_loop();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glc);
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif

    return 0;
}