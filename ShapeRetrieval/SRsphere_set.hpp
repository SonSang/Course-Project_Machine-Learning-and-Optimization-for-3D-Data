#ifndef __SR_SPHERE_SET_H__
#define __SR_SPHERE_SET_H__

#include "SRsphere.hpp"
#include "../ARender/property_render_geometry.hpp"

namespace AF {
    class SRsphere_set : public property_render {
    public:
        std::vector<property_render_geometry<SRsphere>> set;

        void build_render(); 				// Set rendering info of each nodes.
		void destroy_render();				// Free rendering information.

        virtual void render() const noexcept;
		virtual void render_ui();
    };
}

#endif