#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videocontainer.h"
#include "videoinfo.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->createMenu();

    // Signals from UI
    connect(ui->pauseButton, SIGNAL(clicked()), ui->videocontainer, SLOT(pauseVideo()));
    connect(ui->playButton, SIGNAL(clicked()), ui->videocontainer, SLOT(playVideo()));

    // Signals from VideoContainer
    connect(ui->videocontainer, SIGNAL(videoTimerEvent(VideoInfo)), this, SLOT(updateUI(VideoInfo)));

    ui->videocontainer->initVideo(NULL);
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
}

void MainWindow::openFile()
{
    qDebug() << "Open file!";
}

void MainWindow::updateUI(VideoInfo info)
{
    ui->slider->setRange(0, info.totalDuration);
    ui->slider->setValue(info.currentTime);
}

// Destructor
MainWindow::~MainWindow()
{
    delete ui;
}
