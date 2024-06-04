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

		constexpr uint32_t length() const { return vIndices.size(); }

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
		constexpr const std::vector<int32_t>& operator[](FDataType type) const
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
			if(length() == 0) return Format{{}, {}};

			Format result{{}, {}};

			if(!vIndices.empty() && vIndices[0] != -1)
			{
				bool levelData = std::all_of(vIndices.begin(), vIndices.end(), [&](const int& i) { return i == vIndices.front(); });
				if(!levelData)
					result.indices.push_back(FDataType::v);
				else
					result.levelData.push_back(FDataType::v);
			}
			if(!vnIndices.empty() && vnIndices[0] != -1)
			{
				bool levelData = std::all_of(vnIndices.begin(), vnIndices.end(), [&](const int& i) { return i == vnIndices.front(); });
				if(!levelData)
					result.indices.push_back(FDataType::vn);
				else
					result.levelData.push_back(FDataType::vn);
			}
			if(!vtIndices.empty() && vtIndices[0] != -1)
			{
				bool levelData = std::all_of(vtIndices.begin(), vtIndices.end(), [&](const int& i) { return i == vtIndices.front(); });
				if(!levelData)
					result.indices.push_back(FDataType::vt);
				else
					result.levelData.push_back(FDataType::vt);
			}
			if(!coIndices.empty() && coIndices[0] != -1)
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
			if(length() == 0) return "";

			std::string result;

			for(auto& t : format.levelData)
				result += std::format("{} ", operator[](t)[0]);

			for(int i = 0; i < length(); i++)
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
