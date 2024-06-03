#pragma once

#ifndef _4DO_LIB_INCLUDED
#define _4DO_LIB_INCLUDED

#include "basicIncludes.h"

namespace fdo
{
	inline std::set<uint8_t> supportedSpecVersions { 1 };
}

#include "Format.h"

#include "Point.h"
#include "Orientation.h"
#include "TexCoord.h"
#include "Color.h"

// geometry types
#include "Polyline.h"
#include "Tetrahedron.h"
#include "Cell.h"

#include "Object.h"

#endif
