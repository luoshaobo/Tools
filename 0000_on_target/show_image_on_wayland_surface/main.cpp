#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <QQmlContext>

#define WINDOW_TITLE "show_image_on_wayland_surface"

void Controller::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();

    killTimer(timerId);

    if (m_showingWaylandSurfaceTimerId == timerId) {
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "ivi-shell -a %s:255 &", WINDOW_TITLE);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "ivi-shell -z %s:255 &", WINDOW_TITLE);
        system(cmd);
    }
}

const char *basename(const char *path)
{
    const char *pRet = path;

    if (path != NULL) {
        char *p = strrchr((char *)path, '/');
        if (p != NULL) {
            pRet = (const char *)(p + 1);
        }
    }

    return pRet;
}

bool FileExists(const std::string &sFilePath)
{
    bool bRet;
    FILE *pFile;

    pFile = fopen(sFilePath.c_str(), "rb");
    if (pFile != NULL) {
        fclose(pFile);
        bRet = true;
    } else {
        bRet = false;
    }

    return bRet;
}

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s <image_file_path> [<fill_mode>]\n", basename(argv[0]));
    printf("Args:\n");
    printf("    <fill_mode>:\n");
    printf("      0: Image.Stretch - the image is scaled to fit\n");
    printf("      1: Image.PreserveAspectFit - the image is scaled uniformly to fit without cropping\n");
    printf("      2: Image.PreserveAspectCrop - the image is scaled uniformly to fill, cropping if necessary\n");
    printf("      3: Image.Tile - the image is duplicated horizontally and vertically\n");
    printf("      4: Image.TileVertically - the image is stretched horizontally and tiled vertically\n");
    printf("      5: Image.TileHorizontally - the image is stretched vertically and tiled horizontally\n");
    printf("      6: Image.Pad - the image is not transformed\n");
}

int main(int argc, char *argv[])
{
    std::string sImageFilePath;
    int fillMode = 6;

    do {
        if (argc < 2) {
            usage(argc, argv);
            exit(-1);
        }

        sImageFilePath = argv[1];
        if (!FileExists(sImageFilePath)) {
            printf("*** Error: The file \"%s\" does not exist!\n", sImageFilePath.c_str());
            exit(-1);
        }

        if (sImageFilePath.length() >= 1 && sImageFilePath.at(0) != '/') {
            char *pwd = getenv("PWD");
            if (pwd != NULL) {
                sImageFilePath = std::string(pwd) + "/" + sImageFilePath;
            }
        }

        if (argc >= 3) {
            fillMode = QString(argv[2]).toInt();
        }
    } while(0);

    QGuiApplication app(argc, argv);

    Controller controller;
    controller.setImageFilePath(sImageFilePath.c_str());
    controller.setFillMode(fillMode);

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();
    if (context != NULL) {
        context->setContextProperty("controller", &controller);
    }

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    controller.startShowingWaylandSurfaceTimer();

    return app.exec();
}
