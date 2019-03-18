#include <QCoreApplication>
#include <service-framework/thread.h>
#include <service-framework/runnable.h>

class MyRunnalbe : public Runnable
{
private:
    virtual void run()
    {
        while (true) {
            sleep(1);
            printf("hello\n");
        }
    }
};

class MyThread : public Thread
{
public:
    MyThread(): Thread(), m_myRunnalbe()
    {
        start(m_myRunnalbe);
    }

private:
    MyRunnalbe m_myRunnalbe;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyThread myThread;

    return a.exec();
}
