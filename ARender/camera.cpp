#include "camera.hpp"
#include <SDL2/SDL_opengles2.h>
#include <math.h>

#define DEFAULT_POSITION		0, 0, 1
#define DEFAULT_FRONT			0, 0, -1
#define DEFAULT_UP				0, 1, 0

#define PITCH_UPPER_BOUND		89
#define PITCH_LOWER_BOUND		-89
#define DEFAULT_YAW				-90
#define DEFAULT_PITCH			0

#define DEFAULT_SPEED			1
#define DEFAULT_FOVY			25
#define DEFAULT_MAG				1

//#define DEFAULT_MODE			persp

namespace AF {
    camera::camera() :  pos(DEFAULT_POSITION), front(DEFAULT_FRONT), up(DEFAULT_UP), yaw(DEFAULT_YAW), pitch(DEFAULT_PITCH),  
                        speed(DEFAULT_SPEED), mag(DEFAULT_MAG) {

    }

    void camera::set_position(double x, double y, double z) {
        this->pos.set(x, y, z);
        update_view();
    }
    void camera::set_position(const vec3d &v) {
        this->pos = v;
        update_view();
    }
    void camera::set_front(double x, double y, double z) {
        this->front.set(x, y, z);
        this->front.normalize();
        update_view();
    }
    void camera::set_front(const vec3d &v) {
        this->front = v;
        this->front.normalize();
        update_view();
    }
    void camera::set_up(double x, double y, double z) {
        this->up.set(x, y, z);
        update_view();
    };
    void camera::set_up(const vec3d &v) {
        this->up = v;
        update_view();
    }

    vec3d camera::get_position() const noexcept {
        return this->pos;
    }
    vec3d camera::get_front() const noexcept {
        return this->front;
    }
    vec3d camera::get_up() const noexcept {
        return this->up;
    }

    void camera::set_yaw(double yaw) {
        this->yaw = yaw;
    }
    void camera::set_pitch(double pitch) {
        this->pitch = pitch;
    }

    double camera::get_yaw() const noexcept {
        return this->yaw;
    }
    double camera::get_pitch() const noexcept {
        return this->pitch;
    }

    void camera::set_speed(double speed) {
        this->speed = speed;
    }
    double camera::get_speed() const noexcept {
        return this->speed;
    }

    void camera::set_mag(double mag) {
        this->mag = mag;
    }
    double camera::get_mag() const noexcept {
        return this->mag;
    }

    // Move
    void camera::move_back_forth(int forward, double deltaTime) {
        double
            distance = get_speed() * deltaTime * forward;
        vec3d
            addition = get_front() * distance,
            nposition = get_position() + addition;
        set_position(nposition);
    }
    void camera::move_left_right(int right, double deltaTime) {
        double
            distance = get_speed() * deltaTime * right;
        vec3d
            rvector = get_front().cross(get_up());
        rvector.normalize();
        vec3d
            addition = rvector * distance,
            nposition = get_position() + addition;
        set_position(nposition);
    }

    // Update
    void camera::update_front() {
        double
            pitchR = deg2rad(get_pitch()),
            yawR = deg2rad(get_yaw());
        vec3d
            nfront(cos(pitchR) * cos(yawR), sin(pitchR), cos(pitchR) * sin(yawR));
        set_front(nfront);
    }
    void camera::update_angle(double yawOffset, double pitchOffset) {
        double
            nyaw = get_yaw() + yawOffset,
            npitch = get_pitch() + pitchOffset;
        if (npitch > PITCH_UPPER_BOUND)
            npitch = PITCH_UPPER_BOUND;
        else if (npitch < PITCH_LOWER_BOUND)
            npitch = PITCH_LOWER_BOUND;
        set_yaw(nyaw);
        set_pitch(npitch);
        update_front();
    }
    // == Actual post redisplay only occurs here.
    void camera::update_view() {
        vec3d
            center = get_position() + get_front();
        view_lookat(pos, center, up);
    }
    void camera::update_proj(int width, int height) {
        projection_persp(DEFAULT_FOVY * get_mag(), width / (double)height, near_plane, far_plane);
    }

    // View, projection matrix
    mat4d& camera::get_view_matrix() noexcept {
        return view;
    }
    mat4d& camera::get_projection_matrix() noexcept {
        return projection;
    }
    const mat4d& camera::get_view_matrix_c() const noexcept {
        return view;
    }
    const mat4d& camera::get_projection_matrix_c() const noexcept {
        return projection;
    }

    void camera::view_lookat(const vec3d &pos, const vec3d &target, const vec3d &up) {
        vec3d
            dir = pos - target;
        dir.normalize();
        vec3d
            right = up.cross(dir);
        right.normalize();
        vec3d
            nup = dir.cross(right);
        nup.normalize();
        
        view.clear();
        view.set(0, 0, (float)right[0]);
        view.set(0, 1, (float)right[1]);
        view.set(0, 2, (float)right[2]);

        view.set(1, 0, (float)nup[0]);
        view.set(1, 1, (float)nup[1]);
        view.set(1, 2, (float)nup[2]);

        view.set(2, 0, (float)dir[0]);
        view.set(2, 1, (float)dir[1]);
        view.set(2, 2, (float)dir[2]);

        view.set(0, 3, (float)(-right[0] * pos[0] - right[1] * pos[1] - right[2] * pos[2]));
        view.set(1, 3, (float)(-nup[0] * pos[0] - nup[1] * pos[1] - nup[2] * pos[2]));
        view.set(2, 3, (float)(-dir[0] * pos[0] - dir[1] * pos[1] - dir[2] * pos[2]));

        view.set(3, 3, 1);
    }

    void camera::projection_ortho(double left, double right, double bottom, double top, double near, double far) {
        double
            r_l = right - left,
            t_b = top - bottom,
            f_n = far - near;
        this->projection.clear();
        this->projection.set(0, 0, float(2.0 / r_l));
        this->projection.set(0, 3, float((left + right) / -r_l));
        this->projection.set(1, 1, float(2.0 / t_b));
        this->projection.set(1, 3, float((top + bottom) / -t_b));
        this->projection.set(2, 2, float(-2.0 / f_n));
        this->projection.set(2, 3, float((far + near) / -f_n));
        this->projection.set(3, 3, float(1.0));
    }
    void camera::projection_persp(double fovy, double aspect, double near, double far) {
        double
            cot = 1.0 / tan(fovy * 0.5);
        this->projection.clear();
        this->projection.set(0, 0, float(cot / aspect));
        this->projection.set(1, 1, float(cot));
        this->projection.set(2, 2, float(-(far + near) / (far - near)));
        this->projection.set(2, 3, float(-2.0 * (far * near) / (far - near)));
        this->projection.set(3, 2, float(1.0));     //@TODO : it works... but why not -1?
    }


}