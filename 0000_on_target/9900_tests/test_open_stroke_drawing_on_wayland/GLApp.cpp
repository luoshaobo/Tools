#include "GLApp.h"

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

GLApp::GLApp() :
    m_matrixUniform(0),
    m_program(0),
    m_vertexShader(0),
    m_fragmentShader(0),
    m_bMousePressed(false),
    m_points(),
    m_inputReader("/dev/input/handwriting0"),
    m_inputThread()
{
    m_inputReader.setArea(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    m_inputReader.moveToThread(&m_inputThread);
    connect(&m_inputReader, SIGNAL(sgnReadEvent (QString, int, int, QString)),
            this, SLOT(sltInputReaderReadEvent(QString, int, int, QString)));
    connect(&m_inputReader, SIGNAL(sgnFakeRelease(QString, int, int, QString)),
            this, SLOT(sltInputReaderFakeRelease(QString, int, int, QString)));

    m_inputReader.enableReader();
    m_inputThread.start();

    startTimer(50);
}

GLApp::~GLApp()
{

}

void GLApp::sltInputReaderReadEvent(QString type, int x, int y, QString inputDevice)
{
    //printf("GLApp::sltInputReaderReadEvent()\n");
    if (type == "P") {
        QMouseEvent event(QEvent::MouseButtonPress, QPointF(x, y), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        mousePressEvent(&event);
    } else if (type == "R") {
        QMouseEvent event(QEvent::MouseButtonRelease, QPointF(x, y), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        mouseReleaseEvent(&event);
    } else if (type == "M") {
        QMouseEvent event(QEvent::MouseMove, QPointF(x, y), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        mouseMoveEvent(&event);
    }
}

void GLApp::sltInputReaderFakeRelease(QString type, int x, int y, QString inputDevice)
{
    //printf("GLApp::sltInputReaderFakeRelease()\n");
    QMouseEvent event(QEvent::MouseButtonRelease, QPointF(x, y), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    mouseReleaseEvent(&event);
}

GLuint GLApp::LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader = 0;
   GLint compiled = 0;

   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
    return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );

   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   return shader;
}

void GLApp::initializeGL()
{
    m_vertexShader = LoadShader ( GL_VERTEX_SHADER, vertexShaderSource );
    m_fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fragmentShaderSource );
    m_program = glCreateProgram();

    glAttachShader ( m_program, m_vertexShader );
    glAttachShader ( m_program, m_fragmentShader );

    glBindAttribLocation ( m_program, 0, "posAttr" );
    glBindAttribLocation ( m_program, 1, "colAttr" );

    glLinkProgram ( m_program );
    glUseProgram ( m_program );

    QMatrix4x4 matrix;
    matrix.ortho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1000.0, 1000.0);
    matrix.translate(0, 0, 0);
    glUniformMatrix4fv(0, 1, 0, matrix.constData());

    glViewport(0, 0, 1280, 720);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLApp::resizeEvent(QResizeEvent *event)
{

}

QPointF GLApp::TrasformRelatedToLine(float angle, float xShift, float yShift, QPointF p)
{
    QMatrix4x4 matrix;
    matrix.translate(xShift, yShift);
    matrix.rotate(angle, 0, 0, 1.0);
    QPointF retP = matrix.map(p);
    return retP;
}

void GLApp::drawLine(
    float x1, float y1, float x2, float y2,         // coordinates of the line
    float w,                                        // width/thickness of the line in pixel
    float Cr, float Cg, float Cb, float Ca,          // RGB color components
    float Br, float Bg, float Bb, float Ba         // color of background when alphablend=false
)
{
    //printf("GLApp::drawLine(): pre 1\n");
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

    //printf("GLApp::drawLine(): pre 2\n");
    float angle = 0;
    {
        float cosAngle = (x2 - x1) / sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        angle = acos(cosAngle) / 3.14159265 * 180;
        printf("angle: %f\n", angle);
    }

    //printf("GLApp::drawLine(): drawing 1\n");
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

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    printf("GLApp::drawLine(): drawing 2\n");
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

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, colors);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

    // NOTE: this part seems not necessary.
    //printf("GLApp::drawLine(): drawing 3\n");
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

        // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        // glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, colors);

        // glEnableVertexAttribArray(0);
        // glEnableVertexAttribArray(1);

        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // glDisableVertexAttribArray(1);
        // glDisableVertexAttribArray(0);
    // }
}

extern void swipBuffers();

namespace interpolation
{
    QVector<QPointF> doInterpolation(const QVector<QPointF> &inputPoints);
}

void GLApp::paintGL()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_DITHER);
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    
//    drawLine(100, 100, 400, 400, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);
//    drawLine(10, 100, 400, 800, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);
//    drawLine(10, 10, 10, 400, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);
//    drawLine(10, 400, 300, 400, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);

//    drawLine(400, 400, 100, 100, 2, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.6);

//    drawLine(100, 100, 800, 140, 4, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0);
//    drawLine(400, 10, 430, 600, 4, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0);

//    drawLine(100, 100, 800, 140, 4, 1.0, 1.0, 1.0, 1.0, 0.2, 0.2, 0.2, 0.0);
//    drawLine(600, 10, 600, 600, 2, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0);


    if (m_points.size() >= 2) {
//        QVector<QPointF> newPoints = interpolation::doInterpolation(m_points);
//        m_points = newPoints;
    }

    while (m_points.size() >= 2) {
        QPointF pointBegin = m_points[0];
        m_points.pop_front();
        QPointF pointEnd = m_points[0];
        printf("drawLine(%f,%f -> %f,%f)\n", pointBegin.x(), pointBegin.y(), pointEnd.x(), pointEnd.y());
        //drawLine(pointBegin.x(), SCREEN_HEIGHT - pointBegin.y(), pointEnd.x(), SCREEN_HEIGHT - pointEnd.y(), 4, 1.0, 1.0, 1.0, 1.0, 0.5, 0.5, 0.5, 0.5);
        drawLine(pointBegin.x(), SCREEN_HEIGHT - pointBegin.y(), pointEnd.x(), SCREEN_HEIGHT - pointEnd.y(), 4, 1.0, 1.0, 1.0, 1.0, 0.6, 0.6, 0.6, 0.5);
    };

    if (!m_bMousePressed) {
        m_points.clear();
    }

    swipBuffers();
}

void GLApp::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMousePressed) {
        m_points.push_back(QPointF(event->x(), event->y()));
    }
}

void GLApp::mousePressEvent(QMouseEvent *event)
{
    m_bMousePressed = true;
    m_points.push_back(QPointF(event->x(), event->y()));
}

void GLApp::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMousePressed = false;
    m_points.push_back(QPointF(event->x(), event->y()));
}

void GLApp::mouseDoubleClickEvent(QMouseEvent *ev)
{
    m_bMousePressed = false;
    m_points.clear();
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLApp::timerEvent(QTimerEvent *event)
{
    if (m_points.size() >= 2) {
        paintGL();
    }
}

namespace interpolation
{
using namespace std;

const int MAX = 500;
float x[MAX], y[MAX], h[MAX];
float c[MAX], a[MAX], fxym[MAX];

float f(int x1, int x2, int x3);
void cal_m(int n);
void printout(int n);
void interpolationMain(int n);

QVector<QPointF> outputPoints;

QVector<QPointF> doInterpolation(const QVector<QPointF> &inputPoints)
{
    int i;

    for (i = 0; i < inputPoints.count() && i < MAX; i++) {
        x[i] = inputPoints[i].x();
        y[i] = inputPoints[i].y();
    }
    outputPoints.clear();
    if (i > 1) {
        interpolationMain(i);
    }

    return outputPoints;
}

void interpolationMain(int n)
{
    int i;

    n=n-1;
    for(i=0;i<n;i++)  h[i]=x[i+1]-x[i];

    float f0, f1;
    f0 = 0;
    f1 = 0;

    c[0] = 1; a[n] = 1;
    fxym[0] = 6*((y[1] - y[0]) / (x[1] - x[0]) - f0) / h[0];
    fxym[n] = 6*(f1 - (y[n] - y[n-1]) / (x[n] - x[n-1])) / h[n-1];
    for(i=1;i<n;i++)	fxym[i] = 6*f(i-1,i,i+1);
    for(i=1;i<n;i++)
    {
        a[i] = h[i-1]/(h[i]+h[i-1]);
        c[i] = 1-a[i];
    }
    a[n] = h[n-1]/(h[n-1]+h[n]);
    cal_m(n);
    printout(n);
}

float f(int x1, int x2, int x3)
{
    float a = (y[x3]-y[x2])/(x[x3]-x[x2]);
    float b = (y[x2]-y[x1])/(x[x2]-x[x1]);
    return (a-b)/(x[x3]-x[x1]);
}


void cal_m(int n)
{
    int i;
    float B[MAX];
    B[0] = c[0]/2;
    for(int i=1;i<n;i++)   B[i] = c[i]/(2 - a[i]*B[i-1]);

    fxym[0] = fxym[0] / 2;
    for(i=1;i<=n;i++)      fxym[i] = (fxym[i]-a[i]*fxym[i-1])/(2-a[i]*B[i-1]);
    for(i=n-1;i>=0;i--)    fxym[i] = fxym[i]-B[i]*fxym[i+1];
}

const unsigned int THRESHOLD = 3;

void print_point(float a, float b, float c, float d, int x1, int x2)
{
    int x;
    float last_y = 0;
    float delta_y;

    for (x = x1; x <= x2; x++) {
        float y = a * x * x * x + b * x * x + c * x + d;
        if (x == x1) {
            last_y = y;
        }
        delta_y = y - last_y;
        if (delta_y >= THRESHOLD || delta_y <= -THRESHOLD) {
            outputPoints.push_back(QPointF(x, y));
            last_y = y;
        } else if (x == x1 || x == x2) {
            outputPoints.push_back(QPointF(x, y));
        }
    }
}

void printout(int n)
{
    for(int i = 0; i < n; i++)
    {
        float t1,t2,t3,t4;
        float a,b,c,d;

        t1 = fxym[i]/(6*h[i]);
        t2 = fxym[i+1]/(6*h[i]);
        t3 = (y[i] - fxym[i]*h[i]*h[i]/6)/h[i];
        t4 = (y[i+1] - fxym[i+1]*h[i]*h[i]/6)/h[i];

        a = -t1+t2;
        b = 3*(t1*x[i+1] - t2*x[i]);
        c = 3*(-t1*x[i+1]*x[i+1] + t2*x[i]*x[i]) - t3+t4;
        d = t1*pow(x[i+1],3) - t2*pow(x[i],3) + t3*x[i+1] - t4*x[i];

        print_point(a, b, c, d, x[i], x[i+1]);
    }
}

} //namespace interpolation
