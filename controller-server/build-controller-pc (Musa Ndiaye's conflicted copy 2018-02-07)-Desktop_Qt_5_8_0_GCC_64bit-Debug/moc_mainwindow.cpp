/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../controller-pc/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[25];
    char stringdata0[479];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 20), // "new_packet_available"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 24), // "on_connectButton_clicked"
QT_MOC_LITERAL(4, 58, 27), // "on_disconnectButton_clicked"
QT_MOC_LITERAL(5, 86, 12), // "decodePacket"
QT_MOC_LITERAL(6, 99, 6), // "packet"
QT_MOC_LITERAL(7, 106, 24), // "on_getNodeButton_clicked"
QT_MOC_LITERAL(8, 131, 20), // "processNodeIdMessage"
QT_MOC_LITERAL(9, 152, 10), // "sdnaddr_t*"
QT_MOC_LITERAL(10, 163, 6), // "nodeId"
QT_MOC_LITERAL(11, 170, 29), // "on_radioButton_serial_clicked"
QT_MOC_LITERAL(12, 200, 7), // "checked"
QT_MOC_LITERAL(13, 208, 34), // "on_ipAddress_cursorPositionCh..."
QT_MOC_LITERAL(14, 243, 4), // "arg1"
QT_MOC_LITERAL(15, 248, 4), // "arg2"
QT_MOC_LITERAL(16, 253, 37), // "on_actionSave_Statistics_As_t..."
QT_MOC_LITERAL(17, 291, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(18, 316, 28), // "on_pushButton_update_clicked"
QT_MOC_LITERAL(19, 345, 26), // "on_pushButton_load_clicked"
QT_MOC_LITERAL(20, 372, 21), // "on_clrbrowser_clicked"
QT_MOC_LITERAL(21, 394, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(22, 416, 23), // "on_pushButton_2_clicked"
QT_MOC_LITERAL(23, 440, 30), // "on_sample_rate_editingFinished"
QT_MOC_LITERAL(24, 471, 7) // "myclock"

    },
    "MainWindow\0new_packet_available\0\0"
    "on_connectButton_clicked\0"
    "on_disconnectButton_clicked\0decodePacket\0"
    "packet\0on_getNodeButton_clicked\0"
    "processNodeIdMessage\0sdnaddr_t*\0nodeId\0"
    "on_radioButton_serial_clicked\0checked\0"
    "on_ipAddress_cursorPositionChanged\0"
    "arg1\0arg2\0on_actionSave_Statistics_As_triggered\0"
    "on_actionAbout_triggered\0"
    "on_pushButton_update_clicked\0"
    "on_pushButton_load_clicked\0"
    "on_clrbrowser_clicked\0on_pushButton_clicked\0"
    "on_pushButton_2_clicked\0"
    "on_sample_rate_editingFinished\0myclock"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  100,    2, 0x08 /* Private */,
       4,    0,  101,    2, 0x08 /* Private */,
       5,    1,  102,    2, 0x08 /* Private */,
       7,    0,  105,    2, 0x08 /* Private */,
       8,    1,  106,    2, 0x08 /* Private */,
      11,    1,  109,    2, 0x08 /* Private */,
      13,    2,  112,    2, 0x08 /* Private */,
      16,    0,  117,    2, 0x08 /* Private */,
      17,    0,  118,    2, 0x08 /* Private */,
      18,    0,  119,    2, 0x08 /* Private */,
      19,    0,  120,    2, 0x08 /* Private */,
      20,    0,  121,    2, 0x08 /* Private */,
      21,    0,  122,    2, 0x08 /* Private */,
      22,    0,  123,    2, 0x08 /* Private */,
      23,    0,  124,    2, 0x08 /* Private */,
      24,    0,  125,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->new_packet_available(); break;
        case 1: _t->on_connectButton_clicked(); break;
        case 2: _t->on_disconnectButton_clicked(); break;
        case 3: _t->decodePacket((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 4: _t->on_getNodeButton_clicked(); break;
        case 5: _t->processNodeIdMessage((*reinterpret_cast< sdnaddr_t*(*)>(_a[1]))); break;
        case 6: _t->on_radioButton_serial_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_ipAddress_cursorPositionChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->on_actionSave_Statistics_As_triggered(); break;
        case 9: _t->on_actionAbout_triggered(); break;
        case 10: _t->on_pushButton_update_clicked(); break;
        case 11: _t->on_pushButton_load_clicked(); break;
        case 12: _t->on_clrbrowser_clicked(); break;
        case 13: _t->on_pushButton_clicked(); break;
        case 14: _t->on_pushButton_2_clicked(); break;
        case 15: _t->on_sample_rate_editingFinished(); break;
        case 16: _t->myclock(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::new_packet_available)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow.data,
      qt_meta_data_MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::new_packet_available()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
