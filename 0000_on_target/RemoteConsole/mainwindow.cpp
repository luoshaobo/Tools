#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGlobal>
#include <QDebug>

namespace {
    const unsigned int PRESS_REPEATING_INTERVAL = 50;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_wServerAddr(NULL),
    m_wServerPort(NULL),
    m_repeatingType(RT_NONE),
    m_repeatingTimer(),
    m_socket(),
    m_serverAddr(),
    m_serverPort()
{
    ui->setupUi(this);

    m_wServerAddr = findChild<QLineEdit*>("ServerAddr");
    m_wServerPort = findChild<QLineEdit*>("ServerPort");

    m_serverAddr = m_wServerAddr->text();
    m_serverPort = m_wServerPort->text().toUShort();

    m_repeatingTimer.setInterval(PRESS_REPEATING_INTERVAL);
    connect(&m_repeatingTimer, SIGNAL(timeout()), this, SLOT(on_repeatingTimer_timeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ServerAddr_textChanged(const QString &text)
{
    m_serverAddr = text;
}

void MainWindow::on_ServerPort_textChanged(const QString &text)
{
    m_serverPort = text.toUShort();
}

void MainWindow::sendTextToServer(const std::string &text)
{
    qDebug(text.c_str());
    m_socket.writeDatagram(text.c_str(), text.length(), QHostAddress(m_serverAddr), m_serverPort);
}

void MainWindow::on_RotateC_pressed()
{
    m_repeatingType = RT_ROTATE_C;
    m_repeatingTimer.start();
}

void MainWindow::on_RotateC_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_NONE;
}

void MainWindow::on_RotateCC_pressed()
{
    m_repeatingType = RT_ROTATE_CC;
    m_repeatingTimer.start();
}

void MainWindow::on_RotateCC_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_NONE;
}

void MainWindow::on_M_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_MID_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_MID_SP");
    m_repeatingType = RT_M;
    m_repeatingTimer.start();
}

void MainWindow::on_M_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_M;
    sendTextToServer("PostHkEvent - HK_CONTROL_MID_HPR");
}

void MainWindow::on_U_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_UP_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_UP_SP");
    m_repeatingType = RT_U;
    m_repeatingTimer.start();
}

void MainWindow::on_U_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_U;
    sendTextToServer("PostHkEvent - HK_CONTROL_UP_HPR");
}

void MainWindow::on_D_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWN_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWN_SP");
    m_repeatingType = RT_D;
    m_repeatingTimer.start();
}

void MainWindow::on_D_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_D;
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWN_HPR");
}

void MainWindow::on_L_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_LEFT_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_LEFT_SP");
    m_repeatingType = RT_L;
    m_repeatingTimer.start();
}

void MainWindow::on_L_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_L;
    sendTextToServer("PostHkEvent - HK_CONTROL_LEFT_HPR");
}

void MainWindow::on_R_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_RIGHT_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_RIGHT_SP");
    m_repeatingType = RT_R;
    m_repeatingTimer.start();
}

void MainWindow::on_R_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_R;
    sendTextToServer("PostHkEvent - HK_CONTROL_RIGHT_HPR");
}

void MainWindow::on_UL_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_UPLEFT_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_UPLEFT_SP");
    m_repeatingType = RT_UL;
    m_repeatingTimer.start();
}

void MainWindow::on_UL_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_UL;
    sendTextToServer("PostHkEvent - HK_CONTROL_UPLEFT_HPR");
}

void MainWindow::on_DL_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWNLEFT_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWNLEFT_SP");
    m_repeatingType = RT_DL;
    m_repeatingTimer.start();
}

void MainWindow::on_DL_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_DL;
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWNLEFT_HPR");
}

void MainWindow::on_UR_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_UPRIGHT_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_UPRIGHT_SP");
    m_repeatingType = RT_UR;
    m_repeatingTimer.start();
}

void MainWindow::on_UR_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_UR;
    sendTextToServer("PostHkEvent - HK_CONTROL_UPRIGHT_HPR");
}

void MainWindow::on_DR_pressed()
{
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWNRIGHT_P");
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWNRIGHT_SP");
    m_repeatingType = RT_DR;
    m_repeatingTimer.start();
}

void MainWindow::on_DR_released()
{
    m_repeatingTimer.stop();
    m_repeatingType = RT_DR;
    sendTextToServer("PostHkEvent - HK_CONTROL_DOWNRIGHT_HPR");
}

void MainWindow::on_repeatingTimer_timeout()
{
    QString sRemoteCommand;

    switch (m_repeatingType) {
    case RT_NONE:
    default:
        // do nothing
        break;
    case RT_ROTATE_C:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_CLOCKWISE";
        break;
    case RT_ROTATE_CC:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_COUNTERCLOCKWISE";
        break;
    case RT_M:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_MID_HP";
        break;
    case RT_U:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_UP_HP";
        break;
    case RT_D:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_DOWN_HP";
        break;
    case RT_L:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_LEFT_HP";
        break;
    case RT_R:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_RIGHT_HP";
        break;
    case RT_UL:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_UPLEFT_HP";
        break;
    case RT_UR:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_UPRIGHT_HP";
        break;
    case RT_DL:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_DOWNLEFT_HP";
        break;
    case RT_DR:
        sRemoteCommand = "PostHkEvent - HK_CONTROL_DOWNRIGHT_HP";
        break;
    }

    if (!sRemoteCommand.isEmpty()) {
        sendTextToServer(sRemoteCommand.toStdString());
    }
}
