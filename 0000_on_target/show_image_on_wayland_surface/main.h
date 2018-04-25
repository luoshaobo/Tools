#ifndef MAIN_H
#define MAIN_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>

class Controller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString imageFilePath READ getImageFilePath WRITE setImageFilePath NOTIFY imageFilePathChanged)
    Q_PROPERTY(int fillMode READ getFillMode WRITE setFillMode NOTIFY fillModeChanged)

public:
    Controller() : m_imageFilePath(), m_fillMode(0), m_showingWaylandSurfaceTimerId(-1) {}
    ~Controller() {}

public:
    void startShowingWaylandSurfaceTimer() {
        m_showingWaylandSurfaceTimerId = startTimer(100);
    }

public:
    QString getImageFilePath() {
        return m_imageFilePath;
    }
    void setImageFilePath(QString imageFilePath) {
        m_imageFilePath = imageFilePath;
        emit imageFilePathChanged();
    }

    int getFillMode() {
        return m_fillMode;
    }
    void setFillMode(int fillMode) {
        m_fillMode = fillMode;
        emit fillModeChanged();
    }

protected:
      void timerEvent(QTimerEvent *event);

signals:
    void imageFilePathChanged();
    void fillModeChanged();

private:
    QString m_imageFilePath;
    int m_fillMode;
    int m_showingWaylandSurfaceTimerId;
};

#endif // MAIN_H
