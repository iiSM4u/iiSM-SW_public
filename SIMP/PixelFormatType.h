#ifndef PIXELFORMATTYPE_H
#define PIXELFORMATTYPE_H

#include <QString>

enum class PixelFormatType {
    RGB24,
    RGB32,
    Raw,
};

inline QString toString(PixelFormatType format)
{
    switch (format)
    {
    case PixelFormatType::RGB24: return "RGB24";
    case PixelFormatType::RGB32: return "RGB32";
    case PixelFormatType::Raw: return "Raw";
    default: return "Unknown";
    }
}

#endif // PIXELFORMATTYPE_H
