#include <QCoreApplication>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "main.h"
#include "log.h"
#include "api-surface-gui.h"
#include "api-surface-data.h"

void ExitTimer::timerEvent(QTimerEvent *event)
{
    exitProcess();
}

void ExitTimer::exitProcess()
{
    QCoreApplication::exit(0);
}

void startExitTimer()
{
    static ExitTimer et;

    int exitTimout = 100;
    char *sExitTimout = getenv("WS_TIMEOUT");
    if (sExitTimout != NULL) {
        exitTimout = QString(sExitTimout).toInt();
    }
    et.startTimer(exitTimout);
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

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s [--list|-l]\n", basename(argv[0]));
    printf("    %s [--visibility|-v] <surface_name> [0|1]\n", basename(argv[0]));
    printf("    %s [--position|-p] <surface_name> <left> <top>\n", basename(argv[0]));
    printf("    %s [--size|-s] <surface_name> <width> <height>\n", basename(argv[0]));
    printf("    %s [--zorder|-z] <surface_name> <zorder>\n", basename(argv[0]));
    printf("    %s [--monitor|-m]\n", basename(argv[0]));
    printf("\n");
    printf("Note: \n");
    printf("1) to export WS_DEBUG=1 to enable log.\n");
    printf("2) to export WS_TIMEOUT=<milliseconds> to set the internal to exit.\n");
}

void setSurfaceVisibility(const std::string &surface, bool visible)
{
    api_data::surface::Manager *pManager = api_gui::surface::init();
    api_gui::surface::checkout(pManager);
    api_gui::surface::setOpacity(pManager, surface, (api_data::surface::Ramp)(visible ? 0x01 : 0x00));
    api_gui::surface::commit(pManager);
}

void setSurfacePosition(const std::string &surface, int x, int y)
{
    api_data::surface::Manager *pManager = api_gui::surface::init();
    api_gui::surface::checkout(pManager);
    api_gui::surface::setPosition(pManager, surface, api_data::surface::Size(x, y));
    api_gui::surface::commit(pManager);
}

void setSurfaceSize(const std::string &surface, int w, int h)
{
    api_data::surface::Manager *pManager = api_gui::surface::init();
    api_gui::surface::checkout(pManager);
    api_gui::surface::setSize(pManager, surface, api_data::surface::Size(w, h));
    api_gui::surface::commit(pManager);
}

void setSurfaceZOrder(const std::string &surface, int zorder)
{
    api_data::surface::Manager *pManager = api_gui::surface::init();
    api_gui::surface::checkout(pManager);
    api_gui::surface::setCloseness(pManager, surface, (api_data::surface::Ramp)zorder);
    api_gui::surface::commit(pManager);
}

static bool list_surface_mode = false;
void list_surface_callback(const char *surface)
{
    if (list_surface_mode) {
        printf("  %s\n", surface);
    }
}

void listSurfaces()
{
    list_surface_mode = true;
    printf("Surfaces:\n");
    api_data::surface::Manager *pManager = api_gui::surface::init();
    api_gui::surface::checkout(pManager);
    api_gui::surface::commit(pManager);
}

void setDegbugMode()
{
    bool debugMode = false;
    char *wsDebug = getenv("WS_DEBUG");
    if (wsDebug != NULL) {
        if (std::string(wsDebug) == "1") {
            debugMode = true;
        }
    }
    log_enable(debugMode);
}

static bool g_bMonitorMode = false;

void setMonitorMode(bool bMonitorMode)
{
    g_bMonitorMode = bMonitorMode;
}

bool getMonitorMode()
{
    return g_bMonitorMode;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    setDegbugMode();

    do {
        if (argc >= 2) {
            if (std::string(argv[1]) == "--list" || std::string(argv[1]) == "-l") {
                startExitTimer();
                listSurfaces();
                break;
            }
            
            if (std::string(argv[1]) == "--monitor" || std::string(argv[1]) == "-m") {
                setMonitorMode(true);
                listSurfaces();
                break;
            }
        }

        if (argc < 3) {
            usage(argc, argv);
            exit(0);
        }

        if (std::string(argv[1]) == "--visibility" || std::string(argv[1]) == "-v") {
            if (argc < 4) {
                usage(argc, argv);
                exit(0);
            }

            if (std::string(argv[3]) == "1") {
                setSurfaceVisibility(argv[2], true);
            } else if (std::string(argv[3]) == "0") {
                setSurfaceVisibility(argv[2], false);
            } else {
                usage(argc, argv);
                exit(0);
            }
        }

        if (std::string(argv[1]) == "--position" || std::string(argv[1]) == "-p") {
            if (argc < 5) {
                usage(argc, argv);
                exit(0);
            }

            int x = QString(argv[3]).toInt();
            int y = QString(argv[4]).toInt();
            setSurfacePosition(argv[2], x, y);
        }

        if (std::string(argv[1]) == "--size" || std::string(argv[1]) == "-s") {
            if (argc < 5) {
                usage(argc, argv);
                exit(0);
            }

            int w = QString(argv[3]).toInt();
            int h = QString(argv[4]).toInt();
            setSurfaceSize(argv[2], w, h);
        }

        if (std::string(argv[1]) == "--zorder" || std::string(argv[1]) == "-z") {
            if (argc < 4) {
                usage(argc, argv);
                exit(0);
            }

            int zorder = QString(argv[3]).toInt();
            setSurfaceZOrder(argv[2], zorder);
        }
    } while (false);

    return app.exec();
}
