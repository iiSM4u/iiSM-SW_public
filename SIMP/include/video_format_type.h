#pragma once

#include <QString>

enum class VideoFormatType {
    MP4,
    WMV,
    AVI,
};

inline QString toString(VideoFormatType type)
{
    switch (type)
    {
    case VideoFormatType::MP4: return "mp4";
    case VideoFormatType::WMV: return "wmv";
    case VideoFormatType::AVI: return "avi";
    default: return "Unknown";
    }
}

