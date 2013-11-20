#include "videocontainer.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>
#include <QTimer>
#include "videoinfo.h"

#define TIMER_INTERVAL 1000

VideoContainer::VideoContainer(QWidget *parent) :
    QWidget(parent)
{

}

void VideoContainer::initVideo(const char *videoURL)
{
    gst_init (NULL, NULL);

    // Creating the pipeline and the two sinks
    this->v_pipeline = gst_element_factory_make("playbin2", "player");
    GstElement *videosink = gst_element_factory_make("ximagesink", "video-sink");
    GstElement *audiosink = gst_element_factory_make("alsasink", "audiosink");

    // Connect the audio and video sink, and setting the uri for the playbin2
    g_object_set(G_OBJECT(this->v_pipeline),"audio-sink", audiosink,"video-sink", videosink,NULL);
    g_object_set(G_OBJECT(this->v_pipeline), "video-sink", videosink, NULL);

    if(videoURL){
        g_object_set(G_OBJECT(this->v_pipeline), "uri",videoURL, NULL);
    } else {
        g_object_set(G_OBJECT(this->v_pipeline), "uri","file://localhost/Users/fredriklind/sintel_trailer-480p.webm", NULL);
    }

    this->setState(GST_STATE_READY);

    // Attatch the GStreamer video to the VideoContainer
    if(this->v_pipeline && GST_IS_X_OVERLAY(this->v_pipeline)){
        gst_x_overlay_set_window_handle(GST_X_OVERLAY(this->v_pipeline),this->winId());
    }

    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(internalVideoTimerEvent()));
    this->timer->start(TIMER_INTERVAL);

    emit videoDidInit();
}

/* Public slots */
void VideoContainer::pauseVideo()
{
    this->setState(GST_STATE_PAUSED);
}

void VideoContainer::playVideo()
{
    this->setState(GST_STATE_PLAYING);
}

void VideoContainer::setState(GstState state)
{
    gst_element_set_state(this->v_pipeline, state);
    this->state = state;
    emit videoStateDidChange();
}

// Internal slot
void VideoContainer::internalVideoTimerEvent()
{
    gint64 totalDuration = GST_CLOCK_TIME_NONE;
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 current = -1;

    /* Continue only if PLAYING */
    if (this->state != GST_STATE_PLAYING)
        return;

    /* Fix total duration if it's not set */
    if (!GST_CLOCK_TIME_IS_VALID (totalDuration)) {
      if (!gst_element_query_duration (this->v_pipeline, &fmt, &totalDuration)) {
        g_printerr ("Could not query current duration.\n");
      } else {
          this->totalDuration = (int)((gdouble)totalDuration / GST_SECOND);
      }
    }

    if (gst_element_query_position (this->v_pipeline, &fmt, &current)) {
      this->currentTime = (int)((gdouble)current / GST_SECOND);
    }

    // Construct info
    VideoInfo info;
    info.state          = this->state;
    info.currentTime    = this->currentTime;
    info.totalDuration  = this->totalDuration;

    emit videoTimerEvent(info);
}

