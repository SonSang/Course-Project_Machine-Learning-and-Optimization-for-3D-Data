#ifndef __SR_SPHERE_TREE_H__
#define __SR_SPHERE_TREE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "SRsphere.hpp"
//#include "SRpcl_interface.hpp"
#include "../ARender/property_render_geometry.hpp"
#include "../Dependencies/nanoflann/include/nanoflann.hpp"
#include <map>
#include <vector>
#include <set>

namespace AF {
	class SRpoint_cloud {
	public:
		std::vector<vec3d> pc;

		inline size_t kdtree_get_point_count() const { return pc.size(); }

		inline double kdtree_get_pt(const size_t idx, const size_t dim) const {
			return pc[idx][dim];
		}

		template <class BBOX>
		bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }
	};
	
    // Bounding Volume Hierarchy based on sphere.
	class SRsphere_tree : public property_render {
	public:
		// ============ These data structures are used to build sphere tree fast. 
		using ivpair = std::pair<int, double>;	// id - value pair.
		struct ivpair_comp {
			bool operator() (const ivpair &lhs, const ivpair& rhs) const {
				if(lhs.second == rhs.second)
					return lhs.first < rhs.first;
				else return lhs.second < rhs.second;
			}
		};
		using ivset = std::set<ivpair, ivpair_comp>;
		// pclPC::Ptr pcptr;
		// pclOT::Ptr octree;					// Octree that holds centers of spheres.
		using nano_kdtree = nanoflann::KDTreeSingleIndexDynamicAdaptor<
										nanoflann::L2_Simple_Adaptor<double, SRpoint_cloud>,
										SRpoint_cloud, 3>;
		std::map<vec3d, int> sphere_map;	// Mapping between sphere centers and node id.
											// Because of this, we do not allow duplicate sphere centers.
		ivset cur_nodes;					// Current level's nodes. These nodes are ordered by sphere volume.
		ivset xmin_list;
		ivset xmax_list;
		ivset ymin_list;
		ivset ymax_list; 
		ivset zmin_list;
		ivset zmax_list;

		// =========== These data structures are real data representing this sphere tree.
		class node {
		public:
			std::vector<int> child;
			int parent;
			int level;	// height for leaf, 1 for root.

			property_render_geometry<SRsphere> S;
		};
		std::vector<node> tree;
		int root;
		int height;
		
		// Build this tree from input [ point_cloud ].
		// @multiplier : Amount of nodes that increase when level goes deeper.
		// 				If it is 4, it means that nodes increase in this order : 1 - 4 - 16 - 64 ...
		void build(const std::set<vec3d> &point_cloud, int multiplier = 4);

		// Build this tree from input [ sphere_cloud ].
		void build(const std::vector<SRsphere> &sphere_cloud, int multiplier = 4);

		// Build this tree from input [ mesh3 ].
		void build(const mesh3 &M, int multiplier = 4);

		void clear_minmax_list();
		void get_minmax(const SRsphere &S, double vals[6]);	// xmin, xmax, ymin, ymax, zmin, zmax.
		void insert_minmax_list(int id, const SRsphere &S);
		void delete_minmax_list(int id);

		// Find mutually best fitting nodes to merge in [ cur_nodes ].
		std::pair<int, int> find_best_fit(const nano_kdtree &kdtree);

		// Find best fit node to merge ( that incurs least amount of volume increase ) for the given node in the tree.
		int find_best_fit(const nano_kdtree &kdtree, int id, double &SV);

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