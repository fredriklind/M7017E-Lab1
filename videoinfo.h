#ifndef VIDEOINFO_H
#define VIDEOINFO_H
#include <gst/gst.h>

/**
 * @brief Holds basic information about a video. Is used to for communication between
 * VideoContainer and MainWindow.
 */
class VideoInfo
{
public:
    VideoInfo();
    GstState state; /**< The current state of the video */
    int currentTime; /**< Current playback time (in seconds) of the video */
    int totalDuration; /**< Total duration (in seconds) of the video */
};

#endif // VIDEOINFO_H
