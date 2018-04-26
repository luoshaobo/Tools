#include <main.h>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QStaticText>
#include "log.h"

#define SCREEN_WIDTH                1280
#define SCREEN_HEIGHT               720

struct Rect {
    int x, y;
    unsigned int w, h;
};

extern int simple_touch_main();
extern unsigned char *get_surface_data_buffer();
extern void surface_update_display(int x, int y, unsigned int width, unsigned int height);
extern void display_dispatch_input_events();

AnimationScreen g_AnimationScreen;
QImage *g_pScreenImage = NULL;
QPainter *g_pScreenPainter = NULL;
unsigned char g_yuvBuf[SCREEN_WIDTH * 2 * SCREEN_HEIGHT];
QImage *g_pImageBusyMark = NULL;
QImage *g_pImageBusyMarkRotated[10] = {0};
QImage *g_pImageBg = NULL;

void MyThread::run()
{
    while(1) {
        display_dispatch_input_events();
    }
}

void AnimationScreen::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId) {
        static bool bFirstTime = true;
        static unsigned int index = 0;
        
        if (g_pScreenImage == NULL) {
            LOG_GEN();
            g_pScreenImage = new QImage(get_surface_data_buffer(), 1280, 720, QImage::Format_ARGB32);
        }
        if (g_pScreenPainter == NULL) {
            LOG_GEN();
            g_pScreenPainter = new QPainter(g_pScreenImage);
        }
        if (g_pImageBusyMark == NULL) {
            LOG_GEN();
            g_pImageBusyMark = new QImage("/media/datastore/ui/resources/DYC/assets/images/ambiance_1/ppp_common/icon_busymark.png");
            for (unsigned int i = 0; i < 10; i++) {
                QTransform tr = QTransform().rotate(i * 36);
                g_pImageBusyMarkRotated[i] = new QImage();
                *g_pImageBusyMarkRotated[i] = g_pImageBusyMark->transformed(tr);
            }
        }
        if (g_pImageBg == NULL) {
            LOG_GEN();
            g_pImageBg = new QImage("/home/root/sbin/xihu_1280x720.jpg");
        }

        Rect rect;
        rect.x = 100;
        rect.y = 100;
        rect.w = 300; //SCREEN_WIDTH;
        rect.h = 300; //SCREEN_HEIGHT;

        if (g_pScreenPainter != NULL && g_pScreenImage != NULL) {
            g_pScreenPainter->setCompositionMode(QPainter::CompositionMode_Source);
            
            if (bFirstTime) {
                bFirstTime = false;
                g_pScreenPainter->drawImage(0, 0, *g_pImageBg);
                surface_update_display(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            }
        
            //QColor color((index % 256), 0, 0, 255);
            //g_pScreenPainter->fillRect(rect.x, rect.y, rect.w, rect.h, color);
            
            // QPen pen = g_pScreenPainter->pen();
            // pen.setColor(Qt::white);
            // QFont font = g_pScreenPainter->font();
            // font.setFamily("Eurostile-CH_forGW");
            // font.setBold(true);
            // font.setPixelSize(50);
            // g_pScreenPainter->setPen(pen);
            // g_pScreenPainter->setFont(font);

            // QString sText = QString::asprintf("%d", index++);
            // g_pScreenPainter->drawLine(0, 0, 300, 300);
            // g_pScreenPainter->drawText(0, 50, sText);
            
            unsigned int bmw = g_pImageBusyMarkRotated[index % 10]->width();
            unsigned int bmh = g_pImageBusyMarkRotated[index % 10]->height();
            g_pScreenPainter->drawImage(200 - bmw/2, 200 - bmh/2, *g_pImageBg, 200 - bmw/2, 200 - bmh/2, bmw + 100, bmh + 100);
            g_pScreenPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            g_pScreenPainter->drawImage(200 - bmw/2, 200 - bmh/2, *g_pImageBusyMarkRotated[index % 10]);

            surface_update_display(rect.x, rect.y, rect.w, rect.h);
            
            index++;
        }
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    
    simple_touch_main();

    AnimationScreen as;
    as.startShowing();
    
    MyThread mt;
    mt.start();

    return a.exec();
}
