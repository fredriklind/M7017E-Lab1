#ifndef VIDEOCONTAINER_H
#define VIDEOCONTAINER_H

#include <QWidget>
#include <gst/gst.h>
#include "mainwindow.h"

/**
 * @brief Handles playback of a video file. Provides signals for updates about video state and errors.
 */
class VideoContainer : public QWidget
{
    Q_OBJECT
public:
    explicit VideoContainer(QWidget *parent = 0);
    void initVideo(QString);
    int totalDuration;
    bool seekMutex;

private:
    GstElement *v_pipeline;
    long currentTime;
    GstState getState();
    void setState(GstState state);
    QTimer *timer;
    bool didInitVideo;
    void abortVideo();

signals:
    void videoTimerEvent(VideoInfo);
    void videoDidInit();
    void videoStateDidChange();
    void videoError();

public slots:
    void pauseVideo();
    void playVideo();
    void seekVideo(int);

private slots:
    void internalVideoTimerEvent();

};

#endif // VIDEOCONTAINER_H
