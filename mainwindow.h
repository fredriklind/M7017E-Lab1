#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <gst/gst.h>
#include "videoinfo.h"

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

private:
    Ui::MainWindow *ui;
    QMenu *fileMenu;
    QAction *openAct;
    void createMenu();
};

#endif // MAINWINDOW_H
