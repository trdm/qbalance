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

#include <QtCore/QString>
#include <QDebug>
#include "dictionaries.h"
#include "../kernel/app.h"
#include "../gui/mainwindow.h"
#include "../gui/formgridsearch.h"
#include "../gui/wizarddictionary.h"


Dictionaries::Dictionaries(QObject *parent): Dictionary("vw_доступ_к_справочникам", parent) {
    lInsertable = TApplication::exemplar()->isSA();     // Если работает пользователь SA, то можно добавить новый справочник
    lViewable = TApplication::exemplar()->isSA();       // Если работает пользователь SA, то можно просмотреть свойства справочника
    lDeleteable = TApplication::exemplar()->isSA();       // Если работает пользователь SA, то можно попытаться удалить справочник
    lUpdateable = false;
    lPrintable = false;
}


Dictionary* Dictionaries::getDictionary(QString dictName, int deep, bool add) {
    if (dictName.size() == 0)
        return 0;
    if (!dictionaries.contains(dictName)) {             // Если справочник с таким именем не существует, то попробуем его создать
        if (add)
        {
            if (!addDictionary(dictName, deep))
                return 0;
        }
        else
            return 0;
    }
    return dictionaries[dictName];
}


Saldo* Dictionaries::getSaldo(QString acc, int deep) {
    if (acc.size() == 0)
        return 0;
    QString alias = "saldo" + acc;
    if (!dictionaries.contains(alias)) {             // Если справочник с таким именем не существует, то попробуем его создать
        if (!addSaldo(acc, deep))
            return 0;
    }
    return (Saldo*)dictionaries[alias];
}


bool Dictionaries::addDictionary(QString dictName, int deep) {
    if (dictName.size() == 0)
        return false;
    if (!dictionaries.contains(dictName)) {             // Если справочник с таким именем не существует, то попробуем его создать
        Dictionary* dict;
        dict = new Dictionary(dictName, this);
        dict->setDictionaries(this);
        if (dict->open(deep)) {
            dictionaries.insert(dictName, dict);
            dict->setDictionaries(this);
            return true;
        }
    }
    return false;
}

bool Dictionaries::addSaldo(QString acc, int deep) {
    if (acc.size() == 0)
        return false;
    QString alias = "saldo" + acc;
    if (!dictionaries.contains(alias)) {
        // Имя справочника, который используется в бухгалтерском счете acc возьмем из справочника "Счета"
        Dictionary* accDict = TApplication::exemplar()->getDictionaries()->getDictionary(db->getObjectName("счета"));
        accDict->query(QString("%1='%2'").arg(db->getObjectName("счета.счет")).arg(acc));
        QString dictName = accDict->getValue(db->getObjectName("счета.имясправочника")).toString();
        Saldo* saldo = new Saldo(acc, dictName);
        saldo->setDictionaries(this);
        if (saldo->open(deep)) {
            saldo->getFormWidget()->setWindowTitle(QString(QObject::trUtf8("Остаток на счете %1")).arg(acc));
            dictionaries.insert(alias, saldo);
            saldo->setDictionaries(this);
            return true;
        }
    }
    return false;
}


void Dictionaries::removeDictionary(QString dictName) {
    if (dictName.size() == 0)
        return;
    if (dictionaries.contains(dictName)) {             // Если справочник с таким именем не существует, то попробуем его создать
        Dictionary* dict = getDictionary(dictName);
        dict->close();
        dictionaries.remove(dictName);
    }
}


QString Dictionaries::getDictionaryTitle(QString dictName) {
    return db->getDictionariesProperties(dictName).value(db->getObjectName("имя")).toString().trimmed();
}


bool Dictionaries::add()
{
    bool result = false;
    WizardDictionary* wizard = new WizardDictionary(true);
    wizard->open(TApplication::exemplar()->getMainWindow());
    wizard->getForm()->setWindowTitle(QObject::trUtf8("Новый справочник"));
    wizard->exec();
    wizard->close();
    if (wizard->getResult())
    {   // Если удалось создать справочник, то обновим список справочников
        result = true;
    }
    delete wizard;
    return result;
}


void Dictionaries::view()
{
    QString dictName = getValue("таблица").toString().trimmed();
    WizardDictionary wizard;
    wizard.open(TApplication::exemplar()->getMainWindow(), dictName);
    wizard.getForm()->setWindowTitle(QObject::trUtf8("Свойства справочника"));
    wizard.exec();
    wizard.close();
    if (wizard.getResult())
    {
        removeDictionary(dictName);
    }
}


bool Dictionaries::remove()
{
    if (Essence::remove())
    {
        if (db->removeDictionary(getValue("таблица").toString().trimmed()))
        {   // если удалось удалить справочник, то обновим список справочников
            return true;
        }
    }
    return false;
}


bool Dictionaries::open(bool openForm) {
    if (Essence::open()) {
//        dictListDict = new Dictionary(db->getObjectName("справочники"), this);
//        dictListDict->open();
        tableModel->setSortClause(db->getObjectName("vw_доступ_к_справочникам.имя"));
        if (openForm)
            initForm();
        return true;
    }
    return false;
}


void Dictionaries::close() {
    foreach(Dictionary* dict, dictionaries) {
        dict->close();
    }
    Essence::close();
}


void Dictionaries::query(QString) {
    Dictionary::query(QString("%1=true").arg(db->getObjectName("vw_доступ_к_справочникам.меню")));
}


void Dictionaries::cmdOk() {
    Dictionary::cmdOk();
    QString dictName = getValue("таблица").toString().trimmed();
    if (dictName.size() > 0) {
        Dictionaries* dicts = TApplication::exemplar()->getDictionaries();
        Dictionary* dict = dicts->getDictionary(dictName, 1);         // Откроем справочник и подсправочники 1-го уровня
        if (dict != 0)
        {
            dict->show();
        }
    }
}


void Dictionaries::setForm()
{
    form = new FormGridSearch();

    form->appendToolTip("buttonOk",         trUtf8("Открыть справочник"));
    form->appendToolTip("buttonCancel",     trUtf8("Закрыть список справочников"));
    form->appendToolTip("buttonAdd",        trUtf8("Создать новый справочник"));
    form->appendToolTip("buttonDelete",     trUtf8("Удалить справочник"));
    form->appendToolTip("buttonView",       trUtf8("Просмотреть свойства справочника"));
    form->appendToolTip("buttonRequery",    trUtf8("Обновить список справочников (загрузить повторно с сервера)"));

    form->open(parentForm, this, getTagName());
}
