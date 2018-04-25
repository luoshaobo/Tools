#ifndef MAIN_H_37828423950923784234234823904239423489
#define MAIN_H_37828423950923784234234823904239423489
#include <QObject>

class ExitTimer : public QObject
{
    Q_OBJECT

public:
    ExitTimer() {}
    ~ExitTimer() {}

protected:
    void timerEvent(QTimerEvent *event);

private:
    void exitProcess();
};

#endif // #define MAIN_H_37828423950923784234234823904239423489
