/****************************************************************************
** Meta object code from reading C++ file 'metasqlsaveparameters.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../metasqlsaveparameters.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'metasqlsaveparameters.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MetaSQLSaveParameters[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   23,   22,   22, 0x0a,
      39,   23,   22,   22, 0x0a,
      57,   23,   22,   22, 0x0a,
      74,   23,   22,   22, 0x0a,
      92,   23,   22,   22, 0x0a,
     111,   22,   22,   22, 0x09,
     128,   22,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_MetaSQLSaveParameters[] = {
    "MetaSQLSaveParameters\0\0p\0setGrade(int)\0"
    "setGroup(QString)\0setName(QString)\0"
    "setNotes(QString)\0setSchema(QString)\0"
    "languageChange()\0sGetGroups()\0"
};

const QMetaObject MetaSQLSaveParameters::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MetaSQLSaveParameters,
      qt_meta_data_MetaSQLSaveParameters, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MetaSQLSaveParameters::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MetaSQLSaveParameters::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MetaSQLSaveParameters::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MetaSQLSaveParameters))
        return static_cast<void*>(const_cast< MetaSQLSaveParameters*>(this));
    if (!strcmp(_clname, "Ui::MetaSQLSaveParameters"))
        return static_cast< Ui::MetaSQLSaveParameters*>(const_cast< MetaSQLSaveParameters*>(this));
    return QDialog::qt_metacast(_clname);
}

int MetaSQLSaveParameters::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setGrade((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 1: setGroup((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: setName((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: setNotes((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: setSchema((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: languageChange(); break;
        case 6: sGetGroups(); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE