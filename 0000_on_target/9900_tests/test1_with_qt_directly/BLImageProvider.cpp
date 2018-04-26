#include <QQmlEngine>
#include <QQuickTextureFactory>
#include "BLImageProvider.h"

BLImageProvider::BLImageProvider()
    : QQuickImageProvider(Image)
    , m_ImageDecoderAdaptor(NULL)
{

}

BLImageProvider::~BLImageProvider()
{
    // NOTE: do not destroy m_ImageDecoderAdaptor here!
}

void BLImageProvider::myImageCleanupHandler(void *info)
{
    qDebug("%s: ", __PRETTY_FUNCTION__);
    delete static_cast<ImageDecoderAdaptor *>(info);
}

QImage BLImageProvider::requestImage(const QString &id, QSize *size, const QSize& requestedSize)
{
    bool result = true;
    float widthRatio = 1.0f;
    float heightRatio = 1.0f;
    QString pictureFilePath;
    QImage *image = NULL;
    QImage returnImage;

    qDebug("%s: m_imageFilePath=%s", __PRETTY_FUNCTION__, id.toUtf8().constData());

    if (result) {
        QStringList sl = id.split("*");
        if (sl.length() == 3) {
            pictureFilePath = sl[0];
            widthRatio = sl[1].toFloat();
            heightRatio = sl[2].toFloat();
        } else {
            result = false;
        }
    }

    if (result) {
        m_ImageDecoderAdaptor = new ImageDecoderAdaptor;
        if (m_ImageDecoderAdaptor == NULL) {
            result = false;
        } else {
            m_ImageDecoderAdaptor->setImageFilePath(pictureFilePath);
            m_ImageDecoderAdaptor->setRatio(widthRatio, heightRatio);
            bool ret = m_ImageDecoderAdaptor->decode();
            if (!ret) {
                result = false;
            }
        }
    }

    if (result) {
        /*
        QImage::​QImage(const uchar * data, int width, int height, Format format, QImageCleanupFunction cleanupFunction = 0,
        void * cleanupInfo = 0)

        Constructs an image with the given width, height and format, that uses an existing read-only memory buffer, data.
        The width and height must be specified in pixels, data must be 32-bit aligned, and each scanline of data in the
        image must also be 32-bit aligned.

        The buffer must remain valid throughout the life of the QImage and all copies that have not been modified or otherwise
        detached from the original buffer. The image does not delete the buffer at destruction. You can provide a function
        pointer cleanupFunction along with an extra pointer cleanupInfo that will be called when the last copy is destroyed.

        If format is an indexed color format, the image color table is initially empty and must be sufficiently expanded with
        setColorCount() or setColorTable() before the image is used.

        Unlike the similar QImage constructor that takes a non-const data buffer, this version will never alter the contents
        of the buffer. For example, calling QImage::bits() will return a deep copy of the image, rather than the buffer passed
        to the constructor. This allows for the efficiency of constructing a QImage from raw data, without the possibility of
        the raw data being changed.
        */
        image = new QImage(                                                             // NOTE: no pixels copied by using this constructor.
                    const_cast<const uchar *>(m_ImageDecoderAdaptor->pixalData()),
                    m_ImageDecoderAdaptor->width(),
                    m_ImageDecoderAdaptor->height(),
                    QImage::Format_ARGB32,
                    myImageCleanupHandler,
                    static_cast<void *>(m_ImageDecoderAdaptor)
                );
        //QQmlEngine::setObjectOwnership(dynamic_cast<QObject *>(image), QQmlEngine::JavaScriptOwnership);
    }

    if (image != NULL) {
        returnImage = *image;
        delete image;
        image = NULL;
    }
    return returnImage;
}
