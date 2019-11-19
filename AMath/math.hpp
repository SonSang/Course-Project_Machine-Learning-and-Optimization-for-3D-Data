#ifndef __AF_MATH_H__
#define __AF_MATH_H__

#ifdef _MSV_VER
#pragma once
#endif

#define SQ(X) ((X) * (X))

#include <array>

namespace AF {
	const double pi = 3.14159265358979323846;
	const double pi05 = pi * 0.5;
	const double pi15 = pi * 1.5;
	const double pi20 = pi * 2.0;

	double deg2rad(double degree);
	double rad2deg(double radian);

	class vec2d : public std::array<double, 2> {
	public:
		vec2d() = default;
		vec2d(double x, double y);
		void set(double x, double y);
		void clear();
	};

	class vec3d : public std::array<double, 3> {
	public:
		vec3d() = default;
		vec3d(double x, double y, double z);
		void set(double x, double y, double z);
		vec3d operator+(const vec3d &v) const;
		vec3d operator-(const vec3d &v) const;
		vec3d operator*(double c) const;
		vec3d operator/(double c) const;
		void operator+=(const vec3d &v);
		void operator-=(const vec3d &v);
		void operator*=(double c);
		void operator/=(double c);
		double dot(const vec3d &v) const noexcept;
		vec3d cross(const vec3d &v) const noexcept;
		double lensq() const noexcept;
		double len() const noexcept;
		double distsq(const vec3d &v) const noexcept;
		double dist(const vec3d &v) const noexcept;
		vec3d normalize() const;
		void normalize();
		void clear();
	};

	class mat3d : public std::array<std::array<double, 3>, 3> {
	public:
		void set(int i, int j, double v);
		vec3d operator*(const vec3d &v) const noexcept;
		mat3d operator*(const mat3d &m2) const noexcept;
		void operator*=(const mat3d &m2) noexcept;
		mat3d transpose() const noexcept;
		void clear() noexcept;
		void identity() noexcept;
	};

	class mat4d : public std::array<std::array<double, 4>, 4> {
	public:
		void set(int i, int j, double v);
		mat4d transpose() const noexcept;
		void clear() noexcept;
		void identity() noexcept;

		void to_array(double arr[16], bool transpose) const noexcept;
		void to_array(float arr[16], bool transpose) const noexcept;
	};

	using translation = vec3d;
	using rotation = mat3d;

	class transform {
	private:
		translation T;
		rotation	R;
	public:
		transform();
		void set_translation(const translation &T);
		void set_rotation(const rotation &R);

		void identity();
		vec3d apply(const vec3d &pt) const noexcept;
		vec3d applyR(const vec3d &pt) const noexcept;

		// update this transform by applying [follow] transform to it.
		void update(const transform &follow);
		transform update(const transform &follow) const noexcept;

		// Given transform [A], [B], get transform that moves a point in local coordinates in [A] to local coordinates in [B].
		static void move(const transform &A, const transform &B, transform &atob);

		void to_array(double arr[16], bool transpose) const noexcept;
		void to_array(float arr[16], bool transpose) const noexcept;
	};
}

#endif