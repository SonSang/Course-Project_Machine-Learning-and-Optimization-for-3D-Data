#include "SRsphere_tree.hpp"
#include "../Dependencies/nanoflann/examples/utils.hpp"
#include <algorithm>
#include <math.h>

namespace AF {
	SRpoint_cloud kdpc;

	SRpoint_cloud get_nf_point_cloud(const std::set<vec3d> &pc) {
		SRpoint_cloud ret;
		ret.pc.resize(pc.size());
		int i = 0;
		for(auto it = pc.begin(); it != pc.end(); it++)
			ret.pc[i++] = *it;
		return ret;
	}

    void SRsphere_tree::build(const std::set<vec3d> &point_cloud, int multiplier) {
		tree.clear();
		cur_nodes.clear();
		//sphere_map.clear();

		// K-D tree init.
		// std::map<vec3d, int> kdpcmap;
		kdpc.pc.resize(point_cloud.size());
		int i = 0;
		for(auto it = point_cloud.begin(); it != point_cloud.end(); it++) {
			kdpc.pc[i] = *it;
			//kdpcmap.insert({*it, i});
			i++;
		}
		nano_kdtree kdtree(3, kdpc, nanoflann::KDTreeSingleIndexAdaptorParams(10));

		// Number of nodes for current level.
		int cur_node_size = 1;
		height = 1;
		while(cur_node_size < point_cloud.size()) {
			cur_node_size *= multiplier;
			height++;
		}			
		cur_node_size /= multiplier;

		if(cur_node_size > 1024) {
			cur_node_size = 1024;
			multiplier = 4;
			height = 7;
		}

		int cur_level = height;
		for (auto it = point_cloud.begin(); it != point_cloud.end(); it++) {
			int id = (int)tree.size();
			
			node N;
			N.level = cur_level;
			N.S.get_geometry().set_center(*it);
			N.S.get_geometry().set_radius(1e-3);
			tree.push_back(N);

			cur_nodes.insert({id, N.S.get_geometry().volume()});
			//sphere_map.insert({N.S.get_geometry().get_center(), id});
		}

		do {
			std::set<int> new_nodes;	// Newly added [ cur_nodes ] in this step.
			cur_level--;
			while (cur_nodes.size() > cur_node_size) {
				// Find best fitting nodes.
				std::pair<int, int> best_fit = find_best_fit(kdtree);

				// Merge best fitting nodes and feed them back.
				int aid = best_fit.first, bid = best_fit.second;
				bool aid_new_nodes, bid_new_nodes;	// Indicate whether best fitting nodes are newly create nodes in this level.
													// We need this, because if we merge newly create nodes, we have to make new nodes
													// that have appropriate child information.

				aid_new_nodes = (new_nodes.find(aid) != new_nodes.end());
				bid_new_nodes = (new_nodes.find(bid) != new_nodes.end());

				int nid = (int)tree.size();
				node N;
				N.level = cur_level;
				N.S.set_geometry(SRsphere::merge(tree.at(aid).S.get_geometry(), tree.at(bid).S.get_geometry()));
				if(aid_new_nodes && bid_new_nodes) {
					N.child.insert(N.child.end(), tree.at(aid).child.begin(), tree.at(aid).child.end());
					N.child.insert(N.child.end(), tree.at(bid).child.begin(), tree.at(bid).child.end());

					for(auto it = tree.at(aid).child.begin(); it != tree.at(aid).child.end(); it++) 
						tree.at(*it).parent = nid;
					for(auto it = tree.at(bid).child.begin(); it != tree.at(bid).child.end(); it++) 
						tree.at(*it).parent = nid;

					new_nodes.erase(aid);
					new_nodes.erase(bid);
				}
				else if(aid_new_nodes && !bid_new_nodes) {
					N.child.insert(N.child.end(), tree.at(aid).child.begin(), tree.at(aid).child.end());
					N.child.push_back(bid);
					
					for(auto it = tree.at(aid).child.begin(); it != tree.at(aid).child.end(); it++) 
						tree.at(*it).parent = nid;
					tree.at(bid).parent = nid;

					new_nodes.erase(aid);
				}
				else if(!aid_new_nodes && bid_new_nodes) {
					N.child.push_back(aid);
					N.child.insert(N.child.end(), tree.at(bid).child.begin(), tree.at(bid).child.end());

					tree.at(aid).parent = nid;
					for(auto it = tree.at(bid).child.begin(); it != tree.at(bid).child.end(); it++) 
						tree.at(*it).parent = nid;

					new_nodes.erase(bid);
				}
				else {
					N.child.push_back(aid);
					N.child.push_back(bid);

					tree.at(aid).parent = nid;
					tree.at(bid).parent = nid;
				}
				new_nodes.insert(nid);

				// Update [ tree ], [ cur_nodes ], [ kdtree ], [ minmax ], [ sphere_map ].
				tree.push_back(N);

				cur_nodes.erase({aid, tree.at(aid).S.get_geometry().volume()});
				cur_nodes.erase({bid, tree.at(bid).S.get_geometry().volume()});
				cur_nodes.insert({nid, N.S.get_geometry().volume()});

				//int removeAid = kdpcmap.at(tree.at(aid).S.get_geometry().get_center());
				//int removeBid = kdpcmap.at(tree.at(bid).S.get_geometry().get_center());
				// int removeAid = -1, removeBid = -1;
				// for(int i = 0; i < kdpc.pc.size(); i++) {
				// 	if(kdpc.pc[i] == tree.at(aid).S.get_geometry().get_center())
				// 		removeAid = i;
				// 	if(kdpc.pc[i] == tree.at(bid).S.get_geometry().get_center())
				// 		removeBid = i;
				// 	//if(removeAid != -1 && removeBid != -1) break;
				// } 
				//kdtree.removePoint(removeAid);
				//kdtree.removePoint(removeBid);
				kdtree.removePoint(aid);
				kdtree.removePoint(bid);

				kdpc.pc.push_back(tree.at(nid).S.get_geometry().get_center());
				//kdpcmap.insert({N.S.get_geometry().get_center(), kdpc.pc.size() - 1});
				kdtree.addPoints(kdpc.pc.size() - 1, kdpc.pc.size() - 1);

				// sphere_map.erase(tree.at(aid).S.get_geometry().get_center());
				// sphere_map.erase(tree.at(bid).S.get_geometry().get_center());
				// sphere_map.insert({tree.at(nid).S.get_geometry().get_center(), nid});

				if(cur_nodes.size() % 100 == 0)
					std::cout<<"Current node size : "<<cur_nodes.size()<<std::endl;
			}
			auto copy = cur_nodes;
			for(auto it = cur_nodes.begin(); it != cur_nodes.end(); it++) {
				if(tree.at(it->first).level == cur_level + 1) {
					int nid = (int)tree.size();
					node N = tree.at(it->first);
					N.level = cur_level;
					N.child.clear();
					N.child.push_back(it->first);
					
					tree.at(it->first).parent = nid;
					
					tree.push_back(N);
					kdtree.removePoint(it->first);
					kdpc.pc.push_back(N.S.get_geometry().get_center());
					kdtree.addPoints(kdpc.pc.size() - 1, kdpc.pc.size() - 1);

					copy.erase(*it);
					copy.insert({nid, N.S.get_geometry().volume()});
					//sphere_map.at(N.S.get_geometry().get_center()) = nid;
				}
			}
			cur_nodes = copy;

			cur_node_size /= multiplier;
		} while (cur_node_size > 0);
		root = tree.size() - 1;
	}

	void SRsphere_tree::build(const std::vector<SRsphere> &sphere_cloud, int multiplier) {
		tree.clear();
		cur_nodes.clear();
		sphere_map.clear();

		// K-D tree init.
		std::set<vec3d> point_cloud;
		for(auto it = sphere_cloud.begin(); it != sphere_cloud.end(); it++) 
			point_cloud.insert(it->get_center());
		kdpc = get_nf_point_cloud(point_cloud);	// Point cloud for KDtree.
		nano_kdtree kdtree(3, kdpc, nanoflann::KDTreeSingleIndexAdaptorParams(10));

		// Number of nodes for current level.
		int cur_node_size = 1;
		height = 1;
		while(cur_node_size < sphere_cloud.size()) {
			cur_node_size *= multiplier;
			height++;
		}			
		cur_node_size /= multiplier;

		if(cur_node_size > 1024) {
			cur_node_size = 1024;
			multiplier = 4;
			height = 7;
		}

		int cur_level = height;
		for (auto it = sphere_cloud.begin(); it != sphere_cloud.end(); it++) {
			int id = (int)tree.size();
			
			node N;
			N.level = cur_level;
			N.S.set_geometry(*it);
			tree.push_back(N);

			cur_nodes.insert({id, it->volume()});
			sphere_map.insert({it->get_center(), id});
		}

		// std::vector<node> real_tree;
		// std::map<int, int> tmp_real_tree_map;

		do {
			std::set<int> new_nodes;	// Newly added [ cur_nodes ] in this step.
			cur_level--;
			while (cur_nodes.size() > cur_node_size) {
				// Find best fitting nodes.
				std::pair<int, int> best_fit = find_best_fit(kdtree);

				// Merge best fitting nodes and feed them back.
				int aid = best_fit.first, bid = best_fit.second;
				bool aid_new_nodes, bid_new_nodes;	// Indicate whether best fitting nodes are newly create nodes in this level.
													// We need this, because if we merge newly create nodes, we have to make new nodes
													// that have appropriate child information.

				aid_new_nodes = (new_nodes.find(aid) != new_nodes.end());
				bid_new_nodes = (new_nodes.find(bid) != new_nodes.end());

				int nid = (int)tree.size();
				node N;
				N.level = cur_level;
				N.S.set_geometry(SRsphere::merge(tree.at(aid).S.get_geometry(), tree.at(bid).S.get_geometry()));
				if(aid_new_nodes && bid_new_nodes) {
					N.child.insert(N.child.end(), tree.at(aid).child.begin(), tree.at(aid).child.end());
					N.child.insert(N.child.end(), tree.at(bid).child.begin(), tree.at(bid).child.end());

					for(auto it = tree.at(aid).child.begin(); it != tree.at(aid).child.end(); it++) 
						tree.at(*it).parent = nid;
					for(auto it = tree.at(bid).child.begin(); it != tree.at(bid).child.end(); it++) 
						tree.at(*it).parent = nid;

					new_nodes.erase(aid);
					new_nodes.erase(bid);
				}
				else if(aid_new_nodes && !bid_new_nodes) {
					N.child.insert(N.child.end(), tree.at(aid).child.begin(), tree.at(aid).child.end());
					N.child.push_back(bid);
					
					for(auto it = tree.at(aid).child.begin(); it != tree.at(aid).child.end(); it++) 
						tree.at(*it).parent = nid;
					tree.at(bid).parent = nid;

					new_nodes.erase(aid);
				}
				else if(!aid_new_nodes && bid_new_nodes) {
					N.child.push_back(aid);
					N.child.insert(N.child.end(), tree.at(bid).child.begin(), tree.at(bid).child.end());

					tree.at(aid).parent = nid;
					for(auto it = tree.at(bid).child.begin(); it != tree.at(bid).child.end(); it++) 
						tree.at(*it).parent = nid;

					new_nodes.erase(bid);
				}
				else {
					N.child.push_back(aid);
					N.child.push_back(bid);

					tree.at(aid).parent = nid;
					tree.at(bid).parent = nid;
				}
				new_nodes.insert(nid);

				// Update [ tree ], [ cur_nodes ], [ kdtree ], [ minmax ], [ sphere_map ].
				tree.push_back(N);

				cur_nodes.erase({aid, tree.at(aid).S.get_geometry().volume()});
				cur_nodes.erase({bid, tree.at(bid).S.get_geometry().volume()});
				cur_nodes.insert({nid, N.S.get_geometry().volume()});

				int removeAid = -1, removeBid = -1;
				for(int i = 0; i < kdpc.pc.size(); i++) {
					if(kdpc.pc[i] == tree.at(aid).S.get_geometry().get_center())
						removeAid = i;
					if(kdpc.pc[i] == tree.at(bid).S.get_geometry().get_center())
						removeBid = i;
					//if(removeAid != -1 && removeBid != -1) break;
				} 
				kdtree.removePoint(removeAid);
				kdtree.removePoint(removeBid);

				kdpc.pc.push_back(tree.at(nid).S.get_geometry().get_center());
				kdtree.addPoints(kdpc.pc.size() - 1, kdpc.pc.size() - 1);

				// delete_minmax_list(aid);
				// delete_minmax_list(bid);
				// insert_minmax_list(nid, tree.at(nid).S.get_geometry());

				sphere_map.erase(tree.at(aid).S.get_geometry().get_center());
				sphere_map.erase(tree.at(bid).S.get_geometry().get_center());
				sphere_map.insert({tree.at(nid).S.get_geometry().get_center(), nid});

				if(cur_nodes.size() % 100 == 0)
					std::cout<<"Current node size : "<<cur_nodes.size()<<std::endl;
			}
			auto copy = cur_nodes;
			for(auto it = cur_nodes.begin(); it != cur_nodes.end(); it++) {
				if(tree.at(it->first).level == cur_level + 1) {
					int nid = (int)tree.size();
					node N = tree.at(it->first);
					N.level = cur_level;
					N.child.clear();
					N.child.push_back(it->first);
					
					tree.at(it->first).parent = nid;
					
					tree.push_back(N);
					copy.erase(*it);
					copy.insert({nid, N.S.get_geometry().volume()});
					sphere_map.at(N.S.get_geometry().get_center()) = nid;
				}
			}
			cur_nodes = copy;
			// std::cout<<"Current node size : "<<cur_nodes.size()<<std::endl;
			// // insert into real tree.
			// for(auto it = cur_nodes.begin(); it != cur_nodes.end(); it++) {
			// 	int rid = (int)real_tree.size();
			// 	node N = tree.at(it->first);
			// 	if((height == 7 && N.level == 6) || N.level == height)
			// 		N.child.clear();
			// 	else {
			// 		std::vector<int> child;
			// 		for(auto it = N.child.begin(); it != N.child.end(); it++) {
			// 			int real_child = tmp_real_tree_map.at(*it);
			// 			child.push_back(real_child);
			// 			real_tree.at(real_child).parent = rid;
			// 		}
			// 		N.child = child;
			// 	}
			// 	real_tree.push_back(N);
			// 	tmp_real_tree_map.insert({it->first, rid});
			// }

			cur_node_size /= multiplier;
		} while (cur_node_size > 0);
		//tree = real_tree;
		root = tree.size() - 1;
	}

	void SRsphere_tree::build(const mesh3 &M, int multiplier) {
		std::vector<SRsphere> sc;
		std::cout<<"Build tree, # of faces : "<<M.get_faces_c().size()<<std::endl;
		for(auto it = M.get_faces_c().begin(); it != M.get_faces_c().end(); it++) {
			const vec3d 
				&a = M.get_vertices_c().at(it->at(0)),
				&b = M.get_vertices_c().at(it->at(1)),
				&c = M.get_vertices_c().at(it->at(2));
			SRsphere S;
			S.set_center((a + b + c) / 3.0);
			S.set_radius((S.get_center() - a).len());
			sc.push_back(S);
		}
		build(sc, multiplier);
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

	std::pair<int, int> SRsphere_tree::find_best_fit(const nano_kdtree &kdtree) {
		std::pair<int, int> best;
		double bestSV = 1e+10;	//	Least surplus volume. We store for the case when there is no mutually best fit at all...

		int aid, bid, cid;
		double SV0, SV1;
		
		int max_iter = 5;		// @TODO : for speed up...
		int iter = 0;
		
		for(auto it = cur_nodes.begin(); it != cur_nodes.end(); it++) {
			aid = it->first;		// Select sphere in ascending order.
			bid = find_best_fit(kdtree, aid, SV0);
			best.first = aid;
			best.second = bid;
			return best;
			// cid = find_best_fit(kdtree, bid, SV1);
			// if(aid != cid) {
			// 	if(SV0 < SV1 && SV0 < bestSV)
			// 		best = std::pair<int, int>(aid, bid);
			// 	else if(SV1 < SV0 && SV1 < bestSV)
			// 		best = std::pair<int, int>(cid, bid);
			// 	continue;
			// }
			// else {
			// 	// Since [ aid ] and [ bid ] are mutually best fit, return them.
			// 	return std::pair<int, int>(aid, bid);
			// }
			// if(++iter == max_iter)
			// 	break;
		}
		// There is no mutually best fit at all...
		return best;
	}

	int SRsphere_tree::find_best_fit(const nano_kdtree &kdtree, int id, double &SV) {
		vec3d cenv = tree.at(id).S.get_geometry().get_center();
		double cenvd[3] = { cenv[0], cenv[1], cenv[2] };

		// 1. We can use rough estimation of best fit node by using kdtree's nearest search algorithm.
		ivpair bestfit = {-1, 1e+10};
		int guess;
		{
			// Find best fit among previous nodes.
			size_t k = 3;
			size_t ret_index[3];
			double ret_dist[3];

			nanoflann::KNNResultSet<double> resultSet(k);
			resultSet.init(ret_index, ret_dist);
			kdtree.findNeighbors(resultSet, cenvd, nanoflann::SearchParams(10));
			
			// std::vector<int> nearest;
			// std::vector<float> distance;
			// octree->nearestKSearch(cenp, k, nearest, distance);
			//octree->approxNearestSearch(get_pcl_point(cenv), bestfit.first, dist);
			for(int i = 0; i < k; i++) {
				if(ret_index[i] >= kdpc.pc.size())
					continue;
				vec3d approxv = kdpc.pc.at(ret_index[i]);
				// pclPT approx_nearest_cen = octree->getInputCloud()->at(nearest.at(i));
				// if(pcl_point_same(cenp, approx_nearest_cen))
				// 	continue;
				if(approxv == cenv)
					continue;
				//int find = sphere_map.find(approxv)->second;
				double SV = compute_surplus_volume(id, ret_index[i]);
				if(SV < bestfit.second) {
					bestfit.first = guess = ret_index[i];
					bestfit.second = SV;
				}
			}
		}
		return bestfit.first;
		
		// double valid_offset = compute_valid_offset(id, bestfit.second);

		// double cx = cenv[0];
		// double cy = cenv[1];
		// double cz = cenv[2];

		// double vxmin, vxmax, vymin, vymax, vzmin, vzmax;
		// vxmin = cx - valid_offset; vxmax = cx + valid_offset;
		// vymin = cy - valid_offset; vymax = cy + valid_offset;
		// vzmin = cz - valid_offset; vzmax = cz + valid_offset;

		// // 2. Extract only nodes that fall into [ vmin, vmax ] domain.
		// std::set<int> valid_nodes;
		// for(auto it = cur_nodes.begin(); it != cur_nodes.end(); it++)
		// 	valid_nodes.insert(it->first);
		// valid_nodes.erase(id);
		// {
		// 	// Delete invalid nodes from [ x ] domain.
		// 	{
		// 		auto it = xmax_list.insert({-1, vxmin}).first;
		// 		for(auto it2 = xmax_list.begin(); it2 != it; it2++)
		// 			valid_nodes.erase(it2->first);
		// 		xmax_list.erase(it);
		// 	}
		// 	{
		// 		auto it = xmin_list.insert({-1, vxmax}).first;
		// 		for(auto it2 = it; it2 != xmin_list.end(); it2++)
		// 			valid_nodes.erase(it2->first);
		// 		xmin_list.erase(it);
		// 	}
		// 	// Delete invalid nodes from [ y ] domain.
		// 	{
		// 		auto it = ymax_list.insert({-1, vymin}).first;
		// 		for(auto it2 = ymax_list.begin(); it2 != it; it2++)
		// 			valid_nodes.erase(it2->first);
		// 		ymax_list.erase(it);
		// 	}
		// 	{
		// 		auto it = ymin_list.insert({-1, vymax}).first;
		// 		for(auto it2 = it; it2 != ymin_list.end(); it2++)
		// 			valid_nodes.erase(it2->first);
		// 		ymin_list.erase(it);
		// 	}
		// 	// Delete invalid nodes from [ z ] domain.
		// 	{
		// 		auto it = zmax_list.insert({-1, vzmin}).first;
		// 		for(auto it2 = zmax_list.begin(); it2 != it; it2++)
		// 			valid_nodes.erase(it2->first);
		// 		zmax_list.erase(it);
		// 	}
		// 	{
		// 		auto it = zmin_list.insert({-1, vzmax}).first;
		// 		for(auto it2 = it; it2 != zmin_list.end(); it2++)
		// 			valid_nodes.erase(it2->first);
		// 		zmin_list.erase(it);
		// 	}
		// }

		// // 3. Check remaining valid nodes for best fit.
		// for(auto it = valid_nodes.begin(); it != valid_nodes.end(); it++) {
		// 	if(*it == guess)
		// 		continue;
		// 	double SV = compute_surplus_volume(id, *it);
		// 	if(SV < bestfit.second) {
		// 		bestfit.first = *it;
		// 		bestfit.second = SV;
		// 	}
		// }

		// //octree->addPointFromCloud(find_id, nullptr);

		// SV = bestfit.second;

		// return bestfit.first;
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
		return pow((total_volume / pi) * 0.75, 1.0 / 3.0);
	}

	// Rendering.
	void SRsphere_tree::build_render() {
		for(auto it = tree.begin(); it != tree.end(); it++) {
			if(it->level >= 7)
				continue;
			auto &S = it->S.get_geometry();
			it->S.set_shader(this->get_shader());
			it->S.build_BO_mesh3(S.get_mesh2());
			it->S.get_config().M = it->S.get_config().WIREFRAME;	
		}
		render_nodes.clear();
		render_nodes.push_back(root);
	}
	void SRsphere_tree::render_nodes_parent() {
		int cur_level = tree.at(render_nodes.at(0)).level;
		if(cur_level <= 1)
			return;

		render_nodes.clear();
		cur_level--;
		for(auto it = tree.begin(); it != tree.end(); it++) {
			if(it->level == cur_level) {
				if(tree.at(it->child.at(0)).parent == std::distance(tree.begin(), it))
					render_nodes.push_back(std::distance(tree.begin(), it));
			}
		}
		// std::set<int> rn;
		// for(auto it= render_nodes.begin(); it != render_nodes.end(); it++) {
		// 	rn.insert(tree.at(*it).parent);
		// }
		// if(!rn.empty()) {
		// 	render_nodes.clear();
		// 	render_nodes.insert(render_nodes.end(), rn.begin(), rn.end());
		// }
		std::cout<<"# of nodes : "<<render_nodes.size()<<std::endl;
	}
	void SRsphere_tree::render_nodes_child() {
		int cur_level = tree.at(render_nodes.at(0)).level;
		if(cur_level >= 6)
			return;

		render_nodes.clear();
		cur_level++;
		for(auto it = tree.begin(); it != tree.end(); it++) {
			if(it->level == cur_level) {
				if(tree.at(it->child.at(0)).parent == std::distance(tree.begin(), it))
					render_nodes.push_back(std::distance(tree.begin(), it));
			}
		}
		// std::vector<int> rn;
		// for(auto it = render_nodes.begin(); it != render_nodes.end(); it++) {
		// 	rn.insert(rn.end(), tree.at(*it).child.begin(), tree.at(*it).child.end());
		// }
		// if(!rn.empty())
		// 	render_nodes = rn;
		std::cout<<"# of nodes : "<<render_nodes.size()<<std::endl;
	}
	void SRsphere_tree::set_render_mode(int mode) {
		for(auto it = tree.begin(); it != tree.end(); it++) {
			auto &S = it->S.get_geometry();
			if(mode == 0)
				it->S.get_config().M = it->S.get_config().WIREFRAME;	
			else 
				it->S.get_config().M = it->S.get_config().PHONG;			
		}
	}
	void SRsphere_tree::render() const noexcept {
		for(auto it = render_nodes.begin(); it != render_nodes.end(); it++) {
			if(tree.at(*it).level >= 7) 
				continue;
			tree.at(*it).S.render();
		}
	}
	void SRsphere_tree::render_ui() {
		return;
	}
}