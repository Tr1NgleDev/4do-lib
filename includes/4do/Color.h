#pragma once

#include "basicIncludes.h"

#ifdef GLM_SETUP_INCLUDED
struct glm::u8vec4;
#endif

namespace fdo
{
	struct Color
	{
		uint8_t r,g,b,a;

		Color(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
		Color(uint8_t v) : r(v), g(v), b(v), a(v) {}
		Color(const Color& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;
		}
		Color(Color&& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			other.r = 0;
			other.g = 0;
			other.b = 0;
			other.a = 0;
		}

		constexpr Color& operator=(const Color& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			return *this;
		}
		constexpr Color& operator=(Color&& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;

			other.r = 0;
			other.g = 0;
			other.b = 0;
			other.a = 0;

			return *this;
		}

		constexpr bool operator==(const Color& other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }

		constexpr uint8_t& operator[](size_t i)
		{
			if(i < 0 || i >= 4)
				throw std::out_of_range("fdo::Color::operator[]: Index out of range.");
			return *(&r + i);
		}
		constexpr const uint8_t operator[](size_t i) const
		{
			if(i < 0 || i >= 4)
				throw std::out_of_range("fdo::Color::operator[]: Index out of range.");
			return *(&r + i);
		}

		std::string toString() const
		{
			return std::format("{} {} {} {}", r, g, b, a);
		}

		// additional operators/constructors for compatibility with glm::u8vec4
		#ifdef GLM_SETUP_INCLUDED
		Color(const glm::u8vec4& other)
		{
			this->r = other.x;
			this->g = other.y;
			this->b = other.z;
			this->a = other.w;
		}

		Color(glm::u8vec4&& other)
		{
			this->r = other.x;
			this->g = other.y;
			this->b = other.z;
			this->a = other.z;

			other.x = 0;
			other.y = 0;
			other.z = 0;
			other.z = 0;
		}

		constexpr Color& operator=(const glm::u8vec4& other)
		{
			this->r = other.x;
			this->g = other.y;
			this->b = other.z;
			this->a = other.w;

			return *this;
		}

		constexpr Color& operator=(glm::u8vec4&& other)
		{
			this->r = other.x;
			this->g = other.y;
			this->b = other.z;
			this->a = other.w;

			other.r = 0;
			other.g = 0;
			other.b = 0;
			other.a = 0;

			return *this;
		}

		operator glm::u8vec4() const { return glm::u8vec4{ r, g, b, a }; }
		#endif
	};
	typedef Color u8vec4;
}

namespace std
{
	inline ostream& operator<<(ostream& os, const fdo::Color& c)
	{
		return os << c.toString();
	}
	inline string to_string(const fdo::Color& c) noexcept
	{
		return c.toString();
	}
}
