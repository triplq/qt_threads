#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <atomic>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    int count = 0;
    double dummy = 0.0;
    std::atomic<bool> isCanceled{false};

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void startProcess();

    void terminProcess();

    void checkStatus();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
