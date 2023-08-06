#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/font.hpp"
#include "nimbus/renderer/texture.hpp"
#include "nimbus/renderer/fontData.hpp"
#include "nimbus/core/resourceManager.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "msdf-atlas-gen/msdf-atlas-gen.h"
#pragma GCC diagnostic pop

#include <thread>
#include <atomic>

namespace nimbus
{

Font::Font(const std::string& fontPath)
    : m_path(fontPath), m_data(new FontData())
{
    m_workerThread = std::thread(&Font::_loadFont, this);
    m_workerThread.detach();  // Detach the thread so it runs independently
}

ref<Font> Font::s_create(const std::string& fontPath)
{
    // This trick allows ref (make_shared) to get around the fact that
    // it itself can't access the private constructor of Font.
    // So we directly invoke the constructor itself here, where we can
    // instead of in the shared_ptr implementation (Where it can't).
    // Subsequently, we need to provide a custom deleter.
    // Note this could also be done by making a wrapper class such as
    // those used for the texture/shaders to support multiple platforms
    // because the platform constructors are public just not exposed to 
    // the nimbus API. In this case, font is not platform specific, so
    // it has no platform wrapper, so we play this trick to ensure
    // the only way to construct a font is through the resource manager
    return ref<Font>(new Font(fontPath), [](Font* p) { delete p; });
}

Font::~Font()
{
    if(m_data->pixels != nullptr)
    {
        free(m_data->pixels);
    }

    delete m_data;
}

void Font::_loadFont()
{
    // Initialize instance of FreeType library
    if (msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype())
    {
        // Load font file
        if (msdfgen::FontHandle* font = msdfgen::loadFont(
                ft, m_path.c_str()))
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
            attributes.scanlinePass          = true;
            generator.setAttributes(attributes);
            generator.setThreadCount(16);
            // Generate atlas bitmap
            generator.generate(m_data->glyphs.data(), m_data->glyphs.size());
            // The atlas bitmap can now be retrieved via atlasStorage as a
            // BitmapConstRef. The glyphs array (or fontGeometry) contains
            // positioning data for typesetting text.

            msdfgen::BitmapConstRef<msdfgen::byte, 3> bitmap
                = (msdfgen::BitmapConstRef<msdfgen::byte, 3>)
                      generator.atlasStorage();

            m_data->width  = bitmap.width;
            m_data->height = bitmap.height;
            m_data->pixels = malloc(m_data->width * m_data->height * 3);

            memcpy(m_data->pixels,
                   bitmap.pixels,
                   m_data->width * m_data->height * 3);

            msdfgen::destroyFont(font);
        }
        msdfgen::deinitializeFreetype(ft);
    }

    m_isDone.store(true);  // Set the flag when done
}

void Font::_initializeTexture()
{
    Texture::Spec texSpec;

    texSpec.format         = Texture::Format::RGB;
    texSpec.formatInternal = Texture::FormatInternal::RGB8;
    texSpec.width          = m_data->width;
    texSpec.height         = m_data->height;

    m_atlasTex = Texture::s_create(Texture::Type::DIFFUSE, texSpec);

    // todo configure size
    m_atlasTex->setData((void*)m_data->pixels,
                        m_data->width * m_data->height * 3);

    // Cleanup
    free(m_data->pixels);
    m_data->pixels = nullptr;

    m_loaded = true;
}

};  // namespace nimbus