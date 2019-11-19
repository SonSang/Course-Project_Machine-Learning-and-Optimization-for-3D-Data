#include "object.hpp"

namespace AF {
    using uint = unsigned int;

    // object
    object::object(const std::string &name_) : name(name_) {

    }
    void object::set_name(const std::string &name_) {
        this->name = name_;
    }
    const std::string& object::get_name() const noexcept {
        return this->name;
    }
    uint object::get_id() const noexcept {
        return this->uid;
    }
    void object::set_id(uint id) noexcept {
        this->uid = id;
    }
    void object::add_property(std::shared_ptr<property> property_) {
        properties.push_back(property_);
        property_->set_owner(this);
    }
    void object::del_property(uint id) {
        auto
            tar = properties.begin();
        std::advance(tar, id);
        properties.erase(tar);
    }
    std::shared_ptr<property> object::get_property(uint id) {
        auto
            tar = properties.begin();
        std::advance(tar, id);
        return *tar;
    }
    int object::property_size() const noexcept {
        return (int)properties.size();
    }
    vec3d object::get_origin() const noexcept {
        return this->TR.apply(vec3d(0, 0, 0));
    }
    void object::set_transform(const transform &TR) noexcept {
        this->TR = TR;
    }
    const transform& object::get_transform() const noexcept {
        return this->TR;
    }
    void object::translate(double x, double y, double z) {
        //TR.translate(x, y,  z);
    }
    void object::rotate(const vec3d &axisA, const vec3d &axisB, double radian) {
        //TR.rotate(axisA, axisB, radian);
    }
}