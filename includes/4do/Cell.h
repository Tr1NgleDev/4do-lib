#pragma once

#include "basicIncludes.h"

namespace fdo
{
	struct Cell
	{
		std::vector<int32_t> tIndices; // Indices pointing to tetrahedra.

		std::string toString() const
		{
			if(tIndices.empty()) return "";

			std::string result;

			for(auto& t : tIndices)
				result += std::format("{} ", t);

			result.erase(result.size() - 1, 1);

			return result;
		}
	};
}
