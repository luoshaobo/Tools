/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -a NetCorpMyAppFrobberAdaptor ../net.Corp.MyApp.Frobber.xml
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#ifndef NETCORPMYAPPFROBBERADAPTOR_H
#define NETCORPMYAPPFROBBERADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface net.Corp.MyApp.Frobber
 */
class FrobberAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "net.Corp.MyApp.Frobber")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"net.Corp.MyApp.Frobber\">\n"
"    <method name=\"HelloWorld\">\n"
"      <arg direction=\"in\" type=\"s\" name=\"greeting\"/>\n"
"      <arg direction=\"out\" type=\"s\" name=\"response\"/>\n"
"    </method>\n"
"    <signal name=\"Notification\">\n"
"      <arg type=\"ay\" name=\"icon_blob\"/>\n"
"      <arg type=\"i\" name=\"height\"/>\n"
"      <arg type=\"as\" name=\"messages\"/>\n"
"    </signal>\n"
"    <property access=\"readwrite\" type=\"b\" name=\"Verbose\"/>\n"
"  </interface>\n"
        "")
public:
    FrobberAdaptor(QObject *parent);
    virtual ~FrobberAdaptor();

public: // PROPERTIES
    Q_PROPERTY(bool Verbose READ verbose WRITE setVerbose)
    bool verbose() const;
    void setVerbose(bool value);

public Q_SLOTS: // METHODS
    QString HelloWorld(const QString &greeting);
Q_SIGNALS: // SIGNALS
    void Notification(const QByteArray &icon_blob, int height, const QStringList &messages);
};

#endif
