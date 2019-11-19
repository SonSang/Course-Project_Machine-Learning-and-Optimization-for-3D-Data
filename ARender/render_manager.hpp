#ifndef __AF_RENDER_MANAGER_H__
#define __AF_RENDER_MANAGER_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "camera.hpp"
#include "object.hpp"
#include "property_render.hpp"

#include <set>
#include <map>

namespace AF {
    class render_manager {
        using uint = unsigned int;
    public:
        class node {
        public:
            using children = std::set<uint>;
        private:
            children children_;
            std::shared_ptr<object> obj;	// [obj] containing [property_render]s.
        public:
            node(const std::shared_ptr<object> &obj_ = nullptr);
            const std::string& get_name() const noexcept;
            std::set<std::shared_ptr<property_render>> get_properties() const noexcept;
            const children& get_children() const noexcept;
            void add_child(uint c);
            void del_child(uint c);
            unsigned int get_id() const noexcept;

            // @TR : Model transform applied prior to this node.
            void render(const std::map<uint, node> &nodes, const camera &CAM, transform TR) const noexcept;
        };
    private:
        std::map<uint, node> nodes;		// node id is same as its object.
        node root;
    public:
        render_manager();

        node& get_root() noexcept;
        node& get_node(uint id);
        const node& get_node_const(uint id) const;

        bool has_node(uint id) const noexcept;

        void add_node(const node &node_, uint parent, bool parent_is_root = false);
        void del_node(uint id);

        void render(const camera &CAM) const noexcept;
        void render_mouse_pick(void(*mouse_pick_callback)(unsigned int id)) const noexcept;
    };
}

#endif