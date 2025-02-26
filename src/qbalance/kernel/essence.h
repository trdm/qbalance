/************************************************************************************************************
Copyright (C) Morozov Vladimir Aleksandrovich
MorozovVladimir@mail.ru

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*************************************************************************************************************/

#ifndef ESSENCE_H
#define ESSENCE_H

#define TMP_DIR         "./tmp/"

#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSql/QSqlTableModel>
#include <QtCore/QProcess>
#include <QtCore/QPointer>
#include <qmath.h>

#include <QtXml/QDomNode>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtGui/QDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptValue>
#include "../storage/filetype.h"
#include "../engine/reportcontext.h"
#include "table.h"

class TApplication;
class Dialog;
class Form;
class FormGrid;
class TableView;
class ScriptEngine;
class DocumentScriptEngine;
class Dictionaries;

struct urlId {
    QString     id;                 // скачать файл с таким ID
    QString     copyTo;             // скопировать его в файл
};


class Essence : public Table {
    Q_OBJECT

private:
    bool                photoPathVerified;
    QString             photoPath;
    QString             photoIdField;
    QString             photoNameField;
    bool                loading;            // Сейчас идет заполнение документа (справочника) из файла

    QHash<QString, urlId>  urls;                               // URL картинок в интернете и их локальные идентификаторы
    QNetworkAccessManager*  m_networkAccessManager;


private slots:
    void                replyFinished(QNetworkReply*);

protected:
    bool                scriptEngineEnabled;
    QString             formTitle;
    QString             idFieldName;
    QString             nameFieldName;
    FormGrid*           form;
    Dictionaries*       dictionaries;
    TableView*          grdTable;
    QWidget*            parentForm;
    ScriptEngine*       scriptEngine;
    QString             scriptFileName;                     // Имя файла со скриптами
    QSqlQuery           preparedSelectCurrentRow;           // Содержит подготовленный запрос для обновления текущей строки при вычислениях
    QString             defaultFilter;
    bool                filterEnabled;
    bool                lInsertable;
    bool                lDeleteable;
    bool                lViewable;
    bool                lUpdateable;
    bool                lPrintable;
    bool                isDictionary;
    bool                enabled;
    bool                doSubmit;
    bool                photoEnabled;
    QHash<QString, QVariant>             oldValues;              // Старые значения для текущей строки
    DocumentScriptEngine* reportScriptEngine;
    bool                lIsDocument;
    bool                sortedTable;
    bool                cardReaderEnabled;
    QHash<QString, QVariant> printValues;
    bool                menuMode;           // Форма справочника принимает форму меню
    bool                isView;
    bool                modified;

    virtual void        preparePrintValues();                   // Готовит значения для печати
    virtual void        prepareSelectCurrentRowCommand();
    virtual void        openScriptEngine();
    virtual void        closeScriptEngine();
    virtual void        openReportScriptEngine();
    virtual void        closeReportScriptEngine();
    bool                isDocumentLoading();

    Essence(QString = "", QObject* = 0 /*nullptr*/);
    virtual void postInitialize(QString, QObject*);

public:
    ~Essence();

    Q_INVOKABLE virtual bool            open(QString = "");
    Q_INVOKABLE virtual void            close();
    void                                initForm();
    Q_INVOKABLE virtual bool            add() { return true; }                      // Добавление записи
    Q_INVOKABLE virtual bool            remove(bool = false);     // Удаление записи
    Q_INVOKABLE virtual void            view();                         // Просмотр записи
    Q_INVOKABLE virtual void            print(QString, bool = false, bool = false, int = 1, QString = "", int = -1);  // Печать
    Q_INVOKABLE virtual void            printLabel(QString, int = 1);                 // Печать этикеток
    Q_INVOKABLE virtual void            printLabel1(QString, int = -1);                 // Печать этикеток
    virtual void                        load();

// Функции для получения, сохранения данных модели
    Q_INVOKABLE virtual int             getId(int row = -1);
    Q_INVOKABLE virtual QString         getName(int row = -1);
    Q_INVOKABLE virtual bool            setId(int);
    Q_INVOKABLE virtual int             locateId(int);            // Возвращает номер строки с заданным полем КОД
    Q_INVOKABLE virtual int             locateValue(QString, QVariant);  // Возвращает номер строки с заданным значением поля
    Q_INVOKABLE virtual bool            isFieldExists(QString field);
    Q_INVOKABLE virtual QVariant        getValue(QString, int row = -1);                 // Возвращает значение заданного поля в текущей записи
    Q_INVOKABLE virtual QVariant        getOldValue(QString field);
    Q_INVOKABLE virtual QVariant        getOldValue();
    Q_INVOKABLE virtual void            setValue(QString, QVariant, int row = -1);           // Устанавливает значение заданного поля в текущей записи
    Q_INVOKABLE QVariant                getSumValue(QString name);
    Q_INVOKABLE void                    setDoSubmit(bool submit);
    Q_INVOKABLE void                    setFilterEnabled(bool);
    Q_INVOKABLE void                    setFilter(const QString &filter);
    Q_INVOKABLE virtual void query(QString = "", bool = false);
    Q_INVOKABLE virtual void            setOrderClause(QString = "") { ; }
    Q_INVOKABLE int                     getRowCount();
    Q_INVOKABLE int                     rowCount();
    Q_INVOKABLE virtual bool            saveChanges();
    Q_INVOKABLE void                    setReadOnly(bool);


// Функции для работы с модулем GUI
    Q_INVOKABLE virtual FormGrid* getForm();
    Q_INVOKABLE virtual void cmdOk();                       // Обработка нажатий кнопок "Ok"
    Q_INVOKABLE virtual void cmdCancel();                   // и "Cancel"
    Q_INVOKABLE virtual bool isFormSelected();
    Q_INVOKABLE virtual int exec();                 // Показать форму в модальном режиме
    Q_INVOKABLE virtual void show();                // Показать форму в немодальном режиме
    Q_INVOKABLE virtual void hide();                // Скрыть форму
    Q_INVOKABLE void setFormTitle(QString);         // Установить заголовок формы
    Q_INVOKABLE QString getFormTitle();             // прочитать заголовок формы
    Q_INVOKABLE virtual Dialog* getFormWidget();
    Q_INVOKABLE virtual void setPhotoEnabled(bool enabled);
    Q_INVOKABLE virtual bool isPhotoEnabled();
    Q_INVOKABLE virtual void setPhotoPath(QString path);
    Q_INVOKABLE virtual QString getPhotoPath();
    Q_INVOKABLE virtual void setPhotoIdField(QString field);
    Q_INVOKABLE virtual void setPhotoNameField(QString field);
    Q_INVOKABLE virtual QString getPhotoNameField();
    Q_INVOKABLE virtual void removePhoto(QString = "");
    Q_INVOKABLE bool isInsertable();
    Q_INVOKABLE bool isDeleteable();
    Q_INVOKABLE bool isViewable();
    Q_INVOKABLE bool isUpdateable();
    Q_INVOKABLE bool isPrintable();
    Q_INVOKABLE void setInsertable(bool b);
    Q_INVOKABLE void setDeleteable(bool b);
    Q_INVOKABLE void setViewable(bool b);
    Q_INVOKABLE void setUpdateable(bool b);
    Q_INVOKABLE void setPrintable(bool b);
    Q_INVOKABLE virtual void setEnabled(bool);
    Q_INVOKABLE virtual bool isEnabled();
    Q_INVOKABLE void hideAllGridSections();
    Q_INVOKABLE void hideGridSection(QString columnName);
    Q_INVOKABLE void showGridSection(QString columnName);
    Q_INVOKABLE void showAllGridSections();
    Q_INVOKABLE virtual void setForm(QString = "") { ; }
    Q_INVOKABLE bool isDefaultForm();
    Q_INVOKABLE bool isFormVisible();
    Q_INVOKABLE bool getIsDictionary();
    Q_INVOKABLE virtual bool isVisible();
    Q_INVOKABLE TableView* getGrdTable();
    Q_INVOKABLE void setGrdTable(TableView* gt);
    Q_INVOKABLE virtual void    setMenuMode(bool);
    Q_INVOKABLE virtual bool    isMenuMode();
    Q_INVOKABLE virtual bool isModified();
    Q_INVOKABLE virtual void setModified(bool);


// Функции для обеспечения работы скриптов
    Q_INVOKABLE virtual void openScripts();
    virtual void        setScriptEngine();
    ScriptEngine*       getScriptEngine();
    DocumentScriptEngine*       getReportScriptEngine();
    Q_INVOKABLE ReportContext* getReportContext();
    void                setScriptEngineEnabled(bool enabled);
    virtual void        evaluateEngine();
    virtual bool        calculate(bool = true);
    virtual void        saveOldValues();                // Сохраняет значения полей текущей строки перед вычислениями
    virtual void        submit();
    Q_INVOKABLE virtual void restoreOldValues();
    Q_INVOKABLE virtual QScriptValue evaluateScript(QString);


// Скриптовые события
    void                initFormEvent();
    void                beforeShowFormEvent(Form *);
    void                afterShowFormEvent(Form *);
    bool                beforeHideFormEvent(Form *);
    void                afterHideFormEvent(Form *);
    void                closeFormEvent(Form *);
    QString             preparePictureUrl();
    QString             prepareBarCodeData();
    virtual void        afterRowChanged();
    virtual void        beforeRowChanged();

// Прочие функции
    Q_INVOKABLE         virtual QString getPhotoFile(QString copyTo = "");
    Q_INVOKABLE         virtual QString getLocalPhotoFile(QString = "");
    static bool         getFile(QString, QString, FileType);
    virtual bool        barCodeReaded(QString);    // прочитана строка со сканера штрих-кода
    virtual void        updateCurrentRow(int = 0);
    virtual QModelIndex getCurrentIndex();
    void setCurrentIndex(QModelIndex);
    Q_INVOKABLE QString         getCurrentFieldName();
    Q_INVOKABLE virtual void    setCurrentFieldName(QString);
    Q_INVOKABLE virtual int     getCurrentRow();
    Q_INVOKABLE virtual int     getCurrentColumn();
    Q_INVOKABLE virtual void    setCurrentRow(int);
    Q_INVOKABLE virtual void    setCurrentColumn(int);
    virtual Dictionaries* getDictionaries();
    void setDictionaries(Dictionaries* dicts);
    bool    isLoading();
    Q_INVOKABLE void    clearPrintValues();
    Q_INVOKABLE void    appendPrintValue(QString name, QVariant value);
    Q_INVOKABLE void    appendPrintValues(QString str, QSqlQuery* query);
    Q_INVOKABLE QVariant getPrintValue(QString name);
    bool                isDocument();
    Q_INVOKABLE void    showPhoto();

    Q_INVOKABLE void    setCardReaderEnabled(bool);
    Q_INVOKABLE void    exportData(QString = "", QStringList = QStringList());

signals:
    void                photoLoaded();
    void                showError(QString);

public slots:
    void                cardCodeReaded(QString);


};

#endif // ESSENCE_H
