#include "SRsphere.hpp"
#include "../ARender/property_render_geometry.hpp"
#include <math.h>

namespace AF {
    bool SRsphere::contain(const SRsphere &other) {
        double cdist = (this->get_center() - other.get_center()).len();
		return (cdist + other.get_radius() <= this->get_radius());
    }
    double SRsphere::volume() const noexcept {
        static const double vconst = 4.0 / 3.0;
        return vconst * pi * pow(get_radius(), 3.0);
    }
    SRsphere SRsphere::merge(const SRsphere &a, const SRsphere &b) {
        double cdist = (a.get_center() - b.get_center()).len();
		double det = cdist + a.get_radius() + b.get_radius();
		if (det <= a.get_radius() * 2.0) {
			// [ b ] is contained in [ a ].
			return a;
		}
		else if (det <= b.get_radius() * 2.0) {
			// [ a ] is contained in [ b ].
			return b;
		}
		else {
			SRsphere ret;
			ret.set_radius(det * 0.5);
			vec3d cdiff = b.get_center() - a.get_center();
			cdiff /= cdist; // [ cdist ] cannot be zero : 
							// If it was, one must have contained the other.
			ret.set_center(a.get_center() + cdiff * (ret.get_radius() - a.get_radius()));
			return ret;
		}
    }
    bool SRsphere::overlap(const SRsphere &a, const SRsphere &b) {
        double cdist = (a.get_center() - b.get_center()).len();
		return (cdist <= a.get_radius() + b.get_radius());
    }

	// property_render_geometry
	template<>
	void property_render_geometry<SRsphere>::build_BO() {
		mesh3 M = get_geometry().build_mesh3();
		build_BO_mesh3(M);
	}
	template<>
    void property_render_geometry<SRsphere>::render() const noexcept {
		render_mesh3();
	}
	template<>
    void property_render_geometry<SRsphere>::render_ui() {
		return;
	}
}