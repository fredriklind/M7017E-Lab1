#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <gst/gst.h>
#include "videoinfo.h"

// Used for fast forward/backward
typedef enum {
    WIND_DIRECTION_WEST             = 0,
    WIND_DIRECTION_EAST             = 1
} WindDirection;
//----

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void updateUI(VideoInfo);
    void openFile();
    void videoError();
    void videoInit();
    void windLoop();
    void windStartEast();
    void windStartWest();
    void windEnd();
    void fullscreenMode();
    void on_slider_sliderPressed();
    void on_slider_sliderReleased();

    void on_slider_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QMenu *fileMenu;
    QMenu *viewMenu;
    QAction *openAct;
    QAction *fsAct;
    void createMenu();
    QTimer *windTimer;
    WindDirection windDirection;
    bool shouldRespondToVideoTimerEvents;
};

#endif // MAINWINDOW_H
