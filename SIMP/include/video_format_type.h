#pragma once

#include <QString>

enum class VideoFormatType {
    MJPEG,
    XVID,
    MP4V,
    NONE
};


inline QString toString(VideoFormatType type)
{
    switch (type)
    {
    case VideoFormatType::MJPEG: return "MJPEG";
    case VideoFormatType::XVID: return "XVID";
    case VideoFormatType::MP4V: return "MP4V";
    case VideoFormatType::NONE: return "NONE";
    default: return "None";
    }
}

