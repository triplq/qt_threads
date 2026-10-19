#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStatusBar>
#include <QLineEdit>
#include <QThreadPool>
#include <cmath>


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
    QThreadPool::globalInstance()->start([this, number]{
        double sum = 0.0;
        for (size_t i = 0; i < number; i++) {
            if (isCanceled.load()) {
                break;
            }
            sum += std::sin(std::sqrt(i));
        }

        QMetaObject::invokeMethod(this, [this, sum]{
            ui->statusbar->showMessage(QString::number(sum), 3000);
            ui->lineEdit->setReadOnly(false);
        });
    });
}


void MainWindow::checkStatus()
{
    count++;
    ui->statusbar->showMessage(QString::number(count), 2000);
}

