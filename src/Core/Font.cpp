#include <Aka/Core/Font.h>

#include <freetype/freetype.h>
#include <stdexcept>

#include <Aka/OS/Logger.h>
#include <Aka/OS/ImagePacker.h>
#include <Aka/Core/Debug.h>

#define FREETYPE_CHECK_RESULT(result)\
{                                    \
    FT_Error res = (result);         \
    if (res) {                       \
        char buffer[256];            \
        snprintf(                    \
            buffer,                  \
            256,                     \
            "%s (%s at %s:%d)",      \
            "[freetype]",            \
            AKA_STRINGIFY(result),   \
            __FILE__,                \
            __LINE__				 \
        );							 \
        ::aka::Logger::error(buffer);\
        throw std::runtime_error(buffer);\
	}								 \
}

namespace aka {

Font::Font(const Path& path, uint32_t height)
{
    FT_Library ft;
    FREETYPE_CHECK_RESULT(FT_Init_FreeType(&ft));

    FT_Face face;
    std::vector<uint8_t> font = File::readBinary(path);
    FREETYPE_CHECK_RESULT(FT_New_Memory_Face(ft, font.data(), (FT_Long)font.size(), 0, &face));
    FREETYPE_CHECK_RESULT(FT_Set_Pixel_Sizes(face, 0, height));

    m_familyName = face->family_name;
    m_styleName = face->style_name;
    m_height = (face->size->metrics.height >> 6) + 1;
    m_advance = face->size->metrics.max_advance >> 6;
    m_characters.resize(255);
    Packer packer((uint32_t)m_characters.size(), m_advance, m_height);
    for (uint32_t c = 0; c < (uint32_t)m_characters.size(); c++)
    {
        // FT_Get_Char_Index (if zero returned, missing glyph)
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            Logger::error("[freetype] Failed to load glyph '", (char)c, "'");
            continue;
        }
        packer.add(c, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
        // now store character for later use
        m_characters[c] = {
            vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<uint32_t>(face->glyph->advance.x >> 6), // bitshift by 6 to get value in pixels (2^6 = 64)
            SubTexture()
        };
    }
    // Generate the atlas and store it.
    Image atlas = packer.pack();
    Texture::Ptr textureAtlas = Texture::create2D(
        atlas.width, atlas.height, 
        TextureFormat::UnsignedByte,
        TextureComponent::RGBA, 
        TextureFlag::None, 
        Sampler::nearest(), // TODO do not force nearest.
        atlas.bytes.data()
    );
    //atlas.save("atlas.png");
    for (unsigned char c = 0; c < (unsigned char)m_characters.size(); c++)
    {
        m_characters[c].texture.texture = textureAtlas;
        m_characters[c].texture.region = packer.getRegion(c);
        m_characters[c].texture.update();
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // TODO use stb_truetype.h instead ?

    /*stbtt_fontinfo font;
    std::vector<uint8_t> data = loadFromBinaryFile(path);
    stbtt_InitFont(&font, data.data(), 0);

    int ascent;
    int descent;
    int line_gap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &line_gap);*/
}

vec2i Font::size(const String& text) const
{
    vec2i size(0);
	const char* start = text.begin();
	const char* end = text.end();
    while (start < end)
    {
        uint32_t c = encoding::next(start, end);
        const Character& ch = m_characters[c];
        size.x += ch.advance;
        size.y = max(size.y, ch.size.y);
    }
    return size;
}

const Character& Font::getCharacter(uint32_t c) const
{
	AKA_ASSERT(c < m_characters.size(), "Glyph out of range");
    return m_characters[c];
}

size_t Font::count() const
{
    return m_characters.size();
}

const String& Font::family() const
{
    return m_familyName;
}

const String& Font::style() const
{
    return m_styleName;
}

uint32_t Font::height() const
{
    return m_height;
}

uint32_t Font::advance() const
{
    return m_advance;
}

}