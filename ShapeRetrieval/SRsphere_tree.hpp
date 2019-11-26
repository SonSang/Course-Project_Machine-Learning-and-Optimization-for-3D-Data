#ifndef __SR_SPHERE_TREE_H__
#define __SR_SPHERE_TREE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "SRsphere.hpp"
#include "../ARender/property_render_geometry.hpp"
#include <map>
#include <vector>
#include <set>

namespace AF {
    // Bounding Volume Hierarchy based on sphere.
	class SRsphere_tree : public property_render {
	public:
		class node {
		public:
			std::vector<int> child;
			int parent;
			bool leaf;

			property_render_geometry<SRsphere> S;
		};
		using ivpair = std::pair<int, double>;	// id - value pair.
		struct ivpair_comp {
			bool operator() (const ivpair &lhs, const ivpair& rhs) const {
				return lhs.second < rhs.second;
			}
		};
		using ivset = std::set<ivpair, ivpair_comp>;

		std::vector<node> tree;
		int root;

		std::vector<int> prev_nodes;	// Node IDs from previous level.
		std::vector<int> cur_nodes;		// Node IDs from current level.
		ivset xmin_list;
		ivset xmax_list;
		ivset ymin_list;
		ivset ymax_list; 
		ivset zmin_list;
		ivset zmax_list;

		// Build this tree from input [ point_cloud ].
		void build(const std::set<vec3d> &point_cloud);

		void clear_minmax_list();
		void get_minmax(const SRsphere &S, double vals[6]);	// xmin, xmax, ymin, ymax, zmin, zmax.
		void insert_minmax_list(int id, const SRsphere &S);
		void delete_minmax_list(int id);

		// Find best fit node to merge ( that incurs least amount of volume increase ) 
		// for the given node in the tree.
		// @include_cur_nodes : Also find best fit node from [ cur_nodes ].
		int find_best_fit(int id, bool include_cur_nodes);

		// Find closest node for the given node ( only consider distance between centers ).
		// @TODO : we could better than this with KD tree algorithm...
		int find_closest_node(int id, bool include_cur_nodes, int sample_num = 100);

		// Compute volume increasement when we merge sphere [ a ] and sphere [ b ].
		double compute_surplus_volume(int a, int b) const;

		// Compute allowable offset from a sphere's center for given surplus volume.
		double compute_valid_offset(int id, double surplus_volume) const;

		// Rendering functions.
		std::vector<int> render_nodes;		// Nodes that are currently rendered.
		void build_render(); 				// Set rendering info of each nodes.
		void render_nodes_parent();			// Set [ render_nodes ] to parents of current ones.
		void render_nodes_child();			// Set [ render_nodes ] to child of current ones.
		void set_render_mode(int mode);		// Set render mode for spheres : 0 for wireframe, 1 for phong.
		virtual void render() const noexcept;
		virtual void render_ui();
	};
}

#endif