#pragma once

#include "basicIncludes.h"
#include "Format.h"

namespace fdo
{
	struct Tetrahedron
	{
		std::array<int32_t, 4> vIndices{-1,-1,-1,-1}; // Indices pointing to vertices.
		std::array<int32_t, 4> vnIndices{-1,-1,-1,-1}; // Indices pointing to vertex normals.
		std::array<int32_t, 4> vtIndices{-1,-1,-1,-1}; // Indices pointing to texture coordinates.
		std::array<int32_t, 4> coIndices{-1,-1,-1,-1}; // Indices pointing to colors.

		constexpr std::array<int32_t, 4>& operator[](FDataType type)
		{
			switch(type)
			{
			case FDataType::None:
			case FDataType::v: return vIndices;
			case FDataType::vn: return vnIndices;
			case FDataType::vt: return vtIndices;
			case FDataType::co: return coIndices;
			}
		}
	};
}
