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

#ifndef MYPLUGINS_H
#define MYPLUGINS_H

#include "dialogplugin.h"
#include "pictureplugin.h"
#include "tableviewplugin.h"
#include "searchparametersplugin.h"
#include "docparametersplugin.h"
#include "mynumericeditplugin.h"
//#include <QtDesigner/QtDesigner>
#include <QtCore/qplugin.h>

class MyPlugins: public QObject, public QDesignerCustomWidgetCollectionInterface {
    Q_OBJECT

    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "MyPlugins")
#endif

public:
    MyPlugins(QObject *parent = 0);

    virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

private:
    QList<QDesignerCustomWidgetInterface*> widgets;
};

//Q_DECLARE_INTERFACE(MyPlugins, "org.QBalance.MyPlugins")

#endif // MYPLUGINS_H
