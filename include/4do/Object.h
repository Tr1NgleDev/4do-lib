#pragma once

#include "basicIncludes.h"
#include "Point.h"
#include "Orientation.h"
#include "TexCoord.h"

#include "Tetrahedron.h"
#include "Polyline.h"
#include "Cell.h"
#include "Color.h"
#include "Format.h"

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

		Object(uint8_t specVer = 1) : specVer(specVer) {}

		bool isInvalid() const { return _invalid; }

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
			return vertices.size();
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
							for (int i = 0; i < 4; ++i)
							{
								result.orientation[i] = StringToAxis(data[i]);
								if (result.orientation[i] == UNKNOWN)
								{
									result.orientation = Orientation{ X,Y,Z,W };
									Logger::logError(std::format("{}: \"{}\" is not a valid axis!", lineInd + 1, utils::toUpperCopy(data[i])));
									goto nextLine;
								}
								if (axes.contains(result.orientation[i]))
								{
									result.orientation = Orientation{ X,Y,Z,W };
									Logger::logError(std::format("{}: Each of the axes MUST be included exactly once!", lineInd + 1));
									goto nextLine;
								}
								axes.insert(result.orientation[i]);
							}

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
							if (data.size() != 4 && data.size() != 3 && data.size() != 1)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `co`:\n\tKeyword `co` MUST either have 3 or 4 values denoting the color RGB (A=255) or RGBA values \n\t"
															   "OR a single HEX value denoting either 0xRRGGBB or 0xRRGGBBAA!",
									lineInd + 1));
								goto nextLine;
							}
							Color color{ 255, 255, 255, 255 };
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
								if(data.size() != 1) // R G B or R G B A values
								{
									if(v > 255 || v < 0)
										Logger::logWarning(std::format("{}: Color values MUST all be in the range [0,255]!\n\tThe values will be clamped.", lineInd + 1));

									color[i] = (uint8_t)std::clamp(v, 0LL, 255LL);
								}
								else // 0xRRGGBB or 0xRRGGBBAA value
								{
									uint32_t v32 = (uint32_t)v;
									if(v32 <= 0x00FFFFFF) // if 0xRRGGBB then offset and add 0xFF to make it 0xRRGGBBFF
										v32 = (v32 << 8) + 0xFF;

									color.r = (v32 >> 24) & 0xFF;
									color.g = (v32 >> 16) & 0xFF;
									color.b = (v32 >> 8) & 0xFF;
									color.a = v32 & 0xFF;
								}
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
							if (data.size() < 1)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `tformat`.", lineInd + 1));
								goto nextLine;
							}

							Format newFormat{{}, {}};
							bool containsV = false;

							while(data.size() > 1) // tetrahedron-level data
							{
								FDataType type = StringToFDataType(data[0]);
								if(type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, data[0]));
									goto nextLine;
								}
								if(utils::vectorContains(newFormat.levelData, type))
								{
									Logger::logWarning(std::format("{}: Tetrahedron Format Data Types MUST NOT be repeated.", lineInd + 1));
									goto nextLine;
								}
								if(type == FDataType::v)
									containsV = true;
								newFormat.levelData.push_back(type);

								data.erase(data.begin());
							}

							std::vector<std::string> dataTypes = utils::split(data[0], '/');

							for(auto& typeStr : dataTypes)
							{
								FDataType type = StringToFDataType(typeStr);
								if (type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, typeStr));
									goto nextLine;
								}
								if(utils::vectorContains(newFormat.levelData, type) || utils::vectorContains(newFormat.indices, type))
								{
									Logger::logWarning(std::format("{}: Tetrahedron Format Data Types MUST NOT be repeated.", lineInd + 1));
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
							const size_t expectedSize = 4 + tformat.levelData.size();
							if (data.size() != expectedSize)
							{
								invalidT:
								Logger::logWarning(std::format("{}: Invalid usage of `t`:\n\tKeyword `t` MUST follow the tformat, which is \"{}\"!",
									lineInd + 1, expectedSize, tformat.toString()));
								goto nextLine;
							}

							Tetrahedron tet{};

							if(!tformat.levelData.empty())
							{
								for(auto& levelData : tformat.levelData)
								{
									bool succeded = false;
									long long v = utils::toLL(data[0], succeded);

									if(succeded && v >= 0 && v < result.getVectorSize(levelData))
										for(int i = 0; i < 4; i++) tet[levelData][i] = (int32_t)v;
									else
									{
										Logger::logError(std::format("{}: Indices MUST be non-negative real integers and MUST NOT refer to out-of-bounds data positions!", lineInd + 1));
										result._invalid = true;
										goto nextLine;
									}
									data.erase(data.begin());
								}
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
							if (data.size() < 1)
							{
								Logger::logWarning(std::format("{}: Invalid usage of `pformat`.", lineInd + 1));
								goto nextLine;
							}

							Format newFormat{{}, {}};

							bool containsV = false;

							while(data.size() > 1) // polyline-level data
							{
								FDataType type = StringToFDataType(data[0]);
								if(type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, data[0]));
									goto nextLine;
								}
								if(utils::vectorContains(newFormat.levelData, type))
								{
									Logger::logWarning(std::format("{}: Polyline Format Data Types MUST NOT be repeated.", lineInd + 1));
									goto nextLine;
								}
								if(type == FDataType::v)
									containsV = true;
								newFormat.levelData.push_back(type);

								data.erase(data.begin());
							}

							std::vector<std::string> dataTypes = utils::split(data[0], '/');
							for(auto& typeStr : dataTypes)
							{
								FDataType type = StringToFDataType(typeStr);
								if (type == FDataType::None)
								{
									Logger::logWarning(std::format("{}: Unknown data type {}.", lineInd + 1, typeStr));
									goto nextLine;
								}
								if(utils::vectorContains(newFormat.levelData, type) || utils::vectorContains(newFormat.indices, type))
								{
									Logger::logWarning(std::format("{}: Polyline Format Data Types MUST NOT be repeated.", lineInd + 1));
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
							const size_t expectedSize = 2 + pformat.levelData.size();
							if (data.size() < expectedSize)
							{
								invalidP:
								Logger::logWarning(std::format("{}: Invalid usage of `p`:\n\tKeyword `p` MUST contain 2 or more indices and follow the pformat, which is \"{}\"!",
									lineInd + 1, pformat.toString()));
								goto nextLine;
							}
							const size_t dataSize = data.size();
							const size_t verticesCount = dataSize - pformat.levelData.size();

							Polyline p{};

							if(!pformat.levelData.size())
							{
								for(auto& levelDataT : pformat.levelData)
								{
									int32_t levelData = -1;
									bool succeded = false;
									long long v = utils::toLL(data[0], succeded);

									if (succeded && v >= 0 && v < result.getVectorSize(levelDataT))
										levelData = (int32_t)v;
									else
									{
										Logger::logError(std::format(
											"{}: Indices MUST be non-negative real integers and MUST NOT refer to out-of-bounds data positions!",
											lineInd + 1));
										result._invalid = true;
										goto nextLine;
									}
									data.erase(data.begin());

									p[levelDataT].reserve(verticesCount);
									for (int i = 0; i < verticesCount; i++)
										p[levelDataT].push_back(levelData);
								}
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

		// Constructing the Object.

		/**
		 * Changes the orientation of the Object.
		 * Will modify vertices!
		 * @param newOrientation The new orientation.
		 * @returns *this (for chaining)
		 */
		Object& orient(const Orientation& newOrientation)
		{
			for(auto& v : vertices)
				v = Orientation::transform(v, orientation, newOrientation);
			for (auto& n : normals)
				n = Orientation::transform(n, orientation, newOrientation);

			orientation = newOrientation;

			return *this;
		}

		/**
		 * Acts as the keyword `v`.
		 * @param vertex The vertex position.
		 * @returns *this (for chaining)
		 */
		Object& pushVertex(const Point& vertex)
		{
			if(std::isnan(vertex.x) || std::isnan(vertex.y) || std::isnan(vertex.z) || std::isnan(vertex.w) ||
				std::isinf(vertex.x) || std::isinf(vertex.y) || std::isinf(vertex.z) || std::isinf(vertex.w))
			{
				Logger::logError("pushVertex(): Vertex Position MUST NOT contain NaN or INF values.");
				return *this;
			}
			vertices.emplace_back(vertex);
			return *this;
		}
		/**
		 * Acts as the keyword `v`.
		 * @param x,y,z,w The vertex position.
		 * @returns *this (for chaining)
		 */
		Object& pushVertex(float x, float y, float z, float w) { return pushVertex(Point{x,y,z,w}); }

		/**
		 * Acts as the keyword `vn`.
		 * @param normal The vertex normal.
		 * @returns *this (for chaining)
		 */
		Object& pushNormal(const Point& normal)
		{
			if(std::isnan(normal.x) || std::isnan(normal.y) || std::isnan(normal.z) || std::isnan(normal.w) ||
				std::isinf(normal.x) || std::isinf(normal.y) || std::isinf(normal.z) || std::isinf(normal.w))
			{
				Logger::logError("pushNormal(): Vertex Normals MUST NOT contain NaN or INF values.");
				return *this;
			}
			if(Point::length(normal))
			{
				Point normalized = Point::normalize(normal);
				Logger::logWarning(std::format("pushNormal(): Vertex Normals SHOULD be normalized.\n\tNormalized variant: {{{:.4f}, {:.4f}, {:.4f}, {:.4f}}}",
					normalized.x,
					normalized.y,
					normalized.z,
					normalized.w));
			}
			normals.emplace_back(normal);
			return *this;
		}
		/**
		 * Acts as the keyword `vn`.
		 * @param x,y,z,w The vertex normal.
		 * @returns *this (for chaining)
		 */
		Object& pushNormal(float x, float y, float z, float w) { return pushNormal(Point{x,y,z,w}); }

		/**
		 * Acts as the keyword `vt`.
		 * @param texCoord The texture coordinates.
		 * @returns *this (for chaining)
		 */
		Object& pushTexCoord(const TexCoord& texCoord)
		{
			if(std::isnan(texCoord.x) || std::isnan(texCoord.y) || std::isnan(texCoord.z) ||
				std::isinf(texCoord.x) || std::isinf(texCoord.y) || std::isinf(texCoord.z))
			{
				Logger::logError("pushTexCoord(): Vertex Texture Coordinates MUST NOT contain NaN or INF values.");
				return *this;
			}
			if(
				!utils::inRangeII(texCoord.x, 0, 1) ||
				!utils::inRangeII(texCoord.y, 0, 1) ||
				!utils::inRangeII(texCoord.z, 0, 1) ||
				!utils::inRangeII(texCoord.w, 0, 1))
				Logger::logWarning("pushTexCoord(): Vertex Texture Coordinates SHOULD be in range [0,1].");
			texCoords.emplace_back(texCoord);
			return *this;
		}
		/**
		 * Acts as the keyword `vt`.
		 * @param u,v,w The texture coordinates.
		 * @returns *this (for chaining)
		 */
		Object& pushTexCoord(float u, float v, float w) { return pushTexCoord(TexCoord{u,v,w}); }

		/**
		 * Acts as the keyword `co`.
		 * @param color The color.
		 * @returns *this (for chaining)
		 */
		Object& pushColor(const Color& color)
		{
			colors.emplace_back(color);
			return *this;
		}
		/**
		 * Acts as the keyword `co`.
		 * @param r,g,b,a The color.
		 * @returns *this (for chaining)
		 */
		Object& pushColorF(float r, float g, float b, float a = 1.f) { return pushColor(Color{(uint8_t)(r * 255.f),(uint8_t)(g * 255.f),(uint8_t)(b * 255.f),(uint8_t)(a * 255.f)}); }
		/**
		 * Acts as the keyword `co`.
		 * @param r,g,b,a The color.
		 * @returns *this (for chaining)
		 */
		Object& pushColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) { return pushColor(Color{r,g,b,a}); }
		/**
		 * Acts as the keyword `co`.
		 * @param hex 0xRRGGBBAA or 0xRRGGBB The color hex.
		 * @returns *this (for chaining)
		 */
		Object& pushColor(uint32_t hex)
		{
			Color col{};

			if(hex <= 0x00FFFFFF) // if RGB
				hex = (hex << 8) + 0xFF; // make it RGBA with A 255

			col.r = (hex >> 24) & 0xff;
			col.g = (hex >> 16) & 0xff;
			col.b = (hex >> 8) & 0xff;
			col.a = hex & 0xff;

			return pushColor(col);
		}

		/**
		 * Acts as the keyword `t`.
		 * @param tet The tetrahedron.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(const Tetrahedron& tet)
		{
			if(std::any_of(tet.vIndices.cbegin(), tet.vIndices.cend(), [this](const int32_t& ind){ return ind >= (int)vertices.size(); }))
				Logger::logWarning("pushTetrahedron(): One of the vertex indices is pointing outside the vertices data!");
			if(std::any_of(tet.vnIndices.cbegin(), tet.vnIndices.cend(), [this](const int32_t& ind){ return ind >= (int)normals.size(); }))
				Logger::logWarning("pushTetrahedron(): One of the normal indices is pointing outside the normals data!");
			if(std::any_of(tet.vtIndices.cbegin(), tet.vtIndices.cend(), [this](const int32_t& ind){ return ind >= (int)texCoords.size(); }))
				Logger::logWarning("pushTetrahedron(): One of the texture coordinate indices is pointing outside the texture coords data!");
			if(std::any_of(tet.coIndices.cbegin(), tet.coIndices.cend(), [this](const int32_t& ind){ return ind >= (int)colors.size(); }))
				Logger::logWarning("pushTetrahedron(): One of the color indices is pointing outside the colors data!");
			tetrahedra.emplace_back(tet);
			return *this;
		}
		/**
		 * Acts as the keyword `t`.
		 * @param vIndices,vnIndices,vtIndices,coIndices The tetrahedron data arrays.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			const std::array<int32_t, 4>& vIndices,
			const std::array<int32_t, 4>& vnIndices,
			const std::array<int32_t, 4>& vtIndices,
			const std::array<int32_t, 4>& coIndices)
		{
			return pushTetrahedron({vIndices, vnIndices, vtIndices, coIndices});
		}
		/**
		 * Acts as the keyword `t`.
		 * @param vIndices,vnIndices,vtIndices,coIndices The tetrahedron data arrays.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			int32_t (&vIndices )[4],
			int32_t (&vnIndices)[4],
			int32_t (&vtIndices)[4],
			int32_t (&coIndices)[4])
		{
			return pushTetrahedron(std::to_array(vIndices), std::to_array(vnIndices), std::to_array(vtIndices), std::to_array(coIndices));
		}
		/**
		 * @param v0,v1,v2,v3 Vertex Positions of the tetrahedron.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			int32_t v0,
			int32_t v1,
			int32_t v2,
			int32_t v3)
		{
			return pushTetrahedron(std::array<int, 4>{v0, v1, v2, v3}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1});
		}
		/**
		 * @param v0,v1,v2,v3 Vertex Positions of the tetrahedron.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			int32_t v0,
			int32_t v1,
			int32_t v2,
			int32_t v3,
			int32_t vn0,
			int32_t vn1,
			int32_t vn2,
			int32_t vn3)
		{
			return pushTetrahedron(std::array<int, 4>{v0, v1, v2, v3}, std::array<int, 4>{vn0,vn1,vn2,vn3}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1});
		}
		/**
		 * @param v0,v1,v2,v3 Vertex Positions of the tetrahedron.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			int32_t v0,
			int32_t v1,
			int32_t v2,
			int32_t v3,
			int32_t co)
		{
			return pushTetrahedron(std::array<int, 4>{v0, v1, v2, v3}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{co,co,co,co});
		}
		/**
		 * @param v0,v1,v2,v3 Vertex Positions of the tetrahedron.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			const Point& v0,
			const Point& v1,
			const Point& v2,
			const Point& v3)
		{
			pushVertex(v0); int i0 = vertices.size() - 1;
			pushVertex(v1); int i1 = vertices.size() - 1;
			pushVertex(v2); int i2 = vertices.size() - 1;
			pushVertex(v3); int i3 = vertices.size() - 1;

			return pushTetrahedron(std::array<int, 4>{i0, i1, i2, i3}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1});
		}
		/**
		 * @param v0,v1,v2,v3 Vertex Positions of the tetrahedron.
		 * @param co The color assigned to the whole tetrahedron.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			const Point& v0,
			const Point& v1,
			const Point& v2,
			const Point& v3,
			const Color& co)
		{
			pushVertex(v0); int i0 = vertices.size() - 1;
			pushVertex(v1); int i1 = vertices.size() - 1;
			pushVertex(v2); int i2 = vertices.size() - 1;
			pushVertex(v3); int i3 = vertices.size() - 1;
			pushColor(co); int co0 = colors.size() - 1;

			return pushTetrahedron(std::array<int, 4>{i0, i1, i2, i3}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{co0,co0,co0,co0});
		}
		/**
		 * @param v0,v1,v2,v3 Vertex Positions of the tetrahedron.
		 * @param vn0,vn1,vn2,vn3 Vertex Normals of the tetrahedron.
		 * @returns *this (for chaining)
		 */
		Object& pushTetrahedron(
			const Point& v0,
			const Point& v1,
			const Point& v2,
			const Point& v3,
			const Point& vn0,
			const Point& vn1,
			const Point& vn2,
			const Point& vn3)
		{
			pushVertex(v0); int i0 = vertices.size() - 1;
			pushVertex(v1); int i1 = vertices.size() - 1;
			pushVertex(v2); int i2 = vertices.size() - 1;
			pushVertex(v3); int i3 = vertices.size() - 1;
			pushNormal(vn0); int in0 = normals.size() - 1;
			pushNormal(vn1); int in1 = normals.size() - 1;
			pushNormal(vn2); int in2 = normals.size() - 1;
			pushNormal(vn3); int in3 = normals.size() - 1;

			return pushTetrahedron(std::array<int, 4>{i0, i1, i2, i3}, std::array<int, 4>{in0, in1, in2, in3}, std::array<int, 4>{-1,-1,-1,-1}, std::array<int, 4>{-1,-1,-1,-1});
		}
		// ok im tired of doing pushTetrahedron variations

		/**
		 * Acts as the keyword `p`.
		 * @param tet The polyline.
		 * @returns *this (for chaining)
		 */
		Object& pushPolyline(const Polyline& line)
		{
			if(line.length() < 2)
			{
				Logger::logError("pushPolyline(): Line MUST be atleast of length 2.");
				return *this;
			}
			if(std::any_of(line.vIndices.cbegin(), line.vIndices.cend(), [this](const int32_t& ind){ return ind >= (int)vertices.size(); }))
				Logger::logWarning("pushPolyline(): One of the vertex indices is pointing outside the vertices data!");
			if(std::any_of(line.vnIndices.cbegin(), line.vnIndices.cend(), [this](const int32_t& ind){ return ind >= (int)normals.size(); }))
				Logger::logWarning("pushPolyline(): One of the normal indices is pointing outside the normals data!");
			if(std::any_of(line.vtIndices.cbegin(), line.vtIndices.cend(), [this](const int32_t& ind){ return ind >= (int)texCoords.size(); }))
				Logger::logWarning("pushPolyline(): One of the texture coordinate indices is pointing outside the texture coords data!");
			if(std::any_of(line.coIndices.cbegin(), line.coIndices.cend(), [this](const int32_t& ind){ return ind >= (int)colors.size(); }))
				Logger::logWarning("pushPolyline(): One of the color indices is pointing outside the colors data!");
			polylines.emplace_back(line);
			return *this;
		}
		/**
		 * Acts as the keyword `p`.
		 * @param vIndices,vnIndices,vtIndices,coIndices The polyline data arrays.
		 * @returns *this (for chaining)
		 */
		Object& pushPolyline(
			const std::vector<int32_t>& vIndices,
			const std::vector<int32_t>& vnIndices,
			const std::vector<int32_t>& vtIndices,
			const std::vector<int32_t>& coIndices)
		{
			Polyline line{vIndices, vnIndices, vtIndices, coIndices};
			if(vnIndices.size() < vIndices.size())
			{
				line.vnIndices.reserve(vIndices.size());
				for(int i = 0; i < vIndices.size() - vnIndices.size(); i++)
					line.vnIndices.push_back(-1);
			}
			if(vtIndices.size() < vIndices.size())
			{
				line.vtIndices.reserve(vIndices.size());
				for(int i = 0; i < vIndices.size() - vtIndices.size(); i++)
					line.vtIndices.push_back(-1);
			}
			if(coIndices.size() < vIndices.size())
			{
				line.coIndices.reserve(vIndices.size());
				for(int i = 0; i < vIndices.size() - coIndices.size(); i++)
					line.coIndices.push_back(-1);
			}
			return pushPolyline(line);
		}

		/**
		 * Acts as the keyword `c`.
		 * @param cell The cell.
		 * @returns *this (for chaining)
		 */
		Object& pushCell(const Cell& cell)
		{
			if(std::any_of(cell.tIndices.cbegin(), cell.tIndices.cend(), [this](const int32_t& ind){ return ind >= tetrahedra.size(); }))
				Logger::logWarning("pushCell(): One of the tetrahedron indices is pointing outside the tetrahedra data!");

			if(cell.tIndices.empty())
			{
				Logger::logError("pushCell(): The indices list must not be empty!");
				return *this;
			}

			cells.push_back(cell);
			return *this;
		}
		/**
		 * @param tetra The tetrahedra list.
		 * @returns *this (for chaining)
		 */
		Object& pushCell(const std::vector<Tetrahedron>& tetra)
		{
			Cell cell{};
			for(auto& tet : tetra)
			{
				pushTetrahedron(tet);
				cell.tIndices.push_back(tetrahedra.size() - 1);
			}
			return pushCell(cell);
		}
		/**
		 * @param tIndices The indices list.
		 * @returns *this (for chaining)
		 */
		Object& pushCell(const std::vector<int32_t>& tIndices)
		{
			Cell cell{tIndices};
			return pushCell(cell);
		}

		/**
		 * Combines this Object with another Object.
		 * @param other The Object to combine with.
		 * @return *this (for chaining)
		 */
		Object& combineWith(const Object& other)
		{
			size_t startV = vertices.size();
			size_t startVN = normals.size();
			size_t startVT = texCoords.size();
			size_t startCO = colors.size();

			vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
			normals.insert(normals.end(), other.normals.begin(), other.normals.end());
			texCoords.insert(texCoords.end(), other.texCoords.begin(), other.texCoords.end());
			colors.insert(colors.end(), other.colors.begin(), other.colors.end());

			size_t startT = tetrahedra.size();

			for(auto& tet : other.tetrahedra)
			{
				Tetrahedron newTet{tet};

				for(auto& ind : newTet.vIndices)
					ind += startV;
				for(auto& ind : newTet.vnIndices)
					ind += startVN;
				for(auto& ind : newTet.vtIndices)
					ind += startVT;
				for(auto& ind : newTet.coIndices)
					ind += startCO;

				tetrahedra.emplace_back(newTet);
			}
			for(auto& pl : other.polylines)
			{
				Polyline newPl{pl};

				for(auto& ind : newPl.vIndices)
					ind += startV;
				for(auto& ind : newPl.vnIndices)
					ind += startVN;
				for(auto& ind : newPl.vtIndices)
					ind += startVT;
				for(auto& ind : newPl.coIndices)
					ind += startCO;

				polylines.emplace_back(newPl);
			}
			for(auto& c : other.cells)
			{
				Cell newC{c};

				for(auto& ind : newC.tIndices)
					ind += startT;

				cells.emplace_back(newC);
			}

			return *this;
		}

		/**
		 * @return The median point of all vertices.
		 */
		Point getCenter()
		{
			if(vertices.empty()) return {0,0,0,0};

			Point center{};
			int i = 0;

			for(auto& v : vertices)
			{
				center += v;
				i++;
			}

			center /= i;

			return center;
		}

		/**
		 * Translates all vertices by an amount
		 * @param v The translation amount.
		 * @return *this (for chaining)
		 */
		Object& translate(const Point& v)
		{
			for(auto& vert : vertices)
				vert += v;

			return *this;
		}
		/**
		 * Scales all vertices by an amount from an origin.
		 * @param v The scale amount.
		 * @param origin The scale origin.
		 * @return *this (for chaining)
		 */
		Object& scale(const Point& v, const Point& origin)
		{
			for(auto& vert : vertices)
				vert = origin + (vert - origin) * v;

			return *this;
		}
		/**
		 * Scales all vertices by an amount from the center.
		 * @param v The scale amount.
		 * @return *this (for chaining)
		 */
		Object& scale(const Point& v) { return scale(v, getCenter()); }
		/**
		 * Applies whatever transformation you want.
		 * @param verticesT The transformation function.
		 * @return *this (for chaining)
		 */
		Object& transformVertices(
			const std::function<Point(const Point& vertex)>& verticesT)
		{
			for(auto& vert : vertices)
				vert = verticesT(vert);

			return *this;
		}
		/**
		 * Applies whatever transformation you want.
		 * @param normalsT The transformation function.
		 * @return *this (for chaining)
		 */
		Object& transformNormals(
			const std::function<Point(const Point& normal)>& normalsT)
		{
			for(auto& n : normals)
				n = normalsT(n);

			return *this;
		}
		/**
		 * Applies whatever transformation you want.
		 * @param texCoordsT The transformation function.
		 * @return *this (for chaining)
		 */
		Object& transformTexCoords(
			const std::function<TexCoord(const TexCoord& texCoord)>& texCoordsT)
		{
			for(auto& texCoord : texCoords)
				texCoord = texCoordsT(texCoord);

			return *this;
		}
		/**
		 * Applies whatever transformation you want.
		 * @param colorsT The transformation function.
		 * @return *this (for chaining)
		 */
		Object& transformColors(
			const std::function<Color(const Color& color)>& colorsT)
		{
			for(auto& color : colors)
				color = colorsT(color);

			return *this;
		}
		/**
		 * Applies whatever transformation you want.
		 * @param verticesT,normalsT,texCoordsT,colorsT The transformation functions.
		 * @return *this (for chaining)
		 */
		Object& transform(
			const std::function<Point(const Point& vertex)>& verticesT,
			const std::function<Point(const Point& normal)>& normalsT,
			const std::function<TexCoord(const TexCoord& texCoord)>& texCoordsT,
			const std::function<Color(const Color& color)>& colorsT)
		{
			transformVertices(verticesT);
			transformNormals(normalsT);
			transformTexCoords(texCoordsT);
			transformColors(colorsT);

			return *this;
		}

		std::string save4DO(const std::vector<std::string>& comments)
		{
			std::string result = "# Generated with 4DO-Lib\n";

			for(auto& comment : comments)
				result += std::format("# {}\n", comment);

			result += "\n# Header\n";
			result += std::format("4DO {}\n", specVer);
			result += std::format("orient {}\n", orientation.toString());
			if(!tetrahedra.empty())
			{
				tformat = tetrahedra[0].guessFormat();
				result += std::format("tformat {}\n", tformat.toString());
			}
			if(!polylines.empty())
			{
				pformat = polylines[0].guessFormat();
				result += std::format("pformat {}\n", pformat.toString());
			}

			if(!colors.empty())
			{
				result += "\n# Colors\n";

				for (auto& color : colors)
					result += std::format("co {}\n", color.toString());
			}

			if(!vertices.empty())
			{
				result += "\n# Vertices\n";

				for (auto& vert : vertices)
					result += std::format("v {}\n", vert.toString());
			}

			if(!normals.empty())
			{
				result += "\n# Normals\n";

				for (auto& n : normals)
					result += std::format("vn {}\n", n.toString());
			}

			if(!texCoords.empty())
			{
				result += "\n# Texture Coordinates\n";

				for (auto& tc : texCoords)
					result += std::format("vt {}\n", tc.toString());
			}

			if(!tetrahedra.empty())
			{
				result += "\n# Tetrahedra\n";

				for (auto& t : tetrahedra)
					result += std::format("t {}\n", t.toString(tformat));
			}

			if(!cells.empty())
			{
				result += "\n# Cells\n";

				for (auto& c : cells)
					result += std::format("c {}\n", c.toString());
			}

			if(!polylines.empty())
			{
				result += "\n# Polylines\n";

				for (auto& p : polylines)
					result += std::format("p {}\n", p.toString(pformat));
			}

			return result;
		}

		std::string save4DO() { return save4DO({}); }

		/**
		 * Saves the Object in 4DO file format to the given `path`.
		 * @param path The output filepath.
		 * @param comments Custom comments to be added to the top of the file.
		 * @returns `false` if failed, otherwise `true`.
		 */
		bool save4DOToFile(const std::string& path, const std::vector<std::string>& comments)
		{
			std::ofstream file = std::ofstream(path);
			if(!file.is_open())
			{
				Logger::logError(std::format("fdo::Object::save4DOToFile: Failed to open file at path \"{}\".", path));
				return false;
			}

			std::string data = save4DO(comments);

			file << data;

			return true;
		}

		/**
		 * Saves the Object in 4DO file format to the given `path`.
		 * @param path The output filepath.
		 * @returns `false` if failed, otherwise `true`.
		 */
		bool save4DOToFile(const std::string& path) { return save4DOToFile(path, {}); }

		/**
		 * Converts the Object data into GPU-compatible single index buffer format.
		 * @param indexBuffer The output indices.
		 * @param pos The output vertex positions. Can be left NULL if you don't need it.
		 * @param norm The output vertex normals. Can be left NULL if you don't need it.
		 * @param uvw The output vertex texture coordinates. Can be left NULL if you don't need it.
		 * @param col The output vertex colors. Can be left NULL if you don't need it.
		 * @param optimizeData When `true`, will try to optimize the output data.
		 */
		void tetrahedralize(
			std::vector<uint32_t>& indexBuffer,
			std::vector<fdo::Point>* pos = nullptr,
			std::vector<fdo::Point>* norm = nullptr,
			std::vector<fdo::TexCoord>* uvw = nullptr,
			std::vector<fdo::Color>* col = nullptr,
			bool optimizeData = true
		)
		{
			if (!pos && !norm && !uvw && !col)
				return;

			if (pos)
			{
				pos->clear();
				pos->reserve(tetrahedra.size() * 4);
			}
			if (norm)
			{
				norm->clear();
				norm->reserve(tetrahedra.size() * 4);
			}
			if (uvw)
			{
				uvw->clear();
				uvw->reserve(tetrahedra.size() * 4);
			}
			if (col)
			{
				col->clear();
				col->reserve(tetrahedra.size() * 4);
			}

			indexBuffer.clear();
			indexBuffer.reserve(tetrahedra.size() * 4);

			for (int tetraIndex = 0; tetraIndex < tetrahedra.size(); ++tetraIndex)
				for (int vertIndex = 0; vertIndex < 4; ++vertIndex)
				{
					if (pos)
					{
						auto v = tetrahedra[tetraIndex].vIndices[vertIndex];
						if (v >= 0 && v < vertices.size())
							pos->emplace_back(vertices[tetrahedra[tetraIndex].vIndices[vertIndex]]);
					}
					if (norm)
					{
						auto vn = tetrahedra[tetraIndex].vnIndices[vertIndex];
						if (vn >= 0 && vn < normals.size())
							norm->emplace_back(normals[tetrahedra[tetraIndex].vnIndices[vertIndex]]);
					}
					if (uvw)
					{
						auto vt = tetrahedra[tetraIndex].vtIndices[vertIndex];
						if (vt >= 0 && vt < texCoords.size())
							uvw->emplace_back(texCoords[tetrahedra[tetraIndex].vtIndices[vertIndex]]);
					}
					if (col)
					{
						auto co = tetrahedra[tetraIndex].coIndices[vertIndex];
						if (co >= 0 && co < colors.size())
							col->emplace_back(colors[tetrahedra[tetraIndex].coIndices[vertIndex]]);
					}
					indexBuffer.emplace_back(tetraIndex * 4 + vertIndex);
				}

			// optimize the data
			if (optimizeData)
			{
				struct HashVertData
				{
					fdo::Point pos{ 0,0,0,0 };
					fdo::Point norm{ 0,0,0,0 };
					fdo::TexCoord uvw{ 0,0,0 };
					fdo::Color col{ 0,0,0,0 };
					constexpr bool operator==(const HashVertData& other) const
					{
						return pos == other.pos && norm == other.norm && uvw == other.uvw && col == other.col;
					}
				};
				struct hash
				{
					size_t operator()(const HashVertData& va) const
					{
						auto hash_combine = [](size_t& h, auto val)
							{
								h ^= std::hash<std::decay_t<decltype(val)>>()(val) + 0x9e3779b9 + (h << 6) + (h >> 2);
							};

						size_t h = 0;
						hash_combine(h, va.pos.x);
						hash_combine(h, va.pos.y);
						hash_combine(h, va.pos.z);
						hash_combine(h, va.norm.x);
						hash_combine(h, va.norm.y);
						hash_combine(h, va.norm.z);
						hash_combine(h, va.uvw.u);
						hash_combine(h, va.uvw.v);
						hash_combine(h, va.col.r);
						hash_combine(h, va.col.g);
						hash_combine(h, va.col.b);
						hash_combine(h, va.col.a);
						return h;
					}
				};

				std::unordered_map<HashVertData, uint32_t, hash> vertexMap;
				std::vector<uint32_t> newIndexBuffer;
				std::vector<fdo::Point> newPos;
				std::vector<fdo::Point> newNorm;
				std::vector<fdo::TexCoord> newUVW;
				std::vector<fdo::Color> newCol;
				newIndexBuffer.reserve(indexBuffer.size());

				for (uint32_t ind : indexBuffer)
				{
					HashVertData v
					{
						(pos ? pos->at(ind) : fdo::Point{0,0,0,0}),
						(norm ? norm->at(ind) : fdo::Point{0,0,0,0}),
						(uvw ? uvw->at(ind) : fdo::TexCoord{0,0,0}),
						(col ? col->at(ind) : fdo::Color{0,0,0,0}),
					};

					auto it = vertexMap.find(v);
					if (it != vertexMap.end())
					{
						newIndexBuffer.emplace_back(it->second);
					}
					else
					{
						uint32_t newInd = newPos.size();
						if (pos)
							newPos.emplace_back(v.pos);
						if (norm)
							newNorm.emplace_back(v.norm);
						if (uvw)
							newUVW.emplace_back(v.uvw);
						if (col)
							newCol.emplace_back(v.col);
						vertexMap[v] = newInd;
						newIndexBuffer.emplace_back(newInd);
					}
				}

				indexBuffer = std::move(newIndexBuffer);
				if (pos)
					*pos = std::move(newPos);
				if (norm)
					*norm = std::move(newNorm);
				if (uvw)
					*uvw = std::move(newUVW);
				if (col)
					*col = std::move(newCol);
			}
		}
	};
}
