#include "SRsearch_tree.hpp"

namespace AF {   
    AF::SRsphere_tree& get_model_sphere_tree(std::shared_ptr<object> optr) {
        return *(*(optr->get_property<AF::SRsphere_tree>().begin()));
    }
    int SRsearch_tree::find_best_fit(const SRsphere_tree &stree, transform &TR) {
        modelID5 fits = searchID(stree);
        int leaf_node_id = model_node_map.find(fits[0])->second;

        TR.identity();
        for(int i = 1; i <= height; i++)
            SRsphere_tree::align_icp(tree.at(leaf_node_id).ST, stree, i, TR);
        
        return leaf_node_id;
    }
    int SRsearch_tree::find_add_place(SRsphere_tree &stree, transform &TR) {
        int best_leaf_node = find_best_fit(stree, TR);

        stree.applyTR(TR);

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
            for(int i = 1; i <= stree.height; i++) {
                node N;
                N.ST = stree;
                if(i < stree.height) 
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
                for(int i = plevel + 1; i <= stree.height; i++) {
                    node N;
                    N.ST = stree;
                    if(i < stree.height)
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
    SRsearch_tree::modelID5 SRsearch_tree::searchID(std::shared_ptr<object> &query) {
        const auto &mesh = (*(*query->get_property<property_render_geometry<rmesh3>>().begin())).get_geometry_c();
        SRsphere_tree qtree;
        qtree.build(mesh);
        
        return searchID(qtree);
    }
    SRsearch_tree::modelID5 SRsearch_tree::searchID(const SRsphere_tree &stree) {
        modelID5 result = { -1, -1, -1, -1, -1 };
        int rnum = 0;
        if(tree.empty())
            return result;

        struct queryItem {
            int id;
            double error;
            transform TR;
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
        SRsphere_tree::ivset semi_result;
        {
            queryItem qi;
            qi.id = root;
            qi.error = 0;
            translation T;
            const auto &rootNode = tree.at(root).ST;
            vec3d t = rootNode.tree.at(rootNode.root).S.get_geometry_c().get_center() - stree.tree.at(stree.root).S.get_geometry_c().get_center();

            T.set(t[0], t[1], t[2]);
            qi.TR.set_translation(T);
            queue.insert(qi);
        }

        while(!queue.empty()) {
            queryItem item = *queue.begin();
            queue.erase(item);
            
            if(tree.at(item.id).level == height) {
                const auto &models = tree.at(item.id).models;
                for(auto it = models.begin(); it != models.end(); it++) {
                    const auto &ST = get_model_sphere_tree(*it);
                    transform TR = item.TR;
                    SRsphere_tree::align_icp(ST, stree, height, TR);
                    double emd = SRsphere_tree::compute_pseudo_emd(ST, stree, height, TR);
                    semi_result.insert({(*it)->get_id(), emd});
                }
                if(semi_result.size() >= 5) {
                    auto end = semi_result.begin();
                    std::advance(end, 5);
                    int i = 0;
                    for(auto it = semi_result.begin(); it != end; it++) {
                        result[i++] = it->first;
                    }
                    return result;
                }
            }
            else {
                const auto &child = tree.at(item.id).child;
                for(auto it = child.begin(); it != child.end(); it++) {
                    transform TR = item.TR;
                    double emd = align_compute_pseudo_emd(*it, stree, TR);
                    queryItem qi;
                    qi.id = *it;
                    qi.error = emd;
                    qi.TR = TR;
                    queue.insert(qi);
                }
            }
        }

        int i = 0;
        for(auto it = semi_result.begin(); it != semi_result.end(); it++) {
            result[i++] = it->first;
        }
        return result;
    }
    double SRsearch_tree::align_compute_pseudo_emd(int nodeID, const SRsphere_tree &stree, transform &TR) {
        SRsphere_tree::align_icp(tree.at(nodeID).ST, stree, tree.at(nodeID).level, TR);
        return SRsphere_tree::compute_pseudo_emd(tree.at(nodeID).ST, stree, tree.at(nodeID).level, TR);
    }
    void SRsearch_tree::add(std::shared_ptr<object> &model) {
        auto &stree = get_model_sphere_tree(model);

        int lnode = add(stree);
        tree[lnode].models.push_back(model);
        model_node_map.insert({model->get_id(), lnode});
    }
}