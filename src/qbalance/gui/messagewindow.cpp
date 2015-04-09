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

#include <QtGui/QTextEdit>
#include "messagewindow.h"
#include "../kernel/app.h"


MessageWindow::MessageWindow() :
    QObject()
{
    subWindow = 0;
    app = 0;
    textEditor = new QTextEdit();
    textEditor->setParent(subWindow);
    textEditor->setWindowTitle(QObject::trUtf8("Сообщения"));
}


MessageWindow::~MessageWindow()
{
    if (subWindow != 0)
    {
        writeSettings();
        TApplication::exemplar()->getMainWindow()->removeMdiWindow(subWindow);
        subWindow = 0;
    }
    delete textEditor;
}


void MessageWindow::print(QString str)
{
    textEditor->append(str);
    if (str.size() > 0)
        show();
}


void MessageWindow::show()
{
    if (subWindow == 0)
    {
        app = TApplication::exemplar();
        subWindow = app->getMainWindow()->appendMdiWindow(textEditor);
        readSettings();
    }

    if (subWindow != 0)
    {
        textEditor->show();
        subWindow->show();
    }
}


void MessageWindow::readSettings()
{
    QString configName = "messagesWindow";
    QHash<QString, int> settingValues;
    QSettings settings;
    if (settings.status() == QSettings::NoError)
    {
        settings.beginGroup(configName);
        if (settings.contains("x") &&
            settings.contains("y") &&
            settings.contains("width") &&
            settings.contains("height") &&
            settings.value("width", 0).toInt() > 0 &&
            settings.value("height", 0).toInt() > 0)
        {
            settingValues.insert("x", settings.value("x").toInt());
            settingValues.insert("y", settings.value("y").toInt());
            settingValues.insert("width", settings.value("width").toInt());
            settingValues.insert("height", settings.value("height").toInt());
        }
        else
        {
            // Если локальные значения координат и размеров окна прочитать не удалось, попытаемся загрузить их с сервера
            app->showMessageOnStatusBar(tr("Загрузка с сервера геометрии окна справочника ") + configName + "...");
            QSqlQuery config = app->getDBFactory()->getConfig();
            config.first();
            while (config.isValid())
            {
                if (config.record().value("group").toString() == configName)
                {
                    settingValues.remove(config.record().value("name").toString());
                    settingValues.insert(config.record().value("name").toString(), config.record().value("value").toInt());
                }
                config.next();
            }
            app->showMessageOnStatusBar("");
        }
        settings.endGroup();

        int x = settingValues.value("x", 0);
        int y = settingValues.value("y", 0);
        int w = settingValues.value("width", 400);
        int h = settingValues.value("height", 200);

        subWindow->setGeometry(x, y, w, h);
    }
}


void MessageWindow::writeSettings()
{
    QString configName = "messagesWindow";
    // Сохраним данные локально, на компьютере пользователя
    QSettings settings;
    settings.beginGroup(configName);
    settings.setValue("x", subWindow->x());
    settings.setValue("y", subWindow->y());
    settings.setValue("width", subWindow->frameGeometry().width());
    settings.setValue("height", subWindow->frameGeometry().height());
    settings.endGroup();

    // И если работает пользователь SA, то сохраним конфигурацию окна на сервере
    if (app->isSA() && app->getSaveFormConfigToDb())
    {
        app->showMessageOnStatusBar(tr("Сохранение на сервере геометрии окна справочника ") + configName + "...");
        app->getDBFactory()->setConfig(configName, "x", QString("%1").arg(subWindow->geometry().x()));
        app->getDBFactory()->setConfig(configName, "y", QString("%1").arg(subWindow->geometry().y()));
        app->getDBFactory()->setConfig(configName, "width", QString("%1").arg(subWindow->geometry().width()));
        app->getDBFactory()->setConfig(configName, "height", QString("%1").arg(subWindow->geometry().height()));
        app->showMessageOnStatusBar("");
    }
}
