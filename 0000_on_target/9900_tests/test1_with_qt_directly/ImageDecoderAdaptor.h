#ifndef ImageDecoderAdaptor_H
#define ImageDecoderAdaptor_H

#include <QObject>
#include <QImage>

class ImageDecoderAdaptor : public QObject
{
    Q_OBJECT

public:
    explicit ImageDecoderAdaptor(QObject *parent = 0);
    ~ImageDecoderAdaptor();

    void setImageFilePath(const QString &imageFilePath) { m_imageFilePath = imageFilePath; }
    void setRatio(float widthRatio, float heightRatio) { m_widthRatio = widthRatio; m_heightRatio = heightRatio; }

    bool decode();

    int orignalWidth() const { return m_orignalWidth; }
    int orignalHeight() const { return m_orignalHeight; }
    int width() const { return m_width; }
    int height() const { return m_height; }
    unsigned char *pixalData() const { return m_pixalDataBuf; }

signals:

public slots:

private:
    QString m_imageFilePath;
    float m_widthRatio;
    float m_heightRatio;

    int m_orignalWidth;
    int m_orignalHeight;
    int m_width;
    int m_height;
    int m_pitch;

    static unsigned char m_pixalDataBuf[];
};

#endif // ImageDecoderAdaptor_H
