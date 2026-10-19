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

    if (m_thread && m_thread->isRunning()) {
        m_worker->cancel();
        m_thread->quit();
        m_thread->wait();
    }

    delete m_worker;
    // delete m_thread;
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

    m_thread = new QThread(this);
    m_worker = new Worker();

    m_worker->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_worker, [this, number]{
        m_worker->doWork(number);
    });
    connect(ui->cancelButton, &QPushButton::clicked, m_worker, &Worker::cancel, Qt::DirectConnection);

    connect(m_worker, &Worker::resultReady, this, [this](double res){
        ui->statusbar->showMessage(QString::number(res), 3000);
    });

    connect(m_worker, &Worker::resultReady, m_thread, &QThread::quit);
    connect(m_thread, &QThread::finished, this, [this]{
        ui->lineEdit->setReadOnly(false);
    });

    m_thread->start();
}


void MainWindow::checkStatus()
{
    count++;
    ui->statusbar->showMessage(QString::number(count), 2000);
}

