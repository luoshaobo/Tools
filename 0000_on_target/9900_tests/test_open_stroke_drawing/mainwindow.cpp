#include "mainwindow.h"
#include <math.h>

#define SCREEN_WIDTH      1280
#define SCREEN_HEIGHT     720

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute highp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying highp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

MainWindow::MainWindow() :
    QOpenGLWindow(),
    m_posAttr(0),
    m_colAttr(0),
    m_matrixUniform(0),
    m_program(NULL),
    m_bMousePressed(false),
    m_points()
{
    setTitle("open_gl_drawing");
    resize(SCREEN_WIDTH, SCREEN_HEIGHT);
    setFlags(Qt::FramelessWindowHint);

    startTimer(40);
}

MainWindow::~MainWindow()
{
    if (m_program != NULL) {
        m_program->release();
        delete m_program;
        m_program = NULL;
    }
}

void MainWindow::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.ortho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1000.0, 1000.0);
    matrix.translate(0, 0, 0);
    m_program->setUniformValue(m_matrixUniform, matrix);

    glViewport(0, 0, 1280, 720);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{

}

QPointF MainWindow::TrasformRelatedToLine(float angle, float xShift, float yShift, QPointF p)
{
    QMatrix4x4 matrix;
    matrix.translate(xShift, yShift);
    matrix.rotate(angle, 0, 0, 1.0);
    QPointF retP = matrix.map(p);
    return retP;
}

void MainWindow::drawLine(
    float x1, float y1, float x2, float y2,         // coordinates of the line
    float w,                                        // width/thickness of the line in pixel
    float Cr, float Cg, float Cb, float Ca,          // RGB color components
    float Br, float Bg, float Bb, float Ba         // color of background when alphablend=false
)
{
    {
        float t;
        if (y2 < y1) {
            t = x1;
            x1 = x2;
            x2 = t;

            t = y1;
            y1 = y2;
            y2 = t;
        }
    }

    float angle = 0;
    {
        float cosAngle = (x2 - x1) / sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        angle = acos(cosAngle) / 3.14159265 * 180;
        printf("angle: %f\n", angle);
    }

    {
        GLfloat vertices[6][2];
        GLfloat colors[6][4];

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
        colors[0][3] = Ba;

        colors[1][0] = Br;
        colors[1][1] = Bg;
        colors[1][2] = Bb;
        colors[1][3] = Ba;

        colors[2][0] = Cr;
        colors[2][1] = Cg;
        colors[2][2] = Cb;
        colors[2][3] = Ca;

        colors[3][0] = Cr;
        colors[3][1] = Cg;
        colors[3][2] = Cb;
        colors[3][3] = Ca;

        colors[4][0] = Br;
        colors[4][1] = Bg;
        colors[4][2] = Bb;
        colors[4][3] = Ba;

        colors[5][0] = Br;
        colors[5][1] = Bg;
        colors[5][2] = Bb;
        colors[5][3] = Ba;

        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(m_colAttr, 4, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    {
        GLfloat vertices[4][2];
        GLfloat colors[4][4];

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
        colors[0][3] = Ba;

        colors[1][0] = Br;
        colors[1][1] = Bg;
        colors[1][2] = Bb;
        colors[1][3] = Ba;

        colors[2][0] = Cr;
        colors[2][1] = Cg;
        colors[2][2] = Cb;
        colors[2][3] = Ca;

        colors[3][0] = Br;
        colors[3][1] = Bg;
        colors[3][2] = Bb;
        colors[3][3] = Ba;

        glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(m_colAttr, 4, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    // {
        // GLfloat vertices[4][2];
        // GLfloat colors[4][4];

        // QPointF p2(x1, y1);
        // QPointF p3(x2, y2);
        // QPointF p0 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, w/2));
        // QPointF p4 = TrasformRelatedToLine(angle, x1, y1, QPointF(0, -w/2));
        // float len = QVector2D(p2).distanceToPoint(QVector2D(p3));
        // QPointF p1 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, w/2));
        // QPointF p5 = TrasformRelatedToLine(angle, x1, y1, QPointF(len, -w/2));

        // QPointF pE2 = TrasformRelatedToLine(angle, x1, y1, QPointF(len + w/2, 0));

        // printf("len: %f\n", len);
        // printf("p0: (%f,%f)\n", p0.x(), p0.y());
        // printf("p1: (%f,%f)\n", p1.x(), p1.y());
        // printf("p2: (%f,%f)\n", p2.x(), p2.y());
        // printf("p3: (%f,%f)\n", p3.x(), p3.y());
        // printf("p4: (%f,%f)\n", p4.x(), p4.y());
        // printf("p5: (%f,%f)\n", p5.x(), p5.y());
        // printf("pE2: (%f,%f)\n", pE2.x(), pE2.y());

        // vertices[0][0] = p1.x();
        // vertices[0][1] = p1.y();

        // vertices[1][0] = p3.x();
        // vertices[1][1] = p3.y();

        // vertices[2][0] = pE2.x();
        // vertices[2][1] = pE2.y();

        // vertices[3][0] = p5.x();
        // vertices[3][1] = p5.y();

        // colors[0][0] = Br;
        // colors[0][1] = Bg;
        // colors[0][2] = Bb;
        // colors[0][3] = Ba;

        // colors[1][0] = Cr;
        // colors[1][1] = Cg;
        // colors[1][2] = Cb;
        // colors[1][3] = Ca;

        // colors[2][0] = Br;
        // colors[2][1] = Bg;
        // colors[2][2] = Bb;
        // colors[2][3] = Ba;

        // colors[3][0] = Br;
        // colors[3][1] = Bg;
        // colors[3][2] = Bb;
        // colors[3][3] = Ba;

        // glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        // glVertexAttribPointer(m_colAttr, 4, GL_FLOAT, GL_FALSE, 0, colors);

        // glEnableVertexAttribArray(0);
        // glEnableVertexAttribArray(1);

        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // glDisableVertexAttribArray(1);
        // glDisableVertexAttribArray(0);
    // }
}

void MainWindow::paintGL()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DITHER);
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    
   // drawLine(100, 100, 400, 400, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);
   // drawLine(10, 100, 400, 800, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);
   // drawLine(10, 10, 10, 400, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);
   // drawLine(10, 400, 300, 400, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);

//    drawLine(400, 400, 100, 100, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);

    //drawLine(100, 100, 800, 140, 4, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0);
    //drawLine(400, 10, 430, 600, 4, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0);
    
    //drawLine(100, 100, 800, 140, 4, 1.0, 1.0, 1.0, 1.0, 0.2, 0.2, 0.2, 0.0);
    //drawLine(600, 10, 600, 600, 2, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0);

    while (m_points.size() >= 2) {
        QPointF pointBegin = m_points[0];
        m_points.pop_front();
        QPointF pointEnd = m_points[0];
        drawLine(pointBegin.x(), SCREEN_HEIGHT - pointBegin.y(), pointEnd.x(), SCREEN_HEIGHT - pointEnd.y(), 4, 1.0, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0.5);
    };

    if (!m_bMousePressed) {
        m_points.clear();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMousePressed) {
        m_points.push_back(QPointF(event->x(), event->y()));
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_bMousePressed = true;
    m_points.push_back(QPointF(event->x(), event->y()));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMousePressed = false;
    m_points.push_back(QPointF(event->x(), event->y()));
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *ev)
{
    m_bMousePressed = false;
    m_points.clear();
    glClear(GL_COLOR_BUFFER_BIT);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    update();
}
