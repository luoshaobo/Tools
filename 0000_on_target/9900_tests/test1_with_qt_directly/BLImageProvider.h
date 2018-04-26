#ifndef BLImageProvider_H
#define BLImageProvider_H

#include <QObject>
#include <QQuickItem>
#include <QQuickImageProvider>
#include "ImageDecoderAdaptor.h"

class BLImageProvider : public QQuickImageProvider
{
    //Q_OBJECT

public:
    BLImageProvider();
    ~BLImageProvider();

    virtual QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize);

private:
    static void myImageCleanupHandler(void *info);

private:
    ImageDecoderAdaptor *m_ImageDecoderAdaptor;
};

#endif // BLImageProvider_H
