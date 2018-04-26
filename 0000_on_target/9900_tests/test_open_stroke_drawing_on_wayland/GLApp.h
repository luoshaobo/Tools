#ifndef GLAPP_H_783478239491094782378423847237235782374878
#define GLAPP_H_783478239491094782378423847237235782374878

#include <math.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <QObject>
#include <QThread>
#include <QPointF>
#include <QVector>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QMatrix4x4>
#include "input-reader.h"

class GLApp : public QObject
{
    Q_OBJECT

public:
    explicit GLApp();
    virtual ~GLApp();

public:
    virtual void initializeGL();
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintGL();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *ev);
    virtual void timerEvent(QTimerEvent *event);

private:
    GLuint LoadShader ( GLenum type, const char *shaderSrc );
    QPointF TrasformRelatedToLine(float angle, float xShift, float yShift, QPointF p);
    void drawLine(
        float x1, float y1, float x2, float y2,         // coordinates of the line
        float w,                                        // width/thickness of the line in pixel
        float Cr, float Cg, float Cb, float Ca,         // RGB color components
        float Br, float Bg, float Bb, float Ba          // color of background when alphablend=false
    );

private slots:
    void sltInputReaderReadEvent(QString type, int x, int y, QString inputDevice);
    void sltInputReaderFakeRelease(QString type, int x, int y, QString inputDevice);

private:
    GLuint m_matrixUniform;
    GLuint m_program;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;

    bool m_bMousePressed;
    QVector<QPointF> m_points;

    InputReader m_inputReader;
    QThread m_inputThread;
};

#endif // GLAPP_H_783478239491094782378423847237235782374878
