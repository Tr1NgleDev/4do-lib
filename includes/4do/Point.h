#pragma once

#include "basicIncludes.h"

#ifdef GLM_SETUP_INCLUDED
struct glm::vec4;
#endif

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
		Point(Point&& other)
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
		constexpr Point& operator=(Point&& other)
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

		Point(glm::vec4&& other)
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

		constexpr Point& operator=(glm::vec4&& other)
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
