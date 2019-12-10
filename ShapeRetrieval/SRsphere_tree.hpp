#ifndef __SR_SPHERE_TREE_H__
#define __SR_SPHERE_TREE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "SRsphere.hpp"
#include "SRsphere_set.hpp"
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

			double volume;		// Volume sum of leaf nodes that belong to this node.

			property_render_geometry<SRsphere> S;
		};
		std::vector<node> tree;
		int root;
		int height;

		void load(const std::string &path);
		void save(const std::string &path);

		// Newly added volume...
		// void load2(const std::string &path);		
		// void save2(const std::string &path);

		void set_volume();	// Compute and set volume value of each nodes.
		
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

		// Return sphere set at certain level.
		SRsphere_set get_sphere_set(int level) const;

		void applyTR(const transform &TR);

		// Rendering functions.
		std::vector<int> render_nodes;		// Nodes that are currently rendered.
		void build_render(); 				// Set rendering info of each nodes.
		void destroy_render();				// Free rendering information.
		std::set<int> get_level_set(int) const;	// Get nodes at certain level.
		void get_level_set(int level, int &first, int &last) const;	// Get first and last index of certain level.
		void render_nodes_parent();			// Set [ render_nodes ] to parents of current ones.
		void render_nodes_child();			// Set [ render_nodes ] to child of current ones.
		void set_render_mode(int mode);		// Set render mode for spheres : 0 for wireframe, 1 for phong.
		virtual void render() const noexcept;
		virtual void render_ui();

		// ======================================== HAUSDORFF DIST ========================================= //
		struct align_var {
		public:
			// Variables needed to define alignment.
			double rx;
			double ry;
			double rz;	// Rotation around x, y, z axis.
			double tx;
			double ty;
			double tz;	// Translation along x, y, z axis.
			//double scale;	// Scale factor.

			align_var(double rx, double ry, double rz, double tx, double ty, double tz) :
				rx(rx), ry(ry), rz(rz), tx(tx), ty(ty), tz(tz) {}
		};
		// Compute pseudo MAX value of Hausdorff distance of another sphere tree against this tree ( both at same level ).
		// We do not implement another sphere set class, because we have to use tree structure for collision detection.
		double compute_pseudo_hausdorff(const SRsphere_tree &other, int level) const;

		// Compute pseudo Earth Moving Distance(EMD) between two trees.
		static double compute_pseudo_emd(const SRsphere_tree &a, const SRsphere_tree &b, int level);
		static double compute_pseudo_emd(const SRsphere_tree &a, const SRsphere_tree &b, int level, const transform &bTR);
		
		// Compute pseudo EMD multiplication between two trees.
		static double compute_pseudo_emd_mult(const SRsphere_tree &a, const SRsphere_tree &b, int level);
		static double compute_pseudo_emd_mult(const SRsphere_tree &a, const SRsphere_tree &b, int level, const transform &bTR);

		// Test : return residual spheres...
		static void test_pseudo_emd(const SRsphere_tree &a, const SRsphere_tree &b, int level, SRsphere_set &subA, SRsphere_set &subB);
		static void test_pseudo_hd(const SRsphere_tree &a, const SRsphere_tree &b, int level, SRsphere_set &subA, SRsphere_set &subB, vec3d &apt, vec3d &bpt);

		// Compute whole volume of spheres at certain level.
		double compute_level_volume(int level) const;
		// Compute specific pseudo EMD.
		static void compute_pseudo_emd_spec(const SRsphere_tree &a, const SRsphere_tree &b, int level, double &emd_a, double &emd_b);

		static transform alignTR(const align_var &param);
		// Align [ source ] model to [ base model ] using [ param ] variables.
		// We use non-linear optimization technique to find best alignment.
		static void align_emd(const SRsphere_tree &base, const SRsphere_tree &source, int level, align_var &param);
		// We use simple ICP technique to find best alignment.
		static void align_icp(const SRsphere_tree &base, const SRsphere_tree &source, int level, transform &TR);

		// Compute Chamfer's distance between two trees.
		static double computeCD(const SRsphere_tree &a, const SRsphere_tree &b, int level);
		static double computeEMD(const SRsphere_tree &a, const SRsphere_tree &b, int level);
	};
}

#endif