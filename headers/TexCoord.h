#pragma once

#include "basicIncludes.h"

#ifdef GLM_SETUP_INCLUDED
struct glm::vec3;
#endif

namespace fdo
{
	struct TexCoord
	{
		union { float i,x,u; };
		union { float j,y,v; };
		union { float k,z,w; };

		TexCoord(float i = 0, float j = 0, float k = 0) : i(i), j(j), k(k) {}
		TexCoord(float v) : i(v), j(v), k(v) {}
		TexCoord(const TexCoord& other)
		{
			this->i = other.i;
			this->j = other.j;
			this->k = other.k;
		}
		TexCoord(TexCoord&& other)
		{
			this->i = other.i;
			this->j = other.j;
			this->k = other.k;

			other.i = 0;
			other.j = 0;
			other.k = 0;
		}

		constexpr TexCoord& operator=(const TexCoord& other)
		{
			this->i = other.i;
			this->j = other.j;
			this->k = other.k;

			return *this;
		}
		constexpr TexCoord& operator=(TexCoord&& other)
		{
			this->i = other.i;
			this->j = other.j;
			this->k = other.k;

			other.i = 0;
			other.j = 0;
			other.k = 0;

			return *this;
		}

		constexpr bool operator==(const TexCoord& other) const { return i == other.i && j == other.j && k == other.k; }

		TexCoord operator+(const TexCoord& other) const { return TexCoord{i + other.i, j + other.j, k + other.k}; }
		TexCoord& operator+=(const TexCoord& other) { return *this = *this + other; }

		TexCoord operator-(const TexCoord& other) const { return TexCoord{i - other.i, j - other.j, k - other.k}; }
		TexCoord& operator-=(const TexCoord& other) { return *this = *this - other; }

		TexCoord operator/(const TexCoord& other) const { return TexCoord{i / other.i, j / other.j, k / other.k}; }
		TexCoord& operator/=(const TexCoord& other) { return *this = *this / other; }

		TexCoord operator*(const TexCoord& other) const { return TexCoord{i * other.i, j * other.j, k * other.k}; }
		TexCoord& operator*=(const TexCoord& other) { return *this = *this * other; }

		TexCoord operator+(float other) const { return TexCoord{i + other, j + other, k + other}; }
		TexCoord& operator+=(float other) { return *this = *this + other; }

		TexCoord operator-(float other) const { return TexCoord{i - other, j - other, k - other}; }
		TexCoord& operator-=(float other) { return *this = *this - other; }

		TexCoord operator/(float other) const { return TexCoord{i / other, j / other, k / other}; }
		TexCoord& operator/=(float other) { return *this = *this / other; }

		TexCoord operator*(float other) const { return TexCoord{i * other, j * other, k * other}; }
		TexCoord& operator*=(float other) { return *this = *this * other; }

		constexpr float& operator[](size_t i)
		{
			if(i < 0 || i >= 3)
				throw std::out_of_range("fdo::TexCoord::operator[]: Index out of range.");
			return *(&this->i + i);
		}
		constexpr const float operator[](size_t i) const
		{
			if(i < 0 || i >= 3)
				throw std::out_of_range("fdo::Point::operator[]: Index out of range.");
			return *(&this->i + i);
		}

		std::string toString() const
		{
			return std::format("{} {} {}", i, j, k);
		}

		inline static constexpr float lengthSqr(const TexCoord& v)
		{
			return
				v.i * v.i +
					v.j * v.j +
					v.k * v.k;
		}

		inline static constexpr float length(const TexCoord& v)
		{
			float lSqr = lengthSqr(v);
			return sqrtf(lSqr);
		}

		inline static constexpr float dot(const TexCoord& l, const TexCoord& r)
		{
			return l.i * r.i + l.j * r.j + l.k * r.k;
		}

		inline static TexCoord normalize(const TexCoord& v)
		{
			float l = length(v);
			if(l <= 0.00000001) l = 1;
			float lInv = 1.f / l;
			return v * lInv;
		}

		// additional operators/constructors for compatibility with glm::vec3
		#ifdef GLM_SETUP_INCLUDED
		TexCoord(const glm::vec3& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;
		}

		TexCoord(glm::vec3&& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;

			other.x = 0;
			other.y = 0;
			other.z = 0;
		}

		constexpr TexCoord& operator=(const glm::vec3& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;

			return *this;
		}

		constexpr TexCoord& operator=(glm::vec3&& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->z = other.z;

			other.x = 0;
			other.y = 0;
			other.z = 0;

			return *this;
		}

		operator glm::vec3() const { return glm::vec3{ x, y, z }; }
		#endif
	};
	typedef TexCoord vec3;
}

namespace std
{
	inline ostream& operator<<(ostream& os, const fdo::TexCoord& c)
	{
		return os << c.toString();
	}
	inline string to_string(const fdo::TexCoord& c) noexcept
	{
		return c.toString();
	}
}
