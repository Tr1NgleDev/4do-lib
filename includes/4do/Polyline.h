#pragma once

#include "basicIncludes.h"

namespace fdo
{
	struct Polyline
	{
		std::vector<int32_t> vIndices{}; // Indices pointing to vertices.
		std::vector<int32_t> vnIndices{}; // Indices pointing to vertex normals.
		std::vector<int32_t> vtIndices{}; // Indices pointing to texture coordinates.
		std::vector<int32_t> coIndices{}; // Indices pointing to colors.

		uint32_t length() { return vIndices.size(); }

		constexpr std::vector<int32_t>& operator[](FDataType type)
		{
			switch(type)
			{
			case FDataType::None:
			case FDataType::v: return vIndices;
			case FDataType::vn: return vnIndices;
			case FDataType::vt: return vtIndices;
			case FDataType::co: return coIndices;
			}
			return vIndices;
		}
	};
}
