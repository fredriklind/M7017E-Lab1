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
* Handles all UI and user interaction.
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
 * Slot called when the video has been initialized
*/
void MainWindow::videoInit(){
    qDebug() << "Video is initialized!";
    this->windTimer = new QTimer(this);
    connect(this->windTimer, SIGNAL(timeout()), this, SLOT(windLoop()));
}

/**
 * Start fast forwariding
*/
void MainWindow::windStartEast(){
    if(!this->windTimer) return;
    ui->videocontainer->pauseVideo();
    this->shouldRespondToVideoTimerEvents = false;
    this->windDirection = WIND_DIRECTION_EAST;
    this->windTimer->start(500);
}

/**
 * Start rewinding
*/
void MainWindow::windStartWest(){
    if(!this->windTimer) return;
    ui->videocontainer->pauseVideo();
    this->shouldRespondToVideoTimerEvents = false;
    this->windDirection = WIND_DIRECTION_WEST;
    this->windTimer->start(500);
}

/**
 * This is the loop that seeks in the video when a user is pressing the ff/backward buttons
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

void MainWindow::windEnd(){
    if(!this->windTimer) return;
    this->shouldRespondToVideoTimerEvents = true;
    this->windTimer->stop();
    ui->videocontainer->playVideo();
}

void MainWindow::videoError(){
    QMessageBox msgBox;
    msgBox.setText("Error while playing the video file.");
    msgBox.setInformativeText("Choose another file.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

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

void MainWindow::on_slider_sliderMoved(int position)
{
    ui->currentTimeLabel->setNum(position);
}

void MainWindow::on_slider_sliderPressed()
{
    this->shouldRespondToVideoTimerEvents = false;
}

void MainWindow::on_slider_sliderReleased()
{
    this->shouldRespondToVideoTimerEvents = true;
}

// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}
