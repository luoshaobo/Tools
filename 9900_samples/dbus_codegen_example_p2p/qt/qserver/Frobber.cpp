#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Frobber.h"

Frobber::Frobber(QObject *parent /*= NULL*/) : QObject(parent), m_verbose(false)
{

}

Frobber::~Frobber()
{

}

bool Frobber::verbose() const
{
    return m_verbose;
}

void Frobber::setVerbose(bool value)
{
    m_verbose = value;
}

QString Frobber::HelloWorld(const QString &greeting)
{
    QString output;
    output.sprintf("Hello, %s!", (const char *)greeting.toLatin1().data());

    printf("%s\n", (const char *)output.toLatin1().data());

    QByteArray icon_blob;
    int height = 98;
    QStringList messages;
    icon_blob.append("(icon from qt)");
    messages.append("message 001 from qt");
    messages.append("message 002 from qt");
    messages.append("message 003 from qt");
    emit Notification(icon_blob, height, messages);

    return output;
}
