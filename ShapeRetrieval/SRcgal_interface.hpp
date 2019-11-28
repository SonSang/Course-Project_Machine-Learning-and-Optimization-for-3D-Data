#ifndef __SR_CGAL_INTERFACE_H__
#define __SR_CGAL_INTERFACE_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "SRsphere.hpp"
#include "../AGeometry/geometry.hpp"

namespace AF {
    std::vector<SRsphere> get_cgal_mesh_skeleton(const mesh3 &M);
}

#endif