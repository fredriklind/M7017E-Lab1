#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videocontainer.h"
#include "videoinfo.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QSlider>

#define WIND_AMOUNT 3

/**
* @brief Handles all UI and user interaction.
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ZyberPlayer");
    this->createMenu();
    this->shouldRespondToVideoTimerEvents = true;

    // Signals from UI
    connect(ui->pauseButton, SIGNAL(clicked()), ui->videocontainer, SLOT(pauseVideo()));
    connect(ui->playButton, SIGNAL(clicked()), ui->videocontainer, SLOT(playVideo()));
    connect(ui->slider, SIGNAL(valueChanged(int)), ui->videocontainer, SLOT(seekVideo(int)));

    //Forward
    connect(ui->forwardButton, SIGNAL(pressed()), this, SLOT(windStartEast()));
    connect(ui->forwardButton, SIGNAL(released()), this, SLOT(windEnd()));

    //Backward
    connect(ui->backwardButton, SIGNAL(pressed()), this, SLOT(windStartWest()));
    connect(ui->backwardButton, SIGNAL(released()), this, SLOT(windEnd()));

    // Signals from VideoContainer
    connect(ui->videocontainer, SIGNAL(videoDidInit()), this, SLOT(videoInit()));
    connect(ui->videocontainer, SIGNAL(videoError()), this, SLOT(videoError()));
    connect(ui->videocontainer, SIGNAL(videoTimerEvent(VideoInfo)), this, SLOT(updateUI(VideoInfo)));
}

/**
 * @brief Slot called when the video has been initialized
*/
void MainWindow::videoInit(){
    qDebug() << "Video is initialized!";
    this->windTimer = new QTimer(this);
    connect(this->windTimer, SIGNAL(timeout()), this, SLOT(windLoop()));
}

/**
 * @brief Start fast forwariding
*/
void MainWindow::windStartEast(){
    if(!this->windTimer) return;
    ui->videocontainer->pauseVideo();
    this->shouldRespondToVideoTimerEvents = false;
    this->windDirection = WIND_DIRECTION_EAST;
    this->windTimer->start(500);
}

/**
 * @brief Start rewinding
*/
void MainWindow::windStartWest(){
    if(!this->windTimer) return;
    ui->videocontainer->pauseVideo();
    this->shouldRespondToVideoTimerEvents = false;
    this->windDirection = WIND_DIRECTION_WEST;
    this->windTimer->start(500);
}

/**
 * @brief This is the loop that seeks in the video when a user is pressing the ff/backward buttons
*/
void MainWindow::windLoop(){
    if(this->windDirection == WIND_DIRECTION_EAST){
        ui->slider->setValue(ui->slider->value() + WIND_AMOUNT);
        ui->currentTimeLabel->setNum(ui->slider->value() + WIND_AMOUNT);
    }

    if(this->windDirection == WIND_DIRECTION_WEST){
        ui->slider->setValue(ui->slider->value() - WIND_AMOUNT);
        ui->currentTimeLabel->setNum(ui->slider->value() - WIND_AMOUNT);
    }
}

/**
 * @brief Stops fast forward/backward
 */
void MainWindow::windEnd(){
    if(!this->windTimer) return;
    this->shouldRespondToVideoTimerEvents = true;
    this->windTimer->stop();
    ui->videocontainer->playVideo();
}

/**
 * @brief This slot is signaled when the videocontainer has any kind of error;
 */
void MainWindow::videoError(){
    QMessageBox msgBox;
    msgBox.setText("Error while playing the video file.");
    msgBox.setInformativeText("Choose another file.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

/**
 * @brief Gets a file path from the system using a file open dialog and passes the path to the videocontainer, initializing it.
 */
void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"),"", tr("Files (*.*)"));
    //QString fileName = "/Users/fredriklind/Desktop/big-buck-bunny_trailer.webm";
    if(fileName != NULL){
        qDebug() << "Filename is: " << fileName;
        this->setWindowTitle(fileName);
        ui->videocontainer->initVideo(fileName);
    }
}

/**
 * @brief Creates the drop down menus, such as "File", and "View" and their corresponding menu items.
 */
void MainWindow::createMenu()
{
    //Initialize the action
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a video file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    //Create menu item
    fileMenu = ui->menu->addMenu(tr("&File"));
    fileMenu->addAction(openAct);

    //Initialize the action
    fsAct = new QAction(tr("&Fullscreen"), this);
    fsAct->setShortcuts(QKeySequence::FullScreen);
    fsAct->setStatusTip(tr("View the video in fullscreen mode"));
    connect(fsAct, SIGNAL(triggered()), this, SLOT(fullscreenMode()));

    //Create menu item
    viewMenu = ui->menu->addMenu(tr("&View"));
    viewMenu->addAction(fsAct);
}

/**
 * @brief Puts the app in fullscreen mode, hiding all of the playback controls
 */
void MainWindow::fullscreenMode(){
    if(this->isFullScreen()){
        ui->pauseButton->show();
        ui->playButton->show();
        ui->slider->show();
        ui->forwardButton->show();
        ui->backwardButton->show();
        ui->currentTimeLabel->show();
        ui->totalDurationLabel->show();
        ui->statusBar->show();
        this->showNormal();
    } else {
        ui->pauseButton->hide();
        ui->playButton->hide();
        ui->slider->hide();
        ui->forwardButton->hide();
        ui->backwardButton->hide();
        ui->currentTimeLabel->hide();
        ui->totalDurationLabel->hide();
        ui->statusBar->hide();
        this->showFullScreen();
    }
}

/**
 * @brief This slot is caleld every time the videocontainer emits the VideoTimerEvent() signal
 * it is responsible for updating the UI so that it accurately represents the state of the video
 * being played. This method can be made "inactive" by setting shouldRespondToVideoTimerEvents on
 * MainWindow.
 * @param info Provides info about the current state of the video being played.
 */
void MainWindow::updateUI(VideoInfo info)
{
    if(this->shouldRespondToVideoTimerEvents){
        ui->slider->setRange(0, info.totalDuration);

        ui->videocontainer->seekMutex = true;
        ui->slider->setValue(info.currentTime);
        ui->videocontainer->seekMutex = false;

        ui->totalDurationLabel->setNum(info.totalDuration);
        ui->currentTimeLabel->setNum(info.currentTime);
    }
}

/**
 * @brief Called every time when the user moves the slider. This method makes sure that the current
 * time is instantly updated to reflect where the slider is at any given time.
 * @param position The position that the slider was moved to.
 */
void MainWindow::on_slider_sliderMoved(int position)
{
    ui->currentTimeLabel->setNum(position);
}

/**
 * @brief Makes sure that the UI is not updated when the user "holds" the slider handle. This
 * prevents the slider handle from "fighting" the user when the user really should be in control
 * of the handle.
 */
void MainWindow::on_slider_sliderPressed()
{
    this->shouldRespondToVideoTimerEvents = false;
}

/**
 * @brief Gives back control of the slider handle to MainWindow.
 */
void MainWindow::on_slider_sliderReleased()
{
    this->shouldRespondToVideoTimerEvents = true;
}

/**
 * @brief Destroys the UI object
 */
MainWindow::~MainWindow()
{
    delete ui;
}
