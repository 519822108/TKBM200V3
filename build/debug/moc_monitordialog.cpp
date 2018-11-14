/****************************************************************************
** Meta object code from reading C++ file 'monitordialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../monitordialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'monitordialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MonitorDialog_t {
    QByteArrayData data[9];
    char stringdata0[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MonitorDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MonitorDialog_t qt_meta_stringdata_MonitorDialog = {
    {
QT_MOC_LITERAL(0, 0, 13), // "MonitorDialog"
QT_MOC_LITERAL(1, 14, 17), // "sig_send_board_id"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 2), // "id"
QT_MOC_LITERAL(4, 36, 16), // "timer100_timeout"
QT_MOC_LITERAL(5, 53, 27), // "on_lw_fid_itemDoubleClicked"
QT_MOC_LITERAL(6, 81, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 98, 4), // "item"
QT_MOC_LITERAL(8, 103, 27) // "on_lw_bid_itemDoubleClicked"

    },
    "MonitorDialog\0sig_send_board_id\0\0id\0"
    "timer100_timeout\0on_lw_fid_itemDoubleClicked\0"
    "QListWidgetItem*\0item\0on_lw_bid_itemDoubleClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MonitorDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   37,    2, 0x0a /* Public */,
       5,    1,   38,    2, 0x08 /* Private */,
       8,    1,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void MonitorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MonitorDialog *_t = static_cast<MonitorDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sig_send_board_id((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->timer100_timeout(); break;
        case 2: _t->on_lw_fid_itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->on_lw_bid_itemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MonitorDialog::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MonitorDialog::sig_send_board_id)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MonitorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MonitorDialog.data,
      qt_meta_data_MonitorDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MonitorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MonitorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MonitorDialog.stringdata0))
        return static_cast<void*>(const_cast< MonitorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int MonitorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void MonitorDialog::sig_send_board_id(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
