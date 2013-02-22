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

#include <QtCore/QObject>
#include <QtCore/QTextStream>
#include <QtCore/QResource>
#include <QtUiTools/QUiLoader>
#include <QtCore/QTextCodec>
#include <QtCore/QList>
#include <QtGui/QPushButton>
#include <QtGui/QMdiSubWindow>
#include <QDebug>
#include "form.h"
#include "../kernel/app.h"
#include "mainwindow.h"
#include "dialog.h"

class TApplication;

Form::Form(QObject* par/* = NULL*/): QObject(par)
{
    parent = 0;
    formWidget = 0;
    subWindow = 0;
    cmdButtonLayout = 0;
    vbxLayout = 0;
    buttonOk = 0;
    buttonCancel = 0;
    appendToMdi = true;
    app = TApplication::exemplar();
    mainWindow = app->getMainWindow();
    db = app->getDBFactory();
    formX = 0;
    formY = 0;
    formW = 0;
    formH = 0;
}


Form::~Form()
{
}


bool Form::open(QWidget* pwgt, Essence* par, QString fName) {
    parent = par;
    fileName = fName;
    if (fileName.size() == 0)
        createForm("mainform", pwgt);
    else
        createForm(fileName, pwgt);
    app->setIcons(formWidget);
    readSettings();
    buttonOk = qFindChild<QPushButton*>(formWidget, "buttonOk");

    //  Установим подписи ко всем кнопкам
    foreach (QString key, toolTips.keys())
    {
        QPushButton* button = qFindChild<QPushButton*>(formWidget, key);
        if (button != 0 && button->toolTip().isEmpty())
            button->setToolTip(toolTips.value(key, ""));
    }
    return true;
}


void Form::close() {
    disconnect(formWidget, 0, 0, 0);
    writeSettings();
}


void Form::createForm(QString fileName, QWidget* pwgt) {
    if (parent != 0)
    {
        configName = app->getConfigPrefix() + "." + getParent()->getTagName();
    }
    else
    {
        configName = app->getConfigPrefix() + ".Form";
    }
    setObjectName(configName);
    uiCreated = false;
    formWidget = 0;
    defaultForm = true;
    script = "";
    if (fileName != "mainform")
    {
        formWidget = app->createForm(fileName);
    }
    if (formWidget != 0)
    {
        formWidget->setParent(pwgt);
        formWidget->setVisible(false);
        formWidget->setWindowFlags(Qt::Window | Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowContextHelpButtonHint);
        buttonOk = qFindChild<QPushButton*>(formWidget, "buttonOk");
        buttonCancel = qFindChild<QPushButton*>(formWidget, "buttonCancel");
        cmdButtonLayout = qFindChild<QHBoxLayout*>(formWidget, "cmdButtonLayout");
        vbxLayout = qFindChild<QVBoxLayout*>(formWidget, "vbxLayout");
        connect(formWidget, SIGNAL(finished(int)), this, SLOT(hide()));
        defaultForm = false;
    }
    else
    {
        formWidget = new Dialog(pwgt);
        formWidget->setVisible(false);
        buttonOk = new QPushButton();
        buttonOk->setObjectName("buttonOk");
        buttonCancel = new QPushButton();
        buttonCancel->setObjectName("buttonCancel");

        cmdButtonLayout = new QHBoxLayout();
        cmdButtonLayout->setObjectName("cmdButtonLayout");
        cmdButtonLayout->addStretch(1);
        cmdButtonLayout->addWidget(buttonOk);
        cmdButtonLayout->addWidget(buttonCancel);

        vbxLayout = new QVBoxLayout();
        vbxLayout->setObjectName("vbxLayout");

        vbxLayout->addLayout(cmdButtonLayout);
        formWidget->setLayout(vbxLayout);
    }
    if (buttonOk != 0)
        connect(buttonOk, SIGNAL(clicked()), SLOT(cmdOk()));
    if (buttonCancel != 0)
    {
        connect(buttonCancel, SIGNAL(clicked()), SLOT(cmdCancel()));
        if (defaultForm)
            connect(formWidget, SIGNAL(finished(int)), SLOT(cmdCancel()));
        buttonCancel->hide();
    }
    formWidget->setFocusPolicy(Qt::StrongFocus);
}


void Form::cmdOk() {
    hide();
    lSelected = true;
    if (parent != 0)
        parent->cmdOk();
}


void Form::cmdCancel() {
    lSelected = false;
    hide();
}


int Form::exec() {
    if (formWidget != 0) {
        lSelected = false;
        if (parent != 0)
            parent->beforeShowFormEvent();
//        formWidget->setWindowModality(Qt::ApplicationModal);

        if (appendToMdi)
        {
            formWidget->formX = formX;
            formWidget->formY = formY;
            formWidget->formW = formW;
            formWidget->formH = formH;
        }
        formWidget->setFreeWindow(!appendToMdi);

        formWidget->exec();
        return lSelected;
    }
    return 0;
}


void Form::show() {
    if (formWidget != 0) {
//        if (app->isSA())
//            createUi();
        lSelected = false;
        if (parent != 0)
            parent->beforeShowFormEvent();
/*
        if (appendToMdi)
        {
            subWindow = mainWindow->appendMdiWindow(formWidget);
            if (subWindow != 0)
            {
                formWidget->show();
                subWindow->setGeometry(formX, formY, formW, formH);
                subWindow->show();
            }
        }
        else
            formWidget->show();
*/

        if (appendToMdi)
        {
            formWidget->formX = formX;
            formWidget->formY = formY;
            formWidget->formW = formW;
            formWidget->formH = formH;
        }
        formWidget->setFreeWindow(!appendToMdi);
        formWidget->show();

    }
}


void Form::hide() {
    if (formWidget != 0) {
/*
        if (appendToMdi && subWindow != 0)
        {
            formX = subWindow->x();
            formY = subWindow->y();
            formW = subWindow->width();
            formH = subWindow->height();
            mainWindow->removeMdiWindow(subWindow);
        }
        else
            formWidget->hide();
*/
        formWidget->hide();

        formX = formWidget->formX;
        formY = formWidget->formY;
        formW = formWidget->formW;
        formH = formWidget->formH;

        if (parent != 0)
            parent->afterHideFormEvent();

    }
}


void Form::appendToolTip(QString name, QString value)
{
    if (toolTips.contains(name))
        toolTips.remove(name);
    toolTips.insert(name, value);
}


void Form::setButtonsSignals()
{
    QList<QPushButton*> allButtons = formWidget->findChildren<QPushButton*>();
    foreach (QPushButton* button, allButtons)
    {
        connect(button, SIGNAL(clicked()), SLOT(buttonPressedSignalSend()));
    }
}


void Form::readSettings() {
    QMap<QString, int> settingValues;
    // Попытаемся сначала прочитать локальные значения координат окна и размеров на компьютере пользователя
    QSettings settings;
    if (settings.status() == QSettings::NoError)
    {
        settings.beginGroup(configName);
        settingValues.insert("x", settings.value("x", -1).toInt());
        settingValues.insert("y", settings.value("y", -1).toInt());
        settingValues.insert("width", settings.value("width", -1).toInt());
        settingValues.insert("height", settings.value("height", -1).toInt());
        settings.endGroup();
    }

    // Если локальные значения координат и размеров окна прочитать не удалось, попытаемся загрузить их с сервера
    if (settingValues.value("x") == -1 ||
        settingValues.value("y") == -1 ||
        settingValues.value("width") == -1 ||
        settingValues.value("height") == -1)
    {
        app->showMessageOnStatusBar(tr("Загрузка с сервера геометрии окна справочника ") + configName + "...");
        QSqlQuery config = db->getConfig();
        config.first();
        while (config.isValid())
        {
            if (config.record().value("group").toString() == configName)
            {
                settingValues.remove(config.value(0).toString());
                settingValues.insert(config.value(0).toString(), config.value(1).toInt());
            }
            config.next();
        }
        app->showMessageOnStatusBar("");
    }

    // Установим координаты и размеры окна
    QWidget* widget;
    if (subWindow != 0)         // Если используется mdi-окно
        widget = subWindow;     // то установим координаты для него
    else
        widget = formWidget;
    int x = settingValues.value("x") == -1 ? 100 : settingValues.value("x");
    int y = settingValues.value("y") == -1 ? 100 : settingValues.value("y");
    int w = settingValues.value("width") == -1 ? 600 : settingValues.value("width");
    int h = settingValues.value("height") == -1 ? 300 : settingValues.value("height");
    x = x == 0 ? 100 : x;
    y = y == 0 ? 100 : y;
    w = w == 0 ? 600 : w;
    h = h == 0 ? 300 : h;
    widget->setGeometry(x, y, w, h);
    formX = x;
    formY = y;
    formW = w;
    formH = h;
}


void Form::writeSettings() {
    // Сохраним координаты и размеры окна
    QWidget* widget;
    if (subWindow != 0)         // Если используется mdi-окно
        widget = subWindow;     // то будем сохранять его координаты и размеры
    else
        widget = formWidget;

    // Сохраним данные локально, на компьютере пользователя
    QSettings settings;
    settings.beginGroup(configName);
    settings.setValue("x", widget->geometry().x());
    settings.setValue("y", widget->geometry().y());
    settings.setValue("width", widget->geometry().width());
    settings.setValue("height", widget->geometry().height());
    settings.endGroup();

    // И если работает пользователь SA, то сохраним конфигурацию окна на сервере
    if (app->getSaveFormConfigToDb())
    {
        app->showMessageOnStatusBar(tr("Сохранение на сервере геометрии окна справочника ") + configName + "...");
        db->setConfig(configName, "x", QString("%1").arg(formWidget->geometry().x()));
        db->setConfig(configName, "y", QString("%1").arg(formWidget->geometry().y()));
        db->setConfig(configName, "width", QString("%1").arg(formWidget->geometry().width()));
        db->setConfig(configName, "height", QString("%1").arg(formWidget->geometry().height()));
        app->showMessageOnStatusBar("");
    }
}

/*
QDomElement Form::createBoxLayoutElement(QLayout* obj) {
    QDomDocument* doc = new QDomDocument();
    QDomElement element;
    element = doc->createElement("widget");
    element.setAttribute("class", "QWidget");
    if (obj->objectName().size() > 0)
        element.setAttribute("name", obj->objectName());
    QDomElement geometry = doc->createElement("property");
    geometry.setAttribute("name", "geometry");
    QDomElement rect = doc->createElement("rect");
    QDomElement x = doc->createElement("x");
    x.appendChild(doc->createTextNode(QString("%1").arg(obj->geometry().x())));
    rect.appendChild(x);
    QDomElement y = doc->createElement("y");
    y.appendChild(doc->createTextNode(QString("%1").arg(obj->geometry().y())));
    rect.appendChild(y);
    QDomElement width = doc->createElement("width");
    width.appendChild(doc->createTextNode(QString("%1").arg(obj->geometry().width())));
    rect.appendChild(width);
    QDomElement height = doc->createElement("height");
    height.appendChild(doc->createTextNode(QString("%1").arg(obj->geometry().height())));
    rect.appendChild(height);
    geometry.appendChild(rect);
    element.appendChild(geometry);
    delete doc;
    return element;
}


QDomElement Form::createPushButtonElement(QWidget* obj) {
//#ifdef Q_OS_WIN32
//    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
//#else
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
//#endif
    QDomDocument* doc = new QDomDocument();
    QDomElement element;
    element = doc->createElement("widget");
    element.setAttribute("class", "QPushButton");
    if (obj->objectName().size() > 0)
        element.setAttribute("name", obj->objectName());
    QDomElement geometry = doc->createElement("property");
    geometry.setAttribute("name", "geometry");
    QDomElement rect = doc->createElement("rect");
    QDomElement x = doc->createElement("x");
    x.appendChild(doc->createTextNode(QString("%1").arg(obj->x())));
    rect.appendChild(x);
    QDomElement y = doc->createElement("y");
    y.appendChild(doc->createTextNode(QString("%1").arg(obj->y())));
    rect.appendChild(y);
    QDomElement width = doc->createElement("width");
    width.appendChild(doc->createTextNode(QString("%1").arg(obj->width())));
    rect.appendChild(width);
    QDomElement height = doc->createElement("height");
    height.appendChild(doc->createTextNode(QString("%1").arg(obj->height())));
    rect.appendChild(height);
    geometry.appendChild(rect);
    element.appendChild(geometry);
    QDomElement text = doc->createElement("property");
    text.setAttribute("name", "text");
    QDomElement textString = doc->createElement("string");
    textString.appendChild(doc->createTextNode(QString(codec->toUnicode(((QPushButton*)obj)->text().toUtf8()))));
    text.appendChild(textString);
    element.appendChild(text);
    QDomElement icon = doc->createElement("property");
    icon.setAttribute("name", "icon");
    QDomElement str1, str2;
    str1 = doc->createElement("iconset");
    str1.setAttribute("resource", TApplication::resourcesFile());
    str2 = doc->createElement("normalon");
    str2.appendChild(doc->createTextNode(":" + ((QPushButton*)obj)->objectName()));
    str1.appendChild(str2);
    str2 = doc->createElement("normaloff");
    str2.appendChild(doc->createTextNode(":" + ((QPushButton*)obj)->objectName()));
    str1.appendChild(str2);
    str2 = doc->createElement("activeon");
    str2.appendChild(doc->createTextNode(":" + ((QPushButton*)obj)->objectName()));
    str1.appendChild(str2);
    str2 = doc->createElement("activeoff");
    str2.appendChild(doc->createTextNode(":" + ((QPushButton*)obj)->objectName()));
    str1.appendChild(str2);
    str2 = doc->createElement("selectedon");
    str2.appendChild(doc->createTextNode(":" + ((QPushButton*)obj)->objectName()));
    str1.appendChild(str2);
    str2 = doc->createElement("selectedoff");
    str2.appendChild(doc->createTextNode(":" + ((QPushButton*)obj)->objectName()));
    str1.appendChild(str2);
    icon.appendChild(str1);
    element.appendChild(icon);
    delete doc;
    return element;
}


QDomElement Form::createLabelElement(QString label) {
//#ifdef Q_OS_WIN32
//    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
//#else
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
//#endif
    QDomDocument* doc = new QDomDocument();
    QDomElement item, widget;
    widget = doc->createElement("widget");
    widget.setAttribute("class", "QLabel");
    widget.setAttribute("name", "label");
    item = doc->createElement("property");
    item.setAttribute("name", "sizePolicy");
    QDomElement sizePolicy = doc->createElement("sizePolicy");
    sizePolicy.setAttribute("hsizetype", "Fixed");
    sizePolicy.setAttribute("vsizetype", "Fixed");
    item.appendChild(sizePolicy);
    widget.appendChild(item);
    item = doc->createElement("property");
    item.setAttribute("name", "text");
    QDomElement textString = doc->createElement("string");
    textString.appendChild(doc->createTextNode(QString(codec->toUnicode(label.toUtf8()))));
    item.appendChild(textString);
    widget.appendChild(item);
    delete doc;
    return widget;
}


QDomElement Form::createLineEditElement(QLineEdit* obj) {
    QDomDocument* doc = new QDomDocument();
    QDomElement item, element, widget;
    widget = doc->createElement("widget");
    widget.setAttribute("class", obj->metaObject()->className());
    widget.setAttribute("name", obj->objectName());
    item = doc->createElement("property");
    item.setAttribute("name", "sizePolicy");
    QDomElement sizePolicy = doc->createElement("sizePolicy");
    sizePolicy.setAttribute("hsizetype", "Fixed");
    sizePolicy.setAttribute("vsizetype", "Fixed");
    item.appendChild(sizePolicy);
    widget.appendChild(item);
    item = doc->createElement("property");
    item.setAttribute("name", "font");
    QDomElement font = doc->createElement("font");
    element = doc->createElement("pointsize");
    element.appendChild(doc->createTextNode(QString("%1").arg(obj->font().pointSize())));
    font.appendChild(element);
    element = doc->createElement("bold");
    element.appendChild(doc->createTextNode(QString(obj->font().bold() ? "true" : "false")));
    font.appendChild(element);
    item.appendChild(font);
    widget.appendChild(item);
    item = doc->createElement("property");
    item.setAttribute("name", "readOnly");
    element = doc->createElement("bool");
    element.appendChild(doc->createTextNode(QString(obj->isReadOnly() ? "true" : "false")));
    item.appendChild(element);
    widget.appendChild(item);
    delete doc;
    return widget;
}


QDomElement Form::createDateEditElement(QtCore/QDateEdit* obj) {
    QDomDocument* doc = new QDomDocument();
    QDomElement item, element, widget;
    widget = doc->createElement("widget");
    widget.setAttribute("class", obj->metaObject()->className());
    widget.setAttribute("name", obj->objectName());
    item = doc->createElement("property");
    item.setAttribute("name", "sizePolicy");
    QDomElement sizePolicy = doc->createElement("sizePolicy");
    sizePolicy.setAttribute("hsizetype", "Fixed");
    sizePolicy.setAttribute("vsizetype", "Fixed");
    item.appendChild(sizePolicy);
    widget.appendChild(item);
    item = doc->createElement("property");
    item.setAttribute("name", "font");
    QDomElement font = doc->createElement("font");
    element = doc->createElement("pointsize");
    element.appendChild(doc->createTextNode(QString("%1").arg(obj->font().pointSize())));
    font.appendChild(element);
    element = doc->createElement("bold");
    element.appendChild(doc->createTextNode(QString(obj->font().bold() ? "true" : "false")));
    font.appendChild(element);
    item.appendChild(font);
    widget.appendChild(item);
    item = doc->createElement("property");
    item.setAttribute("name", "readOnly");
    element = doc->createElement("bool");
    element.appendChild(doc->createTextNode(QString(obj->isReadOnly() ? "true" : "false")));
    item.appendChild(element);
    widget.appendChild(item);
    delete doc;
    return widget;
}


QDomElement Form::createHStretchElement() {
    QDomDocument* doc = new QDomDocument();
    QDomElement element, str1;
    element = doc->createElement("spacer");
    element.setAttribute("name", "horizontalSpacer");
    QDomElement orientation = doc->createElement("property");
    orientation.setAttribute("name", "orientation");
    str1 = doc->createElement("enum");
    str1.appendChild(doc->createTextNode("Qt::Horizontal"));
    orientation.appendChild(str1);
    element.appendChild(orientation);
    QDomElement sizeHint = doc->createElement("property");
    sizeHint.setAttribute("name", "sizeHint");
    sizeHint.setAttribute("stdset", "0");
    QDomElement size = doc->createElement("size");
    QDomElement width = doc->createElement("width");
    width.appendChild(doc->createTextNode(QString("%1").arg(40)));
    size.appendChild(width);
    QDomElement height = doc->createElement("height");
    height.appendChild(doc->createTextNode(QString("%1").arg(20)));
    size.appendChild(height);
    sizeHint.appendChild(size);
    element.appendChild(sizeHint);
    delete doc;
    return element;
}


QDomElement Form::createVStretchElement() {
    QDomDocument* doc = new QDomDocument();
    QDomElement element, str1;
    element = doc->createElement("spacer");
    element.setAttribute("name", "vertucalSpacer");
    QDomElement orientation = doc->createElement("property");
    orientation.setAttribute("name", "orientation");
    str1 = doc->createElement("enum");
    str1.appendChild(doc->createTextNode("Qt::Vertical"));
    orientation.appendChild(str1);
    element.appendChild(orientation);
    QDomElement sizeHint = doc->createElement("property");
    sizeHint.setAttribute("name", "sizeHint");
    sizeHint.setAttribute("stdset", "0");
    QDomElement size = doc->createElement("size");
    QDomElement width = doc->createElement("width");
    width.appendChild(doc->createTextNode(QString("%1").arg(40)));
    size.appendChild(width);
    QDomElement height = doc->createElement("height");
    height.appendChild(doc->createTextNode(QString("%1").arg(20)));
    size.appendChild(height);
    sizeHint.appendChild(size);
    element.appendChild(sizeHint);
    delete doc;
    return element;
}


QDomElement Form::createWidgetsStructure() {
    QDomDocument* doc = new QDomDocument();
    QDomElement item;

    // создадим горизонтальный лейаут с кнопками
    QDomElement hboxLayout,
                hlayout;
    if (cmdButtonLayout != 0) {
        hlayout = doc->createElement("layout");
        hlayout.setAttribute("class", "QHBoxLayout");
        hlayout.setAttribute("name", cmdButtonLayout->objectName());
        item = doc->createElement("item");
        item.appendChild(createHStretchElement());
        hlayout.appendChild(item);
        if (buttonOk != 0) {
            item = doc->createElement("item");
            item.appendChild(createPushButtonElement((QWidget*)buttonOk));
            hlayout.appendChild(item);
        }
        if (buttonCancel != 0) {
            item = doc->createElement("item");
            item.appendChild(createPushButtonElement((QWidget*)buttonCancel));
            hlayout.appendChild(item);
        }
        hboxLayout = createBoxLayoutElement(cmdButtonLayout);
        hboxLayout.appendChild(hlayout);
    }

    QDomElement vlayout;
    if (vbxLayout != 0) {
       vlayout = doc->createElement("layout");
       vlayout.setAttribute("class", "QVBoxLayout");
       vlayout.setAttribute("name", vbxLayout->objectName());
       item = doc->createElement("item");
       item.appendChild(hboxLayout);
       vlayout.appendChild(item);
    }
    delete doc;
    return vlayout;
}


void Form::createUi() {
//#ifdef Q_OS_WIN32
//    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");
//#else
    QTextCodec* codec = QTextCodec::codecForName("UTF-8");
//#endif
    if (getConfigName().size() > 0) {
        QString path = app->applicationDirPath() + "/defaultforms";
        if (!QDir(path).exists())
            QDir().mkpath(path);
        QString fileName = path + "/" + getParent()->getTagName() + ".ui";
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream text(&file);
            QDomDocument* doc = new QDomDocument();
            QDomElement root;
            QDomElement cl;
            root = doc->createElement("UI");
            root.setAttribute("version", "4.0");
            root.setAttribute("stdsetdef", "1");
            cl = doc->createElement("class");
            cl.appendChild(doc->createTextNode("Dialog"));
            root.appendChild(cl);
            cl = doc->createElement("widget");
            cl.setAttribute("class", "Dialog");
            cl.setAttribute("name", formWidget->objectName());
            QDomElement nameString = doc->createElement("string");
            nameString.appendChild(doc->createTextNode(QString(codec->toUnicode(configName.toUtf8()))));
            QDomElement name = doc->createElement("property");
            name.setAttribute("name", "name");
            name.appendChild(nameString);
            cl.appendChild(name);
            QDomElement geometry = doc->createElement("property");
            geometry.setAttribute("name", "geometry");
            QDomElement rect = doc->createElement("rect");
            QDomElement xForm = doc->createElement("x");
            xForm.appendChild(doc->createTextNode(QString("%1").arg(formWidget->x())));
            rect.appendChild(xForm);
            QDomElement yForm = doc->createElement("y");
            yForm.appendChild(doc->createTextNode(QString("%1").arg(formWidget->y())));
            rect.appendChild(yForm);
            QDomElement widthForm = doc->createElement("width");
            widthForm.appendChild(doc->createTextNode(QString("%1").arg(formWidget->width())));
            rect.appendChild(widthForm);
            QDomElement heightForm = doc->createElement("height");
            heightForm.appendChild(doc->createTextNode(QString("%1").arg(formWidget->height())));
            rect.appendChild(heightForm);
            geometry.appendChild(rect);
            cl.appendChild(geometry);
            QDomElement caption = doc->createElement("property");
            caption.setAttribute("name", "caption");
            QDomElement captionString = doc->createElement("string");
            captionString.appendChild(doc->createTextNode(QString(codec->toUnicode(formWidget->windowTitle().toUtf8()))));
            caption.appendChild(captionString);
            cl.appendChild(caption);
            cl.appendChild(createWidgetsStructure());
            root.appendChild(cl);
            QDomElement resources = doc->createElement("resources");
            QDomElement include = doc->createElement("include");
            include.setAttribute("location", TApplication::resourcesFile());
            resources.appendChild(include);
            root.appendChild(resources);
            doc->appendChild(root);
            text << "<?xml " + doc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"").data() + "?>" << endl;
            text << doc->toString();
//            doc->save(text, 4);
            file.close();
            uiCreated = true;
            delete doc;
        }
    }
}
*/