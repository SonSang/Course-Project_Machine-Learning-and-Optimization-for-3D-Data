#include "render_manager.hpp"
#include <SDL2/SDL_opengles2.h>
#include <deque>

namespace AF {
    // node
    render_manager::node::node(const std::shared_ptr<object> &obj_) : obj(obj_) {

    }
    const std::string& render_manager::node::get_name() const noexcept {
        return this->obj->get_name();
    }
    std::set<std::shared_ptr<property_render>> render_manager::node::get_properties() const noexcept {
        return this->obj->get_derived_property<property_render>();
    }
    const render_manager::node::children& render_manager::node::get_children() const noexcept {
        return this->children_;
    }
    void render_manager::node::add_child(uint c) {
        this->children_.insert(c);
    }
    void render_manager::node::del_child(uint c) {
        this->children_.erase(c);
    }
    unsigned int render_manager::node::get_id() const noexcept {
        return this->obj->get_id();
    }
    void render_manager::node::render(const std::map<uint, node> &nodes, const camera &CAM, transform TR) const noexcept {
        int
            size = this->obj->property_size();
        
        // Apply object's transformation.
        const transform
            &curTR = this->obj->get_transform();
        TR.update(curTR);
        
        for (int i = 0; i < size; i++) {
            std::shared_ptr<property_render>
                cast = std::dynamic_pointer_cast<property_render>(this->obj->get_property(i));
            if (cast != nullptr && cast->is_valid()) {
                cast->get_shader_c().set_uniform_model_matrix(TR);
                cast->get_shader_c().set_uniform_view_matrix(CAM.get_view_matrix_c());
                cast->get_shader_c().set_uniform_projection_matrix(CAM.get_projection_matrix_c());
                cast->render();
            }					
        }

        for(auto it = get_children().begin(); it != get_children().end(); it++)
            nodes.at(*it).render(nodes, CAM, TR);
    }

    // scene graph.
    render_manager::render_manager() : root(nullptr) {

    }
    render_manager::node& render_manager::get_root() noexcept {
        return this->root;
    }
    render_manager::node& render_manager::get_node(uint id) {
        return this->nodes.at(id);
    }
    const render_manager::node& render_manager::get_node_const(uint id) const {
        return this->nodes.at(id);
    }
    bool render_manager::has_node(uint id) const noexcept {
        return this->nodes.find(id) != this->nodes.end();
    }
    void render_manager::add_node(const node &node_, uint parent, bool parent_is_root) {
        uint
            id = node_.get_id();
        this->nodes[id] = node_;
        if (parent_is_root)
            get_root().add_child(id);
        else
            this->nodes.at(parent).add_child(id);
    }
    void render_manager::del_node(uint id) {
        this->nodes.erase(id);
    }
    void render_manager::render(const camera &CAM) const noexcept {
        transform I;
        I.identity();
        for(auto it = root.get_children().begin(); it != root.get_children().end(); it++)
            nodes.at(*it).render(nodes, CAM, I);    // Render recursively.
    }
    void render_manager::render_mouse_pick(void(*mouse_pick_callback)(unsigned int id)) const noexcept {
        // std::deque<uint>
        //     render_queue;
        // for (auto it = root.get_children().begin(); it != root.get_children().end(); it++)
        //     render_queue.push_back(*it);
        // while (!render_queue.empty()) {
        //     uint
        //         nid = render_queue.front();
        //     render_queue.pop_front();

        //     mouse_pick_callback(nodes.at(nid).get_id() + 4); // to exclude "0", "x", "y", "z", add 4.
        //     nodes.at(nid).render();		// RENDER

        //     for (auto it = get_node_const(nid).get_children().begin(); it != get_node_const(nid).get_children().end(); it++)
        //         render_queue.push_front(*it);
        // }
    }
}