#include <QCoreApplication>

extern int gl_sample_main();

QCoreApplication *g_pApp = NULL;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    g_pApp = &a;

    gl_sample_main();

    return 0;
}

int eventLoop()
{
    printf("eventLoop()\n");
    return g_pApp->exec();
}
