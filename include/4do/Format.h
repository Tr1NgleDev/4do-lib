#pragma once

#include "basicIncludes.h"

namespace fdo
{
	// Format Data Types for tformat and pformat keywords
	enum class FDataType : uint8_t
	{
		None, // none
		v, // Vertex Position
		vn, // Vertex Normal
		vt, // Vertex Texture Coordinate
		co, // Color Data (Vertex Color)
	};
	inline std::string FDataTypeToString(FDataType type)
	{
		switch(type)
		{
		case FDataType::None: return "";
		case FDataType::v: return "v";
		case FDataType::vn: return "vn";
		case FDataType::vt: return "vt";
		case FDataType::co: return "co";
		}
		return "";
	}
	inline FDataType StringToFDataType(const std::string& str)
	{
		if(str == "v") return FDataType::v;
		if(str == "vn") return FDataType::vn;
		if(str == "vt") return FDataType::vt;
		if(str == "co") return FDataType::co;
		return FDataType::None;
	}
	struct Format
	{
		std::vector<FDataType> indices{ FDataType::v };
		std::vector<FDataType> levelData{};

		std::string toString() const
		{
			std::string result;
			for(auto& l : levelData)
				result += std::format("{} ", FDataTypeToString(l));

			for(auto& f : indices)
				result += std::format("{}/", FDataTypeToString(f));

			result.erase(result.size() - 1, 1); // remove the last /

			return result;
		}
	};
}
