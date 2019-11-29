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
		return (offset(a, b) <= 0);
    }
	double SRsphere::offset(const SRsphere &a, const SRsphere &b) {
		return (a.get_center() - b.get_center()).len() - a.get_radius() - b.get_radius();
	}
	void SRsphere::subtract(const SRsphere &other) {
		if(get_radius() == 0.0 || other.get_radius() == 0.0)
			return;
		double cdist = (this->get_center() - other.get_center()).len();
		bool fast = true;
		if(cdist + other.get_radius() <= this->get_radius()) {
			// This sphere contains the other sphere.
			if(cdist == 0.0) {
				// Same center : Rarely happens, choose random direction : (1, 0, 0).
				set_radius((get_radius() - other.get_radius()) * 0.5);
				vec3d cadd(other.get_radius() + get_radius(), 0, 0);
				set_center(get_center() + cadd);
			}
			else {
				fast = false;
			}
		}
		else if(cdist + this->get_radius() <= other.get_radius()) {
			// Other sphere contains this sphere.
			set_radius(0);
		}
		else if(cdist - this->get_radius() - other.get_radius() <= 0) {
			// Partial overlap.
			fast = false;
		}
		if(!fast) {
			// Leave only biggest sub sphere.
			double R = this->get_radius();
			double r = other.get_radius();
			double nR = (R - (r - cdist)) * 0.5;
			set_radius(nR);

			vec3d v = this->get_center() - other.get_center();
			v.normalize();
			v *= (r - cdist + nR);
			set_center(this->get_center() + v);
		}
	}

	// property_render_geometry
	template<>
	void property_render_geometry<SRsphere>::build_BO() {
		mesh3 M = get_geometry().get_mesh3();
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