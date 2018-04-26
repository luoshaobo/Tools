#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_bMousePressed(false),
    m_points()
{
    ui->setupUi(this);

    setWindowTitle("drawing");
    resize(1280, 720);
    setAttribute(Qt::WA_NoSystemBackground);

    startTimer(20);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bMousePressed) {
        m_points.push_back(QPoint(event->x(), event->y()));
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_bMousePressed = true;
    m_points.push_back(QPoint(event->x(), event->y()));
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_bMousePressed = false;
    m_points.push_back(QPoint(event->x(), event->y()));
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPen pen(Qt::blue);
    QPainter painter(this);

    {
        static bool bFirstTime = true;

        if (bFirstTime) {
            bFirstTime = false;
            QColor bgColor(255, 255, 255, 128);
            painter.fillRect(0, 0, 1280, 720, bgColor);
        }

    }

    pen.setWidth(5);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPolyline(m_points.data(), m_points.count());

    if (!m_bMousePressed) {
        m_points.clear();
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    repaint();
}
