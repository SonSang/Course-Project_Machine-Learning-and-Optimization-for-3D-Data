#include "SRpcl_interface.hpp"
#include "../Dependencies/masbcpp/src/compute_ma_processing.h"
#include "../Dependencies/masbcpp/src/compute_normals_processing.h"
#include <boost/make_shared.hpp>

namespace AF {
    pclPC::Ptr get_pcl_point_cloud(const std::set<vec3d> &mypc) {
        pclPC::Ptr pc(new pclPC);
        pc->resize(mypc.size());
        int i = 0;
        for(auto it = mypc.begin(); it != mypc.end(); it++)
            (*pc)[i++] = get_pcl_point(*it);
        return pc;
    }

    bool sphere_in_box(const SRsphere &S, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax) {
        if(S.get_center()[0] - S.get_radius() < xmin || S.get_center()[0] + S.get_radius() > xmax)
            return false;
        if(S.get_center()[1] - S.get_radius() < ymin || S.get_center()[1] + S.get_radius() > ymax)
            return false;
        if(S.get_center()[2] - S.get_radius() < zmin || S.get_center()[2] + S.get_radius() > zmax)
            return false;
        return true;
    }

    std::vector<SRsphere> get_pcl_medial_axis_balls(const std::set<vec3d> &mypc, bool flip) {
        // 1. We use [ masbcpp ] library here.
        // Make data structure to use it.
        ma_data md;
        md.coords = get_pcl_point_cloud(mypc);
        md.normals.reset(new pclNC);

        // 2. Compute normals for each point in the cloud.
        normals_parameters np;
        np.k = 9;
        compute_normals(np, md);

        // 3. Compute medial axis.
        ma_parameters mp;
        mp.initial_radius = 10.0f;
        mp.denoise_preserve = (pi / 180.0) * 20;
        mp.denoise_planar = (pi / 180.0) * 32;
        mp.nan_for_initr = true;

        md.ma_coords.reset(new pclPC);
        md.ma_coords->resize(2 * md.coords->size());
        md.ma_qidx.resize(2 * md.coords->size());
        compute_masb_points(mp, md);

        // 4. Make balls and return.
        int size = md.coords->size();
        std::vector<SRsphere> ret;

        // for(int i = 0; i < size; i++) {
        //     SRsphere S;
        //     bool nan0, nan1;
        //     nan0 = std::isnan(md.ma_coords->at(i).x);
        //     nan1 = std::isnan(md.ma_coords->at(i + size).x);
        //     if(nan0 && nan1)
        //         continue;
        //     else if(nan0) {
        //         vec3d cen = get_pcl_vec3d(md.ma_coords->at(i + size));
        //         vec3d base = get_pcl_vec3d(md.coords->at(i));
        //         S.set_center(cen);
        //         S.set_radius((cen - base).len());
        //         if(sphere_in_box(S, xmin, xmax, ymin, ymax, zmin, zmax))
        //             ret.push_back(S);
        //     }
        //     else if(nan1) {
        //         vec3d cen = get_pcl_vec3d(md.ma_coords->at(i));
        //         vec3d base = get_pcl_vec3d(md.coords->at(i));
        //         S.set_center(cen);
        //         S.set_radius((cen - base).len());
        //         if(sphere_in_box(S, xmin, xmax, ymin, ymax, zmin, zmax))
        //             ret.push_back(S);
        //     }
        //     else {
        //         SRsphere S2;

        //         S.set_center(get_pcl_vec3d(md.ma_coords->at(i)));
        //         S.set_radius((S.get_center() - get_pcl_vec3d(md.coords->at(i))).len());

        //         S2.set_center(get_pcl_vec3d(md.ma_coords->at(i + size)));
        //         S2.set_radius((S.get_center() - get_pcl_vec3d(md.coords->at(i))).len());

        //         bool inbox0, inbox1;
        //         inbox0 = sphere_in_box(S, xmin, xmax, ymin, ymax, zmin, zmax);
        //         inbox1 = sphere_in_box(S2, xmin, xmax, ymin, ymax, zmin, zmax);

        //         if(inbox0 && inbox1) 
        //             ret.push_back((S.get_radius() < S2.get_radius() ? S : S2));
        //         else if(inbox0)
        //             ret.push_back(S);
        //         else if(inbox1)
        //             ret.push_back(S2);
        //     }
        // }

        // Optional 1 : Cull out those spheres who are outside of this model's AABB.
        double xmin = 1e+10, xmax = -1e+10, ymin = 1e+10, ymax = -1e+10, zmin = 1e+10, zmax = -1e+10;
        for(auto it = mypc.begin(); it != mypc.end(); it++) {
            double x = (*it)[0], y = (*it)[1], z = (*it)[2];
            if(x < xmin) xmin = x;
            if(x > xmax) xmax = x;
            if(y < ymin) ymin = y;
            if(y > ymax) ymax = y;
            if(z < zmin) zmin = z;
            if(z > zmax) zmax = z;
        }

        for(int i = 0; i < size; i++) {
            SRsphere S;
            int fi = (flip ? i + size : i);
            if(std::isnan(md.ma_coords->at(fi).x))
                continue;
            vec3d cen = get_pcl_vec3d(md.ma_coords->at(fi));
            vec3d base = get_pcl_vec3d(md.coords->at(i));
            S.set_center(cen);
            S.set_radius((cen - base).len());
            if(sphere_in_box(S, xmin, xmax, ymin, ymax, zmin, zmax))
                ret.push_back(S);
        }
        
        return ret;
    }

    pclOT::Ptr get_pcl_octree(const std::set<vec3d> &mypc) {
        pclPC::Ptr pc = get_pcl_point_cloud(mypc);
        return get_pcl_octree(pc);
    }

    pclOT::Ptr get_pcl_octree(pclPC::Ptr pc) {
        pclOT::Ptr ret(new pclOT(1e-8));
        //ret->enableDynamicDepth(1); // Make only single point exists in leaf node : this enables easy addition / removal of point.
        ret->setInputCloud(pc);
        ret->addPointsFromInputCloud();
        return ret;
    }

    pclPT get_pcl_point(const vec3d &v) {
        return pclPT((float)v[0], (float)v[1], (float)v[2]);
    }
    vec3d get_pcl_vec3d(const pclPT &p) {
        return vec3d(p.x, p.y, p.z);
    }
    bool pcl_point_same(const pclPT &a, const pclPT &b) {
        return (a.x == b.x && a.y == b.y && a.z == b.z);
    }
}

