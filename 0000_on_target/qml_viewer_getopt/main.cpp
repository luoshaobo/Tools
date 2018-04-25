#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include <QFile>
#include <QDir>
#include <QLibrary>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickWindow>
#include <QQmlComponent>
#include <QQuickItem>

Controller *g_pController = NULL;

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

bool ExecFileExistsInPath(const std::string &sFilePath)
{
    bool bRet = false;

    int nPos = QString(sFilePath.c_str()).indexOf('/');
    if (nPos != -1) {
        return FileExists(sFilePath);
    }

    char *pEnvVarPath = getenv("PATH");
    QStringList envVarPaths = QString(pEnvVarPath).split(":", QString::SkipEmptyParts);
    if (envVarPaths.size() != 0) {
        for (int i = 0; i < envVarPaths.size(); i++) {
            QString envVarPath = envVarPaths[i];
            QString sFullPath;
            if (envVarPath.size() > 0) {
                if (envVarPath[envVarPath.size() - 1] == '/') {
                    sFullPath = envVarPath + QString(sFilePath.c_str());
                } else {
                    sFullPath = envVarPath + QString("/") + QString(sFilePath.c_str());
                }
                bRet = FileExists(sFullPath.toUtf8().data());
                if (bRet) {
                    break;
                }
            }
        }
    }

    return bRet;
}

bool Tools::fileExists(const QString &sFilePath)
{
    return QFile(sFilePath).exists();
}

void Controller::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();

    killTimer(timerId);

    if (m_showingWaylandSurfaceTimerId == timerId) {
        if (g_pController != NULL) {
            if (ExecFileExistsInPath("ivi-shell")) {               
                QString sCmd = QString::asprintf("ivi-shell -a %s:%u -z %s:%u -p %s:%d,%d >/dev/null 2>&1", 
                    g_pController->getWindowTitle().toUtf8().data(), g_pController->getWindowAlpha(),
                    g_pController->getWindowTitle().toUtf8().data(), g_pController->getWindowZorder(),
                    g_pController->getWindowTitle().toUtf8().data(), g_pController->getWindowX(), g_pController->getWindowY()
                );
                system(sCmd.toUtf8().data());
            } else {
                fprintf(stderr, "*** Waring: no ivi-shell!\n");
            }
        }
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

bool loadPlugin(QString sPluginPath, QQmlContext *pContext)
{
    bool bRet = true;
    QLibrary dll;
    bool (*pOnLoadPlugin)(QQmlContext *) = NULL;
    
    if (bRet) {
        if (sPluginPath.length() >= 1) {
            if (sPluginPath[0] == '/') {
                dll.setFileName(sPluginPath);
            } else {
                dll.setFileName(QDir::currentPath() + "/" + sPluginPath);
            }
        } else {
            bRet = false;
        }
    }
    
    if (bRet) {
        if (dll.load()) {
            pOnLoadPlugin = (bool (*)(QQmlContext *))dll.resolve("onLoadPlugin");
            if (pOnLoadPlugin != NULL) {
                bRet = pOnLoadPlugin(pContext);
                if (!bRet) {
                    bRet = false;
                    fprintf(stderr, "*** Error: loadPlugin(): pOnLoadPlugin() failed\n");
                }
            } else {
                bRet = false;
                fprintf(stderr, "*** Error: loadPlugin(): dll.resolve(\"onLoadPlugin\") failed\n");
            }
        } else {
            bRet = false;
            fprintf(stderr, "*** Error: loadPlugin(): dll.load() failed\n");
            fprintf(stderr, "loadPlugin(): %s", dll.errorString().toUtf8().data());
        }
    }
    
    return bRet;
}

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s "
            "[--help|-H] "
            "[--win-pos|-p <x>,<y>] "
            "[--win-size|-s <w>,<h>] "
            "[--win-bg-color|-c <bg-color>] "
            "[--win-title|-t <title>] "
            "[--win-zorder|-z <zorder>] "
            "[--win-alpha|-a <alpha>] "
            "[--win-frameless|-f <frameless>] "
            "[--plugin|-P <dll_path>] "
            "<qml-path>\n", 
            basename(argv[0]));
    printf("Comments:\n");
    printf("    <x>: the default value is: 0.\n");
    printf("        NOTE: (***) olny for internal window.\n");
    printf("    <y>: the default value is: 0.\n");
    printf("        NOTE: (***) olny for internal window.\n");
    printf("    <width>: the default value is: 1280.\n");
    printf("        NOTE: (***) olny for internal window.\n");
    printf("    <height>: the default value is: 720.\n");
    printf("        NOTE: (***) olny for internal window.\n");
    printf("    <bg-color>: the default value is: #00000000.\n");
    printf("        NOTE: (***) format: #AARRGGBB, or red/green/blue/white/black, etc.\n");
    printf("        NOTE: (***) olny for internal window.\n");
    printf("        NOTE: (***) use \\#AARRGGBB or \"#AARRGGBB\" or \'#AARRGGBB\' to get #AARRGGBB in UNIX shell command line.\n");
    printf("    <title>: the default value is: qml_viewer.\n");
    printf("        NOTE: (***) olny for internal window.\n");
    printf("    <zorder>: the default value is: 255.\n");
    printf("        NOTE: (***) scope: [0, 255].\n");
    printf("        NOTE: (***) olny for wayland window.\n");
    printf("    <alpha>: the default value is: 255.\n");
    printf("        NOTE: (***) scope: [0, 255].\n");
    printf("        NOTE: (***) olny for wayland window.\n");
    printf("    <frameless>: 1 for true, 0 for false. The default value is 1.\n");
    printf("        NOTE: (***) olny for internal window.\n");
    printf("    <dll_path>: the path of the dll plugin to be loaded.\n");
    printf("        NOTE: (***) it must export bool onLoadPlugin(QQmlContext *pContext); by C style.\n");
    printf("    <qml-path>: QML file to be loaded. The default value is the empty string.\n");
    printf("Notes:\n");
    printf("    1) If the type of the root object in the qml is not Window, the an internal Window will be created.\n");
}

int main(int argc, char *argv[])
{
    int nRet = 0;

    Tools tools;
    QGuiApplication app(argc, argv);
    Controller *pController = NULL;
    QQmlApplicationEngine *pEngine = NULL;
    QQmlContext *pContext = NULL;
    QList<QObject *> rootObjects;
    QQuickWindow *pQuickWindow = NULL;
    QQuickItem *pQuickItem = NULL;
    QQmlComponent *pQmlComponent = NULL;
    bool bInternalQuickWindow = true;

    QString sQmlFilePath = "-";
    int nWindowX = 0;
    int nWindowY = 0;
    unsigned int nWindowWidth = 1280;
    unsigned int nWindowHeight = 720;
    QString sWindowBgColor = "#00000000";
    QString sWindowTitle = "qml_viewer";
    unsigned int nWindowZorder = 255;
    unsigned int nWindowAlpha = 255;
    bool bWindowFrameless = true;
    QString sPluginPath;

    int opt;
    const char *short_options = "Hp:s:c:t:z:a:f:P:";
    struct option long_options[] = {
        { "help", no_argument, NULL, 'H' },
        { "win-pos",  required_argument, NULL, 'p' },
        { "win-size", required_argument, NULL, 's' },
        { "win-bg-color", required_argument, NULL, 'c' },
        { "win-title", required_argument, NULL, 't' },
        { "win-zorder", required_argument, NULL, 'z' },
        { "win-alpha", required_argument, NULL, 'a' },
        { "win-frameless", required_argument, NULL, 'f' },
        { "plugin", required_argument, NULL, 'P' },
        { 0, 0, 0, 0 }
    };
    
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        switch (opt) {
            case 'H':
                {
                    usage(argc, argv);
                    exit(1);
                }
                break;
            case 'p':
                {
                    QStringList posList = QString(optarg).split(",", QString::KeepEmptyParts);
                    if (posList.size() == 2) {
                        nWindowX = posList[0].toInt();
                        nWindowY = posList[1].toInt();
                    } else {
                        usage(argc, argv);
                        exit(1);
                    }
                }
                break;
            case 's':
                {
                    QStringList sizeList = QString(optarg).split(",", QString::KeepEmptyParts);
                    if (sizeList.size() == 2) {
                        nWindowWidth = sizeList[0].toUInt();
                        nWindowHeight = sizeList[1].toUInt();
                    } else {
                        usage(argc, argv);
                        exit(1);
                    }
                }
                break;
            case 'c':
                {
                    sWindowBgColor = QString(optarg);
                }
                break;
            case 't':
                {
                    sWindowTitle = QString(optarg);
                }
                break;
            case 'z':
                {
                    nWindowZorder = QString(optarg).toUInt();
                    if (nWindowZorder > 255) {
                        usage(argc, argv);
                        exit(1);
                    }
                }
                break;
            case 'a':
                {
                    nWindowAlpha = QString(optarg).toUInt();
                    if (nWindowAlpha > 255) {
                        usage(argc, argv);
                        exit(1);
                    }
                }
                break;
            case 'f':
                {
                    if (QString(optarg) != "0" && QString(optarg) != "1") {
                        usage(argc, argv);
                        exit(1);
                    }
                    bWindowFrameless = QString(optarg) == "0" ? false : true;
                }
                break;
            case 'P':
                {
                    sPluginPath = QString(optarg);
                }
                break;
            case '?':
                {

                }
                break;
            case ':':
                {

                }
                break;
            default:
                break;
        }
    }

    if (optind > 0 && optind < argc) {
        sQmlFilePath = argv[optind];
    }

    if (nRet == 0) {
        if (sQmlFilePath != "-") {
            if (nRet == 0) {
                if (!FileExists(sQmlFilePath.toUtf8().data())) {
                    fprintf(stderr, "*** Error: The file \"%s\" does not exist!\n", sQmlFilePath.toUtf8().data());
                    nRet = -1;
                }
            }

            if (nRet == 0) {
                if (sQmlFilePath.length() >= 1 && sQmlFilePath.at(0) != '/') {
                    char *pwd = getenv("PWD");
                    if (pwd != NULL) {
                        sQmlFilePath = QString(pwd) + "/" + sQmlFilePath;
                    }
                }
            }
        }
    }

    if (nRet == 0) {
        pController = new Controller();
        if (pController == NULL) {
            nRet = -1;
        }
        g_pController = pController;
    }

    if (nRet == 0) {
        pController->setQmlFilePath(sQmlFilePath);
        pController->setWindowX(nWindowX);
        pController->setWindowY(nWindowY);
        pController->setWindowWidth(nWindowWidth);
        pController->setWindowHeight(nWindowHeight);
        pController->setWindowBgColor(sWindowBgColor);
        pController->setWindowTitle(sWindowTitle);
        pController->setWindowZorder(nWindowZorder);
        pController->setWindowAlpha(nWindowAlpha);
        pController->setWindowFrameless(bWindowFrameless);
    }

    if (nRet == 0) {
        pEngine = new QQmlApplicationEngine();
        if (pEngine == NULL) {
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        pEngine->addImageProvider(QLatin1String("colors"), new ColorImageProvider);
    }

    if (nRet == 0) {
        pContext = pEngine->rootContext();
        if (pContext != NULL) {
            pContext->setContextProperty("controller", pController);
            pContext->setContextProperty("tools", &tools);
        } else {
            nRet = -1;
        }
    }
    
    if (nRet == 0) {
        if (!sPluginPath.isEmpty()) {
            if (!loadPlugin(sPluginPath, pContext)) {
                nRet = -1;
            }
        }
    }

    if (nRet == 0) {
        if (pController->getQmlFilePath() != "-") {
            pEngine->load(QUrl(pController->getQmlFilePath()));
            rootObjects = pEngine->rootObjects();
            if (rootObjects.size() >= 1) {
                pQuickWindow = qobject_cast<QQuickWindow *>(rootObjects[0]);
                if (pQuickWindow != NULL) {
                    bInternalQuickWindow = false;
                }
                pQuickItem = qobject_cast<QQuickItem *>(rootObjects[0]);
            } else {
                nRet = -1;
            }
        }
    }

    if (nRet == 0) {
        if (pQuickWindow == NULL) {
            pEngine->load(QUrl(QStringLiteral("qrc:/main.qml")));
            rootObjects = pEngine->rootObjects();
            if (rootObjects.size() >= 2) {
                pQuickWindow = qobject_cast<QQuickWindow *>(rootObjects[1]);
                if (pQuickWindow != NULL) {
                    bInternalQuickWindow = true;
                }
            } else if (rootObjects.size() >= 1) {
                pQuickWindow = qobject_cast<QQuickWindow *>(rootObjects[0]);
                if (pQuickWindow != NULL) {
                    bInternalQuickWindow = true;
                }
            } else {
                nRet = -1;
            }
        }
    }

    if (nRet == 0) {
        if (pQuickWindow != NULL) {
            if (pQuickItem != NULL) {
                QQuickItem *pParentQuickItem = pQuickWindow->contentItem();
                if (pParentQuickItem != NULL) {
                    pQuickItem->setParentItem(pParentQuickItem);
                } else {
                    nRet = -1;
                }
            }
        } else {
            nRet = -1;
        }
    }

    if (nRet == 0) {
        if (!bInternalQuickWindow) {
            pController->setWindowTitle(pQuickWindow->title());
        }
    }
    
    if (nRet == 0) {
        fprintf(stdout, "window: visibility: 0x%08X, hidden: %d\n", pQuickWindow->visibility(), (pQuickWindow->visibility() & QWindow::Hidden) ? 1 : 0);
        fprintf(stdout, "window: x: %d\n", pQuickWindow->x());
        fprintf(stdout, "window: y: %d\n", pQuickWindow->y());
        fprintf(stdout, "window: width: %u\n", pQuickWindow->width());
        fprintf(stdout, "window: height: %u\n", pQuickWindow->height());
        fprintf(stdout, "window: title: %s\n", pQuickWindow->title().toUtf8().data());
        unsigned int flags = (unsigned int)pQuickWindow->flags();
        fprintf(stdout, "window: flags: 0x%08X, framleless: %u\n", flags, (flags & Qt::FramelessWindowHint) ? 1 : 0);
        QColor bgColor = pQuickWindow->color();
        fprintf(stdout, "window: color: 0x%02X%02X%02X%02X\n", bgColor.alpha(), bgColor.red(), bgColor.green(), bgColor.blue());
    }

    if (0) {
        if (nRet == 0) {
            if (pController->getQmlFilePath() != "-") {
                pQmlComponent = new QQmlComponent(pEngine, pController->getQmlFilePath(), QQmlComponent::PreferSynchronous);
                if (pQmlComponent != NULL) {
                    QQuickItem *pQuickItem = qobject_cast<QQuickItem*>(pQmlComponent->create(pContext));
                    if (pQuickItem != NULL) {
                        QQuickItem *pParentQuickItem = pQuickWindow->contentItem();
                        if (pParentQuickItem != NULL) {
                            pQuickItem->setParentItem(pParentQuickItem);
                        } else {
                            nRet = -1;
                        }
                    } else {
                        nRet = -1;
                    }
                } else {
                    nRet = -1;
                }
            }
        }
    }

    if (nRet == 0) {
        pController->startShowingWaylandSurfaceTimer();
    }

    if (nRet == 0) {
        nRet = app.exec();
    }

    if (pEngine != NULL) {
        delete pEngine;
        pEngine = NULL;
        pContext = NULL;
        pQuickWindow = NULL;
        pQuickItem = NULL;
        rootObjects.clear();
    }

    if (pQmlComponent != NULL) {
        delete pQmlComponent;
        pQmlComponent = NULL;
    }

    if (pController != NULL) {
        g_pController = NULL;
        delete pController;
        pController = NULL;
    }

    return nRet;
}
