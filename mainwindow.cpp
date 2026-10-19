#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include <QStatusBar>
#include <QLineEdit>
#include <QThreadPool>
#include <QThread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &MainWindow::terminProcess);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startProcess);
    connect(ui->uiButton, &QPushButton::clicked, this, &MainWindow::checkStatus);
}

MainWindow::~MainWindow()
{
    isCanceled = true;
    QThreadPool::globalInstance()->waitForDone();
    delete ui;
}

void MainWindow::terminProcess()
{
    isCanceled = true;
    ui->statusbar->showMessage("Process is terminated");
}


void MainWindow::startProcess()
{
    QString txt = ui->lineEdit->text();
    ui->lineEdit->setText("");
    bool ok;
    int number = txt.toInt(&ok);
    if (ok && number >= 0) {
        ui->statusbar->showMessage("Process is started", 3000);
        ui->lineEdit->setReadOnly(true);
    }
    else {
        ui->statusbar->showMessage("No number here", 3000);
        return;
    }

    isCanceled = false;

    QThread *thread = new QThread(this);
    Worker *worker = new Worker();

    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, [worker, number]{
        worker->doWork(number);
    });
    connect(ui->cancelButton, &QPushButton::clicked, worker, &Worker::cancel);

    connect(worker, &Worker::resultReady, this, [this](double res){
        ui->statusbar->showMessage(QString::number(res), 3000);
    });

    connect(worker, &Worker::resultReady, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(thread, &QThread::finished, this, [this]{
        ui->lineEdit->setReadOnly(false);
    });

    thread->start();
}


void MainWindow::checkStatus()
{
    count++;
    ui->statusbar->showMessage(QString::number(count), 2000);
}

