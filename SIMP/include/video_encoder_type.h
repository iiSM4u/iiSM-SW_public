#pragma once

#include <QString>

enum class VideoEncoderType {
    MJPEG,
    NONE_COMPRESSOR
};

inline QString toString(VideoEncoderType type)
{
    switch (type)
    {
    case VideoEncoderType::MJPEG: return "mjpeg";
    case VideoEncoderType::NONE_COMPRESSOR: return "non compressor";
    default: return "Unknown";
    }
}

