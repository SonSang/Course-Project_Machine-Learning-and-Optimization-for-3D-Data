#ifndef __SRPCL_INTERFACE_H__
#define __SRPCL_INTERFACE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AMath/math.hpp"
#include "SRsphere.hpp"
#include <pcl/octree/octree_search.h>
#include <set>

// Since we use a lot of PCL library codes here, it will help to make interface to it.
namespace AF {
    using pclPT = pcl::PointXYZ;
    using pclNM = pcl::Normal;
    using pclPC = pcl::PointCloud<pclPT>;
    using pclNC = pcl::PointCloud<pclNM>;
    using pclOT = pcl::octree::OctreePointCloudSearch<pclPT>;

    // Convert our humble point cloud to PCL library's point cloud.
    pclPC::Ptr get_pcl_point_cloud(const std::set<vec3d> &mypc);

    // For the given point cloud, compute spheres on medial axis.
    std::vector<SRsphere> get_pcl_medial_axis_balls(const std::set<vec3d> &mypc, bool flip = true);

    // Convert our humble point cloud to PCL library's Octree structure.
    pclOT::Ptr get_pcl_octree(const std::set<vec3d> &mypc);
    pclOT::Ptr get_pcl_octree(pclPC::Ptr pc);

    pclPT get_pcl_point(const vec3d &v);
    vec3d get_pcl_vec3d(const pclPT &p);

    bool pcl_point_same(const pclPT &a, const pclPT &b);
}

#endif