#include "nimbus/nmpch.hpp"
#include "nimbus/core.hpp"

#include "nimbus/renderer/font.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/fontData.hpp"
#include "nimbus/resourceManager.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "msdf-atlas-gen/msdf-atlas-gen.h"
#pragma GCC diagnostic pop

namespace nimbus
{

Font::Font(const std::string& fontPath)
    : m_path(fontPath), m_data(new FontData())
{
    // Initialize instance of FreeType library
    if (msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype())
    {
        // Load font file
        if (msdfgen::FontHandle* font = msdfgen::loadFont(
                ft, fontPath.c_str()))
        {
            m_data->fontGeometry = msdf_atlas::FontGeometry(&m_data->glyphs);
            // FontGeometry is a helper class that loads a set of glyphs from a
            // single font. It can also be used to get additional font metrics,
            // kerning information, etc.
            // Load a set of character glyphs:
            // The second argument can be ignored unless you mix different font
            // sizes in one atlas. In the last argument, you can specify a
            // charset other than ASCII. To load specific glyph indices, use
            // loadGlyphs instead.
            int glyphsLoaded = m_data->fontGeometry.loadCharset(
                font, 1.0, msdf_atlas::Charset::ASCII);

            Log::coreInfo("Loaded %i glyphs out of %i from %s",
                          glyphsLoaded,
                          msdf_atlas::Charset::ASCII.size(),
                          m_path.c_str());

            // Apply MSDF edge coloring. See edge-coloring.h for other coloring
            // strategies.
            const double maxCornerAngle = 3.0;
            for (msdf_atlas::GlyphGeometry& glyph : m_data->glyphs)
                glyph.edgeColoring(
                    &msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
            // TightAtlasPacker class computes the layout of the atlas.
            msdf_atlas::TightAtlasPacker packer;
            // Set atlas parameters:
            // setDimensions or setDimensionsConstraint to find the best value
            // RRR - makes it tilted, is that okay?
            // packer.setDimensionsConstraint(
            //     msdf_atlas::TightAtlasPacker::DimensionsConstraint::SQUARE);
            // setScale for a fixed size or setMinimumScale to use the largest
            // that fits
            // TODO: determine parameters
            // packer.setMinimumScale(24.0); 
            packer.setScale(40.0f);
            // setPixelRange or setUnitRange
            m_data->pixelRange = 2.0;
            packer.setPixelRange(m_data->pixelRange);
            packer.setMiterLimit(1.0);
            // Compute atlas layout - pack glyphs
            packer.pack(m_data->glyphs.data(), m_data->glyphs.size());
            // Get final atlas dimensions
            int width = 0, height = 0;
            packer.getDimensions(width, height);
            // The ImmediateAtlasGenerator class facilitates the generation of
            // the atlas bitmap.
            msdf_atlas::ImmediateAtlasGenerator<
                float,  // pixel type of buffer for individual glyphs depends on
                        // generator function
                3,      // number of atlas color channels
                &msdf_atlas::msdfGenerator,  // function to generate bitmaps for
                                             // individual glyphs
                msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte,
                                               3>  // class that stores the
                                                   // atlas bitmap
                // For example, a custom atlas storage class that stores it in
                // VRAM can be used.
                >
                generator(width, height);
            // GeneratorAttributes can be modified to change the generator's
            // default settings.
            msdf_atlas::GeneratorAttributes attributes;

    		attributes.config.overlapSupport = true;
	    	attributes.scanlinePass = true;
            generator.setAttributes(attributes);
            generator.setThreadCount(16);
            // Generate atlas bitmap
            generator.generate(m_data->glyphs.data(), m_data->glyphs.size());
            // The atlas bitmap can now be retrieved via atlasStorage as a
            // BitmapConstRef. The glyphs array (or fontGeometry) contains
            // positioning data for typesetting text.
            // success = myProject::submitAtlasBitmapAndLayout(
            //     generator.atlasStorage(), glyphs);

            msdfgen::BitmapConstRef<msdfgen::byte, 3> bitmap
                = (msdfgen::BitmapConstRef<msdfgen::byte, 3>)
                      generator.atlasStorage();

            // msdf_atlas::saveImage(bitmap,
            //                       msdf_atlas::ImageFormat::PNG,
            //                       "test.png",
            //                       msdf_atlas::YDirection::TOP_DOWN);

            Texture::Spec texSpec;

            texSpec.format         = Texture::Format::RGB;
            texSpec.formatInternal = Texture::FormatInternal::RGB8;
            texSpec.width          = bitmap.width;
            texSpec.height         = bitmap.height;

            m_atlasTex = Texture::s_create(Texture::Type::DIFFUSE, texSpec);

            // todo configure size
            m_atlasTex->setData((void*)bitmap.pixels,
                               bitmap.width * bitmap.height * 3);

            // Cleanup
            msdfgen::destroyFont(font);
        }
        msdfgen::deinitializeFreetype(ft);
    }
}

Font::~Font()
{
    delete m_data;
}
};  // namespace nimbus