/****************************************************************************
** Meta object code from reading C++ file 'BusinessLogic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "BusinessLogic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BusinessLogic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BusinessLogic_t {
    QByteArrayData data[8];
    char stringdata[121];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BusinessLogic_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BusinessLogic_t qt_meta_stringdata_BusinessLogic = {
    {
QT_MOC_LITERAL(0, 0, 13), // "BusinessLogic"
QT_MOC_LITERAL(1, 14, 30), // "sgnMouseAreaClickProcessedByBL"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 14), // "mouseEventData"
QT_MOC_LITERAL(4, 61, 15), // "MouseEventData*"
QT_MOC_LITERAL(5, 77, 18), // "onMouseAreaClicked"
QT_MOC_LITERAL(6, 96, 10), // "scrollText"
QT_MOC_LITERAL(7, 107, 13) // "scrollingText"

    },
    "BusinessLogic\0sgnMouseAreaClickProcessedByBL\0"
    "\0mouseEventData\0MouseEventData*\0"
    "onMouseAreaClicked\0scrollText\0"
    "scrollingText"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BusinessLogic[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       1,   46, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       3,    0,   40,    2, 0x02 /* Public */,
       5,    0,   41,    2, 0x02 /* Public */,
       5,    1,   42,    2, 0x02 /* Public */,
       6,    0,   45,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    0x80000000 | 4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    3,
    QMetaType::Void,

 // properties: name, type, flags
       7, QMetaType::QString, 0x00095001,

       0        // eod
};

void BusinessLogic::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BusinessLogic *_t = static_cast<BusinessLogic *>(_o);
        switch (_id) {
        case 0: _t->sgnMouseAreaClickProcessedByBL(); break;
        case 1: { MouseEventData* _r = _t->mouseEventData();
            if (_a[0]) *reinterpret_cast< MouseEventData**>(_a[0]) = _r; }  break;
        case 2: _t->onMouseAreaClicked(); break;
        case 3: _t->onMouseAreaClicked((*reinterpret_cast< MouseEventData*(*)>(_a[1]))); break;
        case 4: _t->scrollText(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< MouseEventData* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BusinessLogic::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BusinessLogic::sgnMouseAreaClickProcessedByBL)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject BusinessLogic::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BusinessLogic.data,
      qt_meta_data_BusinessLogic,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *BusinessLogic::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BusinessLogic::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_BusinessLogic.stringdata))
        return static_cast<void*>(const_cast< BusinessLogic*>(this));
    return QObject::qt_metacast(_clname);
}

int BusinessLogic::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = scrollingText(); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void BusinessLogic::sgnMouseAreaClickProcessedByBL()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
