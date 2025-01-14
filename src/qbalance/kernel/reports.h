
/************************************************************************************************************
Copyright (C) Morozov Vladimir Aleksandrovich MorozovVladimir@mail.ru

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

#ifndef REPORTS_H
#define REPORTS_H

#include <QtCore/QString>
#include "dictionary.h"
#include "../gui/calendarform.h"


class Reports : public Dictionary {

private:
CalendarForm* calendar;

protected:
    Q_INVOKABLE virtual void setForm(QString = "");

    Reports(QObject* parent = 0 /*nullptr*/);
    virtual void postInitialize(QObject* parent = 0 /*nullptr*/);

public:
    ~Reports();

    template <class T>
        static T* create(QObject* parent = 0 /*nullptr*/)
        {
            T* p(new T(parent));
            p->postInitialize(parent);
            p->open();
            return p;
        }

    void cmdOk();
    Q_INVOKABLE virtual void show();
};

#endif // REPORTS_H
