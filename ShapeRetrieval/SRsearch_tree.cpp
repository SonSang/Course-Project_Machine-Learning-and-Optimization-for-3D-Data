#include "SRsearch_tree.hpp"
#include <iostream>
#include <fstream>
#include <deque>

namespace AF {   
    AF::SRsphere_tree& get_model_sphere_tree(std::shared_ptr<object> optr) {
        return *(*(optr->get_property<AF::SRsphere_tree>().begin()));
    }
    int SRsearch_tree::find_best_fit(const SRsphere_tree &stree, transform &TR) {
        std::vector<std::string> fits = search(stree);
        int leaf_node_id = model_node_map.find(fits[0])->second;

        // @TODO =================================================================== NO ALIGN
        // TR.identity();
        // for(int i = 1; i <= height; i++)
        //     SRsphere_tree::align_icp(tree.at(leaf_node_id).ST, stree, i, TR);
        // ===================================================================================
        
        return leaf_node_id;
    }
    int SRsearch_tree::find_add_place(SRsphere_tree &stree, transform &TR) {
        int best_leaf_node = find_best_fit(stree, TR);

        // @TODO =================================================================== NO ALIGN
        //stree.applyTR(TR);
        // ===================================================================================

        // Evaluate each level's error compared to entire volume.
        std::vector<int> route_to_leaf;
        route_to_leaf.resize(height);
        int id = best_leaf_node;
        for(int i = height - 1; i >= 0; i--) {
            route_to_leaf.at(i) = id;
            id = tree.at(id).parent;
        }

        for(auto it = route_to_leaf.begin(); it != route_to_leaf.end(); it++) {
            int level = tree.at(*it).level;
            double node_volume_a = tree.at(*it).ST.compute_level_volume(level);
            double node_volume_b = stree.compute_level_volume(level);
            double emd_a, emd_b;
            SRsphere_tree::compute_pseudo_emd_spec(tree.at(*it).ST, stree, level, emd_a, emd_b);

            double errA = emd_a / node_volume_a, errB = emd_b / node_volume_b;
            double det = errA * errB;
            //double error_rate = (emd_a + emd_b) / (node_volume + emd_b);
            if(det > err_threshold) {
                // This is the NODE!
                if(*it == root) 
                    // It rarely happens...?
                    return root;
                else 
                    return tree.at(*it).parent;                
            }
        }
        // Since every nodes in the route are similar, just insert at the leaf node!
        return best_leaf_node;
    }
    int SRsearch_tree::add(SRsphere_tree &stree) {
        if(tree.empty()) {
            // If this is the first insertion...
            for(int i = 1; i <= height; i++) {
                node N;
                N.ST = stree;
                if(i < height) 
                    N.child.push_back(i);
                N.parent = i - 2;
                N.level = i;
                tree.push_back(N);
            }
            root = 0;
            return tree.size() - 1;
        }
        else {
            // First, find best place to insert [ stree ].
            transform TR;
            int parent = find_add_place(stree, TR);
            int plevel = tree.at(parent).level;

            // Create new nodes if necessary.
            int ret = parent;                
            int nchild = -1;
            //stree.applyTR(TR);
            if(plevel < height) {       // If plevel equals height, we do not have to create new nodes...
                // Since branch is made right under [ parent ] node, we can just insert nodes below it...
                nchild = tree.size();
                for(int i = plevel + 1; i <= height; i++) {
                    node N;
                    N.ST = stree;
                    if(i < height)
                        N.child.push_back(tree.size() + 1);
                    if(i == plevel + 1)
                        N.parent = parent;
                    else
                        N.parent = tree.size() - 1;
                    N.level = i;
                    tree.push_back(N);
                }
                ret = tree.size() - 1;
            }

            // Merge from [ parent ] node to upward direction.
            std::vector<int> mergeQ;
            mergeQ.push_back(parent);
            while(!mergeQ.empty()) {
                int m = mergeQ.back(); mergeQ.pop_back();
                int mlevel = tree.at(m).level;
                auto spheres = tree.at(m).ST.get_sphere_set(mlevel).set;
                auto spheresB = stree.get_sphere_set(mlevel).set;
                spheres.insert(spheres.end(), spheresB.begin(), spheresB.end());

                std::vector<SRsphere> buildS;
                buildS.reserve(spheres.size());
                for(auto it = spheres.begin(); it != spheres.end(); it++) {
                    buildS.push_back(it->get_geometry());
                }                
                tree.at(m).ST.build(buildS);
                if(m == parent && nchild != -1) 
                    tree.at(m).child.push_back(nchild);

                if(mlevel > 1)
                    mergeQ.push_back(tree.at(m).parent);
            }
            return ret;
        }
    }
    std::vector<std::string> SRsearch_tree::search(std::shared_ptr<object> &query) {
        const auto &mesh = (*(*query->get_property<property_render_geometry<rmesh3>>().begin())).get_geometry_c();
        SRsphere_tree qtree;
        qtree.build(mesh);
        
        return search(qtree);
    }

    double subroutine(const SRsphere_tree &stree, int streeLevel, const SRsphere_tree &target, int targetLevel) {
        double inVolume = 0;
        using ipair = std::pair<int, int>;

        std::set<int> included;
        std::deque<ipair> queue;

        int curNode = stree.root;
        int curParent = -1;
        bool curIncluded = false;
        bool curParentIncluded = false;
        
        queue.push_back(ipair(stree.root, target.root));
        while(!queue.empty()) {
            ipair item = queue.front(); queue.pop_front();

            if(item.first != curNode) {
                curNode = item.first;

                if(included.find(curNode) != included.end()) {
                    curIncluded = true;
                    continue;
                }
                else 
                    curIncluded = false;
                
                if(stree.tree[curNode].parent != curParent) {
                    curParent = stree.tree[curNode].parent;
                    if(included.find(curParent) != included.end()) {
                        curParentIncluded = true;
                        continue;
                    }
                    else 
                        curParentIncluded = false;
                }
            }
            else {
                if(curIncluded || curParentIncluded)
                    continue;
            }

            const SRsphere_tree::node &nA = stree.tree.at(item.first);
            const SRsphere_tree::node &nB = target.tree.at(item.second);

            const SRsphere &sA = nA.S.get_geometry_c();
            const SRsphere &sB = nB.S.get_geometry_c();

            double cdist = (sA.get_center() - sB.get_center()).len();
            if(nB.level == targetLevel) {
                if(cdist - sA.get_radius() - sB.get_radius() < 0) {
                    // overlap
                    if(cdist + sA.get_radius() <= sB.get_radius()) {
                        // include
                        inVolume += nA.volume;
                        included.insert(item.first);
                        curIncluded = true;
                    }
                    else {
                        // not included
                        if(nA.level == streeLevel) {
                            inVolume += nA.volume;
                            included.insert(item.first);
                            curIncluded = true;
                        } 
                        else {
                            for(auto it = nA.child.begin(); it != nA.child.end(); it++)
                                queue.push_back(ipair(*it, item.second));
                        }
                    }
                }
            } 
            else {
                if(cdist - sA.get_radius() - sB.get_radius() < 0) {
                    for(auto it = nA.child.begin(); it != nA.child.end(); it++) {
                        for(auto it2 = nB.child.begin(); it2 != nB.child.end(); it2++) {
                            queue.push_back(ipair(*it, *it2));
                        }
                    }
                }
            }
        }

        return stree.tree.at(stree.root).volume - inVolume;
    }
    std::vector<std::string> SRsearch_tree::search(const SRsphere_tree &stree) {
        std::vector<std::string> result;
        int rnum = 0;
        if(tree.empty())
            return result;

        struct queryItem {
            int id;
            double error;
            // std::set<int> inNodes;  // ID of nodes that are perfectly included in the compared sphere tree.
            //                         // If leaf node, even if it is not perfectly included, it is considered as that if they collide.
            //transform TR;
        };
        struct queryItemComp {
            bool operator() (const queryItem &lhs, const queryItem& rhs) const {
                if(lhs.error == rhs.error)
                    return lhs.id < rhs.id;
                else return lhs.error < rhs.error;
            }
        };
        using queryQ = std::set<queryItem, queryItemComp>;

        queryQ queue;
        double error_upper_bound = 1e+10;
        {
            queryItem qi;
            qi.id = root;
            qi.error = 0;
            //qi.inNodes.insert(stree.root);
            // translation T;
            // const auto &rootNode = tree.at(root).ST;
            // vec3d t = rootNode.tree.at(rootNode.root).S.get_geometry_c().get_center() - stree.tree.at(stree.root).S.get_geometry_c().get_center();

            // T.set(t[0], t[1], t[2]);
            // qi.TR.set_translation(T);
            queue.insert(qi);
        }


        struct resultItem {
            std::string path;
            double error;
        };
        struct resultItemComp {
            bool operator() (const resultItem &lhs, const resultItem& rhs) const {
                if(lhs.error == rhs.error)
                    return true;
                else return lhs.error < rhs.error;
            }
        };
        using resultQ = std::set<resultItem, resultItemComp>;

        resultQ results;
        while(!queue.empty()) {
            queryItem item = *queue.begin();
            queue.erase(item);
            
            if(item.error > error_upper_bound)
                continue;

            if(tree.at(item.id).level == height) {
                const auto &models = tree.at(item.id).models;
                for(auto it = models.begin(); it != models.end(); it++) {
                    const auto &ST = it->ST;
                    //transform TR = item.TR;
                    // @TODO =================================================================== NO ALIGN
                    //SRsphere_tree::align_icp(ST, stree, height, TR);  
                    //double metric = SRsphere_tree::compute_pseudo_emd_mult(ST, stree, height, TR);
                    //double metric = SRsphere_tree::compute_pseudo_emd(ST, stree, height, TR);
                    // ===================================================================================
                    double metric = SRsphere_tree::compute_pseudo_emd(ST, stree, height);
                    
                    // Choice 1 : DEFAULT
                    results.insert({it->path, metric});

                    if(results.size() <= 5) {
                        error_upper_bound = results.cbegin()->error;
                    }
                    else {
                        auto ptr = results.begin();
                        std::advance(ptr, 5);
                        error_upper_bound = ptr->error;
                    }
                }
            }
            else {
                const auto &child = tree.at(item.id).child;
                for(auto it = child.begin(); it != child.end(); it++) {
                    queryItem qi;
                    qi.id = *it;
                    qi.error = subroutine(stree, height, tree.at(*it).ST, tree.at(*it).level);
                    queue.insert(qi);
                }
            }
        }

        int i = 0;
        for(auto it = results.begin(); it != results.end(); it++) {
            result.push_back(it->path);
        }
        return result;
    }
    double SRsearch_tree::align_compute_pseudo_emd(int nodeID, const SRsphere_tree &stree, transform &TR) {
        SRsphere_tree::align_icp(tree.at(nodeID).ST, stree, tree.at(nodeID).level, TR);
        return SRsphere_tree::compute_pseudo_emd(tree.at(nodeID).ST, stree, tree.at(nodeID).level, TR);
    }
    void SRsearch_tree::add(std::shared_ptr<object> &NM) {
        auto &stree = get_model_sphere_tree(NM);

        int lnode = add(stree);
        
        model M;
        M.path = NM->get_name();
        M.ST = stree;

        tree[lnode].models.push_back(M);
        model_node_map.insert({M.path, lnode});
    }
    void SRsearch_tree::add(const std::string &path, SRsphere_tree &ST) {
        int lnode = add(ST);
        
        model M;
        M.path = path;
        M.ST = ST;

        tree[lnode].models.push_back(M);
        model_node_map.insert({M.path, lnode});
    }

    SRsphere_tree loadSphereTree(std::ifstream &ifs) {
        SRsphere_tree ST;
        char input[100];
        std::string inputStr;
        double inputDouble;
        int inputInt;
        
        ifs.getline(input, 100);
        inputStr = std::string(input);
        if(inputStr == std::string("ST_BEGIN"))
            ifs.getline(input, 100);
        inputInt = atoi(input);
        ST.root = inputInt;

        ifs.getline(input, 100);
        inputInt = atoi(input);
        ST.height = inputInt;

        while(true) {
            ifs.getline(input, 100);
            inputStr = std::string(input);
            if(inputStr == std::string("ST_END"))
                break;
            
            if(inputStr == "N") {
                SRsphere_tree::node N;
                ifs.getline(input, 100);    // C
                while(true) {
                    ifs.getline(input, 100);
                    if(input[0] == 'P')
                        break;
                    else {
                        inputInt = atoi(input);
                        N.child.push_back(inputInt);
                    }
                }
                double cen[3];
                double radius;
                {
                    ifs.getline(input, 100);
                    inputInt = atoi(input);
                    N.parent = inputInt;

                    ifs.getline(input, 100);
                    inputInt = atoi(input);
                    N.level = inputInt;

                    ifs.getline(input, 100);
                    inputDouble = atof(input);
                    N.volume = inputDouble;

                    
                    ifs.getline(input, 100);
                    inputDouble = atof(input);
                    cen[0] = inputDouble;

                    ifs.getline(input, 100);
                    inputDouble = atof(input);
                    cen[1] = inputDouble;

                    ifs.getline(input, 100);
                    inputDouble = atof(input);
                    cen[2] = inputDouble;

                    
                    ifs.getline(input, 100);
                    inputDouble = atof(input);
                    radius = inputDouble;
                }
                
                SRsphere S;
                S.set_center(vec3d(cen[0], cen[1], cen[2]));
                S.set_radius(radius);
                N.S.set_geometry(S);

                ST.tree.push_back(N);
            }
        }
        return ST;
    }
    SRsearch_tree::node loadNode(std::ifstream &ifs) {
        SRsearch_tree::node N;
        char input[100];
        std::string inputStr;
        double inputDouble;
        int inputInt;

        ifs.getline(input, 100);
        inputStr = std::string(input);
        if(inputStr == std::string("NODE_BEGIN")) 
            ifs.getline(input, 100);
        inputInt = atoi(input);
        N.parent = inputInt;

        ifs.getline(input, 100);
        inputInt = atoi(input);
        N.level = inputInt;

        ifs.getline(input, 100);    // "C"
        while(true) {
            ifs.getline(input, 100);
            if(input[0] == 'M')
                break;
            inputInt = atoi(input);
            N.child.push_back(inputInt);
        }

        while(true) {
            ifs.getline(input, 100);
            inputStr = std::string(input);
            if(inputStr == std::string("ST"))
                break;
            SRsearch_tree::model M;
            M.path = inputStr;
            M.ST = loadSphereTree(ifs);
            N.models.push_back(M);
        }

        N.ST = loadSphereTree(ifs);

        return N;
    }
    void SRsearch_tree::load(const std::string &path) {
        std::ifstream ifs;
        ifs.open(path);

        if(ifs.is_open()) {
            char input[100];
            std::string inputStr;
            double inputDouble;
            int inputInt;
            
            ifs.getline(input, 100);
            inputInt = atoi(input);
            this->height = inputInt;

            ifs.getline(input, 100);
            inputInt = atoi(input);
            this->root = inputInt;

            model_node_map.clear();
            while(true) {
                ifs.getline(input, 100);
                if(std::string(input) == std::string("TREE"))
                    break;
                std::string path = std::string(input);

                ifs.getline(input, 100);
                int id = atoi(input);

                model_node_map.insert({path, id});
            }

            tree.clear();
            while(true) {
                auto N = loadNode(ifs);
                tree.push_back(N);
                
                char input[100];
                ifs.getline(input, 100);
                if(std::string(input) == std::string(""))
                    break;
            }

            std::cout<<"Load complete."<<std::endl;
        }
        else
            std::cerr<<"No such file : "<<path<<std::endl;

        ifs.close();
    }

    void saveSphereTree(std::ofstream &ofs, SRsphere_tree &ST, int nodeLevel) {
        ofs<<"ST_BEGIN"<<std::endl;
        ofs << ST.root <<std::endl;
		ofs << nodeLevel <<std::endl;
        for(auto it = ST.tree.begin(); it != ST.tree.end(); it++) {
            if(it->level > nodeLevel)
                continue;
			ofs<<"N"<<std::endl;
			ofs<<"C"<<std::endl;
			for(auto c = it->child.begin(); c != it->child.end(); c++)
				ofs<<*c<<std::endl;
			ofs<<"P"<<std::endl;
			ofs<<it->parent<<std::endl;
			ofs<<it->level<<std::endl;
			ofs<<it->volume<<std::endl;
			ofs<<it->S.get_geometry_c().get_center()[0]<<std::endl;
			ofs<<it->S.get_geometry_c().get_center()[1]<<std::endl;
			ofs<<it->S.get_geometry_c().get_center()[2]<<std::endl;
			ofs<<it->S.get_geometry_c().get_radius()<<std::endl;
		}
        ofs<<"ST_END"<<std::endl;
    }
    void saveNode(std::ofstream &ofs, SRsearch_tree::node &node) {
        ofs<<"NODE_BEGIN"<<std::endl;
        ofs<<node.parent<<std::endl;
        ofs<<node.level<<std::endl;
        
        ofs<<"C"<<std::endl;
        for(auto it = node.child.begin(); it != node.child.end(); it++)
            ofs<<*it<<std::endl;
        
        ofs<<"M"<<std::endl;
        for(auto it = node.models.begin(); it != node.models.end(); it++) {
            ofs<<it->path<<std::endl;
            saveSphereTree(ofs, it->ST, 6); // @TODO hard coded leaf node level...
        }

        ofs<<"ST"<<std::endl;
        saveSphereTree(ofs, node.ST, node.level);
    }
    void SRsearch_tree::save(const std::string &path) {
        std::ofstream ofs;
        ofs.open(path);

        ofs<<height<<std::endl;
        ofs<<root<<std::endl;

        for(auto it = model_node_map.begin(); it != model_node_map.end(); it++) {
            ofs<<it->first<<std::endl;
            ofs<<it->second<<std::endl;
        }

        ofs<<"TREE"<<std::endl;
        for(auto it = tree.begin(); it != tree.end(); it++)
            saveNode(ofs, *it);

        ofs.close();
        std::cout<<"Save complete."<<std::endl;
    }
}