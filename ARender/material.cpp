#include "material.hpp"

namespace AF {
    void material::set_emmision(const color &c) {
        this->emmision = c;
    }
    void material::set_ambient(const color &c) {
        this->ambient = c;
    }
    void material::set_diffuse(const color &c) {
        this->diffuse = c;
    }
    void material::set_specular(const color &c) {
        this->specular = c;
    }
    void material::set_shininess(double sh) {
        this->shininess = sh;
    }

    color material::get_emmision() const noexcept {
        return emmision;
    }
    color material::get_ambient() const noexcept {
        return ambient;
    }
    color material::get_diffuse() const noexcept {
        return diffuse;
    }
    color material::get_specular() const noexcept {
        return specular;
    }
    double material::get_shininess() const noexcept {
        return shininess;
    }
}