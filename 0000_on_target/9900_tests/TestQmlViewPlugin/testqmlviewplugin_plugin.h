#ifndef TESTQMLVIEWPLUGIN_PLUGIN_H
#define TESTQMLVIEWPLUGIN_PLUGIN_H

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
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QQuickImageProvider>

#include <QQmlExtensionPlugin>

class TestQmlViewPluginPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

class GlobalConfig : public QObject
{
    Q_OBJECT

    Q_ENUMS(BgColor)

    Q_PROPERTY(BgColor bgColor READ bgColor WRITE setBgColor NOTIFY bgColorChanged)
    Q_PROPERTY(int screenWidth READ screenWidth NOTIFY screenWidthChanged)
    Q_PROPERTY(int screenHeight READ screenHeight NOTIFY screenHeightChanged)
    Q_PROPERTY(QString configName MEMBER m_configName NOTIFY configNameChanged)

public:
    enum BgColor {
        Red = 0,
        Green,
        Blue,
    };

public:
    GlobalConfig() : m_bgColor(Red), m_configName("(no name)") {
        connect(this, SIGNAL(testSignal001()), this, SLOT(onTestSignal001()));
    }
    ~GlobalConfig() {}

    BgColor bgColor() const {
        printf("=== %s(): bgColor=%d\n", __FUNCTION__, m_bgColor);
        return m_bgColor;
    }

    void setBgColor(BgColor color) {
        m_bgColor = color;
        printf("=== %s(): bgColor=%d\n", __FUNCTION__, m_bgColor);
        emit bgColorChanged(color);
    }

    int screenWidth() const {
        int value = 1100;
        printf("=== %s(): screenWidth=%d\n", __FUNCTION__, value);
        return value;
    }
    int screenHeight() const {
        int value = 800;
        printf("=== %s(): screenHeight=%d\n", __FUNCTION__, value);
        return value;
    }

public slots:
    Q_INVOKABLE void testOp001() {
        printf("=== %s(): \n", __FUNCTION__);
    }
    Q_INVOKABLE void testOp002() {
        printf("=== %s(): \n", __FUNCTION__);
    }

signals:
    void bgColorChanged(BgColor bgColor);
    void screenWidthChanged(int screenWidth);
    void screenHeightChanged(int screenHeight);
    void configNameChanged();

    void testSignal001();

private slots:
    void onTestSignal001() {
        m_bgColor = Green;
        emit bgColorChanged(m_bgColor);
        printf("=== %s(): \n", __FUNCTION__);
    }

private:
    BgColor m_bgColor;
    QString m_configName;
};

#endif // TESTQMLVIEWPLUGIN_PLUGIN_H
