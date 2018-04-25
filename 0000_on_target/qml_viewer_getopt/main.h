#ifndef MAIN_H
#define MAIN_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QQuickImageProvider>

class Tools : public QObject
{
    Q_OBJECT
    
public:
    Tools() {}
    ~Tools() {}
    
public slots:
    Q_INVOKABLE bool fileExists(const QString &sFilePath);
};

class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString qmlFilePath READ getQmlFilePath WRITE setQmlFilePath NOTIFY qmlFilePathChanged)
    Q_PROPERTY(int windowX READ getWindowX WRITE setWindowX NOTIFY windowXChanged)
    Q_PROPERTY(int windowY READ getWindowY WRITE setWindowY NOTIFY windowYChanged)
    Q_PROPERTY(unsigned int windowWidth READ getWindowWidth WRITE setWindowWidth NOTIFY windowWidthChanged)
    Q_PROPERTY(unsigned int windowHeight READ getWindowHeight WRITE setWindowHeight NOTIFY windowHeightChanged)
    Q_PROPERTY(QString windowBgColor READ getWindowBgColor WRITE setWindowBgColor NOTIFY windowBgColorChanged)
    Q_PROPERTY(QString windowTitle READ getWindowTitle WRITE setWindowTitle NOTIFY windowTitleChanged)
    Q_PROPERTY(unsigned int windowZorder READ getWindowZorder WRITE setWindowZorder NOTIFY windowZorderChanged)
    Q_PROPERTY(unsigned int windowAlpha READ getWindowAlpha WRITE setWindowAlpha NOTIFY windowAlphaChanged)
    Q_PROPERTY(bool windowFrameless READ getWindowFrameless WRITE setWindowFrameless NOTIFY windowFramelessChanged)

public:
    Controller() :
        m_qmlFilePath(), m_windowX(0), m_windowY(0), m_windowWidth(0), m_windowHeight(0), m_windowBgColor(),
        m_windowTitle(), m_windowZorder(0),m_windowAlpha(0), m_windowFrameless(false),
        m_showingWaylandSurfaceTimerId(-1) {}
    ~Controller() {}

public:
    void startShowingWaylandSurfaceTimer() {
        m_showingWaylandSurfaceTimerId = startTimer(100);
    }

public:
    QString getQmlFilePath() {
        return m_qmlFilePath;
    }
    void setQmlFilePath(QString qmlFilePath) {
        m_qmlFilePath = qmlFilePath;
        emit qmlFilePathChanged();
    }

    int getWindowX() {
        return m_windowX;
    }
    void setWindowX(int windowX) {
        m_windowX = windowX;
        emit windowXChanged();
    }

    int getWindowY() {
        return m_windowY;
    }
    void setWindowY(int windowY) {
        m_windowY = windowY;
        emit windowYChanged();
    }

    unsigned int getWindowWidth() {
        return m_windowWidth;
    }
    void setWindowWidth(unsigned int windowWidth) {
        m_windowWidth = windowWidth;
        emit windowWidthChanged();
    }

    unsigned int getWindowHeight() {
        return m_windowHeight;
    }
    void setWindowHeight(unsigned int windowHeight) {
        m_windowHeight = windowHeight;
        emit windowHeightChanged();
    }

    QString getWindowBgColor() {
        return m_windowBgColor;
    }
    void setWindowBgColor(QString windowBgColor) {
        m_windowBgColor = windowBgColor;
        emit windowBgColorChanged();
    }

    QString getWindowTitle() {
        return m_windowTitle;
    }
    void setWindowTitle(QString windowTitle) {
        m_windowTitle = windowTitle;
        emit windowTitleChanged();
    }

    unsigned int getWindowZorder() {
        return m_windowZorder;
    }
    void setWindowZorder(unsigned int windowZorder) {
        m_windowZorder = windowZorder;
        emit windowZorderChanged();
    }

    unsigned int getWindowAlpha() {
        return m_windowAlpha;
    }
    void setWindowAlpha(unsigned int windowAlpha) {
        m_windowAlpha = windowAlpha;
        emit windowAlphaChanged();
    }

    bool getWindowFrameless() {
        return m_windowFrameless;
    }
    void setWindowFrameless(bool windowFrameless) {
        m_windowFrameless = windowFrameless;
        emit windowFramelessChanged();
    }

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void qmlFilePathChanged();
    void windowXChanged();
    void windowYChanged();
    void windowWidthChanged();
    void windowHeightChanged();
    void windowBgColorChanged();
    void windowTitleChanged();
    void windowZorderChanged();
    void windowAlphaChanged();
    void windowFramelessChanged();

private:
    QString m_qmlFilePath;
    int m_windowX;
    int m_windowY;
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    QString m_windowBgColor;
    QString m_windowTitle;
    unsigned int m_windowZorder;
    unsigned int m_windowAlpha;
    bool m_windowFrameless;
    int m_showingWaylandSurfaceTimerId;
};

class ColorImageProvider : public QQuickImageProvider
{
public:
    ColorImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Pixmap)
    {
    }

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
    {
        int width = 100;
        int height = 100;

        if (size) {
            *size = QSize(width, height);
        }
        QPixmap pixmap(
            requestedSize.width() > 0 ? requestedSize.width() : width,
            requestedSize.height() > 0 ? requestedSize.height() : height
        );
        pixmap.fill(QColor(id).rgba());

        return pixmap;
    }
};

#endif // MAIN_H
