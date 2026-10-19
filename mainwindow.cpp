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

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startProcess);
    connect(ui->uiButton, &QPushButton::clicked, this, &MainWindow::checkStatus);
}

MainWindow::~MainWindow()
{

    if (m_worker)
        m_worker->cancel();
    if (m_thread) {
        m_thread->quit();
        m_thread->wait();
    }

    delete ui;
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

    QThread *thread = new QThread(this);
    Worker *worker = new Worker();
    worker->moveToThread(thread);
    m_worker = worker;
    m_thread = thread;


    connect(m_thread, &QThread::started, worker, [worker, number]{
        worker->doWork(number);
    });
    connect(ui->cancelButton, &QPushButton::clicked, worker, &Worker::cancel, Qt::DirectConnection);

    connect(worker, &Worker::resultReady, this, [this](double res){
        ui->statusbar->showMessage(QString::number(res), 3000);
    });

    connect(worker, &Worker::resultReady, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
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

