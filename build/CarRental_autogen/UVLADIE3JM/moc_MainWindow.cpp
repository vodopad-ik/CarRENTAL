

#include <memory>
#include "../../../src/MainWindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MainWindow_t {
    uint offsetsAndSizes[22];
    char stringdata0[11];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[6];
    char stringdata4[18];
    char stringdata5[11];
    char stringdata6[16];
    char stringdata7[16];
    char stringdata8[16];
    char stringdata9[18];
    char stringdata10[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MainWindow_t::offsetsAndSizes) + ofs), len
Q_CONSTINIT static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
        QT_MOC_LITERAL(0, 10),
        QT_MOC_LITERAL(11, 16),
        QT_MOC_LITERAL(28, 0),
        QT_MOC_LITERAL(29, 5),
        QT_MOC_LITERAL(35, 17),
        QT_MOC_LITERAL(53, 10),
        QT_MOC_LITERAL(64, 15),
        QT_MOC_LITERAL(80, 15),
        QT_MOC_LITERAL(96, 15),
        QT_MOC_LITERAL(112, 17),
        QT_MOC_LITERAL(130, 8)
    },
    "MainWindow",
    "onCarRentClicked",
    "",
    "carId",
    "onBookmarkToggled",
    "bookmarked",
    "onShowMyRentals",
    "onShowBookmarks",
    "onSearchChanged",
    "onCurrencyChanged",
    "onLogout"
};
#undef QT_MOC_LITERAL
}

Q_CONSTINIT static const uint qt_meta_data_MainWindow[] = {


      10,
       0,
       0,    0,
       7,   14,
       0,    0,
       0,    0,
       0,    0,
       0,
       0,


       1,    1,   56,    2, 0x08,    1 ,
       4,    2,   59,    2, 0x08,    3 ,
       6,    0,   64,    2, 0x08,    6 ,
       7,    0,   65,    2, 0x08,    7 ,
       8,    0,   66,    2, 0x08,    8 ,
       9,    0,   67,    2, 0x08,    9 ,
      10,    0,   68,    2, 0x08,   10 ,


    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    3,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSizes,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t,

        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>,

        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onCarRentClicked((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->onBookmarkToggled((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 2: _t->onShowMyRentals(); break;
        case 3: _t->onShowBookmarks(); break;
        case 4: _t->onSearchChanged(); break;
        case 5: _t->onCurrencyChanged(); break;
        case 6: _t->onLogout(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
