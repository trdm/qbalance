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

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QDebug>
#include "../kernel/app.h"
#include "../kernel/dictionary.h"
#include "formgridsearch.h"
#include "searchparameters.h"

#define LABEL_SEARCH_PARAMETERS     QObject::trUtf8("Параметры для поиска:")

FormGridSearch::FormGridSearch(QObject* parent/* = NULL*/) : FormGrid(parent)
{
    parameters = 0;
}


FormGridSearch::~FormGridSearch()
{
}


void FormGridSearch::createForm(QString fileName, QWidget* pwgt/* = 0*/)
{
    FormGrid::createForm(fileName, pwgt);
    formWidget->resize(600, formWidget->height());
    if (defaultForm)
    {
        if (vbxLayout != 0)
        {
            QHBoxLayout* hbxLayout = new QHBoxLayout();
            parameters = new SearchParameters();
            parameters->setObjectName("searchParameters");
            hbxLayout->insertWidget(0, parameters);
            QLabel* label = new QLabel(LABEL_SEARCH_PARAMETERS, formWidget);
            hbxLayout->insertWidget(0, label);
            vbxLayout->insertLayout(0, hbxLayout);
        }
    }
    else
    {
        parameters = (SearchParameters*)qFindChild<QFrame*>(formWidget, "searchParameters");
    }
    if (parameters != 0)
    {
        parameters->setDictionaries(((Dictionary*)parent)->getDictionaries());
        parameters->setApp(app);
        parameters->setParent(formWidget);
        parameters->setFormGrid(this);
        parameters->setProgramIdFieldName(db->getObjectName("код").toLower());
        parameters->setProgramNameFieldName(db->getObjectName("имя").toLower());
        parameters->setFieldsList(parent->getFieldsList());
        connect(parameters, SIGNAL(requery()), this, SLOT(cmdRequery()));
    }
}


void FormGridSearch::close() {
    if (!defaultForm)
        if (parameters != 0) {
            disconnect(parameters, 0, 0, 0);
            parameters->close();
        }
    FormGrid::close();
}


QString FormGridSearch::getFilter()
{
    return parameters->getFilter();
}


int FormGridSearch::exec()
{
    int result = FormGrid::exec();
    if (parameters != 0)
        parameters->setFocus();
    return result;
}


void FormGridSearch::show()
{
    FormGrid::show();
    if (parameters != 0)
        parameters->setFocus();
}


/*
QDomElement FormGridSearch::createWidgetsStructure() {
    QDomDocument* doc = new QDomDocument();
    QDomElement vboxLayout = FormGrid::createWidgetsStructure();
    if (vbxLayout != 0) {
        QDomElement item, widget, hlayout;
        hlayout = doc->createElement("layout");
        hlayout.setAttribute("class", "QHBoxLayout");
        widget = createLabelElement(LABEL_SEARCH_PARAMETERS);
        item = doc->createElement("item");
        item.appendChild(widget);
        hlayout.appendChild(item);
        if (parameters != 0) {
            widget = doc->createElement("widget");
            widget.setAttribute("class", parameters->metaObject()->className());
            widget.setAttribute("name", parameters->objectName());
            item = doc->createElement("item");
            item.appendChild(widget);
            hlayout.appendChild(item);
        }
        item = doc->createElement("item");
        item.appendChild(hlayout);
        vboxLayout.insertBefore(item, QDomNode());
    }
    delete doc;
    return vboxLayout;
}
*/

