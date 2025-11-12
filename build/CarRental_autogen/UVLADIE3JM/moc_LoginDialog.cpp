

#include <memory>
#include "../../../src/LoginDialog.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LoginDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_LoginDialog_t {
    uint offsetsAndSizes[8];
    char stringdata0[12];
    char stringdata1[8];
    char stringdata2[1];
    char stringdata3[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LoginDialog_t::offsetsAndSizes) + ofs), len
Q_CONSTINIT static const qt_meta_stringdata_LoginDialog_t qt_meta_stringdata_LoginDialog = {
    {
        QT_MOC_LITERAL(0, 11),
        QT_MOC_LITERAL(12, 7),
        QT_MOC_LITERAL(20, 0),
        QT_MOC_LITERAL(21, 10)
    },
    "LoginDialog",
    "onLogin",
    "",
    "onRegister"
};
#undef QT_MOC_LITERAL
}

Q_CONSTINIT static const uint qt_meta_data_LoginDialog[] = {


      10,
       0,
       0,    0,
       2,   14,
       0,    0,
       0,    0,
       0,    0,
       0,
       0,


       1,    0,   26,    2, 0x08,    1 ,
       3,    0,   27,    2, 0x08,    2 ,


    QMetaType::Void,
    QMetaType::Void,

       0
};

Q_CONSTINIT const QMetaObject LoginDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_LoginDialog.offsetsAndSizes,
    qt_meta_data_LoginDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LoginDialog_t,

        QtPrivate::TypeAndForceComplete<LoginDialog, std::true_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LoginDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LoginDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onLogin(); break;
        case 1: _t->onRegister(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *LoginDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LoginDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LoginDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int LoginDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
