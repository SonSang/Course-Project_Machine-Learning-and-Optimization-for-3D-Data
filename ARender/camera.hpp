#ifndef __AF_CAMERA_H__
#define __AF_CAMERA_H__

#ifdef _MSC_VER
#pragma once
#endif

#include "../AMath/math.hpp"
#include "../AMath/timer.hpp"
#include "color.hpp"

namespace AF {
    class camera {
	private:
        // position
        vec3d pos;
        vec3d front;
        vec3d up;

        // angle
        double yaw;
        double pitch;

        // speed
        double speed;

        // magnify
        double mag;	// 1.0 for default(fovy = 25)

        // near, far plane
        double near_plane = 0.1;
        double far_plane = 10000;		// to reduce flickering,
                                        // enlarge near plane value.

        // background color
        color bgcolor = color(0, 0, 0, 1);

        // statistics
        stat_timer timer;
        double frame_time;

        // mode
        bool ortho = false;

        // view, projection matrix.
        mat4d view;
        mat4d projection;
    public:
        camera();

        // Set
        void set_position(double x, double y, double z);
        void set_position(const vec3d &v);
        void set_front(double x, double y, double z);
        void set_front(const vec3d &v);
        void set_up(double x, double y, double z);
        void set_up(const vec3d &v);

        vec3d get_position() const noexcept;
        vec3d get_front() const noexcept;
        vec3d get_up() const noexcept;

        void set_yaw(double yaw);
        void set_pitch(double pitch);

        double get_yaw() const noexcept;
        double get_pitch() const noexcept;

        void set_speed(double speed);
        double get_speed() const noexcept;

        void set_mag(double mag);
        double get_mag() const noexcept;

        void set_ortho(bool mode);
        bool get_ortho() const noexcept;
        // change between [ortho] and [perspective] mode.
        void toggle_mode(int width, int height);

        void set_bgcolor(const color &color);
        color get_bgcolor() const noexcept;
        void draw_back_ground() const noexcept;

        // Statistics
        void compute_frame_time();
        double get_frame_time() const noexcept;

        // Move
        void move_back_forth(int forward, double deltaTime = 1e-1);
        void move_left_right(int right, double deltaTime = 1e-1);

        // Update
        void update_front();
        void update_angle(double yawOffset = 0, double pitchOffset = 0);
        // == Actual post redisplay only occurs here.
        void update_view();
        void update_proj(int width, int height);

        // View, projection matrix
        mat4d& get_view_matrix() noexcept;
        mat4d& get_projection_matrix() noexcept;
        const mat4d& get_view_matrix_c() const noexcept;
        const mat4d& get_projection_matrix_c() const noexcept;

        void view_lookat(const vec3d &pos, const vec3d &target, const vec3d &up);
        void projection_ortho(double left, double right, double bottom, double top, double near, double far);
        void projection_persp(double fovy, double aspect, double near, double far);
    };
}

#endif