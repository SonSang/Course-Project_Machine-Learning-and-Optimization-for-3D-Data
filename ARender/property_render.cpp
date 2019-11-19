#include "property_render.hpp"

namespace AF {
    property_render::property_render(const std::string &name_) : name(name_) {

    }
    const std::string& property_render::get_name() const noexcept {
        return this->name;
    }
    void property_render::set_name(const std::string &name) {
        this->name = name;
    }
    bool property_render::is_valid() const noexcept {
        return this->valid;
    }
    void property_render::set_valid(bool v) {
        this->valid = v;
    }
    void property_render::build_BO() {
        return;
    }
    void property_render::render() const noexcept {
        return;
    }
    void property_render::render_ui() {
        return;
    }

    // Texture2D
    void property_render::set_texture2D(const texture2D &TEX) {
        this->TEX = TEX;
    }
    texture2D& property_render::get_texture2D() noexcept {
        return TEX;
    }
    const texture2D& property_render::get_texture2D_c() const noexcept {
        return TEX;
    }

    // Material
    void property_render::set_material(const material &MAT) {
        this->MAT = MAT;
    }
    material& property_render::get_material() noexcept {
        return MAT;
    }
    const material& property_render::get_material_c() const noexcept {
        return MAT;
    }

    // Shader
    void property_render::build_shader(const std::string &vert_path, const std::string &frag_path) {
        this->SH.set_program(vert_path, frag_path);
    }
    void property_render::set_shader(const shader &SH) {
        this->SH = SH;
    }
    shader& property_render::get_shader() noexcept {
        return this->SH;
    }
    const shader& property_render::get_shader_c() const noexcept {
        return this->SH;
    }
}