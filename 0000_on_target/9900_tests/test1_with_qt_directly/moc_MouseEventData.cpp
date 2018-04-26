/****************************************************************************
** Meta object code from reading C++ file 'MouseEventData.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MouseEventData.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MouseEventData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MouseEventData_t {
    QByteArrayData data[8];
    char stringdata[61];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MouseEventData_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MouseEventData_t qt_meta_stringdata_MouseEventData = {
    {
QT_MOC_LITERAL(0, 0, 14), // "MouseEventData"
QT_MOC_LITERAL(1, 15, 8), // "accepted"
QT_MOC_LITERAL(2, 24, 6), // "button"
QT_MOC_LITERAL(3, 31, 7), // "buttons"
QT_MOC_LITERAL(4, 39, 9), // "modifiers"
QT_MOC_LITERAL(5, 49, 7), // "wasHeld"
QT_MOC_LITERAL(6, 57, 1), // "x"
QT_MOC_LITERAL(7, 59, 1) // "y"

    },
    "MouseEventData\0accepted\0button\0buttons\0"
    "modifiers\0wasHeld\0x\0y"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MouseEventData[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       7,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Bool, 0x00095003,
       2, QMetaType::Int, 0x00095003,
       3, QMetaType::Int, 0x00095003,
       4, QMetaType::Int, 0x00095003,
       5, QMetaType::Bool, 0x00095003,
       6, QMetaType::Int, 0x00095003,
       7, QMetaType::Int, 0x00095003,

       0        // eod
};

void MouseEventData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject MouseEventData::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MouseEventData.data,
      qt_meta_data_MouseEventData,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MouseEventData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MouseEventData::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MouseEventData.stringdata))
        return static_cast<void*>(const_cast< MouseEventData*>(this));
    return QObject::qt_metacast(_clname);
}

int MouseEventData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = m_accepted; break;
        case 1: *reinterpret_cast< int*>(_v) = m_button; break;
        case 2: *reinterpret_cast< int*>(_v) = m_buttons; break;
        case 3: *reinterpret_cast< int*>(_v) = m_modifiers; break;
        case 4: *reinterpret_cast< bool*>(_v) = m_wasHeld; break;
        case 5: *reinterpret_cast< int*>(_v) = m_x; break;
        case 6: *reinterpret_cast< int*>(_v) = m_y; break;
        default: break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (m_accepted != *reinterpret_cast< bool*>(_v)) {
                m_accepted = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 1:
            if (m_button != *reinterpret_cast< int*>(_v)) {
                m_button = *reinterpret_cast< int*>(_v);
            }
            break;
        case 2:
            if (m_buttons != *reinterpret_cast< int*>(_v)) {
                m_buttons = *reinterpret_cast< int*>(_v);
            }
            break;
        case 3:
            if (m_modifiers != *reinterpret_cast< int*>(_v)) {
                m_modifiers = *reinterpret_cast< int*>(_v);
            }
            break;
        case 4:
            if (m_wasHeld != *reinterpret_cast< bool*>(_v)) {
                m_wasHeld = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 5:
            if (m_x != *reinterpret_cast< int*>(_v)) {
                m_x = *reinterpret_cast< int*>(_v);
            }
            break;
        case 6:
            if (m_y != *reinterpret_cast< int*>(_v)) {
                m_y = *reinterpret_cast< int*>(_v);
            }
            break;
        default: break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
