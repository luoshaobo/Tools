#ifndef FROBBER_H
#define FROBBER_H

#include <QCoreApplication>
#include <QByteArray>
#include <QStringList>

class Frobber : public QObject
{
    Q_OBJECT

public:
    Frobber(QObject *parent = NULL);
    virtual ~Frobber();

public: // PROPERTIES
    Q_PROPERTY(bool Verbose READ verbose WRITE setVerbose)
    bool verbose() const;
    void setVerbose(bool value);

public slots: // METHODS
    QString HelloWorld(const QString &greeting);

signals: // SIGNALS
    void Notification(const QByteArray &icon_blob, int height, const QStringList &messages);

private:
    bool m_verbose;
};

#endif // #ifndef FROBBER_H
