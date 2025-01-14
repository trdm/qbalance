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

#include <QHBoxLayout>
#include "mybuttonlineedititemdelegate.h"
#include "mybuttonlineedit.h"
#include "../kernel/dictionary.h"


MyButtonLineEditItemDelegate::MyButtonLineEditItemDelegate(QObject* parent, FormGrid* form): MyItemDelegate(parent, form)
{
}


void MyButtonLineEditItemDelegate::setFormOnPushButton(QString (*form)())
{
    buttonForm = form;
}


QWidget* MyButtonLineEditItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const
{
    MyButtonLineEdit* editorWidget = 0 /*nullptr*/;
    if (!menuMode)
    {
        editorWidget = new MyButtonLineEdit(parent);
        if (!readOnly)
        {
            if (essence != 0 /*nullptr*/)
            {
                essence->saveOldValues();
                disconnect(this, SIGNAL(closeEditor(QWidget*)), this, SLOT(calculate()));
                connect(this, SIGNAL(closeEditor(QWidget*)), this, SLOT(calculate()));
            }
            editorWidget->setFormOnPushButton(buttonForm);
        }
    }
    return editorWidget;
}


void MyButtonLineEditItemDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    MyButtonLineEdit* buttonLineEdit = static_cast<MyButtonLineEdit*>(editor);
    buttonLineEdit->setValue(value);
}


void MyButtonLineEditItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const
{
    MyButtonLineEdit* buttonLineEdit = static_cast<MyButtonLineEdit*>(editor);
    QString value = buttonLineEdit->value();
    model->setData(index, value, Qt::EditRole);
}
