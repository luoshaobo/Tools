#include <QObject>
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include "BLImageProvider.h"
#include "WaylandIviShell.h"
#include "WaylandController.h"

#define HMI_SURfACE_NAME            "HMI_SURfACE"

using namespace uifwk::zones;

WaylandController *g_waylandController = NULL;

void forceNewSurfaceCreated();
QQuickWindow *createWindow();

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    WaylandIviShell waylandIviShell;
    WaylandController waylandController(waylandIviShell);
    g_waylandController = &waylandController;

    //engine.addImageProvider(QLatin1String("blimageprovider"), new BLImageProvider);

    QQmlContext *qmlContext = engine.rootContext();

#if 0
    {
        engine.load(QUrl(QStringLiteral("main.qml")));
        // NOTE:
        // Multiple qml file with the root object being Window could be loaded simultaneously.
        //engine.load(QUrl(QStringLiteral("statusbarWindow.qml")));

        QObject *topLevel = engine.rootObjects().value(0);
        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
        // NOTE:
        // From here on, we can visit the properties and call methods in the object window in C++.
        window->create();                               // will create a wayland surface with the tile as its ID string.

        window->setSurfaceType(QSurface::OpenGLSurface);
        window->setClearBeforeRendering(true);
        window->setFlags(Qt::FramelessWindowHint);

        window->setPersistentOpenGLContext(true);
        window->setPersistentSceneGraph(true);

        QSurfaceFormat format;
        format.setAlphaBufferSize(8);
        format.setDepthBufferSize(16);
        format.setStencilBufferSize(8);
    #ifdef PLATFORM_OVIP
        format.setRenderableType(QSurfaceFormat::OpenGLES);
    #endif
        window->setFormat(format);

        window->setColor(QColor(255, 255, 255, 255));
        window->setWidth(800);
        window->setHeight(480);

        window->setTitle(HMI_SURfACE_NAME);
        window->show();
    }
#else
    {
        QQuickWindow *window = createWindow();             // will create a wayland surface with the tile as its ID string.
        window->setTitle(HMI_SURfACE_NAME);

        engine.load(QUrl(QStringLiteral("widget.qml")));
        QQuickItem *topLevel = (QQuickItem *)engine.rootObjects().value(0);
        topLevel->setParentItem(window->contentItem());

        window->show();
    }
 #endif // #if 1

    forceNewSurfaceCreated();

    return app.exec();
}

QQuickWindow *createWindow()
{
    QQuickWindow *window = new QQuickWindow;

    window->setSurfaceType(QSurface::OpenGLSurface);
    window->setClearBeforeRendering(true);
    window->setFlags(Qt::FramelessWindowHint);

    window->setPersistentOpenGLContext(true);
    window->setPersistentSceneGraph(true);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    format.setDepthBufferSize(16);
    format.setStencilBufferSize(8);
#ifdef PLATFORM_OVIP
    format.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
    window->setFormat(format);

    window->setColor(QColor(255, 0, 0, 255));
    window->setWidth(800);
    window->setHeight(480);

    //QObject::connect(window, &QQuickWindow::sceneGraphInitialized, this, &WindowManager::sltSceneGraphInitialized);
    //QObject::connect(window, &QQuickWindow::sceneGraphInvalidated, this, &WindowManager::sltSceneGraphInvalidated);

    return window;
}

void forceNewSurfaceCreated()
{
    WaylandAnimation *waylandAnimation = NULL;
    waylandAnimation = g_waylandController->createAnimation();
    delete waylandAnimation;
    waylandAnimation = NULL;
}

void when_surface_registered(const char *surface_id)
{
    if (QString(HMI_SURfACE_NAME) != QString(surface_id)) {
        return;
    }

    WaylandAnimation *waylandAnimation = NULL;
    waylandAnimation = g_waylandController->createAnimation();

//    const char *surface_ids[] = {
////        "HMI_SURfACE",
////        "1.1486.surfaces.zones.uifwk.ovip",   // status bar      : 99
////        "1.2252.surfaces.zones.uifwk.ovip",   // main screen     : 98
////        "2.1486.surfaces.zones.uifwk.ovip",   //                 : 97
////        "3.1486.surfaces.zones.uifwk.ovip",

////        "CARPLAY_LOW_NOTIFICATION_ZONE",
////        "CARPLAY_MAIN_ZONE",
////        "LMM_PLAT_ID_MIRRORLINK",
//    };

//    int idCount = sizeof(surface_ids)/sizeof(const char *);
//    for (int i = 0; i < idCount; i++) {
//        waylandAnimation->setPosition(surface_ids[i], 0, 0);
//        waylandAnimation->setSize(surface_ids[i], 800, 480);
//        waylandAnimation->setOpacity(surface_ids[i], 1);
//        waylandAnimation->setZOrder(surface_ids[i], 100 - (i + 1));
//    }

    waylandAnimation->setPosition(HMI_SURfACE_NAME, 0, 0);
    waylandAnimation->setSize(HMI_SURfACE_NAME, 800, 480);
    waylandAnimation->setOpacity(HMI_SURfACE_NAME, 1);
    waylandAnimation->setZOrder(HMI_SURfACE_NAME, 100);   // more big the z-order, more near to the user

    delete waylandAnimation;
    waylandAnimation = NULL;
}

