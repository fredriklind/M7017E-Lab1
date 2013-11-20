#ifndef VIDEOINFO_H
#define VIDEOINFO_H
#include <gst/gst.h>

class VideoInfo
{
public:
    VideoInfo();
    GstState state;
    int currentTime;
    int totalDuration;
};

#endif // VIDEOINFO_H
