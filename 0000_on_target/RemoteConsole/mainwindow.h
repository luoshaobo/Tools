#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTimer>
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    enum RepeatingType {
        RT_NONE,
        RT_ROTATE_C,
        RT_ROTATE_CC,
        RT_M,
        RT_U,
        RT_D,
        RT_L,
        RT_R,
        RT_UL,
        RT_UR,
        RT_DL,
        RT_DR,
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void sendTextToServer(const std::string &text);

private slots:
    void on_ServerAddr_textChanged(const QString &text);
    void on_ServerPort_textChanged(const QString &text);

    void on_RotateC_pressed();
    void on_RotateC_released();
    void on_RotateCC_pressed();
    void on_RotateCC_released();

    void on_M_pressed();
    void on_M_released();
    void on_U_pressed();
    void on_U_released();
    void on_D_pressed();
    void on_D_released();
    void on_L_pressed();
    void on_L_released();
    void on_R_pressed();
    void on_R_released();
    void on_UL_pressed();
    void on_UL_released();
    void on_DL_pressed();
    void on_DL_released();
    void on_UR_pressed();
    void on_UR_released();
    void on_DR_pressed();
    void on_DR_released();

    void on_repeatingTimer_timeout();

private:
    Ui::MainWindow *ui;

    QLineEdit *m_wServerAddr;
    QLineEdit *m_wServerPort;

    RepeatingType m_repeatingType;
    QTimer m_repeatingTimer;

    QUdpSocket m_socket;
    QString m_serverAddr;
    quint16 m_serverPort;
};

#endif // MAINWINDOW_H
