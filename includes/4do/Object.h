#pragma once

#include "basicIncludes.h"
#include "Point.h"
#include "Orientation.h"
#include "TexCoord.h"

#include "Tetrahedron.h"
#include "Polyline.h"
#include "Cell.h"

#include "Logger.h"

namespace fdo
{
	class Object
	{
	private:
		bool _invalid = false;

	public:
		uint8_t specVer = 1; // Specification Version
		Orientation orientation{X, Y, Z, W}; // Orientation

		std::vector<Point> vertices; // Vertices
		std::vector<Point> normals; // Normals
		std::vector<TexCoord> texCoords; // Texture coordinates
		std::vector<Color> colors; // Colors

		std::vector<Tetrahedron> tetrahedra; // Tetrahedra
		std::vector<Polyline> polylines; // Polylines
		std::vector<Cell> cells; // Cells

		Format tformat{};
		Format pformat{};

		bool isInvalid() { return _invalid; }

		size_t getVectorSize(FDataType type) const
		{
			switch(type)
			{
			case FDataType::None:
			case FDataType::v: return vertices.size();
			case FDataType::vn: return normals.size();
			case FDataType::vt: return texCoords.size();
			case FDataType::co: return colors.size();
			}
		}

		/**
		 * Loads and parses a 4DO file from a given `path`.
		 * Will return an Invalid Object if the given `path` doesn't exist or couldn't be opened.
		 * @param path The path to the 4DO file.
		 * @returns The parsed Object.
		 */
		inline static Object load4DOFromFile(const std::string& path)
		{
			Object result{};
			std::ifstream file = std::ifstream(path);
			if(!file.is_open())
			{
				Logger::logError(std::format("fdo::Object::load4DOFromFile: Failed to open file at path \"{}\".", path));

				result._invalid = true;
				return result;
			}

			std::stringstream buf;
			buf << file.rdbuf();

			result = parse4DO(buf.str());

			return result;
		}
		/**
		 * Parses 4DO file contents.
		 * @param input A string stream of 4DO file contents.
		 * @returns The parsed Object.
		 */
		inline static Object parse4DO(std::stringstream& input)
		{
			Object result{};

			size_t lineInd = 0;
			std::string line;

			size_t cleanLineInd = 0;

			while(std::getline(input, line, '\n'))
			{
				// skip empty lines
				if(line.empty())
				{
					lineInd++;
					continue;
				}

				// remove comments
				std::string cleanLine = line;
				size_t commentInd = line.find('#');
				if(commentInd != std::string::npos)
					cleanLine = line.substr(0, commentInd);

				utils::trim(cleanLine);

				// if the no-comments line is empty, skip the whole line (this may occur if the line starts with a comment)
				if(cleanLine.empty())
				{
					lineInd++;
					continue;
				}

				utils::toLower(cleanLine);

				// 4DO Header
				if(cleanLine.starts_with("4do"))
				{
					if(result.specVer != 1)
					{
						Logger::logWarning(std::format("{}: Unexpected 4DO Header: Specification version is already set to \"{}\".", lineInd + 1, result.specVer));
						goto nextLine;
					}

					if (cleanLineInd > 0)
					{
						Logger::logError(std::format("{}: Unexpected 4DO Header: A 4DO header MUST be on the first \"clean\" line.", lineInd + 1));
						result._invalid = true;
						goto nextLine;
					}

					if(cleanLine.size() < 4)
						goto noVer;

					cleanLine.erase(0, 4); // remove "4do "
					utils::trim(cleanLine);

					if(cleanLine.empty())
					{
						noVer:
						Logger::logWarning(std::format("{}: No specification version provided in the header! Specification version assumed to be \"1\".", lineInd + 1));
						result.specVer = 1;
						goto nextLine;
					}

					if(!utils::isNumber(cleanLine))
					{
						Logger::logError(std::format("{}: Specification version MUST be a number!", lineInd + 1));
						result._invalid = true;
						goto nextLine;
					}

					result.specVer = std::stoi(cleanLine);

					if(!supportedSpecVersions.contains(result.specVer))
					{
						Logger::logWarning(std::format("{}: Unknown/unsupported specification version \"{}\" provided in the header.",
							lineInd + 1, result.specVer));

						goto nextLine;
					}

					goto nextLine;
				}
				// actual keywords
				else
				{
					switch(result.specVer)
					{
					default:
					case 1:
						std::vector<std::string> parts = utils::split(cleanLine, ' ', true);
						std::string keyword = parts[0];
						std::vector<std::string> data = std::vector(parts.begin() + 1, parts.end());

						// model orientation
						if(keyword == "orient")
						{
							if (result.vertices.size() > 0)
							{
								Logger::logWarning(std::format("{}: Orientation MUST be listed before any vertices are listed!",lineInd + 1));
								goto nextLine;
							}
							if (!result.orientation.isDefault())
							{
								Logger::logWarning(std::format("{}: Orientation MUST be listed only once!", lineInd + 1));
								goto nextLine;
							}
							if (data.size() != 4)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `orient`:\n\tKeyword `orient` MUST have 4 axes!", lineInd + 1));
								goto nextLine;
							}

							std::set<Axis> axes{};
							#define setAxis(i) \
                                result.orientation[i] = StringToAxis(data[i]); \
                                if(result.orientation[i] == UNKNOWN) \
                                { \
									result.orientation = Orientation{X,Y,Z,W}; \
                                    Logger::logError(std::format("{}: \"{}\" is not a valid axis!", lineInd + 1, utils::toUpperCopy(data[i]))); \
                                    goto nextLine; \
                                } \
                                if(axes.contains(result.orientation[i])) \
                                { \
                                    result.orientation = Orientation{X,Y,Z,W}; \
                                    Logger::logError(std::format("{}: Each of the axes MUST be included exactly once!", lineInd + 1)); \
                                    goto nextLine; \
                                } \
								axes.insert(result.orientation[i]);

							setAxis(0);
							setAxis(1);
							setAxis(2);
							setAxis(3);

							#undef setAxis

							goto nextLine;
						}
						// vertex position
						else if(keyword == "v")
						{
							if (data.size() != 4)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `v`:\n\tKeyword `v` MUST have 4 values denoting the vertex position coordinates!", lineInd + 1));
								goto nextLine;
							}
							Point vertex{0,0,0,0};
							for(int i = 0; i < 4; i++)
							{
								bool succeded = false;
								float v = utils::toFloat(data[i], succeded);
								if(!succeded || std::isnan(v) || std::isinf(v))
								{
									Logger::logError(std::format("{}: Vertex position values MUST all be valid floats!", lineInd + 1));
									result._invalid = true;
									goto nextLine;
								}
								vertex[i] = v;
							}
							result.vertices.emplace_back(vertex);

							goto nextLine;
						}
						// vertex normal
						else if(keyword == "vn")
						{
							if (data.size() != 4)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `vn`:\n\tKeyword `vn` MUST have 4 values denoting the normal vector!", lineInd + 1));
								goto nextLine;
							}
							Point normal{0,0,0,0};
							for(int i = 0; i < 4; i++)
							{
								bool succeded = false;
								float v = utils::toFloat(data[i], succeded);
								if(!succeded || std::isnan(v) || std::isinf(v))
								{
									Logger::logError(std::format("{}: Vertex normal vector values MUST all be valid floats!", lineInd + 1));
									result._invalid = true;
									goto nextLine;
								}
								normal[i] = v;
							}
							if(Point::length(normal) != 1.f)
								Logger::logWarning(std::format("{}: Vertex normal SHOULD be normalized, but this has a length of {}.\n\tNormalized: \"vn {}\"",
									lineInd + 1,
									Point::length(normal),
									Point::normalize(normal).toString()
									)
								);
							result.normals.emplace_back(normal);

							goto nextLine;
						}
						// vertex texture coordinate
						else if(keyword == "vt")
						{
							if (data.size() != 3)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `vt`:\n\tKeyword `vt` MUST have 3 values denoting the texture coordinates!", lineInd + 1));
								goto nextLine;
							}
							TexCoord texCoord{0,0,0};
							for(int i = 0; i < 3; i++)
							{
								bool succeded = false;
								float v = utils::toFloat(data[i], succeded);
								if(!succeded || std::isnan(v) || std::isinf(v))
								{
									Logger::logError(std::format("{}: Vertex texture coordinate values MUST all be valid floats!", lineInd + 1));
									result._invalid = true;
									goto nextLine;
								}
								if(v > 1.f || v < 0.f)
								{
									Logger::logWarning(std::format("{}: Vertex texture coordinate values SHOULD be in the range [0,1]!", lineInd + 1));
								}
								texCoord[i] = v;
							}
							result.texCoords.emplace_back(texCoord);

							goto nextLine;
						}
						// color data
						else if(keyword == "co")
						{
							if (data.size() != 4 && data.size() != 3)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `co`:\n\tKeyword `co` MUST have 3 or 4 values denoting the color RGB (A=255) or RGBA values!", lineInd + 1));
								goto nextLine;
							}
							Color color{255,255,255, 255};
							for(int i = 0; i < (int)data.size(); i++)
							{
								bool succeded = false;
								long long v = utils::toLL(data[i], succeded);
								if(!succeded)
								{
									Logger::logError(std::format("{}: Color values MUST all be valid integers!", lineInd + 1));
									result._invalid = true;
									goto nextLine;
								}
								if(v > 255 || v < 0)
								{
									Logger::logWarning(std::format("{}: Color values MUST all be in the range [0,255]!\n\tThe values will be clamped.", lineInd + 1));
								}
								color[i] = (uint8_t)std::clamp(v, 0LL, 255LL);
							}
							result.colors.emplace_back(color);

							goto nextLine;
						}
						// tetrahedron format
						else if(keyword == "tformat")
						{
							if (result.tetrahedra.size() > 0)
							{
								Logger::logWarning(std::format("{}: tformat MUST be listed before any tetrahedra are listed!",lineInd + 1));
								goto nextLine;
							}
							if (data.size() != 1 && data.size() != 2)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `tformat`.", lineInd + 1));
								goto nextLine;
							}

							Format newFormat{{}, {}};

							if(data.size() == 2) // tetrahedron-level data
							{
								FDataType type = StringToFDataType(data[0]);
								if(type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, data[0]));
									goto nextLine;
								}
								newFormat.levelData = type;

								data.erase(data.begin());
							}

							std::vector<std::string> dataTypes = utils::split(data[0], '/');
							bool containsV = false;
							for(auto& typeStr : dataTypes)
							{
								FDataType type = StringToFDataType(typeStr);
								if (type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, typeStr));
									goto nextLine;
								}
								if(type == newFormat.levelData)
								{
									Logger::logWarning(std::format("{}: Tetrahedron-level Data Type MUST NOT be repeated.", lineInd + 1, typeStr));
									goto nextLine;
								}
								if(type == FDataType::v)
									containsV = true;
								newFormat.indices.push_back(type);
							}

							if(newFormat.indices.empty() || !containsV)
							{
								Logger::logWarning(std::format("{}: Invalid tformat!", lineInd + 1));
								goto nextLine;
							}

							result.tformat = newFormat;

							goto nextLine;
						}
						// tetrahedron
						else if(keyword == "t")
						{
							const Format& tformat = result.tformat;
							const size_t expectedSize = 4 + (tformat.levelData != FDataType::None ? 1 : 0);
							if (data.size() != expectedSize)
							{
								invalidT:
								Logger::logWarning(std::format("{}: Invalid usage of `t`:\n\tKeyword `t` MUST follow the tformat, which is \"{}\"!",
									lineInd + 1, expectedSize, tformat.toString()));
								goto nextLine;
							}

							Tetrahedron tet{};

							if(tformat.levelData != FDataType::None)
							{
								bool succeded = false;
								long long v = utils::toLL(data[0], succeded);

								if(succeded && v >= 0 && v < result.getVectorSize(tformat.levelData))
									for(int i = 0; i < 4; i++) tet[tformat.levelData][i] = (int32_t)v;
								else
								{
									Logger::logError(std::format("{}: Indices MUST be non-negative real integers and MUST NOT refer to out-of-bounds data positions!", lineInd + 1));
									result._invalid = true;
									goto nextLine;
								}
								data.erase(data.begin());
							}

							for(int i = 0; i < 4; i++)
							{
								std::vector<std::string> dataIndices = utils::split(data[i], '/');
								if(dataIndices.size() != tformat.indices.size())
									goto invalidT;

								int j = 0;
								for(auto& type : tformat.indices)
								{
									bool succeded = false;
									long long v = utils::toLL(dataIndices[j], succeded);

									if(succeded && v >= 0 && v < result.getVectorSize(type))
										tet[type][i] = (int32_t)v;
									else
									{
										Logger::logError(std::format("{}: Indices MUST be non-negative real integers and MUST NOT refer to out-of-bounds data positions!", lineInd + 1));
										result._invalid = true;
										goto nextLine;
									}
									j++;
								}
							}

							result.tetrahedra.emplace_back(tet);

							goto nextLine;
						}
						// cell
						else if(keyword == "c")
						{
							if (data.size() < 1)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `c`:\n\tKeyword `c` MUST be followed by a list of 1 or more tetrahedron indices!",
									lineInd + 1));
								goto nextLine;
							}

							Cell c{};

							for(auto& d : data)
							{
								bool succeded = false;
								long long v = utils::toLL(d, succeded);

								if(succeded && v >= 0 && v < result.tetrahedra.size())
									c.tIndices.push_back((int32_t)v);
								else
								{
									Logger::logError(std::format("{}: Indices MUST be non-negative real integers and MUST NOT refer to out-of-bounds data positions!", lineInd + 1));
									result._invalid = true;
									goto nextLine;
								}
							}

							result.cells.emplace_back(c);

							goto nextLine;
						}
						// polyline format
						else if(keyword == "pformat")
						{
							if (result.polylines.size() > 0)
							{
								Logger::logWarning(std::format("{}: pformat MUST be listed before any polylines are listed!",lineInd + 1));
								goto nextLine;
							}
							if (data.size() != 1 && data.size() != 2)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `pformat`.", lineInd + 1));
								goto nextLine;
							}

							Format newFormat{{}, {}};

							if(data.size() == 2) // polyline-level data
							{
								FDataType type = StringToFDataType(data[0]);
								if(type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, data[0]));
									goto nextLine;
								}
								newFormat.levelData = type;

								data.erase(data.begin());
							}

							std::vector<std::string> dataTypes = utils::split(data[0], '/');
							bool containsV = false;
							for(auto& typeStr : dataTypes)
							{
								FDataType type = StringToFDataType(typeStr);
								if (type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, typeStr));
									goto nextLine;
								}
								if(type == newFormat.levelData)
								{
									Logger::logWarning(std::format("{}: Polyline-level Data Type MUST NOT be repeated.", lineInd + 1, typeStr));
									goto nextLine;
								}
								if(type == FDataType::v)
									containsV = true;
								newFormat.indices.push_back(type);
							}

							if(newFormat.indices.empty() || !containsV)
							{
								Logger::logWarning(std::format("{}: Invalid pformat!", lineInd + 1));
								goto nextLine;
							}

							result.pformat = newFormat;

							goto nextLine;
						}
						// polyline
						else if(keyword == "p")
						{
							const Format& pformat = result.pformat;
							const size_t expectedSize = 2 + (pformat.levelData != FDataType::None ? 1 : 0);
							if (data.size() < expectedSize)
							{
								invalidP:
								Logger::logWarning(std::format("{}: Invalid usage of `p`:\n\tKeyword `p` MUST contain 2 or more indices and follow the pformat, which is \"{}\"!",
									lineInd + 1, pformat.toString()));
								goto nextLine;
							}

							Polyline p{};

							if(pformat.levelData != FDataType::None)
							{
								int32_t levelData = -1;
								bool succeded = false;
								long long v = utils::toLL(data[0], succeded);

								if(succeded && v >= 0 && v < result.getVectorSize(pformat.levelData))
									levelData = (int32_t)v;
								else
								{
									Logger::logError(std::format("{}: Indices MUST be non-negative real integers and MUST NOT refer to out-of-bounds data positions!", lineInd + 1));
									result._invalid = true;
									goto nextLine;
								}
								data.erase(data.begin());

								p[pformat.levelData].reserve(data.size());
								for (int i = 0; i < data.size(); i++)
									p[pformat.levelData].push_back(levelData);
							}

							for(auto& d : data)
							{
								std::vector<std::string> dataIndices = utils::split(d, '/');
								if(dataIndices.size() != pformat.indices.size())
									goto invalidP;

								int j = 0;
								for(auto& type : pformat.indices)
								{
									bool succeded = false;
									long long v = utils::toLL(dataIndices[j], succeded);

									if(succeded && v >= 0 && v < result.getVectorSize(type))
										p[type].push_back((int32_t)v);
									else
									{
										Logger::logError(std::format("{}: Indices MUST be non-negative real integers and MUST NOT refer to out-of-bounds data positions!", lineInd + 1));
										result._invalid = true;
										goto nextLine;
									}
									j++;
								}
							}

							result.polylines.emplace_back(p);

							goto nextLine;
						}
						break;
					}
				}

				nextLine:
				if(result._invalid) break;
				cleanLineInd++;
				lineInd++;
			}

			if(result.vertices.size() == 0)
			{
				Logger::logError("Vertex Positions are REQUIRED, but non were set, invalidating the result.");
				result._invalid = true;
			}

			return result;
		}

		/**
		 * Parses 4DO file contents.
		 * @param input A string of 4DO file contents.
		 * @returns The parsed Object.
		 */
		inline static Object parse4DO(const std::string& input)
		{
			std::stringstream buf;
			buf << input;

			return parse4DO(buf);
		}
	};
}
