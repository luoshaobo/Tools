/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>

//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
    TriangleWindow();

    void initialize() Q_DECL_OVERRIDE;
    void render() Q_DECL_OVERRIDE;

private:
    GLuint loadShader(GLenum type, const char *source);

    void drawLine(
        float x1, float y1, float x2, float y2,         // coordinates of the line
        float w,                                        // width/thickness of the line in pixel
        float Cr, float Cg, float Cb,                   // RGB color components
        float Br, float Bg, float Bb                    // color of background when alphablend=false,
    );

private:
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;
    QOpenGLShaderProgram *m_program;
    int m_frame;
};

TriangleWindow::TriangleWindow()
    : m_program(0)
    , m_frame(0)
{
}
//! [1]

//! [2]
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    TriangleWindow window;
    window.setTitle("open_gl");
    window.setFormat(format);
    window.resize(1280, 720);
    window.setFlags(Qt::FramelessWindowHint);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
//! [2]


//! [3]
static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";
//! [3]

//! [4]
GLuint TriangleWindow::loadShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
    return shader;
}

void TriangleWindow::initialize()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");
}
//! [4]
//!
//!
//!
#include <QQuaternion>
#include <QMatrix4x4>
#include <QPointF>

//QPointF TrasformRelatedToLine(const QPointF &p1, const QPointF &p2, QPointF p)
//{
//    float cosAngle = (p2.x() - p1.x()) / sqrt((p2.x() - p1.x()) * (p2.x() - p1.x()) + (p2.y() - p1.y()) * (p2.y() - p1.y()));
//    float angle = acos(cosAngle) / 3.14159265 * 180;
//    printf("angle: %f\n", angle);

//    QMatrix4x4 matrix;
//    matrix.translate(p1.x(), p1.y());
//    matrix.rotate(angle, 0, 0, 1.0);

//    QPointF retP = matrix.map(p);
//    return retP;
//}

QPointF TrasformRelatedToLine(float angle, float xShift, float yShift, QPointF p)
{
    QMatrix4x4 matrix;
    matrix.translate(xShift, yShift);
    matrix.rotate(angle, 0, 0, 1.0);
    QPointF retP = matrix.map(p);
    return retP;
}

void TriangleWindow::drawLine(
    float x1, float y1, float x2, float y2,         // coordinates of the line
    float w,                                        // width/thickness of the line in pixel
    float Cr, float Cg, float Cb,                   // RGB color components
    float Br, float Bg, float Bb                    // color of background when alphablend=false,
)
{
    float angle = 0;
    {
        float cosAngle = (x2 - x1) / sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        angle = acos(cosAngle) / 3.14159265 * 180;
        printf("angle: %f\n", angle);
    }

    {
        GLfloat vertices[6][2];
        GLfloat colors[6][3];

        QPointF p2(x1, y1);
        QPointF p3(x2, y2);
        QPointF p0 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, w/2));
        QPointF p4 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, -w/2));
        float len = QVector2D(p2).distanceToPoint(QVector2D(p3));
        QPointF p1 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, w/2));
        QPointF p5 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, -w/2));

        printf("len: %f\n", len);
        printf("p0: (%f,%f)\n", p0.x(), p0.y());
        printf("p1: (%f,%f)\n", p1.x(), p1.y());
        printf("p2: (%f,%f)\n", p2.x(), p2.y());
        printf("p3: (%f,%f)\n", p3.x(), p3.y());
        printf("p4: (%f,%f)\n", p4.x(), p4.y());
        printf("p5: (%f,%f)\n", p5.x(), p5.y());

        vertices[0][0] = p0.x();
        vertices[0][1] = p0.y();

        vertices[1][0] = p1.x();
        vertices[1][1] = p1.y();

        vertices[2][0] = p2.x();
        vertices[2][1] = p2.y();

        vertices[3][0] = p3.x();
        vertices[3][1] = p3.y();

        vertices[4][0] = p4.x();
        vertices[4][1] = p4.y();

        vertices[5][0] = p5.x();
        vertices[5][1] = p5.y();

        colors[0][0] = Br;
        colors[0][1] = Bg;
        colors[0][2] = Bb;

        colors[1][0] = Br;
        colors[1][1] = Bg;
        colors[1][2] = Bb;

        colors[2][0] = Cr;
        colors[2][1] = Cg;
        colors[2][2] = Cb;

        colors[3][0] = Cr;
        colors[3][1] = Cg;
        colors[3][2] = Cb;

        colors[4][0] = Br;
        colors[4][1] = Bg;
        colors[4][2] = Bb;

        colors[5][0] = Br;
        colors[5][1] = Bg;
        colors[5][2] = Bb;

        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    {
        GLfloat vertices[4][2];
        GLfloat colors[4][3];

        QPointF p2(x1, y1);
        QPointF p3(x2, y2);
        QPointF p0 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, w/2));
        QPointF p4 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, -w/2));
        float len = QVector2D(p2).distanceToPoint(QVector2D(p3));
        QPointF p1 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, w/2));
        QPointF p5 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, -w/2));

        QPointF pE1 = TrasformRelatedToLine(angle, x1, y1, QPointF(-w/2, 0));

        printf("len: %f\n", len);
        printf("p0: (%f,%f)\n", p0.x(), p0.y());
        printf("p1: (%f,%f)\n", p1.x(), p1.y());
        printf("p2: (%f,%f)\n", p2.x(), p2.y());
        printf("p3: (%f,%f)\n", p3.x(), p3.y());
        printf("p4: (%f,%f)\n", p4.x(), p4.y());
        printf("p5: (%f,%f)\n", p5.x(), p5.y());
        printf("pE1: (%f,%f)\n", pE1.x(), pE1.y());

        vertices[0][0] = p0.x();
        vertices[0][1] = p0.y();

        vertices[1][0] = pE1.x();
        vertices[1][1] = pE1.y();

        vertices[2][0] = p2.x();
        vertices[2][1] = p2.y();

        vertices[3][0] = p4.x();
        vertices[3][1] = p4.y();

        colors[0][0] = Br;
        colors[0][1] = Bg;
        colors[0][2] = Bb;

        colors[1][0] = Br;
        colors[1][1] = Bg;
        colors[1][2] = Bb;

        colors[2][0] = Cr;
        colors[2][1] = Cg;
        colors[2][2] = Cb;

        colors[3][0] = Br;
        colors[3][1] = Bg;
        colors[3][2] = Bb;

        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    {
        GLfloat vertices[4][2];
        GLfloat colors[4][3];

        QPointF p2(x1, y1);
        QPointF p3(x2, y2);
        QPointF p0 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, w/2));
        QPointF p4 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, -w/2));
        float len = QVector2D(p2).distanceToPoint(QVector2D(p3));
        QPointF p1 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, w/2));
        QPointF p5 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, -w/2));

        QPointF pE2 = TrasformRelatedToLine(angle, x1, y1, QPointF(len + w/2, 0));

        printf("len: %f\n", len);
        printf("p0: (%f,%f)\n", p0.x(), p0.y());
        printf("p1: (%f,%f)\n", p1.x(), p1.y());
        printf("p2: (%f,%f)\n", p2.x(), p2.y());
        printf("p3: (%f,%f)\n", p3.x(), p3.y());
        printf("p4: (%f,%f)\n", p4.x(), p4.y());
        printf("p5: (%f,%f)\n", p5.x(), p5.y());
        printf("pE2: (%f,%f)\n", pE2.x(), pE2.y());

        vertices[0][0] = p1.x();
        vertices[0][1] = p1.y();

        vertices[1][0] = p3.x();
        vertices[1][1] = p3.y();

        vertices[2][0] = pE2.x();
        vertices[2][1] = pE2.y();

        vertices[3][0] = p5.x();
        vertices[3][1] = p5.y();

        colors[0][0] = Br;
        colors[0][1] = Bg;
        colors[0][2] = Bb;

        colors[1][0] = Cr;
        colors[1][1] = Cg;
        colors[1][2] = Cb;

        colors[2][0] = Br;
        colors[2][1] = Bg;
        colors[2][2] = Bb;

        colors[3][0] = Br;
        colors[3][1] = Bg;
        colors[3][2] = Bb;

        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }
}

//! [5]
void TriangleWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, 1280, 720);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_BLEND);
//    glEnable(GL_DITHER);
    //glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);

    m_program->bind();

    QMatrix4x4 matrix;
    //matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    //matrix.translate(0, 0, -2);
    //matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    matrix.ortho(0, 1280, 0, 720, -1000.0, 1000.0);
    matrix.translate(0, 0, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

    drawLine(100, 100, 400, 400, 2, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6);
    drawLine(10, 100, 400, 800, 2, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6);
    drawLine(10, 10, 10, 400, 2, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6);
    drawLine(10, 400, 300, 400, 2, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6);

    //drawLine(20, 20, 300, 500, 10, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);

    m_program->release();

    ++m_frame;
}
//! [5]
