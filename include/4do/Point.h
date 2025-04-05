#pragma once

#include "basicIncludes.h"

namespace fdo
{
	struct Point
	{
		float x,y,z,w;

		Point(float x = 0, float y = 0, float z = 0, float w = 0) : x(x), y(y), z(z), w(w) {}
		Point(float v) : x(v), y(v), z(v), w(v) {}
		Point(const Point& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;
		}
		Point(Point&& other) noexcept
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;

			other.x = 0;
			other.y = 0;
			other.z = 0;
			other.w = 0;
		}

		constexpr Point& operator=(const Point& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;

			return *this;
		}
		constexpr Point& operator=(Point&& other) noexcept
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;

			other.x = 0;
			other.y = 0;
			other.z = 0;
			other.w = 0;

			return *this;
		}

		constexpr bool operator==(const Point& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }

		Point operator+(const Point& other) const { return Point{x + other.x, y + other.y, z + other.z, w + other.w}; }
		Point& operator+=(const Point& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			w += other.w;
			return *this;
		}

		Point operator-(const Point& other) const { return Point{x - other.x, y - other.y, z - other.z, w - other.w}; }
		Point& operator-=(const Point& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			w -= other.w;
			return *this;
		}

		Point operator/(const Point& other) const { return Point{x / other.x, y / other.y, z / other.z, w / other.w}; }
		Point& operator/=(const Point& other)
		{
			x /= other.x;
			y /= other.y;
			z /= other.z;
			w /= other.w;
			return *this;
		}

		Point operator*(const Point& other) const { return Point{x * other.x, y * other.y, z * other.z, w * other.w}; }
		Point& operator*=(const Point& other)
		{
			x *= other.x;
			y *= other.y;
			z *= other.z;
			w *= other.w;
			return *this;
		}

		Point operator+(float other) const { return Point{x + other, y + other, z + other, w + other}; }
		Point& operator+=(float other)
		{
			x += other;
			y += other;
			z += other;
			w += other;
			return *this;
		}

		Point operator-(float other) const { return Point{x - other, y - other, z - other, w - other}; }
		Point& operator-=(float other)
		{
			x -= other;
			y -= other;
			z -= other;
			w -= other;
			return *this;
		}

		Point operator/(float other) const { return Point{x / other, y / other, z / other, w / other}; }
		Point& operator/=(float other)
		{
			x /= other;
			y /= other;
			z /= other;
			w /= other;
			return *this;
		}

		Point operator*(float other) const { return Point{x * other, y * other, z * other, w * other}; }
		Point& operator*=(float other)
		{
			x *= other;
			y *= other;
			z *= other;
			w *= other;
			return *this;
		}

		Point operator-() const { return Point{ -x, -y, -z, -w }; }

		constexpr float& operator[](size_t i)
		{
			if(i < 0 || i >= 4)
				throw std::out_of_range("fdo::Point::operator[]: Index out of range.");
			return *(&x + i);
		}
		constexpr const float operator[](size_t i) const
		{
			if(i < 0 || i >= 4)
				throw std::out_of_range("fdo::Point::operator[]: Index out of range.");
			return *(&x + i);
		}

		std::string toString() const
		{
			return std::format("{} {} {} {}", x, y, z, w);
		}

		inline static constexpr float lengthSqr(const Point& v)
		{
			return
				v.x * v.x +
				v.y * v.y +
				v.z * v.z +
				v.w * v.w;
		}

		inline static float length(const Point& v)
		{
			float lSqr = lengthSqr(v);
			return sqrtf(lSqr);
		}

		inline static constexpr float dot(const Point& l, const Point& r)
		{
			return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
		}

		inline static Point cross(const Point& u, const Point& v, const Point& w)
		{
			//  intermediate values
			float a = (v.x * w.y) - (v.y * w.x);
			float b = (v.x * w.z) - (v.z * w.x);
			float c = (v.x * w.w) - (v.w * w.x);
			float d = (v.y * w.z) - (v.z * w.y);
			float e = (v.y * w.w) - (v.w * w.y);
			float f = (v.z * w.w) - (v.w * w.z);

			// result vector
			Point res;

			res.x = (u.y * f) - (u.z * e) + (u.w * d);
			res.y = -(u.x * f) + (u.z * c) - (u.w * b);
			res.z = (u.x * e) - (u.y * c) + (u.w * a);
			res.w = -(u.x * d) + (u.y * b) - (u.z * a);

			return res;
		}

		inline static Point normalize(const Point& v)
		{
			float l = length(v);
			if(l <= 0.00000001) l = 1;
			float lInv = 1.f / l;
			return v * lInv;
		}

		// additional operators/constructors for compatibility with glm::vec4
		#ifdef GLM_SETUP_INCLUDED
		Point(const glm::vec4& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;
		}

		Point(glm::vec4&& other) noexcept
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;

			other.x = 0;
			other.y = 0;
			other.z = 0;
			other.w = 0;
		}

		constexpr Point& operator=(const glm::vec4& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;

			return *this;
		}

		constexpr Point& operator=(glm::vec4&& other) noexcept
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
			this->w = other.w;

			other.x = 0;
			other.y = 0;
			other.z = 0;
			other.w = 0;

			return *this;
		}

		operator glm::vec4() const { return glm::vec4{ x, y, z, w }; }
		#endif
	};

	// additional names

	typedef Point vec4;
}

namespace std
{
	inline ostream& operator<<(ostream& os, const fdo::Point& p)
	{
		return os << p.toString();
	}
	inline string to_string(const fdo::Point& p) noexcept
	{
		return p.toString();
	}
}
