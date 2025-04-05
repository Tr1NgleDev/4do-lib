#pragma once

#include "basicIncludes.h"

namespace fdo
{
	struct Color
	{
		uint8_t r,g,b,a;

		Color(int r = 255, int g = 255, int b = 255, int a = 255)
			: r(r), g(g), b(b), a(a) {}
		Color(float r, float g, float b, float a) :
			r(std::clamp((int)(std::clamp(r, 0.0f, 1.0f) * 255), 0, 255)),
			g(std::clamp((int)(std::clamp(g, 0.0f, 1.0f) * 255), 0, 255)),
			b(std::clamp((int)(std::clamp(b, 0.0f, 1.0f) * 255), 0, 255)),
			a(std::clamp((int)(std::clamp(a, 0.0f, 1.0f) * 255), 0, 255)) {}
		Color(const Color& other)
		{
			this->r = other.r;
			this->g = other.g;
			this->b = other.b;
			this->a = other.a;
		}
		Color(Color&& other) noexcept
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
		constexpr Color& operator=(Color&& other) noexcept
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

		Color operator+(const Color& other) const { return Color{ r / 255.0f + (other.r / 255.0f), g / 255.0f + (other.g / 255.0f), b / 255.0f + (other.b / 255.0f), a / 255.0f + (other.a / 255.0f) }; }
		Color& operator+=(const Color& other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) + (other.r / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) + (other.g / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) + (other.b / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) + (other.a / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator-(const Color& other) const { return Color{ r / 255.0f - (other.r / 255.0f), g / 255.0f - (other.g / 255.0f), b / 255.0f - (other.b / 255.0f), a / 255.0f - (other.a / 255.0f) }; }
		Color& operator-=(const Color& other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) - (other.r / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) - (other.g / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) - (other.b / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) - (other.a / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator/(const Color& other) const { return Color{ r / 255.0f / (other.r / 255.0f), g / 255.0f / (other.g / 255.0f), b / 255.0f / (other.b / 255.0f), a / 255.0f / (other.a / 255.0f) }; }
		Color& operator/=(const Color& other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) / (other.r / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) / (other.g / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) / (other.b / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) / (other.a / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator*(const Color& other) const { return Color{ r / 255.0f * (other.r / 255.0f), g / 255.0f * (other.g / 255.0f), b / 255.0f * (other.b / 255.0f), a / 255.0f * (other.a / 255.0f) }; }
		Color& operator*=(const Color& other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) * (other.r / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) * (other.g / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) * (other.b / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) * (other.a / 255.0f), 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator+(float other) const { return Color{ r / 255.0f + other, g / 255.0f + other, b / 255.0f + other, a / 255.0f + other }; }
		Color& operator+=(float other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) + other, 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) + other, 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) + other, 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) + other, 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator-(float other) const { return Color{ r / 255.0f - other, g / 255.0f - other, b / 255.0f - other, a / 255.0f - other }; }
		Color& operator-=(float other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) - other, 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) - other, 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) - other, 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) - other, 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator/(float other) const { return Color{ r / 255.0f / other, g / 255.0f / other, b / 255.0f / other, a / 255.0f / other }; }
		Color& operator/=(float other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) / other, 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) / other, 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) / other, 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) / other, 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator*(float other) const { return Color{ r / 255.0f * other, g / 255.0f * other, b / 255.0f * other, a / 255.0f * other }; }
		Color& operator*=(float other)
		{
			r = std::clamp((int)(std::clamp((r / 255.0f) * other, 0.0f, 1.0f) * 255), 0, 255);
			g = std::clamp((int)(std::clamp((g / 255.0f) * other, 0.0f, 1.0f) * 255), 0, 255);
			b = std::clamp((int)(std::clamp((b / 255.0f) * other, 0.0f, 1.0f) * 255), 0, 255);
			a = std::clamp((int)(std::clamp((a / 255.0f) * other, 0.0f, 1.0f) * 255), 0, 255);
			return *this;
		}

		Color operator-() const { return Color{ (uint8_t)(255 - r), (uint8_t)(255 - g), (uint8_t)(255 - b), (uint8_t)(255 - a) }; }

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

		Color(glm::u8vec4&& other) noexcept
		{
			this->r = other.x;
			this->g = other.y;
			this->b = other.z;
			this->a = other.w;

			other.x = 0;
			other.y = 0;
			other.z = 0;
			other.w = 0;
		}

		constexpr Color& operator=(const glm::u8vec4& other)
		{
			this->r = other.x;
			this->g = other.y;
			this->b = other.z;
			this->a = other.w;

			return *this;
		}

		constexpr Color& operator=(glm::u8vec4&& other) noexcept
		{
			this->r = other.x;
			this->g = other.y;
			this->b = other.z;
			this->a = other.w;

			other.x = 0;
			other.y = 0;
			other.z = 0;
			other.w = 0;

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
