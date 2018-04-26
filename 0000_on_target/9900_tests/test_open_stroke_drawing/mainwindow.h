#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QOpenGLWindow>
#include <QtGui/QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>

class MainWindow : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MainWindow();
    virtual ~MainWindow();

protected:
    virtual void initializeGL();
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintGL();

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *ev);
    virtual void timerEvent(QTimerEvent *event);

private:
    QPointF TrasformRelatedToLine(float angle, float xShift, float yShift, QPointF p);
    void drawLine(
        float x1, float y1, float x2, float y2,         // coordinates of the line
        float w,                                        // width/thickness of the line in pixel
        float Cr, float Cg, float Cb, float Ca,         // RGB color components
        float Br, float Bg, float Bb, float Ba          // color of background when alphablend=false
    );

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    QOpenGLShaderProgram *m_program;

    bool  m_bMousePressed;
    QVector<QPointF> m_points;
};

#endif // MAINWINDOW_H
