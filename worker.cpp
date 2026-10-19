#include "worker.h"
#include <cmath>

Worker::Worker(QObject *parent) : QObject(parent) { }

Worker::~Worker() {};

void Worker::doWork(int number) {
    cancelled.store(false);
    double sum = 0.0;
    for (size_t i = 0; i < number; i++) {
        if (cancelled.load()) {
            break;
        }
        sum += std::sin(std::sqrt(i));
    }

    emit resultReady(sum);
}

void Worker::cancel() {
    cancelled.store(true);
}