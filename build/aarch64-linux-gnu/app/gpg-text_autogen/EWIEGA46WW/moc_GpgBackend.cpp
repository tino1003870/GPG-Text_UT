/****************************************************************************
** Meta object code from reading C++ file 'GpgBackend.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../GpgBackend.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GpgBackend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GpgBackend_t {
    QByteArrayData data[14];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GpgBackend_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GpgBackend_t qt_meta_stringdata_GpgBackend = {
    {
QT_MOC_LITERAL(0, 0, 10), // "GpgBackend"
QT_MOC_LITERAL(1, 11, 7), // "testGpg"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 14), // "listPublicKeys"
QT_MOC_LITERAL(4, 35, 9), // "deleteKey"
QT_MOC_LITERAL(5, 45, 11), // "fingerprint"
QT_MOC_LITERAL(6, 57, 9), // "importKey"
QT_MOC_LITERAL(7, 67, 8), // "filePath"
QT_MOC_LITERAL(8, 76, 10), // "passphrase"
QT_MOC_LITERAL(9, 87, 12), // "platformName"
QT_MOC_LITERAL(10, 100, 16), // "chooseImportFile"
QT_MOC_LITERAL(11, 117, 11), // "encryptText"
QT_MOC_LITERAL(12, 129, 4), // "text"
QT_MOC_LITERAL(13, 134, 11) // "decryptText"

    },
    "GpgBackend\0testGpg\0\0listPublicKeys\0"
    "deleteKey\0fingerprint\0importKey\0"
    "filePath\0passphrase\0platformName\0"
    "chooseImportFile\0encryptText\0text\0"
    "decryptText"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GpgBackend[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x02 /* Public */,
       3,    0,   55,    2, 0x02 /* Public */,
       4,    1,   56,    2, 0x02 /* Public */,
       6,    2,   59,    2, 0x02 /* Public */,
       9,    0,   64,    2, 0x02 /* Public */,
      10,    0,   65,    2, 0x02 /* Public */,
      11,    2,   66,    2, 0x02 /* Public */,
      13,    1,   71,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::QString,
    QMetaType::QVariantList,
    QMetaType::QString, QMetaType::QString,    5,
    QMetaType::QString, QMetaType::QString, QMetaType::QString,    7,    8,
    QMetaType::QString,
    QMetaType::QString,
    QMetaType::QString, QMetaType::QString, QMetaType::QString,   12,    5,
    QMetaType::QString, QMetaType::QString,   12,

       0        // eod
};

void GpgBackend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GpgBackend *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { QString _r = _t->testGpg();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 1: { QVariantList _r = _t->listPublicKeys();
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 2: { QString _r = _t->deleteKey((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 3: { QString _r = _t->importKey((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 4: { QString _r = _t->platformName();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 5: { QString _r = _t->chooseImportFile();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 6: { QString _r = _t->encryptText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 7: { QString _r = _t->decryptText((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GpgBackend::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_GpgBackend.data,
    qt_meta_data_GpgBackend,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GpgBackend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GpgBackend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GpgBackend.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GpgBackend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
