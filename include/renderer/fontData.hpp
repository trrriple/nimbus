#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "msdf-atlas-gen/msdf-atlas-gen.h"
#pragma GCC diagnostic pop

#include <vector>

namespace nimbus
{

struct FontData
{
    // Storage for glyph geometry and their coordinates in the atlas
    std::vector<msdf_atlas::GlyphGeometry> glyphs;

    // FontGeometry is a helper class that loads a set of glyphs from a
    // single font. It can also be used to get additional font metrics,
    // kerning information, etc.
    msdf_atlas::FontGeometry fontGeometry;

    // the pixel range used to generate atlas
    float pixelRange;
};
}  // namespace nimbus