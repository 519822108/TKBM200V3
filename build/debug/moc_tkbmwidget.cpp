/****************************************************************************
** Meta object code from reading C++ file 'tkbmwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../tkbmwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tkbmwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TkbmWidget_t {
    QByteArrayData data[18];
    char stringdata0[212];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TkbmWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TkbmWidget_t qt_meta_stringdata_TkbmWidget = {
    {
QT_MOC_LITERAL(0, 0, 10), // "TkbmWidget"
QT_MOC_LITERAL(1, 11, 21), // "sig_set_cthread_state"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 7), // "CtlCan*"
QT_MOC_LITERAL(4, 42, 4), // "pCan"
QT_MOC_LITERAL(5, 47, 16), // "sig_process_exit"
QT_MOC_LITERAL(6, 64, 26), // "on_tb_eep_file_cellClicked"
QT_MOC_LITERAL(7, 91, 3), // "row"
QT_MOC_LITERAL(8, 95, 6), // "column"
QT_MOC_LITERAL(9, 102, 17), // "sig_get_can_param"
QT_MOC_LITERAL(10, 120, 3), // "dev"
QT_MOC_LITERAL(11, 124, 3), // "num"
QT_MOC_LITERAL(12, 128, 4), // "rate"
QT_MOC_LITERAL(13, 133, 4), // "port"
QT_MOC_LITERAL(14, 138, 12), // "comm_timeout"
QT_MOC_LITERAL(15, 151, 21), // "on_rb_exp_dis_clicked"
QT_MOC_LITERAL(16, 173, 18), // "update_msg_timeout"
QT_MOC_LITERAL(17, 192, 19) // "update_data_timeout"

    },
    "TkbmWidget\0sig_set_cthread_state\0\0"
    "CtlCan*\0pCan\0sig_process_exit\0"
    "on_tb_eep_file_cellClicked\0row\0column\0"
    "sig_get_can_param\0dev\0num\0rate\0port\0"
    "comm_timeout\0on_rb_exp_dis_clicked\0"
    "update_msg_timeout\0update_data_timeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TkbmWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    2,   58,    2, 0x08 /* Private */,
       9,    4,   63,    2, 0x08 /* Private */,
      14,    0,   72,    2, 0x08 /* Private */,
      15,    0,   73,    2, 0x08 /* Private */,
      16,    0,   74,    2, 0x08 /* Private */,
      17,    0,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    7,    8,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,   10,   11,   12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TkbmWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TkbmWidget *_t = static_cast<TkbmWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sig_set_cthread_state((*reinterpret_cast< CtlCan*(*)>(_a[1]))); break;
        case 1: _t->sig_process_exit(); break;
        case 2: _t->on_tb_eep_file_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->sig_get_can_param((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 4: _t->comm_timeout(); break;
        case 5: _t->on_rb_exp_dis_clicked(); break;
        case 6: _t->update_msg_timeout(); break;
        case 7: _t->update_data_timeout(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TkbmWidget::*_t)(CtlCan * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TkbmWidget::sig_set_cthread_state)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (TkbmWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TkbmWidget::sig_process_exit)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject TkbmWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TkbmWidget.data,
      qt_meta_data_TkbmWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TkbmWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TkbmWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TkbmWidget.stringdata0))
        return static_cast<void*>(const_cast< TkbmWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int TkbmWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void TkbmWidget::sig_set_cthread_state(CtlCan * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TkbmWidget::sig_process_exit()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
struct qt_meta_stringdata_RecvCan_t {
    QByteArrayData data[5];
    char stringdata0[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RecvCan_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RecvCan_t qt_meta_stringdata_RecvCan = {
    {
QT_MOC_LITERAL(0, 0, 7), // "RecvCan"
QT_MOC_LITERAL(1, 8, 14), // "slot_get_state"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 7), // "CtlCan*"
QT_MOC_LITERAL(4, 32, 4) // "pCan"

    },
    "RecvCan\0slot_get_state\0\0CtlCan*\0pCan"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RecvCan[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void RecvCan::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RecvCan *_t = static_cast<RecvCan *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slot_get_state((*reinterpret_cast< CtlCan*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject RecvCan::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_RecvCan.data,
      qt_meta_data_RecvCan,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *RecvCan::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RecvCan::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_RecvCan.stringdata0))
        return static_cast<void*>(const_cast< RecvCan*>(this));
    return QThread::qt_metacast(_clname);
}

int RecvCan::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
