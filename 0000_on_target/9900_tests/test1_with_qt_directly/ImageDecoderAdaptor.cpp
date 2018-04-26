#include "ImageDecoderAdaptor.h"
#include <QColor>

const int MAX_BITMAP_BUF_SIZE = 64 * 1024 * 1024 * 4;

unsigned char ImageDecoderAdaptor::m_pixalDataBuf[MAX_BITMAP_BUF_SIZE];

ImageDecoderAdaptor::ImageDecoderAdaptor(QObject *parent)
    : QObject(parent)
    , m_imageFilePath()
    , m_widthRatio(1.0f)
    , m_heightRatio(1.0f)
    , m_orignalWidth(0)
    , m_orignalHeight(0)
    , m_width(0)
    , m_height(0)
    , m_pitch(0)
{

}

ImageDecoderAdaptor::~ImageDecoderAdaptor()
{

}

#if 0
bool ImageDecoderAdaptor::decode() // by using a phony decoder
{
    bool ret = true;
    int i, j;
    QRgb *rgba;

    qDebug("%s: m_imageFilePath=%s", __PRETTY_FUNCTION__, m_imageFilePath.toUtf8().constData());

    m_orignalWidth = 200;
    m_orignalHeight = 100;

    if (ret) {
        m_width = m_orignalWidth * m_widthRatio;
        m_height = m_orignalHeight * m_heightRatio;
        m_pitch = m_width * 4;

        if (m_width == 0 || m_height == 0) {
            ret = false;
        }
    }

    if (ret) {
        for (i = 0; i < m_height; i++) {
            for (j = 0; j < m_width; j++) {
                rgba = (QRgb *)&m_pixalDataBuf[m_pitch * i + 4 * j];
                *rgba = qRgba(0, 255, 0, 255);
            }
        }
    }

    return ret;
}
#else
bool ImageDecoderAdaptor::decode() // by using QImage decoder
{
    bool ret = true;
    QImage *orignalImage = NULL;
    QImage scaledImage;

    qDebug("%s: m_imageFilePath=%s", __PRETTY_FUNCTION__, m_imageFilePath.toUtf8().constData());

    if (ret) {
        orignalImage = new QImage(m_imageFilePath);
        if (orignalImage == NULL) {
            ret = false;
        }
    }

    if (ret) {
        m_orignalWidth = orignalImage->width();
        m_orignalHeight = orignalImage->height();
        m_width = m_orignalWidth * m_widthRatio;
        m_height = m_orignalHeight * m_heightRatio;
        m_pitch = m_width * 4;

        if (m_width == 0 || m_height == 0) {
            ret = false;
        }
    }

    if (ret) {
        scaledImage = orignalImage->scaled(m_width, m_height);
        memcpy(m_pixalDataBuf, scaledImage.constBits(), m_width * 4 * m_height);
    }

    if (orignalImage != NULL) {
        delete orignalImage;
        orignalImage = NULL;
    }

    return ret;
}
#endif // #if 0
