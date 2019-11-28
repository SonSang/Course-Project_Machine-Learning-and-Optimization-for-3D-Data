#include "SRcgal_interface.hpp"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/HalfedgeDS_default.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <map>

namespace AF {
    using cgalKernel = CGAL::Simple_cartesian<double>;
    using cgalPoint = cgalKernel::Point_3;
    using cgalMesh = CGAL::Surface_mesh<cgalPoint>;
    using cgalPoly = CGAL::Polyhedron_3<cgalKernel>;

    cgalMesh get_cgal_mesh(const mesh3 &M) {
        cgalMesh ret;
        std::set<vec3d> vertices = M.get_vertex_set();
        std::map<vec3d, cgalMesh::Vertex_index> vertex_map;
        for(auto it = vertices.begin(); it != vertices.end(); it++) {
            cgalPoint pt{(*it)[0], (*it)[1], (*it)[2]};
            auto vi = ret.add_vertex(pt);
            vertex_map.insert({*it, vi});
        }
        
        for(auto it = M.get_faces_c().begin(); it != M.get_faces_c().end(); it++) {
            const vec3d 
                &a = M.get_vertices_c().at(it->at(0)),
                &b = M.get_vertices_c().at(it->at(1)),
                &c = M.get_vertices_c().at(it->at(2));
            auto
                ai = vertex_map.find(a)->second,
                bi = vertex_map.find(b)->second,
                ci = vertex_map.find(c)->second;
            auto fd = ret.add_face(ai, bi, ci);
            if(fd == cgalMesh::null_face()) {   // Orientation mismatch
                fd = ret.add_face(ai, ci, bi);
                if(fd == cgalMesh::null_face())
                    throw std::runtime_error("Something wrong with mesh input.");
            }
        }
        
        return ret;
    }

    cgalPoly get_cgal_poly(const mesh3 &M) {
        cgalPoly ret;

        for(auto it = M.get_faces_c().begin(); it != M.get_faces_c().end(); it++) {
            const vec3d 
                &a = M.get_vertices_c().at(it->at(0)),
                &b = M.get_vertices_c().at(it->at(1)),
                &c = M.get_vertices_c().at(it->at(2));
            cgalPoint 
                ap(a[0], a[1], a[2]),
                bp(b[0], b[1], b[2]),
                cp(c[0], c[1], c[2]);
            ret.make_triangle(ap, bp, cp);
        }
        return ret;
    }

    template <class HDS>
    class cgalPolyBuilder : public CGAL::Modifier_base<HDS> {
    public:
        mesh3 inputMesh;

        cgalPolyBuilder() {}
        void operator()(HDS &hds) {
            CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
            cgalMesh testmesh;
            
            std::set<vec3d> vertices = inputMesh.get_vertex_set();
            std::map<vec3d, int> vertex_map;
            std::map<vec3d, cgalMesh::Vertex_index> test_vertex_map;

            B.begin_surface(vertices.size(), inputMesh.get_faces_c().size());
            int i = 0;
            for(auto it = vertices.begin(); it != vertices.end(); it++) {
                cgalPoint pt{(*it)[0], (*it)[1], (*it)[2]};
                B.add_vertex(pt);
                vertex_map.insert({*it, i++});
                test_vertex_map.insert({*it, testmesh.add_vertex(pt)});
            }

            for(auto it = inputMesh.get_faces_c().begin(); it != inputMesh.get_faces_c().end(); it++) {
                const vec3d 
                    &a = inputMesh.get_vertices_c().at(it->at(0)),
                    &b = inputMesh.get_vertices_c().at(it->at(1)),
                    &c = inputMesh.get_vertices_c().at(it->at(2));
                auto
                    ai = vertex_map.find(a)->second,
                    bi = vertex_map.find(b)->second,
                    ci = vertex_map.find(c)->second;

                auto
                    tai = test_vertex_map.find(a)->second,
                    tbi = test_vertex_map.find(b)->second,
                    tci = test_vertex_map.find(c)->second;

                auto fd = testmesh.add_face(tai, tbi, tci);
                if(it != inputMesh.get_faces_c().begin()) {
                    if(fd == cgalMesh::null_face()) {
                        B.begin_facet();
                        B.add_vertex_to_facet(ai);
                        B.add_vertex_to_facet(ci);
                        B.add_vertex_to_facet(bi);
                        B.end_facet();
                        testmesh.add_face(tai, tci, tbi);
                    }
                    else {
                        B.begin_facet();
                        B.add_vertex_to_facet(ai);
                        B.add_vertex_to_facet(bi);
                        B.add_vertex_to_facet(ci);
                        B.end_facet();
                    }
                }
                else {
                    B.begin_facet();
                    B.add_vertex_to_facet(ai);
                    B.add_vertex_to_facet(bi);
                    B.add_vertex_to_facet(ci);
                    B.end_facet();
                }
            }
            B.end_surface();
        }
    };

    std::vector<SRsphere> get_cgal_mesh_skeleton(const mesh3 &M) {
        // using skeletonizer = CGAL::Mean_curvature_flow_skeletonization<cgalMesh>;
        // using skeleton = skeletonizer::Skeleton;
        // using skeleton_vertex = skeleton::vertex_descriptor;
        // using skeleton_edge = skeleton::edge_descriptor;
        using skeletonizer = CGAL::Mean_curvature_flow_skeletonization<cgalPoly>;
        using skeleton = skeletonizer::Skeleton;
        using skeleton_vertex = skeleton::vertex_descriptor;
        using skeleton_edge = skeleton::edge_descriptor;

        //cgalMesh cm = get_cgal_mesh(M);
        //cgalPoly cm = get_cgal_poly(M);
        cgalPoly cm;
        cgalPolyBuilder<cgalPoly::HalfedgeDS> cpb;
        cpb.inputMesh = M;
        cm.delegate(cpb);
        //cm.normalize_border();
        if(!CGAL::is_triangle_mesh(cm)) {
            std::cerr<<"Mesh is not triangular"<<std::endl;
            return std::vector<SRsphere>();
        }
        if(!CGAL::is_closed(cm)) {

            std::cerr<<"Mesh is not closed : "<<cm.size_of_border_edges()<<std::endl;
            return std::vector<SRsphere>();
        }

        skeleton sk;
        CGAL::extract_mean_curvature_flow_skeleton(cm, sk); 

        std::vector<SRsphere> ret;
        for(skeleton_vertex v : CGAL::make_range(vertices(sk))) {
            vec3d vec(sk[v].point.x(), sk[v].point.y(), sk[v].point.z());
            SRsphere S;
            S.set_center(vec);
            S.set_radius(1e-2);
            ret.push_back(S);
        }
        return ret;
    }
}