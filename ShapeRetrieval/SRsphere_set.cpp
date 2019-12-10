#include "SRsphere_set.hpp"

namespace AF {
    void SRsphere_set::build_render() {
        property_render_geometry<SRsphere> sample;
		sample.get_geometry().set_center(vec3d(0, 0, 0));
		sample.get_geometry().set_radius(1.0);
		sample.build_BO_mesh3(sample.get_geometry_c().get_mesh3());
		for(auto it = set.begin(); it != set.end(); it++) {
			//it->set_shader(this->get_shader());
			it->set_BO(sample.get_BO());
			//it->S.build_BO_mesh3(S.get_mesh2());
			//it->get_config().M = it->get_config().WIREFRAME;	
		}
    }
	void SRsphere_set::destroy_render() {
        if(set.empty())
            return;
        auto BO = set.at(0).get_BO();
		for(auto it = BO.VBO.begin(); it != BO.VBO.end(); it++) {
			glDeleteBuffers(1, &(*it));
		}
		for(auto it = BO.EBO.begin(); it != BO.EBO.end(); it++) {
			glDeleteBuffers(1, &(*it));
		}
    }
    void SRsphere_set::render() const noexcept {
        for(auto it = set.begin(); it != set.end(); it++) {
			transform TR;
			TR.identity();
			rotation R;
			translation T;
			for(int i = 0; i < 3; i++) {
				T[i] = it->get_geometry_c().get_center()[i];
				R[i][i] = it->get_geometry_c().get_radius();
				for(int j = 0; j < 3; j++) {
					if(i != j) R[i][j] = 0.0;
				}
			}
			TR.set_rotation(R);
			TR.set_translation(T);

            it->get_shader_c().set_uniform_model_matrix(TR);
            it->render();
		}
    }
	void SRsphere_set::render_ui() {
        return;
    }
}