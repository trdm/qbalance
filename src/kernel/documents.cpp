#include <QString>
#include "documents.h"
#include "document.h"
#include "../kernel/app.h"
#include "../gui/mainwindow.h"
#include "../gui/formgridsearch.h"

Documents::Documents(int opNumber, QObject *parent): Dictionary(parent) {
    lViewable  = true;
    tableName  = "документы";
    operNumber = opNumber;
    tagName    = QString("СписокДокументов%1").arg(operNumber);
    formTitle  = QString("%1 - %2").arg(TApplication::exemplar()->getToperProperty(operNumber, TApplication::nameFieldName()).toString()).arg(QObject::tr("Список документов"));
}

Documents::~Documents() {
}

void Documents::show() {
    query();
    Dictionary::show();
}

bool Documents::add() {
    QDate date = QDate().currentDate();
    if (!(date >= TApplication::exemplar()->getBeginDate() && date <= TApplication::exemplar()->getEndDate())) {
        if (date < TApplication::exemplar()->getBeginDate())
            date = TApplication::exemplar()->getBeginDate();
        else if (date > TApplication::exemplar()->getEndDate())
            date = TApplication::exemplar()->getEndDate();
    }
    return TApplication::exemplar()->getDBFactory()->addDoc(operNumber, date);
}

//QSqlQuery Documents::getColumnsHeaders() {
//    return TApplication::exemplar()->getDBFactory()->getColumnsHeaders(configName);
//}

bool Documents::remove() {
    if (lDeleteable) {
        if (Essence::remove()) {
            TApplication::exemplar()->getDBFactory()->removeDoc(getValue("код").toInt());
            return true;
        }
    }
    else
        showError(QString(QObject::tr("Запрещено удалять документы пользователю %2")).arg(TApplication::exemplar()->getLogin()));
    return false;
}

void Documents::view() {
    currentRow = getForm()->getCurrentIndex().row();
    currentDocument->setDocId(getValue("код").toInt());
    currentDocument->show();
}

void Documents::query(QString filter) {
    Q_UNUSED(filter)
    Essence::query(QString("дата BETWEEN cast('%1' as date) AND cast('%2' as date) AND авто=0 AND опер='%3' ORDER BY дата, код").arg(
        TApplication::exemplar()->getBeginDate().toString("dd.MM.yyyy"),
        TApplication::exemplar()->getEndDate().toString("dd.MM.yyyy"),
        QString::number(operNumber)));
}

bool Documents::open() {
    lInsertable = TApplication::exemplar()->getToperProperty(operNumber, "insertable").toBool();
    lDeleteable = TApplication::exemplar()->getToperProperty(operNumber, "deleteable").toBool();
    lUpdateable = TApplication::exemplar()->getToperProperty(operNumber, "updateable").toBool();
    if (Essence::open()) {     // Откроем этот справочник

        // Установим форму для отображения справочника

        // Установим порядок сортировки и стратегию сохранения данных на сервере
        tableModel->setSort(tableModel->fieldIndex(TApplication::nameFieldName()), Qt::AscendingOrder);

        currentDocument = new Document(operNumber, this);
        if (currentDocument->open())
            initForm();
            return true;
    }
    showError(QString(QObject::tr("Запрещено просматривать операцию <%1> пользователю %2")).arg(
                  formTitle,
                  TApplication::exemplar()->getLogin()));
    return false;
}


void Documents::close() {
    currentDocument->close();
    delete currentDocument;
    Dictionary::close();
}


void Documents::setForm() {
    form = new FormGrid();
    form->open(parentForm, this);
}