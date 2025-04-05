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
			return vIndices;
		}
		constexpr const std::array<int32_t, 4>& operator[](FDataType type) const
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

		Format guessFormat() const
		{
			Format result{{}, {}};

			if(vIndices[0] != -1)
			{
				bool levelData = std::all_of(vIndices.begin(), vIndices.end(), [&](const int& i) { return i == vIndices.front(); });
				if(!levelData)
					result.indices.push_back(FDataType::v);
				else
					result.levelData.push_back(FDataType::v);
			}
			if(vnIndices[0] != -1)
			{
				bool levelData = std::all_of(vnIndices.begin(), vnIndices.end(), [&](const int& i) { return i == vnIndices.front(); });
				if(!levelData)
					result.indices.push_back(FDataType::vn);
				else
					result.levelData.push_back(FDataType::vn);
			}
			if(vtIndices[0] != -1)
			{
				bool levelData = std::all_of(vtIndices.begin(), vtIndices.end(), [&](const int& i) { return i == vtIndices.front(); });
				if(!levelData)
					result.indices.push_back(FDataType::vt);
				else
					result.levelData.push_back(FDataType::vt);
			}
			if(coIndices[0] != -1)
			{
				bool levelData = std::all_of(coIndices.begin(), coIndices.end(), [&](const int& i) { return i == coIndices.front(); });
				if(!levelData)
					result.indices.push_back(FDataType::co);
				else
					result.levelData.push_back(FDataType::co);
			}

			return result;
		}

		std::string toString(const Format& format) const
		{
			std::string result;

			for(auto& t : format.levelData)
				result += std::format("{} ", operator[](t)[0]);

			for(int i = 0; i < 4; i++)
			{
				for(auto& t : format.indices)
					result += std::format("{}/", operator[](t)[i]);
				result.erase(result.size() - 1, 1);
				result += ' ';
			}

			result.erase(result.size() - 1, 1);

			return result;
		}
	};
}
