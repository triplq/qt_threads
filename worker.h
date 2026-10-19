#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <atomic>

class Worker : public QObject
{
    Q_OBJECT
    std::atomic<bool> cancelled{false};

public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker() override;

public slots:
    void doWork(int number);
    void cancel();

signals:
    void resultReady(double result);
};

#endif // WORKER_H
