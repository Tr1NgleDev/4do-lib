#pragma once

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <format>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <set>
#include <locale>
#include <functional>

// utils (mostly for strings)
namespace fdo::utils
{
	inline constexpr void trimStart(std::string& s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](uint8_t c) {
		  return !std::isspace(c);
		}));
	}
	inline constexpr void trimEnd(std::string& s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](uint8_t c) {
		  return !std::isspace(c);
		}).base(), s.end());
	}
	inline constexpr void trim(std::string& s)
	{
		trimStart(s);
		trimEnd(s);
	}
	inline constexpr bool isNumber(const std::string& s)
	{
		return !s.empty() && std::find_if(s.begin(),
			s.end(), [](uint8_t c) { return !std::isdigit(c); }) == s.end();
	}
	inline constexpr void toLower(std::string& s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[](uint8_t c){ return std::tolower(c); });
	}
	inline constexpr void toUpper(std::string& s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[](uint8_t c){ return std::toupper(c); });
	}
	inline constexpr std::string toLowerCopy(const std::string& s)
	{
		std::string result = s;
		toLower(result);
		return result;
	}
	inline constexpr std::string toUpperCopy(const std::string& s)
	{
		std::string result = s;
		toUpper(result);
		return result;
	}
	inline constexpr bool isWhiteSpaceOrEmpty(const std::string& s)
	{
		return std::all_of(s.begin(),s.end(),[](uint8_t c){ return std::isspace(c); });
	}
	// Splits a string by a delimiter.
	inline constexpr std::vector<std::string> split(const std::string& str, char delim, bool skipWhitespace = false)
	{
		std::vector<std::string> tokens;
		size_t pos = 0;
		size_t len = str.length();
		tokens.reserve(len / 2);  // allocate memory for expected number of tokens

		while (pos < len)
		{
			size_t end = str.find_first_of(delim, pos);
			if (end == std::string::npos)
			{
				tokens.emplace_back(str.substr(pos));
				break;
			}
			std::string subStr = str.substr(pos, end - pos);
			if(!skipWhitespace || !isWhiteSpaceOrEmpty(subStr))
				tokens.emplace_back(subStr);
			pos = end + 1;
		}

		return tokens;
	}
	inline float toFloat(const std::string& s, bool& success)
	{
		char* p;
		float f = strtof(s.c_str(), &p);
		success = !(*p);
		return f;
	}
	inline long long toLL(const std::string& s, bool& success)
	{
		char* p;
		long long i;
		if(s.starts_with("0x"))
			i = strtoll(s.c_str(), &p, 16);
		else
			i = strtoll(s.c_str(), &p, 10);
		success = !(*p);
		return i;
	}
	inline constexpr bool inRangeII(float v, float min, float max) { return v >= min && v <= max; }
	inline constexpr bool inRangeIE(float v, float min, float max) { return v >= min && v < max; }
	inline constexpr bool inRangeEI(float v, float min, float max) { return v > min && v <= max; }
	inline constexpr bool inRangeEE(float v, float min, float max) { return v > min && v < max; }
	template<typename T>
	inline constexpr bool vectorContains(const std::vector<T>& vec, const T& obj)
	{
		return std::find(vec.begin(), vec.end(), obj) != vec.end();
	}
}
