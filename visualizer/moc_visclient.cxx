/****************************************************************************
** Meta object code from reading C++ file 'visclient.h'
**
** Created: Fri Mar 27 11:48:47 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "visclient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'visclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CVisClient[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x05,
      38,   11,   11,   11, 0x05,
      65,   57,   11,   11, 0x05,
      95,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     131,  128,  124,   11, 0x0a,
     153,  128,  124,   11, 0x0a,
     174,   11,  124,   11, 0x0a,
     201,  192,  124,   11, 0x0a,
     234,  222,   11,   11, 0x09,
     277,   11,   11,   11, 0x09,
     289,   11,   11,   11, 0x09,
     304,   11,   11,   11, 0x09,
     316,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CVisClient[] = {
    "CVisClient\0\0msg\0errorMessage(QString)\0"
    "newConfigMessage()\0dataMsg\0"
    "newRobotDataMessage(tDataMsg)\0"
    "newTaskDataMessage(tTaskMsg)\0int\0id\0"
    "requestRobotData(int)\0requestRobotMap(int)\0"
    "requestTaskData()\0id,pause\0"
    "pauseRobot(int,bool)\0socketError\0"
    "errorHandler(QAbstractSocket::SocketError)\0"
    "hostFound()\0disconnected()\0connected()\0"
    "dataRecieved()\0"
};

const QMetaObject CVisClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CVisClient,
      qt_meta_data_CVisClient, 0 }
};

const QMetaObject *CVisClient::metaObject() const
{
    return &staticMetaObject;
}

void *CVisClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CVisClient))
        return static_cast<void*>(const_cast< CVisClient*>(this));
    return QObject::qt_metacast(_clname);
}

int CVisClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: errorMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: newConfigMessage(); break;
        case 2: newRobotDataMessage((*reinterpret_cast< tDataMsg(*)>(_a[1]))); break;
        case 3: newTaskDataMessage((*reinterpret_cast< tTaskMsg(*)>(_a[1]))); break;
        case 4: { int _r = requestRobotData((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = requestRobotMap((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { int _r = requestTaskData();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: { int _r = pauseRobot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: errorHandler((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 9: hostFound(); break;
        case 10: disconnected(); break;
        case 11: connected(); break;
        case 12: dataRecieved(); break;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void CVisClient::errorMessage(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CVisClient::newConfigMessage()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CVisClient::newRobotDataMessage(tDataMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CVisClient::newTaskDataMessage(tTaskMsg _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
