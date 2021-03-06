#include "videocontainer.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>
#include <QTimer>
#include "videoinfo.h"

/** Interval between calls to internalVideoTimerEvent() */
#define TIMER_INTERVAL 1000

/**
 * @brief Handles playback of a video file. Provides signals for updates on video state and errors.
 */
VideoContainer::VideoContainer(QWidget *parent) :
    QWidget(parent)
{
    this->didInitVideo = false;
}

/**
 * @brief Initiates a video for playback. Sets up a new GStreamer playbin2 and sets the appropriate
 * attributes for the video to be played. The path is assumed to have to have "file://localhost" prepended
 * to it. This may not be the case on all platforms. A check for this may have to be implemented for cross platform
 * compability. Emits the videoDidInit() sigal when finished, even if the video cannot be played. Playback errors
 * are catched in the internalVideoTimerEvent() method, that is called every TIMER_INTERVAL.
 *
 * @param videoPath The local path of the video to be played
 */
void VideoContainer::initVideo(QString videoPath)
{
    gst_init (NULL, NULL);

    // Creating a pipeline and the two sinks
    this->v_pipeline = gst_element_factory_make("playbin2", "player");
    GstElement *videosink = gst_element_factory_make("ximagesink", "video-sink");
    GstElement *audiosink = gst_element_factory_make("alsasink", "audiosink");

    // Connect the audio and video sink, and setting the uri for the playbin2
    g_object_set(G_OBJECT(this->v_pipeline),"audio-sink", audiosink,"video-sink", videosink,NULL);
    g_object_set(G_OBJECT(this->v_pipeline), "video-sink", videosink, NULL);

    this->setState(GST_STATE_READY);

    // Construct the path and load the uri
    if(videoPath != "" || videoPath != NULL){
        QString path = "file://localhost" + videoPath;
        const char *uri = path.toStdString().c_str();
        g_object_set(G_OBJECT(this->v_pipeline), "uri", uri, NULL);
    } else {
        //Abort init
        return;
    }

    // Attatch the GStreamer video to the VideoContainer
    if(this->v_pipeline && GST_IS_X_OVERLAY(this->v_pipeline)){
        gst_x_overlay_set_window_handle(GST_X_OVERLAY(this->v_pipeline),this->winId());
    }

    this->timer = new QTimer(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(internalVideoTimerEvent()));
    this->timer->start(TIMER_INTERVAL);

    this->didInitVideo = true;
    this->playVideo();
    emit videoDidInit();
}

/**
 * @brief Slot to pause video playback
 */
void VideoContainer::pauseVideo()
{
    this->setState(GST_STATE_PAUSED);
}

/**
 * @brief Slot for starting video playback
 */
void VideoContainer::playVideo()
{
    this->setState(GST_STATE_PLAYING);
}

/**
 * @brief Sets the state of the playbin, does nothing if no video has been initialized.
 * @param state The state to change to
 */
void VideoContainer::setState(GstState state)
{
    if(this->didInitVideo){
        GstStateChangeReturn stateChangeResult = gst_element_set_state(this->v_pipeline, state);

        if(stateChangeResult == GST_STATE_CHANGE_FAILURE){
            qDebug() << "Could not change state";
            abortVideo();
        } else {
            emit videoStateDidChange();
        }
    }
}

/**
 * @brief Puts the current time of the video at the specified seek time.
 * @param value Where (in seconds) to seek in the video
 */
void VideoContainer::seekVideo(int value){
    if(!seekMutex && this->didInitVideo)
    gst_element_seek_simple (this->v_pipeline, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), (gint64)(value * GST_SECOND));
}

/**
 * @brief Used when a playback error was encountered. Rolls back video initialization and stops the internal timer.
 */
void VideoContainer::abortVideo(){
    this->timer->stop();
    this->didInitVideo = false;
    emit videoError();
}

/**
 * @brief Queries the state of the playbin
 * @return The current state of the playbin
 */
GstState VideoContainer::getState(){
    GstState currState;
    GstStateChangeReturn stateQueryResult;
    stateQueryResult = gst_element_get_state(this->v_pipeline, &currState, NULL, GST_SECOND / 3);
    if(stateQueryResult == GST_STATE_CHANGE_FAILURE){
        abortVideo();
        qDebug() << "Could not query state";
    }
    return currState;
}

/**
* @brief This method (that is a private slot) is called every TIMER_INTERVAL by this->timer and it's responsibility
*  is to get total duration and current time of the video and send it to whoever is
*  listening to the videoTimerEvent(Videoinfo) signal.
*/
void VideoContainer::internalVideoTimerEvent()
{
    /* Only run this method if video is PLAYING */
    if (this->getState() != GST_STATE_PLAYING) return;

    gint64 totalDuration = GST_CLOCK_TIME_NONE;
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 current = -1;

    // Get total duration of video (in seconds)
    if (!GST_CLOCK_TIME_IS_VALID (totalDuration)) {
        if (!gst_element_query_duration (this->v_pipeline, &fmt, &totalDuration)) {
            g_printerr ("Could not query current duration.\n");
            abortVideo();
        } else {
            this->totalDuration = (int)((gdouble)totalDuration / GST_SECOND);
        }
    }

    // Get current location in video (in seconds)
    if (gst_element_query_position (this->v_pipeline, &fmt, &current)) {
        int newCurrentTime = (int)((gdouble)current / GST_SECOND);

        //Error checking, sometimes currentTime becomes WAY big
        if(newCurrentTime <= this->totalDuration){
            this->currentTime = newCurrentTime;
        }
    }

    // Construct info & notify observers
    VideoInfo info;
    info.state          = this->getState();
    info.currentTime    = this->currentTime;
    info.totalDuration  = this->totalDuration;

    emit videoTimerEvent(info);
}

