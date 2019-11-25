#include "SRsphere_tree.hpp"
#include <algorithm>
#include <math.h>

namespace AF {
    void SRsphere_tree::build(const std::set<vec3d> &point_cloud) {
		tree.clear();
		prev_nodes.clear();
		cur_nodes.clear();
		clear_minmax_list();
		
		for (auto it = point_cloud.begin(); it != point_cloud.end(); it++) {
			int id = (int)tree.size();
			const vec3d &pt = *it;
			node N;
			N.leaf = true;
			N.S.get_geometry().set_center(*it);
			N.S.get_geometry().set_radius(0);
			tree.push_back(N);
			prev_nodes.push_back(id);
		}

		do {
			while (!prev_nodes.empty()) {
				int id = prev_nodes.back();
				prev_nodes.pop_back();

				int bid = find_best_fit(id, true);
				auto find = std::find(prev_nodes.begin(), prev_nodes.end(), bid);
				bool is_cur_node = (find == prev_nodes.end());

				// @TODO : Fix later...
				if (is_cur_node) {
					tree.at(bid).S.get_geometry() = 
						SRsphere::merge(tree.at(id).S.get_geometry(), tree.at(bid).S.get_geometry());
					tree.at(bid).child.push_back(id);
					tree.at(id).parent = bid;
				}
				else {
					int nid = (int)tree.size();
					cur_nodes.push_back(nid);
					node N;
					N.leaf = false;
					N.S.get_geometry() = 
						SRsphere::merge(tree.at(id).S.get_geometry(), tree.at(bid).S.get_geometry());
					N.child.push_back(id);
					N.child.push_back(bid);

					tree.push_back(N);
					prev_nodes.erase(find);

					tree.at(id).parent = nid;
					tree.at(bid).parent = nid;
				}
			}
			prev_nodes = cur_nodes;
			cur_nodes.clear();
		} while (prev_nodes.size() > 1);
		root = tree.size() - 1;
	}

	void SRsphere_tree::clear_minmax_list() {
		xmin_list.clear();
		xmax_list.clear();
		ymin_list.clear();
		ymax_list.clear();
		zmin_list.clear();
		zmax_list.clear();
	}

	void SRsphere_tree::get_minmax(const SRsphere &S, double vals[6]) {
		vals[0] = S.get_center()[0] - S.get_radius();
        vals[1] = S.get_center()[0] + S.get_radius();
        vals[2] = S.get_center()[1] - S.get_radius();
        vals[3] = S.get_center()[1] + S.get_radius();
        vals[4] = S.get_center()[2] - S.get_radius();
        vals[5] = S.get_center()[2] + S.get_radius();
	}

	void SRsphere_tree::insert_minmax_list(int id, const SRsphere &S) {
		double vals[6];
		get_minmax(S, vals);

		xmin_list.insert(ivpair(id, vals[0]));
		xmax_list.insert(ivpair(id, vals[1]));
		ymin_list.insert(ivpair(id, vals[2]));
		ymax_list.insert(ivpair(id, vals[3]));
		zmin_list.insert(ivpair(id, vals[4]));
		zmax_list.insert(ivpair(id, vals[5]));
	}

	void SRsphere_tree::delete_minmax_list(int id) {
		double vals[6];
		get_minmax(tree.at(id).S.get_geometry(), vals);

		xmin_list.erase(ivpair(id, vals[0]));
		xmax_list.erase(ivpair(id, vals[1]));
		ymin_list.erase(ivpair(id, vals[2]));
		ymax_list.erase(ivpair(id, vals[3]));
		zmin_list.erase(ivpair(id, vals[4]));
		zmax_list.erase(ivpair(id, vals[5]));
	}

	int SRsphere_tree::find_best_fit(int id, bool include_cur_nodes) {
		std::set<int> checked_nodes;	// Nodes that we have checked.
		ivpair best_fit(id, 1e+10);		// [ best fit node's id, surplus volume ].
		double valid_offset = 0;

		const sphere &tS = tree.at(id).S.get_geometry();
		double cx = tS.get_center()[0];
		double cy = tS.get_center()[1];
		double cz = tS.get_center()[2];

		double vxmin, vxmax, vymin, vymax, vzmin, vzmax;
		vxmin = vxmax = cx;
		vymin = vymax = cy;
		vzmin = vzmax = cz;

		bool first = true;
		for (auto it = prev_nodes.begin(); it != prev_nodes.end(); it++) {
			int test_id = *it;
			if (test_id == id)
				continue;

			double vals[6];
			get_minmax(tree.at(test_id).S.get_geometry(), vals);

			if (!first) {
				if (vals[0] > vxmax || vals[1] < vxmin ||
					vals[2] > vymax || vals[3] < vymin ||
					vals[4] > vzmax || vals[5] < vzmin)
					continue;
			}
			else
				first = false;

			double surplus_volume = compute_surplus_volume(id, test_id);
			if (surplus_volume < best_fit.second) {
				best_fit.first = test_id;
				best_fit.second = surplus_volume;
				valid_offset = compute_valid_offset(id, best_fit.second);

				vxmax = cx + valid_offset;
				vxmin = cx - valid_offset;
				vymax = cy + valid_offset;
				vymin = cy - valid_offset;
				vzmax = cz + valid_offset;
				vzmin = cz - valid_offset;
			}
		}

		if (!include_cur_nodes)
			return best_fit.first;

		for (auto it = cur_nodes.begin(); it != cur_nodes.end(); it++) {
			int test_id = *it;
			if (test_id == id)
				continue;

			double vals[6];
			get_minmax(tree.at(test_id).S.get_geometry(), vals);

			if (!first) {
				if (vals[0] > vxmax || vals[1] < vxmin ||
					vals[2] > vymax || vals[3] < vymin ||
					vals[4] > vzmax || vals[5] < vzmin)
					continue;
			}
			else
				first = false;

			double surplus_volume = compute_surplus_volume(id, test_id);
			if (surplus_volume < best_fit.second) {
				best_fit.first = test_id;
				best_fit.second = surplus_volume;
				valid_offset = compute_valid_offset(id, best_fit.second);

				vxmax = cx + valid_offset;
				vxmin = cx - valid_offset;
				vymax = cy + valid_offset;
				vymin = cy - valid_offset;
				vzmax = cz + valid_offset;
				vzmin = cz - valid_offset;
			}
		}

		return best_fit.first;
		//checked_nodes.insert(id);		// Do not check given node itself.

		//// Choose random node for the first test.
		//int test_id;
		//if (id == 0)
		//	test_id = 1;
		//else
		//	test_id = prev_nodes.at(0);

		//bool first = true;
		//std::set<int> feasible_set;

		//while (true) {
		//	double surplus_volume = compute_surplus_volume(id, test_id);
		//	if (surplus_volume < best_fit.second) {
		//		// update [ best_fit ].
		//		best_fit.first = test_id;
		//		best_fit.second = surplus_volume;
		//		valid_offset = compute_valid_offset(id, best_fit.second);
		//	}
		//	checked_nodes.insert(test_id);

		//	// update feasible set.
		//	if (first) {
		//		first = false;


		//	}
		//}
	}

	/*int sphere_tree::find_closest_node(int id, int offset, bool include_cur_nodes, int sample_num) {
		ivpair closest;
		if (include_cur_nodes) {
			int prev_sample_num = sample_num * (prev_nodes.size() / (prev_nodes.size() + cur_nodes.size()));
			int cur_sample_num = sample_num - prev_sample_num;
			int poffset = prev_nodes.size() / prev_sample_num;
			int coffset = cur_nodes.size() / cur_sample_num;
			int pid = 0;
			for (int i = 0; i < prev_sample_num; i++) {
			}
		}
		else {
		}
	}*/

	double SRsphere_tree::compute_surplus_volume(int a, int b) const {
		const SRsphere 
			&sA = tree.at(a).S.get_geometry_c(), 
			&sB = tree.at(b).S.get_geometry_c();
		SRsphere merge = SRsphere::merge(sA, sB);
		if (merge.get_radius() == sA.get_radius() || merge.get_radius() == sB.get_radius())	// One includes the other
			return 0;
		else {
			double
				vA = sA.volume(),
				vB = sB.volume(),
				vM = merge.volume(),
				ret = vM - vA - vB,
				overlap_len = sA.get_radius() + sB.get_radius() - (sA.get_center() - sB.get_center()).len();
			if (overlap_len > 0) {
				// If they overlap...
				// @TODO : We can do better than this...
				return ret - overlap_len * (sA.get_radius() > sB.get_radius() ? sB.get_radius() : sA.get_radius());
			}
			else 
				return ret;
		}
	}

	double SRsphere_tree::compute_valid_offset(int id, double surplus_volume) const {
		double cur_volume = tree.at(id).S.get_geometry_c().volume();
		double total_volume = cur_volume + surplus_volume;
		return pow((total_volume / pi) * 0.5, 1 / 3);
	}

	// Rendering.
	void SRsphere_tree::build_render() {
		for(auto it = tree.begin(); it != tree.end(); it++) {
			auto &S = it->S.get_geometry();
			it->S.set_shader(this->get_shader());
			it->S.build_BO_mesh3(S.get_mesh3());
			it->S.get_config().M = it->S.get_config().WIREFRAME;	
		}
		render_nodes.clear();
		render_nodes.push_back(root);
	}
	void SRsphere_tree::render_nodes_parent() {
		std::vector<int> pnodes;
		for(auto it = render_nodes.begin(); it != render_nodes.end(); it++) {
			int parent = tree.at(*it).parent;
			if(parent > 0 && parent < tree.size())
				pnodes.push_back(parent);
		}			
		if(!pnodes.empty())
			render_nodes = pnodes;
	}
	void SRsphere_tree::render_nodes_child() {
		std::vector<int> cnodes;
		for(auto it = render_nodes.begin(); it != render_nodes.end(); it++) {
			const auto &child = tree.at(*it).child;
			if(child.empty())
				cnodes.push_back(*it);
			else
				cnodes.insert(cnodes.end(), child.begin(), child.end());			
		}
		render_nodes = cnodes;
	}
	void SRsphere_tree::render() const noexcept {
		for(auto it = render_nodes.begin(); it != render_nodes.end(); it++) {
			tree.at(*it).S.render();
		}
	}
	void SRsphere_tree::render_ui() {
		return;
	}
}