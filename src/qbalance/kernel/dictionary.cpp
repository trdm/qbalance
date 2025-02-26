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

#include <QtCore/QDebug>
#include "dictionary.h"
#include "dictionaries.h"
#include "../kernel/app.h"
#include "../kernel/document.h"
#include "../kernel/essence.h"
#include "../gui/mainwindow.h"
#include "../gui/formdocument.h"
#include "../gui/formgridsearch.h"
#include "../gui/searchparameters.h"
#include "../gui/docparameters.h"
#include "../gui/picture.h"
#include "../gui/tableview.h"
#include "../gui/dialog.h"
#include "../storage/mysqlrelationaltablemodel.h"
#include "../storage/dbfactory.h"
#include "../engine/scriptengine.h"
#include "../engine/documentscriptengine.h"
#include "../engine/reportcontext.h"


Dictionary::Dictionary(QString name, QObject *parent): Essence(name, parent)
{
    parentDict = 0 /*nullptr*/;
    parameters = 0 /*nullptr*/;
    filter = "";
}


Dictionary::Dictionary(QObject *parent)
{
    Dictionary("", parent);
}


Dictionary::~Dictionary()
{
    if (tableModel != 0 /*nullptr*/)
    {
        delete tableModel;
        tableModel = 0 /*nullptr*/;
    }
}


void Dictionary::postInitialize(QString name, QObject *parent)
{
    Essence::postInitialize(name, parent);

    prototypeName = name;
    formTitle = "";
    lPrintable = true;
    lCanShow = false;
    lMustShow = false;
    lIsConst = false;
    lAutoSelect = false;
    isDepend = false;
    ftsEnabled = false;
    lIsSaldo = false;
    lIsAutoLoaded = false;
    lsetIdEnabled = true;
    parentDict = 0 /*nullptr*/;
    locked = false;
    getIdRefresh = true;
    parameters = 0 /*nullptr*/;
    doSubmit = true;
    exact = true;
    lNameExist = false;
    filterEnabled = true;
    lIsSet = db->isSet(tableName);

    QSqlRecord tableProperties = db->getDictionariesProperties(tableName);
    if (!tableProperties.isEmpty())
    {
        formTitle = tableProperties.value(db->getObjectName("доступ_к_справочникам.имя_в_форме")).toString().trimmed();
        if (app->isSA())
        {
            lInsertable = true;
            lDeleteable = true;
            lUpdateable = true;
        }
        else
        {
            lInsertable = tableProperties.value("insertable").toBool();
            lDeleteable = tableProperties.value("deleteable").toBool();
            lUpdateable = !tableProperties.value("ТОЛЬКОЧТЕНИЕ").toBool();
        }
    }
    if (isView)
    {
        lInsertable = false;
        lDeleteable = false;
        lUpdateable = false;
    }
}


bool Dictionary::open(QString command, QString tName)
{
    if (tName == "undefined")
        tName = "";

    sqlCommand = command;

    if (tName.size() > 0)
        queryTableName = tName;

    if (tagName.size() == 0)
        tagName = tName;

    if (dictTitle.size() == 0)
        dictTitle = tagName;

    if (Essence::open())
    {
        if (tableName.size() > 0)
        {
            // Проверим, имеется ли в справочнике полнотекстовый поиск
            foreach (QString fieldName, fieldList)
            {
                if (fieldName == "fts")
                {
                    ftsEnabled = true;
                }
                if (fieldName.toUpper() == "ИМЯ")
                {
                    lNameExist = true;
                }
            }

            setOrderClause();

            prepareSelectCurrentRowCommand();

            tableModel->setTestSelect(true);
            query();
            tableModel->setTestSelect(false);

            if (isFieldExists(nameFieldName))
                setPhotoNameField(nameFieldName);

            FieldType fld;
            int keyColumn   = 0;
            for (int i = 0; i < columnsProperties.count(); i++)
            {
                fld = columnsProperties.at(i);

                // Для основной таблицы сохраним информацию для обновления

                if (fld.table == columnsProperties.at(0).table)
                {
                    if (fld.name == idFieldName)
                        keyColumn = i;
                    else if (fieldList.contains(fld.name))
                        tableModel->setUpdateInfo(fld.name, fld.table, fld.name, fld.type, fld.length, fld.precision, i, keyColumn);
                }
            }
        }
        return true;
    }
    return false;
}


void Dictionary::close()
{
    Essence::close();
}


void Dictionary::queryName(QString filter)
{
    query(db->getILIKEexpression(QString("\"%1\".\"ИМЯ\"").arg(tableName), "'%" + filter + "%'"));
}


void Dictionary::setSorted(bool sorted)
{
    sortedTable = sorted;
}


void Dictionary::setAutoLoaded(bool al)
{
    lIsAutoLoaded = al;
}


bool Dictionary::isAutoLoaded()
{
    return lIsAutoLoaded;
}


bool Dictionary::canShow()
{
    return lCanShow;
}


void Dictionary::setCanShow(bool can)
{
    lCanShow = can;
}


bool Dictionary::isCanShow()
{
    return lCanShow;
}


bool Dictionary::isMustShow()
{
    return lMustShow;
}


bool Dictionary::isConst()
{
    return lIsConst;
}


bool Dictionary::isSet()
{
    return lIsSet;
}


bool Dictionary::isSaldo()
{
    return lIsSaldo;
}


void Dictionary::setIsSaldo(bool s)
{
    lIsSaldo = s;
}


void Dictionary::setAutoSelect(bool autoSelect)
{
    lAutoSelect = autoSelect;
}


QString Dictionary::objectName()
{
    return "Dictionary";
}


bool Dictionary::isDependent()
{
    return isDepend;
}


void Dictionary::setDependent(bool d)
{
    isDepend = d;
}


QString Dictionary::getPrototypeName()
{
    return prototypeName;
}


void Dictionary::setPrototypeName(QString prototype)
{
    prototypeName = prototype;
}


bool Dictionary::isFtsEnabled()
{
    return ftsEnabled;
}


QString Dictionary::getDictTitle()
{
    return dictTitle;
}


Dictionary* Dictionary::getParentDict()
{
    return parentDict;
}


void Dictionary::setParentDict(Dictionary* dict)
{
    parentDict = dict;
}


void Dictionary::setIdEnabled(bool e)
{
    lsetIdEnabled = e;
}


bool Dictionary::isLocked()
{
    return locked;
}


bool Dictionary::isPictureExist()
{
    return form->getPicture()->isPictureExist();
}


void Dictionary::setGetIdRefresh(bool val)
{
    getIdRefresh = val;
}


void Dictionary::setExact(bool e)
{
    exact = e;
}


bool Dictionary::getExact()
{
    return exact;
}


bool Dictionary::add()
{
    bool result = false;

    if (!lInsertable)
    {
        app->showError(QString(QObject::trUtf8("Запрещено добавлять записи в справочник %1 пользователю %2")).arg(
                      app->getDictionaries()->getDictionaryTitle(tableName),
                      app->getLogin()));
        return result;
    }

    if (scriptEngineEnabled && scriptEngine != 0 /*nullptr*/)
        result = scriptEngine->eventBeforeAddString();

    if (result)
    {
        QHash<QString, QVariant> values;
        result = true;
        if (!isSet())
        {
            if (parameters != 0 /*nullptr*/)
            {
                QVector<sParam> searchParameters = parameters->getParameters();
                if (searchParameters.size() > 0)
                {
                    for (int i = 0; i < searchParameters.size(); i++) {
                        if (searchParameters[i].table == getTableName())
                            values.insert(searchParameters[i].field, searchParameters[i].value);
                        else
                        {
                            Dictionary* dict = dictionaries->getDictionary(searchParameters[i].table);
                            QString dictName = searchParameters[i].value.toString();
                            if (dictName.size() > 0)
                            {
                                dictName = dict->getName();
                                dict->query(parameters->getFilter(searchParameters[i].table));
                                if (dict->rowCount() == 1)
                                {
                                    // Далее первый параметр такой хитрый с запросом к БД имени поля, т.к. searchParameters[i].table - всегда в нижнем регистре, а idFieldName - может быть и в верхнем и в нижнем
                                    // поэтому настоящее имя поля код_<имя таблицы> получим путем запроса к БД
                                    QString dictTableName = dictName;
                                    QString dictFieldName = idFieldName.toLower() + "_" + searchParameters[i].table;
                                    values.insert(db->getObjectName(QString("%1.%2").arg(dictTableName)
                                                                                    .arg(dictFieldName)),
                                    dict->getId(0));
                                }
                                else
                                {
                                    app->showError(QString(QObject::trUtf8("Уточните, пожалуйста, значение связанного справочника %1.")).arg(dict->getFormTitle()));
                                    result = false;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (result)
        {
            result = false;
            if (!isSet())
            {
                int strNum = db->insertDictDefault(getTableName(), &values);
                if (strNum >= 0)
                {
                    int newRow = tableModel->rowCount();
                    tableModel->insertRow(newRow);
                    setCurrentRow(newRow);
                    updateCurrentRow(strNum);
                    result = true;
                }
            }
            else
            {
                int strNum = getId(0, true);
                if (strNum > 0)
                {
                    updateCurrentRow(strNum);
                    result = true;
                }
            }
            if (grdTable != 0 /*nullptr*/)
                grdTable->setCurrentFocus();
        }
    }
    return result;
}


QString Dictionary::getSearchExpression(QString tName)
{
    QString result;
    QString tableName = tName;
    if (tableName.size() == 0)
        tableName = getTableName();
    if (parameters != 0 /*nullptr*/)
    {
        QVector<sParam> searchParameters = parameters->getParameters();
        if (searchParameters.size() > 0)
        {
            for (int i = 0; i < searchParameters.size(); i++)
            {
                if (searchParameters[i].table == tableName)
                {
                    result = searchParameters[i].value.toString();
                    break;
                }
            }
        }
    }
    return result;
}


bool Dictionary::remove(bool noAsk)
{
    bool canRemove = true;
    if (lDeleteable)
    {
        if (Essence::remove(noAsk))
        {
            int id = getValue("КОД").toInt();
            if (scriptEngineEnabled && scriptEngine != 0 /*nullptr*/)
                canRemove = scriptEngine->eventBeforeDeleteString(id);

            if (canRemove)
            {
                if (db->removeDictValue(tableName, id))
                {
                    query();
                    return true;
                }
                app->showError(QString(QObject::trUtf8("Не удалось удалить строку")));
            }
        }
    }
    else
        app->showError(QString(QObject::trUtf8("Запрещено удалять записи из справочника %1 пользователю %2")).arg(
            app->getDictionaries()->getDictionaryTitle(tableName),
            app->getLogin()));
    return false;
}


void Dictionary::setValue(QString name, QVariant value, int row)
{
    Essence::setValue(name, value, row);
}


void Dictionary::setValue(int id, QString name, QVariant value, int row)
{
    setId(id);
    Essence::setValue(name, value, row);
}


int Dictionary::getId(int row, bool forceToRefresh)
{
    int result = Essence::getId(row);
    if ((result == 0 || forceToRefresh || getIdRefresh) && lIsSet)
    {
        // Если это набор, то продолжаем
        QString filter;
        QHash<QString, QVariant> values;
        for (int i = 0; i < fieldList.count(); i++)
        {       // Просмотрим список полей
            QString name = fieldList.at(i);
            if (name.left(4) == idFieldName + "_")
            {        // Если поле ссылается на другую таблицу
                QString field = db->getObjectNameCom(tableName + "." + name);
                name.remove(0, 4);                          // Уберем префикс "код_", останется только название таблицы, на которую ссылается это поле
                name = name.toLower();                      // и переведем в нижний регистр, т.к. имена таблиц в БД могут быть только маленькими буквами
                Dictionary* dict = dictionaries->getDictionary(name);
                if (dict != 0 /*nullptr*/)                       // Если удалось открыть справочник
                {
                    int id = dict->getId();
                    if (dict->getExact())
                    {
                        if (id != 0)
                        {
                            if (filter.size() > 0)
                                filter.append(" AND ");
                            filter.append(QString("%1.%2=%3").arg(db->getObjectNameCom(tableName)).arg(field).arg(id));
                            values.insert(field, QVariant(id));
                        }
                        else
                        {
                            return result;
                        }
                    }
                }
                else
                    return result;
            }
        }

        if (filter.size() > 0)
        {
            query(filter);
            if (tableModel->rowCount() == 0 && isSet() && !isView)
                result = db->insertDictDefault(tableName, &values);
            else
                result = Essence::getId(0);
        }

    }

    return result;
}


bool Dictionary::setId(int id)
{
    bool result = getValue(idFieldName) == id;  // Проверим, не является ли устанавливаемое значение текущим
    if (lsetIdEnabled && !result)
    {
        result = Essence::setId(id);
        if (isSet())            // Если это набор, то переустановим связанные справочники
        {
            foreach (QString dictName, getChildDicts())
            {
                Dictionary* dict = dictionaries->getDictionary(dictName);
                if (!dict->isSet())
                {
                    int val = getValue(QString("%1_%2").arg(idFieldName).arg(dictName).toUpper(), 0).toInt();
                    if (val >= 0)
                        dict->setId(val);
                }
            }
        }
        lock(true);
    }
    return result;
}


void Dictionary::setForm(QString formName)
{
    if (form != 0 /*nullptr*/ && form->isDefaultForm())
    {
        grdTable = 0 /*nullptr*/;
        form->close();
        delete form;
        form = 0 /*nullptr*/;
    }

    form = new FormGridSearch();

    form->appendToolTip("buttonOk",         trUtf8("Закрыть справочник"));
    form->appendToolTip("buttonAdd",        trUtf8("Создать новую запись в справочнике (Ctrl+Ins)"));
    form->appendToolTip("buttonDelete",     trUtf8("Удалить запись из справочника (Ctrl+Del)"));
    if (isPrintable())
        form->appendToolTip("buttonPrint",      trUtf8("Распечатать выбранные записи из справочника (F4)"));
    form->appendToolTip("buttonRequery",    trUtf8("Обновить справочник (загрузить повторно с сервера) (F3)"));
    form->appendToolTip("buttonQuery",    trUtf8("Выполнить запрос"));

    form->open(parentForm, this, formName.size() == 0 ? getTagName() : formName);
    parameters = static_cast<SearchParameters*>(form->getFormWidget()->findChild("searchParameters"));

    grdTable = form->getGrdTable();

    if (grdTable != 0 /*nullptr*/)
        grdTable->setEssence(this);
}


void Dictionary::setConst(bool isConst)
{
    lIsConst = isConst;
    if (dictionaries != 0 /*nullptr*/ && dictionaries->getDocument() != 0 /*nullptr*/)      // Если справочник является локальным к документу
    {                                                               // То на форме документа мы должны его переместить в строку параметров документа
        FormDocument* docForm = static_cast<FormDocument*>(dictionaries->getDocument()->getForm());
        if (docForm != 0 /*nullptr*/)
        {
            DocParameters* docPar = docForm->getDocParameters();
            if (isConst)
            {
                docPar->addString(prototypeName);
            }
        }
    }
}


QStringList Dictionary::getChildDicts()
{
    QStringList childrenList;
    if (isSet())
    {
        for (int i = 0; i < fieldList.count(); i++)
        {       // Просмотрим список полей
            QString name = fieldList.at(i);
            if (name.left(idFieldName.size() + 1) == idFieldName + "_")
            {        // Если поле ссылается на другую таблицу
                name.remove(0, idFieldName.size() + 1);     // Уберем префикс "код_", останется только название таблицы, на которую ссылается это поле
                name = name.toLower();                      // и переведем в нижний регистр, т.к. имена таблиц в БД могут быть только маленькими буквами
                childrenList.append(name);
            }
        }
    }
    return childrenList;
}


bool Dictionary::setTableModel(int)
{
    QString prefix = "документы";
    if (tableName.left(9) == prefix)
    {
        int oper = QString(tableName).replace(prefix, "").toInt();
        sqlCommand = QString("SELECT * FROM (SELECT * FROM %1 WHERE %2 = %3) %4").arg(db->getObjectNameCom("vw_спрдокументы"))
                                                                             .arg(db->getObjectNameCom("vw_спрдокументы.ОПЕР"))
                                                                             .arg(oper)
                                                                             .arg(tableName);
    }
    if (sqlCommand.size() == 0)
    {
        if (tableName.size() > 0)
        {
            if (Essence::setTableModel(0))
            {
                tableModel->setSelectStatement(Dictionaries::getDictionarySqlSelectStatement(tableName));
                db->getColumnsRestrictions(tableName, &columnsProperties);
                return true;
            }
        }
    }
    else
    {
        tableModel = new MySqlRelationalTableModel("", this);
        tableModel->setSelectStatement(sqlCommand);
        db->getColumnsProperties(&columnsProperties, tableName, tableName);
        db->getColumnsRestrictions(tableName, &columnsProperties);
        return true;
    }
    return false;
}


void Dictionary::query(QString defaultFilter, bool exactlyDefaultFilter)
{
    QString resFilter = defaultFilter;
    if (!exactlyDefaultFilter && filterEnabled)
    {
        if (form != 0 /*nullptr*/)
        {
            QString filter = form->getFilter();
            if (filter.size() > 0)
            {
                if (resFilter.size() > 0)
                    resFilter.append(" AND " + filter);
                else
                    resFilter = filter;
            }
        }

        if (!isDocumentLoading())
        {
            if (scriptEngine != 0 /*nullptr*/)
            {
                resFilter = scriptEngine->getFilter(resFilter);
            }
        }
    }

    Essence::query(resFilter);

    if (tableModel != 0 /*nullptr*/)
    {
        if (tableModel->rowCount() > 0 && grdTable != 0 /*nullptr*/)
        {
            QModelIndex index = getCurrentIndex();

            if (index.row() > tableModel->rowCount() - 1)       // Если старая последняя запись исчезла
                grdTable->selectRow(tableModel->rowCount() - 1);    // то перейдем на новую последнюю
            else
            {
                if (index.row() < 0)
                    grdTable->selectRow(0);
                else
                    grdTable->selectRow(index.row());
            }
        }

        if (tableModel->rowCount() == 1)     // Если включен автоматический выбор позиции и позиция одна, то нажмем кнопку Ok (выберем позицию)
        {
            if (lAutoSelect)
                form->cmdOk();
        }
    }
}


void Dictionary::setOrderClause(QString sOrder)
{
    if (!sortedTable)
    {
        Table::setOrderClause("");
        return;
    }
    if (sOrder.size() > 0)
    {
        Table::setOrderClause(sOrder);
        return;
    }
    if (lIsSet)
    {
        QString sortOrder;
        QStringList tablesList;
        for (int i = 0; i < columnsProperties.count(); i++)
        {
            FieldType fld = columnsProperties.at(i);
            if (fld.table != tableName && !tablesList.contains(fld.table) && fld.table.left(9) != "документы" && fld.table.left(11) != "докатрибуты")
            {
                 tablesList.append(fld.table);
                 if (sortOrder.size() > 0)
                     sortOrder.append(",");
                 sortOrder.append(QString("\"%1\".%2").arg(fld.table)
                                                  .arg(db->getObjectNameCom(fld.table + ".имя")));
             }
         }
        Table::setOrderClause(sortOrder);
    }
    else
    {
        if (tableName.left(9) != "документы" && tableName.size() > 0 && lNameExist)
            Table::setOrderClause(QString("\"%1\".%2").arg(tableName)
                                                     .arg(db->getObjectNameCom(tableName + ".имя")));
    }
}


void Dictionary::updateCurrentRow(int strNum)
{   // Делает запрос к БД по одной строке справочника. Изменяет в текущей модели поля, которые в БД отличаются от таковых в модели.
    // Применяется после работы формул для изменения полей в строке, которые косвенно изменились

    int str = strNum == 0 ? getValue(idFieldName).toInt() : strNum;

    preparedSelectCurrentRow.bindValue(":value", str);

    Essence::updateCurrentRow();
}


void Dictionary::prepareSelectCurrentRowCommand()
{
    preparedSelectCurrentRow.clear();

    // Подготовим приготовленный (PREPARE) запрос для обновления текущей строки при вычислениях
    QString command = tableModel->selectStatement();

    command.replace(" ORDER BY", QString(" %1 \"%2\".\"%3\"=:value ORDER BY").arg(command.contains("WHERE") ? "AND" : "WHERE")
                                                                                .arg(getTableName())
                                                                                .arg(db->getObjectName(getTableName() + "." + idFieldName)));
    preparedSelectCurrentRow.prepare(command);
}


void Dictionary::preparePrintValues()
{
    if (reportScriptEngine != 0 /*nullptr*/)
    {
        if (parameters != 0 /*nullptr*/)
        {
            QVector<sParam> searchParameters = parameters->getParameters();
            if (searchParameters.size() > 0)
            {
                for (int i = 0; i < searchParameters.size(); i++)
                {
                    if (searchParameters[i].value.toString().size() > 0)
                        reportScriptEngine->getReportContext()->setValue(searchParameters[i].table, searchParameters[i].value);
                }
            }
        }
        Essence::preparePrintValues();
    }
}


void Dictionary::setMustShow(bool must)
{
    lMustShow = must;
}


void Dictionary::lock(bool toLock)
// Заблокировать все связанные справочники
{
    if (isSet())
    {
        foreach (QString dictName, getChildDicts())
        {
            Dictionary* dict = dictionaries->getDictionary(dictName);
            if (toLock)
            {
                int id = getValue(idFieldName + "_" + dictName).toInt();
                if (id > 0)
                    dict->setId(id);
            }
            dict->lock(toLock);
        }
    }
    locked = toLock;
}


void Dictionary::setSqlCommand(QString command)
{
    if (opened)
        close();
    open(command);
}


void Dictionary::setFilter(QString f)
{
    filter = f;
}


QString Dictionary::getFilter(QString defFilter) const
{
    QString filter;
//
//    if (filter.size() > 0)
//    {
//        QString text = filter;
//        bool isInt = true;
//        int id = 0;
//        if (text.size() > 0 && text.at(0).isDigit())
//            id = text.toInt(&isInt);    // Проверим, не является ли значение кодом
//        else
//        {
//            isInt = false;
//            text = text.trimmed();
//        }
//        QStringList paramList = text.split(QRegExp("\\s+"));
//        if (isFtsEnabled)   // Если включен полнотектовый поиск
//        {
//            result.append(QString("%1.fts @@ to_tsquery('").arg(db->getObjectNameCom(tableName)));
//            QString f = "";
//            foreach (QString param, paramList)
//            {
//                if (f.size() > 0)
//                    f.append("&");
//                f.append(param);
//            }
//            result.append(f + "')");
//        }
//        else
//        { // Если полнотекстовый поиск отключен
//            int subStrNum = 0;
//            foreach (QString param, paramList)
//            {
//                if (param.size() > 0)
//                {
//                    Dictionary* dict = dictionaries->getDictionary(tableName);    // Поместим связанный справочник в список справочников приложения
//                    if (dict != 0 /*nullptr*/)
//                    {
//                        if (dict->getForm()->isLeftPercent() || subStrNum > 0)     // Отсутствие знака % актуально только для первого слова
//                            param = "%" + param;
//                        if (dict->getForm()->isRightPercent())
//                            param = param + "%";
//                    }
//                }
//                else
//                    param = "%" + param + "%";
//
//                if (result.size() > 0)
//                    result.append(" AND ");
//
//                QString fieldName = QString("%1.").arg(db->getObjectNameCom(tableName));
//                fieldName.append(app->getDBFactory()->getObjectNameCom(searchParameters[i].table + "." + searchParameters[i].field));
//                        filter.append(app->getDBFactory()->getILIKEexpression(fieldName, "'" + param + "'"));
//                        subStrNum++;
//                    }
//                }
//            }
//            if (isInt)
//            {
//                filter = QString("%1.%2 = %3").arg(app->getDBFactory()->getObjectNameCom(searchParameters[i].table))
//                                                       .arg(app->getDBFactory()->getObjectNameCom(searchParameters[i].table + ".КОД"))
//                                                       .arg(id);
//                filter = "(" + filter + ")";
//            }
//            break;
//        }
//    }
//    if (defFilter.size() > 0 && filter.size() > 0)
//        filter = " AND " + filter;

    return defFilter + filter;
}

