#pragma once

#include "basicIncludes.h"

namespace fdo
{
	struct TexCoord
	{
		union { float u,x,i; };
		union { float v,y,j; };
		union { float w,z,k; };

		TexCoord(float u = 0, float v = 0, float w = 0) : u(u), v(v), w(w) {}
		TexCoord(float v) : u(v), v(v), w(v) {}
		TexCoord(const TexCoord& other)
		{
			this->u = other.u;
			this->v = other.v;
			this->w = other.w;
		}
		TexCoord(TexCoord&& other) noexcept
		{
			this->u = other.u;
			this->v = other.v;
			this->w = other.w;

			other.u = 0;
			other.v = 0;
			other.w = 0;
		}

		constexpr TexCoord& operator=(const TexCoord& other)
		{
			this->u = other.u;
			this->v = other.v;
			this->w = other.w;

			return *this;
		}
		constexpr TexCoord& operator=(TexCoord&& other) noexcept
		{
			this->u = other.u;
			this->v = other.v;
			this->w = other.w;

			other.u = 0;
			other.v = 0;
			other.w = 0;

			return *this;
		}

		constexpr bool operator==(const TexCoord& other) const { return u == other.u && v == other.v && w == other.w; }

		TexCoord operator+(const TexCoord& other) const { return TexCoord{u + other.u, v + other.v, w + other.w}; }
		TexCoord& operator+=(const TexCoord& other)
		{
			u += other.u;
			v += other.v;
			w += other.w;
			return *this;
		}

		TexCoord operator-(const TexCoord& other) const { return TexCoord{u - other.u, v - other.v, w - other.w}; }
		TexCoord& operator-=(const TexCoord& other)
		{
			u -= other.u;
			v -= other.v;
			w -= other.w;
			return *this;
		}

		TexCoord operator/(const TexCoord& other) const { return TexCoord{u / other.u, v / other.v, w / other.w}; }
		TexCoord& operator/=(const TexCoord& other)
		{
			u /= other.u;
			v /= other.v;
			w /= other.w;
			return *this;
		}

		TexCoord operator*(const TexCoord& other) const { return TexCoord{u * other.u, v * other.v, w * other.w}; }
		TexCoord& operator*=(const TexCoord& other)
		{
			u *= other.u;
			v *= other.v;
			w *= other.w;
			return *this;
		}

		TexCoord operator+(float other) const { return TexCoord{u + other, v + other, w + other}; }
		TexCoord& operator+=(float other)
		{
			u += other;
			v += other;
			w += other;
			return *this;
		}

		TexCoord operator-(float other) const { return TexCoord{u - other, v - other, w - other}; }
		TexCoord& operator-=(float other)
		{
			u -= other;
			v -= other;
			w -= other;
			return *this;
		}

		TexCoord operator/(float other) const { return TexCoord{u / other, v / other, w / other}; }
		TexCoord& operator/=(float other)
		{
			u /= other;
			v /= other;
			w /= other;
			return *this;
		}

		TexCoord operator*(float other) const { return TexCoord{u * other, v * other, w * other}; }
		TexCoord& operator*=(float other)
		{
			u *= other;
			v *= other;
			w *= other;
			return *this;
		}

		constexpr float& operator[](size_t i)
		{
			if(i < 0 || i >= 3)
				throw std::out_of_range("fdo::TexCoord::operator[]: Index out of range.");
			return *(&u + i);
		}
		constexpr const float operator[](size_t i) const
		{
			if(i < 0 || i >= 3)
				throw std::out_of_range("fdo::TexCoord::operator[]: Index out of range.");
			return *(&u + i);
		}

		std::string toString() const
		{
			return std::format("{} {} {}", u, v, w);
		}

		inline static constexpr float lengthSqr(const TexCoord& v)
		{
			return
				v.u * v.u +
					v.v * v.v +
					v.w * v.w;
		}

		inline static float length(const TexCoord& v)
		{
			float lSqr = lengthSqr(v);
			return sqrtf(lSqr);
		}

		inline static constexpr float dot(const TexCoord& l, const TexCoord& r)
		{
			return l.u * r.u + l.v * r.v + l.w * r.w;
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

		TexCoord(glm::vec3&& other) noexcept
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

		constexpr TexCoord& operator=(glm::vec3&& other) noexcept
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
