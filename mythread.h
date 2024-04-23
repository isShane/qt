#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread();

signals:

protected:
    void run() override;

};

#endif // MYTHREAD_H
