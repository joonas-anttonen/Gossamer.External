#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <cstdint>

enum class Status : int32_t
{
    Failure = -1,
    OK = 0,
    InvalidArgument = 1,
    InvalidData = 2,
};

static Status StatusFromFT_Error(FT_Error error)
{
    switch (error)
    {
    case FT_Err_Ok:
        return Status::OK;
    case FT_Err_Invalid_Argument:
        return Status::InvalidArgument;
    default:
        return Status::Failure;
    }
}

struct FaceData
{
    void *face_ptr = nullptr;
    int32_t ascender = 0;
    int32_t descender = 0;
    int32_t height = 0;
    int32_t glyph_count = 0;
};

struct GlyphData
{
    void *glyph_ptr = nullptr;
    void *bitmap_ptr = nullptr;
    int32_t stride = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t bearing_x = 0;
    int32_t bearing_y = 0;
};

extern "C"
{
    __declspec(dllexport) int32_t ftCreate(void **out_library)
    {
        if (!out_library)
            return static_cast<int32_t>(Status::InvalidArgument);

        auto error = FT_Init_FreeType(
            reinterpret_cast<FT_Library *>(out_library));

        return static_cast<int32_t>(StatusFromFT_Error(error));
    }

    __declspec(dllexport) int32_t ftRelease(void *in_library)
    {
        if (!in_library)
            return static_cast<int32_t>(Status::InvalidArgument);

        auto error = FT_Done_FreeType(
            reinterpret_cast<FT_Library>(in_library));

        return static_cast<int32_t>(StatusFromFT_Error(error));
    }

    __declspec(dllexport) int32_t ftCreateFace(void *in_library, const uint8_t *in_data, size_t in_data_size, int32_t in_width, int32_t in_height, FaceData *out_face)
    {
        if (!in_library || !in_data || in_data_size == 0 || !out_face)
            return static_cast<int32_t>(Status::InvalidArgument);

        auto error = FT_New_Memory_Face(
            reinterpret_cast<FT_Library>(in_library),
            in_data,
            static_cast<FT_Long>(in_data_size),
            0,
            reinterpret_cast<FT_Face *>(&(out_face->face_ptr)));

        if (error != 0)
        {
            return static_cast<int32_t>(StatusFromFT_Error(error));
        }

        error = FT_Set_Char_Size(
            reinterpret_cast<FT_Face>(out_face->face_ptr),
            static_cast<FT_F26Dot6>(in_width * 64),
            static_cast<FT_F26Dot6>(in_height * 64),
            72, 72);

        if (error != 0)
        {
            return static_cast<int32_t>(StatusFromFT_Error(error));
        }

        FT_Face face = reinterpret_cast<FT_Face>(out_face->face_ptr);
        out_face->ascender = static_cast<int32_t>(face->size->metrics.ascender / 64);
        out_face->descender = static_cast<int32_t>(face->size->metrics.descender / 64);
        out_face->height = static_cast<int32_t>(face->size->metrics.height / 64);
        out_face->glyph_count = static_cast<int32_t>(face->num_glyphs);

        return static_cast<int32_t>(StatusFromFT_Error(error));
    }

    __declspec(dllexport) int32_t ftReleaseFace(void *in_face)
    {
        if (!in_face)
            return static_cast<int32_t>(Status::InvalidArgument);

        auto error = FT_Done_Face(
            reinterpret_cast<FT_Face>(in_face));

        return static_cast<int32_t>(StatusFromFT_Error(error));
    }

    __declspec(dllexport) int32_t ftGetCharIndex(void *in_face, int32_t in_charcode)
    {
        if (!in_face)
            return static_cast<int32_t>(Status::InvalidArgument);

        auto error = FT_Get_Char_Index(
            reinterpret_cast<FT_Face>(in_face),
            static_cast<FT_ULong>(in_charcode));

        return static_cast<int32_t>(StatusFromFT_Error(error));
    }

    __declspec(dllexport) int32_t ftCreateGlyph(void *in_face, int32_t in_glyph_index, GlyphData *out_glyph_data)
    {
        if (!in_face || !out_glyph_data)
            return static_cast<int32_t>(Status::InvalidArgument);

        auto error = FT_Load_Glyph(
            reinterpret_cast<FT_Face>(in_face),
            static_cast<FT_UInt>(in_glyph_index),
            FT_LOAD_RENDER);

        if (error != 0)
        {
            return static_cast<int32_t>(StatusFromFT_Error(error));
        }

        FT_GlyphSlot slot = reinterpret_cast<FT_Face>(in_face)->glyph;

        error = FT_Render_Glyph(
            slot,
            FT_RENDER_MODE_SDF);

        if (error != 0)
        {
            return static_cast<int32_t>(StatusFromFT_Error(error));
        }

        error = FT_Get_Glyph(
            slot,
            reinterpret_cast<FT_Glyph *>(&(out_glyph_data->glyph_ptr)));

        if (error != 0)
        {
            return static_cast<int32_t>(StatusFromFT_Error(error));
        }

        out_glyph_data->bitmap_ptr = reinterpret_cast<void *>(slot->bitmap.buffer);
        out_glyph_data->stride = slot->bitmap.pitch;
        out_glyph_data->width = slot->bitmap.width;
        out_glyph_data->height = slot->bitmap.rows;
        out_glyph_data->bearing_x = slot->metrics.horiBearingX / 64;
        out_glyph_data->bearing_y = slot->metrics.horiBearingY / 64;

        return static_cast<int32_t>(StatusFromFT_Error(error));
    }

    __declspec(dllexport) int32_t ftReleaseGlyph(void *in_glyph)
    {
        if (!in_glyph)
            return static_cast<int32_t>(Status::InvalidArgument);

        FT_Done_Glyph(
            reinterpret_cast<FT_Glyph>(in_glyph));

        return static_cast<int32_t>(Status::OK);
    }
}