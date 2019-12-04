#include "SRsphere_tree.hpp"

// For building sphere tree.
#include "../Dependencies/nanoflann/examples/utils.h"

// For nonlinear optimization.
// #include "dlib/optimization.h"
// #include "dlib/global_optimization.h"

// For ICP method.
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/registration/icp.h>

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
		
		// K-D tree init.
		kdpc.pc.resize(sphere_cloud.size());
		int i = 0;
		for(auto it = sphere_cloud.begin(); it != sphere_cloud.end(); it++) {
			kdpc.pc[i] = it->get_center();
			i++;
		}
		nano_kdtree kdtree(3, kdpc, nanoflann::KDTreeSingleIndexAdaptorParams(10));

		// Number of nodes for current level.
		int cur_node_size = 1;
		height = 1;
			while(cur_node_size <= sphere_cloud.size()) {
			cur_node_size *= multiplier;
			height++;
		}			
		cur_node_size /= multiplier;	// Since we do not include original model data in this tree,
		height--;						// we have to exclude the final level from this tree.

		if(cur_node_size > 1024) {		// # of leaf nodes should not exceed 1024.
			cur_node_size = 1024;
			height = 6;
		}

		int cur_level = height + 1;		// Original model data. This will be culled out in the last stage of this algorithm.
		for (auto it = sphere_cloud.begin(); it != sphere_cloud.end(); it++) {
			int id = (int)tree.size();
			
			node N;
			N.level = cur_level;
			N.S.set_geometry(*it);
			tree.push_back(N);

			cur_nodes.insert({id, N.S.get_geometry().volume()});
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

				kdtree.removePoint(aid);
				kdtree.removePoint(bid);

				kdpc.pc.push_back(tree.at(nid).S.get_geometry().get_center());
				kdtree.addPoints(kdpc.pc.size() - 1, kdpc.pc.size() - 1);

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
				}
			}
			cur_nodes = copy;

			cur_node_size /= multiplier;
		} while (cur_node_size > 0);
		root = tree.size() - 1;

		// Now, we have to cull out unnecessary data structure...
		int nsize = 0, nadd = 1;
		for(int i = 0; i < height; i++) {
			nsize += nadd;
			nadd *= multiplier;
		}
		std::vector<node> ntree;
		ntree.resize(nsize);

		struct torganizer {
			int treeId;
			int parentId;
			int myId;
			torganizer(int a, int b, int c) : treeId(a), parentId(b), myId(c){}
		};
		std::deque<torganizer> queue;
		queue.push_back(torganizer(root, -1, 0));	// 0 for root
		int curId = 1;
		while(!queue.empty()) {
			auto item = queue.front(); queue.pop_front();
			node N = tree.at(item.treeId);
			N.parent = item.parentId;
			
			if(N.level < height) {
				std::vector<int> nchild;
				for(auto it = N.child.begin(); it != N.child.end(); it++) {
					queue.push_back(torganizer(*it, item.myId, curId));
					nchild.push_back(curId);
					curId++;
				}
				N.child = nchild;
			}
			else 
				N.child.clear();

			ntree.at(item.myId) = N;
		}
		tree = ntree;
		root = 0;
	}

	SRsphere triangle_sphere(const vec3d &a, const vec3d &b, const vec3d &c) {
		SRsphere S;
		S.set_center((a + b + c) / 3.0);
		double ra = (S.get_center() - a).lensq();
		double rb = (S.get_center() - b).lensq();
		double rc = (S.get_center() - c).lensq();
		if(ra >= rb) {
			if(ra >= rc) S.set_radius(sqrt(ra));
			else S.set_radius(sqrt(rc));
		}
		else {
			if(rb >= rc) S.set_radius(sqrt(rb));
			else S.set_radius(sqrt(rc));
		}
		return S;
	}
	// Subdivide given triangle until each segment could be covered with sphere of radius [ radius ].
	void subdivide_triangle(const vec3d &a, const vec3d &b, const vec3d &c, std::vector<SRsphere> &sc, double radius) {
		double
			len0 = (a - b).lensq(),
			len1 = (b - c).lensq(),
			len2 = (c - a).lensq();
		vec3d d;
		SRsphere S1, S2;
		if(len0 >= len1 && len0 >= len2) {
			d = (a + b) * 0.5;
			S1 = triangle_sphere(d, b, c);
			S2 = triangle_sphere(d, a, c);
			if(S1.get_radius() <= radius) sc.push_back(S1);
			else subdivide_triangle(d, b, c, sc, radius);
			if(S2.get_radius() <= radius) sc.push_back(S2);
			else subdivide_triangle(d, a, c, sc, radius);
		}
		else if(len1 >= len0 && len1 >= len2) {
			d = (b + c) * 0.5;
			S1 = triangle_sphere(d, a, b);
			S2 = triangle_sphere(d, a, c);
			if(S1.get_radius() <= radius) sc.push_back(S1);
			else subdivide_triangle(d, a, b, sc, radius);
			if(S2.get_radius() <= radius) sc.push_back(S2);
			else subdivide_triangle(d, a, c, sc, radius);
		}			
		else {
			d = (c + a) * 0.5;
			S1 = triangle_sphere(d, a, b);
			S2 = triangle_sphere(d, c, b);
			if(S1.get_radius() <= radius) sc.push_back(S1);
			else subdivide_triangle(d, a, b, sc, radius);
			if(S2.get_radius() <= radius) sc.push_back(S2);
			else subdivide_triangle(d, c, b, sc, radius);
		}	
	}
	// Regularize triangles in the mesh [ M ] and wrap them up with sphere cloud [ sc ].
	void regularize_mesh3(const mesh3 &M, std::vector<SRsphere> &sc, double absrad = 0.1) {
		double avgradius = 0;
		std::vector<SRsphere> presc;
		int fnum = M.get_faces_c().size();
		presc.resize(fnum);
		for(int i = 0; i < fnum; i++) {
			const vec3d 
				&a = M.get_vertices_c().at(M.get_faces_c()[i][0]),
				&b = M.get_vertices_c().at(M.get_faces_c()[i][1]),
				&c = M.get_vertices_c().at(M.get_faces_c()[i][2]);
			SRsphere S = triangle_sphere(a, b, c);
			avgradius += S.get_radius();
			presc[i] = S;
		}
		avgradius /= (double)fnum;

		double maxrad = (avgradius < absrad) ? avgradius : absrad;
		sc.clear();
		sc.reserve(fnum);
		for(int i = 0; i < fnum; i++) {
			if(presc[i].get_radius() <= maxrad)
				sc.push_back(presc[i]);
			else {
				std::vector<SRsphere> ns;
				const vec3d 
					&a = M.get_vertices_c().at(M.get_faces_c()[i][0]),
					&b = M.get_vertices_c().at(M.get_faces_c()[i][1]),
					&c = M.get_vertices_c().at(M.get_faces_c()[i][2]);
				subdivide_triangle(a, b, c, ns, maxrad);
				sc.insert(sc.end(), ns.begin(), ns.end());
			}
		}
	}

	void SRsphere_tree::build(const mesh3 &M, int multiplier) {
		std::vector<SRsphere> sc;
		std::cout<<"Build tree, # of faces : "<<M.get_faces_c().size()<<std::endl;
		regularize_mesh3(M, sc);
		// for(auto it = M.get_faces_c().begin(); it != M.get_faces_c().end(); it++) {
		// 	const vec3d 
		// 		&a = M.get_vertices_c().at(it->at(0)),
		// 		&b = M.get_vertices_c().at(it->at(1)),
		// 		&c = M.get_vertices_c().at(it->at(2));
		// 	SRsphere S;
		// 	S.set_center((a + b + c) / 3.0);
		// 	S.set_radius((S.get_center() - a).len());
		// 	sc.push_back(S);
		// }
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

	SRsphere_set SRsphere_tree::get_sphere_set(int level) const {
		int first, last;
		get_level_set(level, first, last);
		SRsphere_set ret;
		ret.set.reserve(last - first);
		for(int i = first; i < last; i++) 
			ret.set.push_back(tree[i].S.get_geometry_c());
		return ret;
	}

	void SRsphere_tree::applyTR(const transform &TR) {
		for(auto it = tree.begin(); it != tree.end(); it++) {
			SRsphere S = it->S.get_geometry_c();
			S.set_center(TR.apply(S.get_center()));
			it->S.set_geometry(S);
		}
	}

	// Rendering.
	void SRsphere_tree::build_render() {
		// We can reuse a single sphere information for all the spheres in this tree.
		property_render_geometry<SRsphere> sample;
		sample.get_geometry().set_center(vec3d(0, 0, 0));
		sample.get_geometry().set_radius(1.0);
		sample.build_BO_mesh3(sample.get_geometry_c().get_mesh3());
		for(auto it = tree.begin(); it != tree.end(); it++) {
			// if(it->level >= 7)
			// 	continue;
			// if(tree.at(it->child.at(0)).parent != std::distance(tree.begin(), it))
			// 	continue;
			auto &S = it->S.get_geometry();
			it->S.set_shader(this->get_shader());
			it->S.set_BO(sample.get_BO());
			//it->S.build_BO_mesh3(S.get_mesh2());
			it->S.get_config().M = it->S.get_config().WIREFRAME;	
		}
		render_nodes.clear();
		render_nodes.push_back(root);
	}
	void SRsphere_tree::destroy_render() {
		auto BO = tree.at(0).S.get_BO();
		for(auto it = BO.VBO.begin(); it != BO.VBO.end(); it++) {
			glDeleteBuffers(1, &(*it));
		}
		for(auto it = BO.EBO.begin(); it != BO.EBO.end(); it++) {
			glDeleteBuffers(1, &(*it));
		}
	}
	std::set<int> SRsphere_tree::get_level_set(int level) const {
		std::set<int> ret;
		std::vector<int> queue;
		queue.push_back(root);
		while(!queue.empty()) {
			int n = queue.back(); queue.pop_back();
			if(tree[n].level == level)
				ret.insert(n);
			else if(tree[n].level < level) 
				queue.insert(queue.end(), tree[n].child.begin(), tree[n].child.end());
		}
		return ret;
	}
	void SRsphere_tree::get_level_set(int level, int &first, int &last) const {
		if(level < 1 || level > 6) {
			throw(std::invalid_argument("Invalid level."));
		}
		first = 0;
		int add = 1;
		for(int i = 0; i < level - 1; i++) {
			first += add;
			add *= 4;
		}
		last = first + add;
	}
	void SRsphere_tree::render_nodes_parent() {
		int cur_level = tree.at(render_nodes.at(0)).level;
		if(cur_level <= 1)
			return;

		render_nodes.clear();
		cur_level--;
		for(auto it = tree.begin(); it != tree.end(); it++) {
			if(it->level == cur_level) {
				//if(tree.at(it->child.at(0)).parent == std::distance(tree.begin(), it))
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
				//if(tree.at(it->child.at(0)).parent == std::distance(tree.begin(), it))
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
			// if(tree.at(*it).level >= 7) 
			// 	continue;

			// Since we use Unit sphere's rendering info, update Matrix info.
			// This is not correct, but for improvisation...
			transform TR;
			TR.identity();
			rotation R;
			translation T;
			for(int i = 0; i < 3; i++) {
				T[i] = tree.at(*it).S.get_geometry_c().get_center()[i];
				R[i][i] = tree.at(*it).S.get_geometry_c().get_radius();
				for(int j = 0; j < 3; j++) {
					if(i != j) R[i][j] = 0.0;
				}
			}
			TR.set_rotation(R);
			TR.set_translation(T);

			tree.at(*it).S.get_shader_c().set_uniform_model_matrix(TR);
			tree.at(*it).S.render();
		}
	}
	void SRsphere_tree::render_ui() {
		return;
	}

	// Hausdorff distance
	double SRsphere_tree::compute_pseudo_hausdorff(const SRsphere_tree &other, int level) const {
		if(level < 0 || level > height || level > other.height)  
			throw(std::invalid_argument("Invalid tree level for collision detection."));

		// 1. Extract subset of [ other ] (@ residue) that is not covered by this tree's spheres.
		// This reduces HD search space that we have to go through...
		std::set<int> residueID = other.get_level_set(level);

		std::vector<SRsphere> residue;
		std::map<int, std::vector<int>> collision;	// Colliding spheres ( other.id, this.id )
		using idpair = std::pair<int, int>;
		std::vector<idpair> queue;					// ( other.id, this.id )

		// 1-1. First extract sphere pairs that collide.
		queue.push_back(idpair(root, other.root));
		while(!queue.empty()) {
			idpair ip = queue.back(); queue.pop_back();
			const auto &otherN = other.tree.at(ip.first);
			const auto &thisN = tree.at(ip.second);
			if(SRsphere::overlap(otherN.S.get_geometry_c(), thisN.S.get_geometry_c())) {
				bool is_level_other = (otherN.level == level);
				bool is_level_this = (thisN.level == level);
				if(is_level_other && is_level_this) {
					auto find = collision.find(ip.first);
					if(find != collision.end()) {
						find->second.push_back(ip.second);
					}
					else {
						std::vector<int> tmp;
						tmp.push_back(ip.second);
						collision.insert({ip.first, tmp});
					}
				}
				else if(is_level_other) {
					for(auto it = thisN.child.begin(); it != thisN.child.end(); it++)
						queue.push_back(idpair(ip.first, *it));
				}
				else if(is_level_this) {
					for(auto it = otherN.child.begin(); it != otherN.child.end(); it++)
						queue.push_back(idpair(*it, ip.second));
				}
				else {
					for(auto it = otherN.child.begin(); it != otherN.child.end(); it++) 
						for(auto it2 = thisN.child.begin(); it2 != thisN.child.end(); it2++)
							queue.push_back(idpair(*it, *it2));
				}
			}
		}

		// 1-2. Extract residue.
		for(auto it = collision.begin(); it != collision.end(); it++) {
			residueID.erase(it->first);
			SRsphere otherS = other.tree.at(it->first).S.get_geometry_c();
			for(auto sub = it->second.begin(); sub != it->second.end(); sub++) {
				otherS.subtract(tree.at(*sub).S.get_geometry_c());
				if(otherS.get_radius() == 0.0)
					break;
			}
			if(otherS.get_radius() > 0.0) 
				residue.push_back(otherS);
		}

		// 1-3. Add non-collided spheres to residue.
		for(auto it = residueID.begin(); it != residueID.end(); it++) {
			residue.push_back(other.tree.at(*it).S.get_geometry_c());
		}

		// 2. Compute HD.
		double lowerHD = 0, upperHD = 1e+10;	// lower bound and upper bound for HD.

		
	}

	// EMD
	double SRsphere_tree::compute_pseudo_emd(const SRsphere_tree &a, const SRsphere_tree &b, int level) {
		if(level < 0 || level > a.height || level > b.height)  
			throw(std::invalid_argument("Invalid tree level for collision detection."));

		// 1. Extract subset of each tree's sphere set that is not covered by the other's spheres.
		std::map<int, SRsphere> level_set_a;
		std::map<int, SRsphere> level_set_b;	// node - sphere pairs.

		std::set<int> level_a = a.get_level_set(level);
		std::set<int> level_b = b.get_level_set(level);

		for(auto it = level_a.begin(); it != level_a.end(); it++) 
			level_set_a.insert({*it, a.tree.at(*it).S.get_geometry_c()});
		for(auto it = level_b.begin(); it != level_b.end(); it++) 
			level_set_b.insert({*it, b.tree.at(*it).S.get_geometry_c()});

		using idpair = std::pair<int, int>;
		std::vector<idpair> queue;					// ( other.id, this.id )

		queue.push_back(idpair(a.root, b.root));
		while(!queue.empty()) {
			idpair ip = queue.back(); queue.pop_back();
			int aid = ip.first, bid = ip.second;
			const auto &nodeA = a.tree.at(aid);
			const auto &nodeB = b.tree.at(bid);
			if(SRsphere::overlap(nodeA.S.get_geometry_c(), nodeB.S.get_geometry_c())) {
				bool is_level_a = (nodeA.level == level);
				bool is_level_b = (nodeB.level == level);
				if(is_level_a && is_level_b) {
					level_set_a.at(aid).subtract(nodeB.S.get_geometry_c());
					level_set_b.at(bid).subtract(nodeA.S.get_geometry_c());
				}
				else if(is_level_a) {
					for(auto it = nodeB.child.begin(); it != nodeB.child.end(); it++)
						queue.push_back(idpair(aid, *it));
				}
				else if(is_level_b) {
					for(auto it = nodeA.child.begin(); it != nodeA.child.end(); it++)
						queue.push_back(idpair(*it, bid));
				}
				else {
					for(auto it = nodeA.child.begin(); it != nodeA.child.end(); it++) 
						for(auto it2 = nodeB.child.begin(); it2 != nodeB.child.end(); it2++)
							queue.push_back(idpair(*it, *it2));
				}
			}
		}

		// 2. Compute EMD : Just add up all sphere's volume!
		double EMD = 0;
		for(auto it = level_set_a.begin(); it != level_set_a.end(); it++) {
			double r = it->second.get_radius();
			EMD += r * r * r;
		}
		for(auto it = level_set_b.begin(); it != level_set_b.end(); it++) {
			double r = it->second.get_radius();
			EMD += r * r * r;
		}
		EMD *= (4.0 / 3.0) * pi;

		return EMD;
	}
	double SRsphere_tree::compute_pseudo_emd(const SRsphere_tree &a, const SRsphere_tree &b, int level, const transform &bTR) {
		if(level > 6 || level < 1) 
			throw(std::invalid_argument("Level must be lower than 7"));
    
		AF::SRsphere_tree bcopy = b;		
		for(auto it = bcopy.tree.begin(); it != bcopy.tree.end(); it++) {
			if(it->level > level) continue;
			AF::SRsphere &S = it->S.get_geometry();
			S.set_center(bTR.apply(S.get_center()));
		}
		return AF::SRsphere_tree::compute_pseudo_emd(a, bcopy, level);
	}
	void SRsphere_tree::test_pseudo_emd(const SRsphere_tree &a, const SRsphere_tree &b, int level, std::vector<SRsphere> &subA, std::vector<SRsphere> &subB) {
		if(level < 0 || level > a.height || level > b.height)  
			throw(std::invalid_argument("Invalid tree level for collision detection."));

		// 1. Extract subset of each tree's sphere set that is not covered by the other's spheres.
		std::map<int, SRsphere> level_set_a;
		std::map<int, SRsphere> level_set_b;	// node - sphere pairs.

		std::set<int> level_a = a.get_level_set(level);
		std::set<int> level_b = b.get_level_set(level);

		for(auto it = level_a.begin(); it != level_a.end(); it++) 
			level_set_a.insert({*it, a.tree.at(*it).S.get_geometry_c()});
		for(auto it = level_b.begin(); it != level_b.end(); it++) 
			level_set_b.insert({*it, b.tree.at(*it).S.get_geometry_c()});

		using idpair = std::pair<int, int>;
		std::vector<idpair> queue;					// ( other.id, this.id )

		queue.push_back(idpair(a.root, b.root));
		while(!queue.empty()) {
			idpair ip = queue.back(); queue.pop_back();
			int aid = ip.first, bid = ip.second;
			const auto &nodeA = a.tree.at(aid);
			const auto &nodeB = b.tree.at(bid);
			if(SRsphere::overlap(nodeA.S.get_geometry_c(), nodeB.S.get_geometry_c())) {
				bool is_level_a = (nodeA.level == level);
				bool is_level_b = (nodeB.level == level);
				if(is_level_a && is_level_b) {
					level_set_a.at(aid).subtract(nodeB.S.get_geometry_c());
					level_set_b.at(bid).subtract(nodeA.S.get_geometry_c());
				}
				else if(is_level_a) {
					for(auto it = nodeB.child.begin(); it != nodeB.child.end(); it++)
						queue.push_back(idpair(aid, *it));
				}
				else if(is_level_b) {
					for(auto it = nodeA.child.begin(); it != nodeA.child.end(); it++)
						queue.push_back(idpair(*it, bid));
				}
				else {
					for(auto it = nodeA.child.begin(); it != nodeA.child.end(); it++) 
						for(auto it2 = nodeB.child.begin(); it2 != nodeB.child.end(); it2++)
							queue.push_back(idpair(*it, *it2));
				}
			}
		}

		subA.clear();
		subB.clear();
		for(auto it = level_set_a.begin(); it != level_set_a.end(); it++) {
			if(it->second.get_radius() > 0)
				subA.push_back(it->second);
		}
		for(auto it = level_set_b.begin(); it != level_set_b.end(); it++) {
			if(it->second.get_radius() > 0)
				subB.push_back(it->second);
		}
	}

	double SRsphere_tree::compute_level_volume(int level) const {
		if(level > 6 || level < 1) 
			throw(std::invalid_argument("Level must be lower than 7"));
		
		double ret = 0;
		std::vector<SRsphere> spheres = get_sphere_set(level).set;
		for(auto it = spheres.begin(); it != spheres.end(); it++) {
			double radius = it->get_radius();
			ret += radius * radius * radius;
		}
		return ret * (4.0 / 3.0) * pi;
	}
	void SRsphere_tree::compute_pseudo_emd_spec(const SRsphere_tree &a, const SRsphere_tree &b, int level, 
												double &emd_a, double &emd_b) {
		if(level < 0 || level > a.height || level > b.height)  
			throw(std::invalid_argument("Invalid tree level for collision detection."));

		// 1. Extract subset of each tree's sphere set that is not covered by the other's spheres.
		std::map<int, SRsphere> level_set_a;
		std::map<int, SRsphere> level_set_b;	// node - sphere pairs.

		std::set<int> level_a = a.get_level_set(level);
		std::set<int> level_b = b.get_level_set(level);

		for(auto it = level_a.begin(); it != level_a.end(); it++) 
			level_set_a.insert({*it, a.tree.at(*it).S.get_geometry_c()});
		for(auto it = level_b.begin(); it != level_b.end(); it++) 
			level_set_b.insert({*it, b.tree.at(*it).S.get_geometry_c()});

		using idpair = std::pair<int, int>;
		std::vector<idpair> queue;					// ( other.id, this.id )

		queue.push_back(idpair(a.root, b.root));
		while(!queue.empty()) {
			idpair ip = queue.back(); queue.pop_back();
			int aid = ip.first, bid = ip.second;
			const auto &nodeA = a.tree.at(aid);
			const auto &nodeB = b.tree.at(bid);
			if(SRsphere::overlap(nodeA.S.get_geometry_c(), nodeB.S.get_geometry_c())) {
				bool is_level_a = (nodeA.level == level);
				bool is_level_b = (nodeB.level == level);
				if(is_level_a && is_level_b) {
					level_set_a.at(aid).subtract(nodeB.S.get_geometry_c());
					level_set_b.at(bid).subtract(nodeA.S.get_geometry_c());
				}
				else if(is_level_a) {
					for(auto it = nodeB.child.begin(); it != nodeB.child.end(); it++)
						queue.push_back(idpair(aid, *it));
				}
				else if(is_level_b) {
					for(auto it = nodeA.child.begin(); it != nodeA.child.end(); it++)
						queue.push_back(idpair(*it, bid));
				}
				else {
					for(auto it = nodeA.child.begin(); it != nodeA.child.end(); it++) 
						for(auto it2 = nodeB.child.begin(); it2 != nodeB.child.end(); it2++)
							queue.push_back(idpair(*it, *it2));
				}
			}
		}

		// 2. Compute EMD : Just add up all sphere's volume!
		emd_a = 0;
		emd_b = 0;
		for(auto it = level_set_a.begin(); it != level_set_a.end(); it++) {
			double r = it->second.get_radius();
			emd_a += r * r * r;
		}
		for(auto it = level_set_b.begin(); it != level_set_b.end(); it++) {
			double r = it->second.get_radius();
			emd_b += r * r * r;
		}
		emd_a *= (4.0 / 3.0) * pi;
		emd_b *= (4.0 / 3.0) * pi;
	}

	void SRsphere_tree::align_icp(const SRsphere_tree &base, const SRsphere_tree &source, int level, transform &TR) {
		pcl::PointCloud<pcl::PointXYZ>::Ptr basePCL(new pcl::PointCloud<pcl::PointXYZ>);
		pcl::PointCloud<pcl::PointXYZ>::Ptr sourcePCL(new pcl::PointCloud<pcl::PointXYZ>);

		if(level == 1) {
			TR.identity();
			translation T;
			vec3d tmpT = base.tree[base.root].S.get_geometry_c().get_center() - source.tree[source.root].S.get_geometry_c().get_center();
			T.set(tmpT[0], tmpT[1], tmpT[2]);
			TR.set_translation(T);
			return;
		}

		std::set<int> level_set_base = base.get_level_set(level);
		std::set<int> level_set_source = source.get_level_set(level);

		// Set [ basePCL ].
		basePCL->width = level_set_base.size();
		basePCL->height = 1;
		basePCL->is_dense = true;
		basePCL->points.resize(basePCL->width * basePCL->height);
		int id = 0;
		for(auto it = level_set_base.begin(); it != level_set_base.end(); it++) {
			const vec3d &C = base.tree[*it].S.get_geometry_c().get_center();
			basePCL->points[id].x = (float)C[0];
			basePCL->points[id].y = (float)C[1];
			basePCL->points[id].z = (float)C[2];
			id++;
		}

		// Set [ sourcePCL ].
		sourcePCL->width = level_set_source.size();
		sourcePCL->height = 1;
		sourcePCL->is_dense = true;
		sourcePCL->points.resize(sourcePCL->width * sourcePCL->height);
		id = 0;
		for(auto it = level_set_source.begin(); it != level_set_source.end(); it++) {
			vec3d C = source.tree[*it].S.get_geometry_c().get_center();
			C = TR.apply(C);	// Use previous transform info.
			sourcePCL->points[id].x = (float)C[0];
			sourcePCL->points[id].y = (float)C[1];
			sourcePCL->points[id].z = (float)C[2];
			id++;
		}

		// Do ICP.
		pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
		icp.setMaxCorrespondenceDistance(1.0);
		icp.setInputSource(sourcePCL);
		icp.setInputTarget(basePCL);
		pcl::PointCloud<pcl::PointXYZ> fin;
		icp.align(fin);
		auto tmpTR = icp.getFinalTransformation();
		
		rotation R;
		translation T;
		for(int i = 0; i < 3; i++) {
			T[i] = tmpTR(i, 3);
			for(int j = 0; j < 3; j++) 
				R[i][j] = tmpTR(i, j);
		}
		TR.set_rotation(R);
		TR.set_translation(T);
	}

	// typedef dlib::matrix<double, 0, 1> column_vector;
	// SRsphere_tree optim_base;
	// SRsphere_tree optim_source;
	// SRsphere_tree optim_source_copy;
	// int optim_level;

	// void align(SRsphere_tree &source, int level, const column_vector &param) {
	// 	double 
	// 		cosx = cos(param(0)),
	// 		sinx = sin(param(0)),
	// 		cosy = cos(param(1)),
	// 		siny = sin(param(1)),
	// 		cosz = cos(param(2)),
	// 		sinz = sin(param(2));
	// 	transform TR;
	// 	rotation R;
	// 	translation T;
	// 	R.set(0, 0, cosz * cosy);
	// 	R.set(0, 1, -sinz * cosx + cosz * siny * sinx);
	// 	R.set(0, 2, sinz * sinx + cosz * siny * cosx);
	// 	R.set(1, 0, sinz * cosy);
	// 	R.set(1, 1, cosz * cosx + sinz * siny * sinx);
	// 	R.set(1, 2, -cosz * sinx + sinz * siny * cosx);
	// 	R.set(2, 0, -siny);
	// 	R.set(2, 1, cosy * sinx);
	// 	R.set(2, 2, cosy * cosx);
	// 	// for(int i = 0; i < 3; i++) 
	// 	// 	for(int j = 0; j < 3; j++)
	// 	// 		R.set(i, j, R[i][j] * param(6));
	// 	T.set(param(3), param(4), param(5));
		
	// 	TR.set_rotation(R);
	// 	TR.set_translation(T);

	// 	std::vector<int> queue;
	// 	queue.push_back(source.root);
	// 	while(!queue.empty()) {
	// 		int n = queue.back(); queue.pop_back();
	// 		if(source.tree[n].level <= level) {
	// 			auto &S = source.tree[n].S.get_geometry();
	// 			S.set_center(TR.apply(S.get_center()));
	// 			//S.set_radius(S.get_radius() * param(6));

	// 			if(source.tree[n].level != level)
	// 				queue.insert(queue.end(), source.tree[n].child.begin(), source.tree[n].child.end());
	// 		}
	// 	}
	// }
	// double align_emd_funct(const column_vector &v) {
	// 	optim_source_copy = optim_source;
	// 	align(optim_source_copy, optim_level, v);
	// 	return SRsphere_tree::compute_pseudo_emd(optim_base, optim_source_copy, optim_level);
	// }
	transform SRsphere_tree::alignTR(const align_var &param) {
		double 
			cosx = cos(param.rx),
			sinx = sin(param.rx),
			cosy = cos(param.ry),
			siny = sin(param.ry),
			cosz = cos(param.rz),
			sinz = sin(param.rz);
		rotation R;
		translation T;
		R.set(0, 0, cosz * cosy);
		R.set(0, 1, -sinz * cosx + cosz * siny * sinx);
		R.set(0, 2, sinz * sinx + cosz * siny * cosx);
		R.set(1, 0, sinz * cosy);
		R.set(1, 1, cosz * cosx + sinz * siny * sinx);
		R.set(1, 2, -cosz * sinx + sinz * siny * cosx);
		R.set(2, 0, -siny);
		R.set(2, 1, cosy * sinx);
		R.set(2, 2, cosy * cosx);
		// for(int i = 0; i < 3; i++) 
		// 	for(int j = 0; j < 3; j++)
		// 		R.set(i, j, R[i][j] * param.scale);
		T.set(param.tx, param.ty, param.tz);

		transform TR;
		TR.set_rotation(R);
		TR.set_translation(T);
		return TR;
	}
	// void SRsphere_tree::align_emd(const SRsphere_tree &base, const SRsphere_tree &source, int level, align_var &param) {
	// 	optim_base = base;
	// 	optim_source = source;
	// 	optim_level = level;
	// 	column_vector vp = { param.rx, param.ry, param.rz, param.tx, param.ty, param.tz };//, param.scale };
	// 	dlib::find_min_using_approximate_derivatives(
	// 		dlib::bfgs_search_strategy(), 
	// 		dlib::objective_delta_stop_strategy(1e-5, 10),
	// 		align_emd_funct, vp, -1, 1e-3
	// 	);
	// 	param.rx = vp(0);
	// 	param.ry = vp(1);
	// 	param.rz = vp(2);
	// 	param.tx = vp(3);
	// 	param.ty = vp(4);
	// 	param.tz = vp(5);
	// 	//param.scale = vp(6);
	// 	// auto result = dlib::find_min_global(
	// 	// 	align_emd_funct,
	// 	// 	{ 0, 0, 0, -1, -1, -1},
	// 	// 	{ pi20, pi20, pi20, 1, 1, 1},
	// 	// 	dlib::max_function_calls(300)
	// 	// );
	// 	// param.rx = result.x(0);
	// 	// param.ry = result.x(1);
	// 	// param.rz = result.x(2);
	// 	// param.tx = result.x(3);
	// 	// param.ty = result.x(4);
	// 	// param.tz = result.x(5);
	// }
}