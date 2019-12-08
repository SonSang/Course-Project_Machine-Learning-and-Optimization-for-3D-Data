#ifndef __SR_SEARCH_TREE_H__
#define __SR_SEARCH_TREE_H__

#include "SRsphere_tree.hpp"
#include "../ARender/object.hpp"
#include <memory>

namespace AF {
    class SRsearch_tree {
    public:
        using model5 = std::array<std::string, 5>; 
    private:
        int find_best_fit(const SRsphere_tree &stree, transform &TR);   // Subroutine of [ find_add_place ].
        int find_add_place(SRsphere_tree &stree, transform &TR);        // Find appropriate place to insert the given sphere tree.
                                                                        // Appropriate place means the parent node that this tree will go below.
                                                                        // Also, we find best transformation to match the [ stree ] to the parent node.
        int add(SRsphere_tree &stree);                // Add given sphere tree to this search tree. Return ID of the leaf node that include this model.


        //model5 searchID(std::shared_ptr<object> &query);  // Find best matching model in the tree to the given query model.
        
    public:
        class model {
        public:
            std::string path;
            SRsphere_tree ST;
        };

        class node {
        public:
            SRsphere_tree ST;
            std::vector<int> child;
            int parent;
            int level;

            std::vector<model> models;                          // If this node is leaf node, it should have names for specific models.
            //std::vector<std::shared_ptr<object>> models;      // If this node is leaf node, it should point to the specific models as child. 
        };

        std::map<std::string, int> model_node_map;              // For model of name [ first ], match leaf node of id [ second ].
        std::vector<node> tree;
        int height = 6;     
        int root = -1;

        double err_threshold = 0.001; // Threshold for making branches in this search tree.

        // Align given [ stree ] with [ nodeID ] node and compute EMD.
        // Alignment starts from given transformation, and ends in alignment result.
        double align_compute_pseudo_emd(int nodeID, const SRsphere_tree &stree, transform &TR);
        void add(std::shared_ptr<object> &NM);                           // Add given model to this search tree. The model should have [ mesh3 ] property.
                                                                            // Before add, the model must have built sphere tree!
        void add(const std::string &path, SRsphere_tree &ST);
        model5 search(std::shared_ptr<object> &query);                      // Find best matching model in the tree to the given query model.
        model5 search(const SRsphere_tree &stree);                          // Find best matching model in the tree to the given query model.

        void load(const std::string &path);
        void save(const std::string &path);
    };
}

#endif