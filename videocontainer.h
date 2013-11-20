#ifndef VIDEOCONTAINER_H
#define VIDEOCONTAINER_H

#include <QWidget>
#include <gst/gst.h>
#include "mainwindow.h"

class VideoContainer : public QWidget
{
    Q_OBJECT
public:
    explicit VideoContainer(QWidget *parent = 0);
    void initVideo(const char *);
    int totalDuration;

private:
    GstElement *v_pipeline;
    long currentTime;
    GstState state;
    void setState(GstState state);
    QTimer *timer;

signals:
    void videoTimerEvent(VideoInfo);
    void videoDidInit();
    void videoStateDidChange();

public slots:
    void pauseVideo();
    void playVideo();

private slots:
    void internalVideoTimerEvent();

};

#endif // VIDEOCONTAINER_H
