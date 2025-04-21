#include <webp/decode.h>
#include <webp/encode.h>

enum class Status
{
    OK = 0,
    InvalidArgument = 1,
    InvalidData = 2,
};

enum class Format
{
    RGBA = 0,
};

extern "C"
{
    __declspec(dllexport) int32_t webpAnalyze(const uint8_t *in_data, size_t in_data_size, int32_t *width, int32_t *height, int32_t *has_alpha)
    {
        if (in_data == nullptr || in_data_size == 0 || width == nullptr || height == nullptr || has_alpha == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        WebPBitstreamFeatures features;
        if (WebPGetFeatures(in_data, in_data_size, &features) == VP8_STATUS_OK)
        {
            *width = features.width;
            *height = features.height;
            *has_alpha = features.has_alpha;
            return static_cast<int32_t>(Status::OK);
        }
        else
        {
            *width = 0;
            *height = 0;
            *has_alpha = 0;
            return static_cast<int32_t>(Status::InvalidData);
        }
    }

    __declspec(dllexport) int32_t webpDecode(const uint8_t *in_data, size_t in_data_size, Format out_format, uint8_t **out_data, size_t *out_data_size)
    {
        if (in_data == nullptr || in_data_size == 0 || out_data == nullptr || out_data_size == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        int32_t width = 0, height = 0;
        if (out_format == Format::RGBA)
        {
            *out_data = WebPDecodeRGBA(in_data, in_data_size, &width, &height);
        }
        else
        {
            out_data = nullptr;
            *out_data_size = 0;
            return static_cast<int32_t>(Status::InvalidArgument);
        }

        if (out_data != nullptr)
        {
            *out_data_size = width * height * 4;
            return static_cast<int32_t>(Status::OK);
        }
        else
        {
            *out_data_size = 0;
            return static_cast<int32_t>(Status::InvalidData);
        }
    }

    __declspec(dllexport) int32_t webpDecodeInto(const uint8_t *in_data, size_t in_data_size, Format out_format, uint8_t *out_data, size_t out_data_size, int32_t out_stride)
    {
        if (in_data == nullptr || in_data_size == 0 || out_data == nullptr || out_data_size == 0)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (out_format == Format::RGBA)
        {
            uint8_t *result = WebPDecodeRGBAInto(in_data, in_data_size, out_data, out_data_size, out_stride);
            if (result != nullptr)
            {
                return static_cast<int32_t>(Status::OK);
            }
            else
            {
                return static_cast<int32_t>(Status::InvalidData);
            }
        }
        else
        {
            return static_cast<int32_t>(Status::InvalidArgument);
        }
    }

    __declspec(dllexport) int32_t webpEncode(const uint8_t *in_data, size_t in_data_size, Format in_format, int32_t width, int32_t height, uint8_t **out_data, size_t *out_data_size, int32_t out_stride)
    {
        if (in_data == nullptr || in_data_size == 0 || width <= 0 || height <= 0 || out_data == nullptr || out_data_size == nullptr || out_stride <= 0)
            return static_cast<int32_t>(Status::InvalidArgument);

        if (in_format == Format::RGBA)
        {
            size_t encoded = WebPEncodeLosslessRGBA(in_data, width, height, out_stride, out_data);
            if (encoded > 0)
            {
                *out_data_size = encoded;
                return static_cast<int32_t>(Status::OK);
            }
            else
            {
                *out_data = nullptr;
                *out_data_size = 0;
                return static_cast<int32_t>(Status::InvalidData);
            }
        }

        *out_data = nullptr;
        *out_data_size = 0;
        return static_cast<int32_t>(Status::InvalidArgument);
    }

    __declspec(dllexport) int32_t webpFree(uint8_t *ptr)
    {
        if (ptr == nullptr)
            return static_cast<int32_t>(Status::InvalidArgument);

        WebPFree(ptr);
        return static_cast<int32_t>(Status::OK);
    }
}