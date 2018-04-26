#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QWindow>
#include <QQuickItem>
#include <QTimer>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickWindow window ;

    window.setHeight(600);
    window.setWidth(600);
    window.setTitle("QtTestWindow");
    window.show();

    QQmlApplicationEngine engine;


    QQmlComponent componentFirst(&engine,QUrl(QStringLiteral("qrc:/main.qml")) );
    QQuickItem *childItemFirst = qobject_cast<QQuickItem*>(componentFirst.create());
    childItemFirst->setParentItem(window.contentItem());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));


    QQmlComponent componentSecond(&engine,QUrl(QStringLiteral("qrc:/second.qml")) );
    QQuickItem *childItemSecond = qobject_cast<QQuickItem*>(componentSecond.create());
    childItemSecond->setParentItem(window.contentItem());
    engine.load(QUrl(QStringLiteral("qrc:/second.qml")));

    qDebug()<<"window before:"<<window.contentItem();
    qDebug()<<"first screen before:"<<childItemFirst;
    qDebug()<<"second screen before:"<<childItemSecond;
    qDebug()<<"z for first before"<<childItemFirst->z();
    qDebug()<<"z for before"<<childItemSecond->z();


    QTimer::singleShot(8000,[&]{
        childItemFirst->setParentItem(0);
        childItemFirst->setParentItem(window.contentItem());
        //childItemFirst->setZ(childItemFirst->z()+ 1); // if I don't increase z coordinate explicitly the  visual item is not reparented.
        //somewhow Qt remembers the original z for an item. Why??

        qDebug()<<"window after:"<<window.contentItem();
        qDebug()<<"first screen after:"<<childItemFirst;
        qDebug()<<"second screen after:"<<childItemSecond;
        qDebug()<<"z for first after"<<childItemFirst->z();
        qDebug()<<"z for second"<<childItemSecond->z();
    });

    //system("ivi-shell -a QtTestWindow:255; ivi-shell -z QtTestWindow:255 &");

    return app.exec();
}


