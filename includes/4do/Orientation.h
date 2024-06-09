#pragma once

#include "basicIncludes.h"
#include "Point.h"

namespace fdo
{
	enum Axis : int
	{
		UNKNOWN,

		X, // +X
		Y, // +Y
		Z, // +Z
		W, // +W

		N_X, // -X
		N_Y, // -Y
		N_Z, // -Z
		N_W // -W
	};
	inline std::string AxisToString(Axis a)
	{
		switch(a)
		{
		case UNKNOWN: return "";
		case X: return "X";
		case Y: return "Y";
		case Z: return "Z";
		case W: return "W";
		case N_X: return "-X";
		case N_Y: return "-Y";
		case N_Z: return "-Z";
		case N_W: return "-W";
		}
		return "";
	}
	inline Axis StringToAxis(const std::string& str)
	{
		std::string s = utils::toUpperCopy(str);
		if(s == "X") return X;
		if(s == "Y") return Y;
		if(s == "Z") return Z;
		if(s == "W") return W;
		if(s == "-X") return N_X;
		if(s == "-Y") return N_Y;
		if(s == "-Z") return N_Z;
		if(s == "-W") return N_W;
		return UNKNOWN;
	}
	struct Orientation
	{
		Axis a = X, b = Y, c = Z, d = W;

		constexpr bool isDefault() const { return a == X && b == Y && c == Z && d == W; }

		// Returns an Axis&
		constexpr Axis& get(size_t i)
		{
			if(i < 0 || i >= 4)
				throw std::out_of_range("fdo::Orientation::operator[]: Index out of range.");
			return *(&a + i);
		}
		// Returns an Axis
		constexpr const Axis get(size_t i) const
		{
			if(i < 0 || i >= 4)
				throw std::out_of_range("fdo::Orientation::operator[]: Index out of range.");
			return *(&a + i);
		}
		// Returns an index for vector/point
		constexpr size_t getInd(size_t i) const
		{
			Axis a = get(i);
			return (a >= N_X ? a - N_X : a - 1);
		}
		// Returns the sign (1 or -1)
		constexpr float getSign(size_t i) const
		{
			Axis a = get(i);
			return a >= N_X ? -1 : 1;
		}

		constexpr Axis& operator[](size_t i) { return get(i); }
		constexpr const Axis operator[](size_t i) const { return get(i); }

		std::string toString() const
		{
			return std::format("{} {} {} {}", AxisToString(a), AxisToString(b), AxisToString(c), AxisToString(d));
		}

		inline static Point transform(const Point& point, const Orientation& from, const Orientation& to)
		{
			Point result{};
			for(size_t i = 0; i < 4; i++)
				result[to.getInd(i)] = point[from.getInd(i)] * from.getSign(i) * to.getSign(i);
			return result;
		}
	};
}
