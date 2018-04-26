#include "mywindow.h"
#include <QPainter>

MyWindow::MyWindow() : QWidget(NULL, Qt::FramelessWindowHint|Qt::Widget)
{
    setWindowTitle("NormalQtWindow");
    resize(1280, 720);
}

void MyWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(0, 0, 1280, 800, QColor("red"));
}
