#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QPaintDeviceWindow>
#include <QWidget>

class MyWindow : public QWidget
{
public:
    MyWindow();

private:
    virtual void paintEvent(QPaintEvent *event);
};

#endif // MYWINDOW_H
