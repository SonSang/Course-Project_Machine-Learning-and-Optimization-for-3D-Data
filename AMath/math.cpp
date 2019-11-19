#include "math.hpp"
#include <cmath>

namespace AF {
	// angles
	double deg2rad(double degree) {
		return (degree / 180.0) * pi;
	}
	double rad2deg(double radian) {
		return radian * (180 / pi);
	}
	// vec2d
	vec2d::vec2d(double x, double y) {
		set(x, y);
	}
	void vec2d::set(double x, double y) {
		(*this)[0] = x;
		(*this)[1] = y;
	}
	void vec2d::clear() {
		(*this)[0] = 0;
		(*this)[1] = 0;
	}
	// vec3d
	vec3d::vec3d(double x, double y, double z) {
        set(x, y, z);
	}
	void vec3d::set(double x, double y, double z) {
		auto &t = *this;
		t[0] = x;
		t[1] = y;
		t[2] = z;
	}
	vec3d vec3d::operator+(const vec3d &v) const {
		const auto &t = *this;
		return vec3d(
			t[0] + v[0],
			t[1] + v[1],
			t[2] + v[2]);
	}
	vec3d vec3d::operator-(const vec3d &v) const {
		const auto &t = *this;
		return vec3d(
			t[0] - v[0],
			t[1] - v[1],
			t[2] - v[2]);
	}
	vec3d vec3d::operator*(double c) const {
		const auto &t = *this;
		return vec3d(
			t[0] * c,
			t[1] * c,
			t[2] * c);
	}
	vec3d vec3d::operator/(double c) const {
		const auto &t = *this;
		return vec3d(
			t[0] / c,
			t[1] / c,
			t[2] / c);
	}
	void vec3d::operator+=(const vec3d &v) {
		auto &t = *this;
		t[0] += v[0];
		t[1] += v[1];
		t[2] += v[2];
	}
	void vec3d::operator-=(const vec3d &v) {
		auto &t = *this;
		t[0] -= v[0];
		t[1] -= v[1];
		t[2] -= v[2];
	}
	void vec3d::operator*=(double c) {
		auto &t = *this;
		t[0] *= c;
		t[1] *= c;
		t[2] *= c;
	}
	void vec3d::operator/=(double c) {
		auto &t = *this;
		t[0] /= c;
		t[1] /= c;
		t[2] /= c;
	}
	double vec3d::dot(const vec3d &v) const noexcept {
		const auto &t = *this;
		return
			t[0] * v[0] + t[1] * v[1] + t[2] * v[2];
	}
	vec3d vec3d::cross(const vec3d &v) const noexcept {
		return vec3d {
			(*this)[1] * v[2] - (*this)[2] * v[1],
			(*this)[2] * v[0] - (*this)[0] * v[2],
			(*this)[0] * v[1] - (*this)[1] * v[0]
		};
	}
	double vec3d::lensq() const noexcept {
		const auto &t = *this;
		return t.dot(t);
	}
	double vec3d::len() const noexcept {
		return sqrt(lensq());
	}
	double vec3d::distsq(const vec3d &v) const noexcept {
		const auto &t = *this;
		return (t - v).lensq();
	}
	double vec3d::dist(const vec3d &v) const noexcept {
		return sqrt(distsq(v));
	}
	vec3d vec3d::normalize() const {
		vec3d copy = *this;
		copy.normalize();
		return copy;
	}
	void vec3d::normalize() {
		auto &t = *this;
		t /= t.len();
	}
	void vec3d::clear() {
		set(0, 0, 0);
	}

	// mat3d
	void mat3d::set(int i, int j, double v) {
		auto &m = *this;
		m.at(i).at(j) = v;
	}
	vec3d mat3d::operator*(const vec3d &v) const noexcept {
		const auto &m = *this;
		double
			x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2],
			y = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2],
			z = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2];
		return vec3d(x, y, z);
	}
	mat3d mat3d::operator*(const mat3d &m2) const noexcept {
		const auto &m = *this;
		mat3d r;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				r[i][j] = 0;
				for (int k = 0; k < 3; k++)
					r[i][j] += m[i][k] * m2[k][j];
			}
		}
		return r;
	}
	void mat3d::operator*=(const mat3d &m2) noexcept {
		auto &m = *this;
		m = m * m2;
	}
	mat3d mat3d::transpose() const noexcept {
		const auto &m = *this;
		mat3d r;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				r[i][j] = m[j][i];
		return r;
	}
	void mat3d::clear() noexcept {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				set(i, j, 0);
	}
	void mat3d::identity() noexcept {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				set(i, j, (i == j) ? 1 : 0);
	}

	// mat4d
	void mat4d::set(int i, int j, double v) {
		(*this).at(i).at(j) = v;
	}
	mat4d mat4d::transpose() const noexcept {
		mat4d ret;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				ret[i][j] = (*this)[j][i];
		return ret;
	}
	void mat4d::clear() noexcept {
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				(*this)[i][j] = 0;
	}
	void mat4d::identity() noexcept {
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				(*this)[i][j] = (i == j) ? 1 : 0;
	}
	void mat4d::to_array(double arr[16], bool transpose) const noexcept {
		int idx = 0;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				arr[idx++] = (transpose ? (*this)[j][i] : (*this)[i][j]);
	}
	void mat4d::to_array(float arr[16], bool transpose) const noexcept {
		int idx = 0;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				arr[idx++] = (transpose ? (float)(*this)[j][i] : (float)(*this)[i][j]);
	}

	// transform
	transform::transform() {
		identity();
	}
	void transform::set_translation(const translation &T) {
		this->T = T;
	}
	void transform::set_rotation(const rotation &R) {
		this->R = R;
	}
	void transform::identity() {
		T.clear();
		R.identity();
	}
	vec3d transform::apply(const vec3d &pt) const noexcept {
		return R * pt + T;
	}
	vec3d transform::applyR(const vec3d &pt) const noexcept {
		return R * pt;
	}
	void transform::update(const transform &follow) {
		R = follow.R * R;
		T = follow.R * T + follow.T;
	}
	transform transform::update(const transform &follow) const noexcept {
		transform r;
		r.update(follow);
		return r;
	}
	void transform::move(const transform &A, const transform &B, transform &atob) {
		translation amb = A.T - B.T;
		rotation bt = B.R.transpose();
		atob.set_translation(bt * amb);
		atob.set_rotation(bt * A.R);
	}
	void transform::to_array(double arr[16], bool transpose) const noexcept {
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++)
				arr[4 * i + j] = R[j][i];
			arr[12 + i] = T[i];
		}			
	}
	void transform::to_array(float arr[16], bool transpose) const noexcept {
		for(int i = 0; i < 3; i++) {
			for(int j = 0; j < 3; j++)
				arr[4 * i + j] = (float)R[j][i];
			arr[12 + i] = (float)T[i];
		}			
	}
}