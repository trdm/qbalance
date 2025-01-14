/***************************************************************************
                          drvfr.h  -  description
                             -------------------
    begin                : Sun Jan 20 2002
    copyright            : (C) 2002 by Igor V. Youdytsky
    email                : Pitcher@gw.tander.ru
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

#include <QtCore/QTextCodec>
#include <QtCore/QString>
#include "driverfr.h"
#include <math.h>
#include "../kernel/tcpserver.h"
#include "../kernel/tcpclient.h"
#include "../kernel/app.h"



unsigned DriverFR::commlen[0x100] =
{
   0,  6,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x00 - 0x0f
   5,  5, 26,  5,  8,  6,  1, 46, 37,  6,  6,  6, 10,  5,  10,  9, // 0x10 - 0x1f
   6,  8,  8,  8,  5,  6,  0,  5,  6,  7,  5,  5,  5,  6,  7,  0, // 0x20 - 0x2f
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x30 - 0x3f
   5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x40 - 0x4f
  10, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x50 - 0x5f
   9,  1,  6,  5,  5, 20, 12, 10,  5,  6,  0,  0,  0,  0,  0,  0, // 0x60 - 0x6f
 255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x70 - 0x7f
  60, 60, 60, 60, 60, 71, 54, 54,  5,  5, 54, 54,  0,  0,  0,  0, // 0x80 - 0x8f
  61, 57, 52, 11, 12, 52,  7,  7,  7,  5, 13,  7,  0,  0,  7,  7, // 0x90 - 0x9f
  13, 11, 12, 10, 10,  8,  7,  5,  0,  0,  0,  0,  5,  5,  0,  0, // 0xa0 - 0xaf
   5,  0,  0,  5,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xb0 - 0xbf
  46,  7, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xc0 - 0xcf
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xd0 - 0xdf
   5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xe0 - 0xef
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,255,  0,  0  // 0xf0 - 0xff
};


const char* DriverFR::errmsg[] =
{
  "Ошибок нет",													//00
  "Не исправен накопитель ФП1, ФП2 или часы",					//01
  "Отсутствует ФП1",											//02
  "Отсутствует ФП2",											//03
  "Не корректные параметры в команде обращения к ФП",			//04
  "Нет запрошенных данных",										//05
  "ФП в режиме вывода данных",									//06
  "Не корректные параметры в команде для данной реализации ФП",	//07
  "Команда не поддерживается в данной реализации ФП",			//08
  "Не корректная длина команды",								//09
  "Формат данных не BCD",										//0a
  "Не исправна ячейка памяти ФП при записи итога",				//0b
  "",															//0c
  "",															//0d
  "",															//0e
  "",															//0f
  "",															//10
  "Не введена лицензия",										//11
  "Заводской номер уже введен",									//12
  "Текущая дата меньше даты последней записи в ФП",				//13
  "Область сменных итогов ФП переполнена",						//14
  "Смена уже открыта",											//15
  "Смена не открыта",											//16
  "Номер первой смены больше номера последней смены",			//17
  "Дата первой смены больше даты последней смены",				//18
  "Нет данных в ФП",											//19
  "Область перерегистраций в ФП переполнена",					//1a
  "Заводской номер не введен",									//1b
  "В заданном диапазоне есть поврежденная запись",				//1c
  "Повреждена последняя запись сменных итогов",					//1d
  "Область перерегистраций ФП переполнена",						//1e
  "Отсутствует память регистров",								//1f
  "Переполнение денежного регистра при добавлении",				//20
  "Вычитаемая сумма больше содержимого денежного регистра",		//21
  "Не верная дата",	     										//22
  "Нет записи активизации",										//23
  "Область активизации переполнена",							//24
  "Нет активизации с запрашиваемым номером",					//25
  "Вносимая клиентом сумма меньше суммы чека",					//26
  "",															//27
  "",															//28
  "",															//29
  "",															//2a
  "Не возможно отменить предыдущую команду",					//2b
  "Обнулённая касса (повторное гашение не возможно)",			//2c
  "Сумма чека по секции меньше суммы сторно",					//2d
  "В ФР нет денег для выплаты",									//2e
  "",															//2f
  "ФР заблокирован, ждет ввода пароля налогового инспектора",	//30
  "",															//31
  "Требуется выполнение общего гашения",						//32
  "Не корректные параметры в команде",							//33
  "Нет данных",                                                 //34
  "Не корректный параметр при данных настройках",				//35
  "Не корректные параметры в комманде для данной реализации ФР",//36
  "Команда не поддерживается в данной реализации ФР",			//37
  "Ошибка в ПЗУ",												//38
  "Внутренняя ошибка ПО ОЗУ",									//39
  "Переполнение накопления по надбавкам в смене",				//3a
  "Переполнение накопления в смене",							//3b
  "Смена открыта - операция не возможна",                       //3c
  "Смена не открыта - операция не возможна",                    //3d
  "Переполнение накопления по секциям в смене",					//3e
  "Переполнение накопления по скидкам в смене",					//3f
  "Переполнение диапазона скидок",								//40
  "Переполнение диапазона оплаты наличными",    				//41
  "Переполнение диапазона оплаты типом 2",						//42
  "Переполнение диапазона оплаты типом 3",						//43
  "Переполнение диапазона оплаты типом 4",						//44
  "Сумма всех типов оплаты меньше итога чека",					//45
  "Не хватает наличности в кассе",								//46
  "Переполнение накопления по налогам в смене",					//47
  "Переполнение итога чека",									//48
  "Операция не возможна в открытом чеке данного типа",			//49
  "Открыт чек - операция не возможна",							//4a
  "Буфер чека переполнен",										//4b
  "Переполнение накопления по обороту налогов в смене",			//4c
  "Вносимая безналичной оплатой сумма больше суммы чека",		//4d
  "Смена превысила 24 часа",									//4e
  "Не верный пароль",											//4f
  "Обработка предыдущей команды",								//50
  "Переполнение накоплений наличными в смене",					//51
  "Переполнение накоплений по типу оплаты 2 в смене",			//52
  "Переполнение накоплений по типу оплаты 3 в смене",			//53
  "Переполнение накоплений по типу оплаты 4 в смене",			//54
  "Чек закрыт - операция не возможна",							//55
  "Нет документа для повтора",									//56
  "ЭКЛЗ: количество закрытых смен не совпадает с ФП",   		//57
  "Ожидание команды продолжения печати",        				//58
  "Документ открыт другим оператором",							//59
  "Скидка превышает накопления в чеке", 						//5a
  "Переполнение диапазона надбавок",							//5b
  "Понижено напряжение 24В",									//5c
  "Таблица не определена",										//5d
  "Не корректная операция",										//5e
  "Отрицательный итог чека",									//5f
  "Переполнение при умножении",									//60
  "Переполнение диапазона цены",								//61
  "Переполнение диапазона количества",							//62
  "Переполнение диапазона отдела",								//63
  "ФП отсутствует", 											//64
  "Не хватает денег в секции",									//65
  "Переполнение денег в секции",								//66
  "Ошибка связи с ФП",											//67
  "Не хватает денег по обороту налогов",						//68
  "Переполнение денег по обороту налогов",						//69
  "Ошибка питания в момент ответа по I2C",  					//6a
  "Нет чековой ленты",      									//6b
  "Нет контрольной ленты",										//6c
  "Не хватает денег по налогу",									//6d
  "Переполнение денег по налогу",								//6e
  "Переполнение по выплате в смене",							//6f
  "Переполнение ФП",											//70
  "Ошибка отрезчика",											//71
  "Команда не поддерживается в данном подрежиме",				//72
  "Команда не поддерживается в данном режиме",					//73
  "Ошибка ОЗУ",                                                 //74
  "Ошибка питания",                                             //75
  "Ошибка принтера: нет импульсов с тахогенератора",            //76
  "Ошибка принтера: нет сигнала с датчиков",                    //77
  "Замена ПО",                                                  //78
  "Замена ФП",                                                  //79
  "Поле не редактируется",                                      //7a
  "Ошибка оборудования",                                        //7b
  "Не совпадает дата",                                          //7c
  "Не верный формат даты",                                      //7d
  "Не верное значение в поле длины",                            //7e
  "Переполнение диапазона итога чека",                          //7f
  "Ошибка связи с ФП",                                          //80
  "Ошибка связи с ФП",                                          //81
  "Ошибка связи с ФП",                                          //82
  "Ошибка связи с ФП",                                          //83
  "Переполнение наличности",                                    //84
  "Переполнение по продажам в смене",                           //85
    "",         // 86
    "",         // 87
    "",         // 88
    "",         // 89
    "",         // 8a
    "",         // 8b
    "",         // 8c
    "",         // 8d
    "",         // 8e
    "",         // 8f

    "",         // 90
    "",         // 91
    "",         // 92
    "",         // 93
    "",         // 94
    "",         // 95
    "",         // 96
    "",         // 97
    "",         // 98
    "",         // 99
    "",         // 9a
    "",         // 9b
    "",         // 9c
    "",         // 9d
    "",         // 9e
    "",         // 9f

    "",         // a0
    "",         // a1
    "",         // a2
    "Не корректное состояние ЭКЛЗ",         // a3
    "",         // a4
    "",         // a5
    "",         // a6
    "",         // a7
    "",         // a8
    "ЭКЛЗ: Нет запрошенных данных",         // a9
    "",         // aa
    "",         // ab
    "",         // ac
    "",         // ad
    "",         // ae
    "",         // af

    "",         // b0
    "",         // b1
    "",         // b2
    "",         // b3
    "",         // b4
    "",         // b5
    "",         // b6
    "",         // b7
    "",         // b8
    "",         // b9
    "",         // ba
    "",         // bb
    "",         // bc
    "",         // bd
    "",         // be
    "",         // bf

    "",         // c0
    "",         // c1
    "",         // c2
    "",         // c3
    "",         // c4
    "",         // c5
    "",         // c6
    "",         // c7
    "",         // c8
    "",         // c9
    "",         // ca
    "",         // cb
    "",         // cc
    "",         // cd
    "",         // ce
    "",         // cf

    "",         // d0
    "",         // d1
    "",         // d2
    "",         // d3
    "",         // d4
    "",         // d5
    "",         // d6
    "",         // d7
    "",         // d8
    "",         // d9
    "",         // da
    "",         // db
    "",         // dc
    "",         // dd
    "",         // de
    "",         // df

    "",         // e0
    "",         // e1
    "",         // e2
    "",         // e3
    "",         // e4
    "",         // e5
    "",         // e6
    "",         // e7
    "",         // e8
    "",         // e9
    "",         // ea
    "",         // eb
    "",         // ec
    "",         // ed
    "",         // ee
    "",         // ef

    "",         // f0
    "",         // f1
    "",         // f2
    "",         // f3
    "",         // f4
    "",         // f5
    "",         // f6
    "",         // f7
    "",         // f8
    "",         // f9
    "",         // fa
    "",         // fb
    "",         // fc
    "",         // fd
    "",         // fe
    ""         // ff
};


const char* DriverFR::errrecomendations[] =
{
    "Выполните \"Прекращение ЭКЛЗ\" в меню ФР",        // 1
};


recomendations DriverFR::referrrecomendations[] =
{
    {163, 1},
    {0, 0}              // Признак конца массива
};


const char* DriverFR::ecrmodedesc[] =
{
  "",
  "Data output",													//0
  "Shift is opened less then 24 hours",											//1
  "Shift is opened more then 24 hours",											//2
  "Shift is closed",													//3
  "Blocked due to wrong Tax Inspector password entry",									//4
  "Waiting for date entry confirmation",										//5
  "Changing decimal point position mode",										//6
  "Document is opened",													//7
  "Tech. reset & tables init. mode",											//8
  "Test feed",														//9
  "Full fiscal report printing",											//10
  "EKLZ report printing"												//11
};


const char* DriverFR::ecrmode8desc[] =
{
  "Sale check is opened",
  "Buy check is opened",
  "Sale return check is opened",
  "Buy return check is opened"
};


const char* DriverFR::ecrsubmodedesc[] =
{
  "",
  "Passive out of paper",
  "Active out of paper",
  "Waiting for print continue",
  "Printing operation",
  "Full printing operation"
};


const char* DriverFR::devcodedesc[] =
{
  "FM1 flash",
  "FM2 flash",
  "Clock",
  "Powerindependent memory",
  "FM processor",
  "FR usr memory",
  "FR base memory"
};


const char* DriverFR::failConnectErrorMessage = "Не удалось соединиться с фискальным регистратором.";


DriverFR::DriverFR(QObject *parent) : QObject(parent)
{
    connected = false;
    remote = false;
    locked = false;
    codec = QTextCodec::codecForName("Windows-1251");
    app = 0 /*nullptr*/;
    progressDialog = 0 /*nullptr*/;
    showProgressBar = false;
    maxTries = MAX_TRIES;
    serialPort = 0 /*nullptr*/;
}


DriverFR::~DriverFR()
{
    delete progressDialog;
}


bool DriverFR::open(QString port, int rate, int timeout, int password)
{
    bool result = false;
    if (app == 0 /*nullptr*/)
        return result;
    locked = false;
    // Установление связи с ккм
    app->showMessageOnStatusBar("Подключаемся к ФР", -1);

    fr.BaudRate      = rate;
    fr.Timeout       = timeout;
    fr.Password      = password;
    serialPort = app->getSerialPort(port);
    if (serialPort != 0 /*nullptr*/)
    {
        // Сначала поищем на удаленном компьютере
        if (serialPort->getTcpClient()->isValid() && app->getConfigValue("FR_USE_REMOTE").toBool())
        {
            remote = true;
            serialPort->setRemote(remote);

//            fr.PortNumber = 0 /*nullptr*/;
//            fr.BaudRate = app->getConfigValue("FR_DRIVER_BOUD_RATE").toInt();
//            if (remote)
//                fr.Timeout = app->getConfigValue("FR_REMOTE_DRIVER_TIMEOUT").toInt();
//            else
//                fr.Timeout = app->getConfigValue("FR_LOCAL_DRIVER_TIMEOUT").toInt();

            serialPort->writeLog(QString("Порт: %1").arg(port));
            serialPort->writeLog(QString("Скорость: %1").arg(rate));
            serialPort->writeLog(QString("Таймаут: %1").arg(timeout));

            if (Connect(false))
            {
                if (app->getConfigValue("FR_CONNECT_SIGNAL").toBool())
                    Beep();
                result = true;
            }
            DisConnect();
        }
        else
            serialPort->getTcpClient()->logError();                 // К удаленному фискальнику не удалось подсоединиться
        if (!result)
        {
            // А теперь поищем фискальник на локальном компьютере
            remote = false;
            serialPort->setRemote(remote);
            serialPort->setBaudRate(rate);
#if  defined(Q_OS_LINUX)
            if (serialPort->open(QIODevice::ReadWrite) && serialPort->isOpen())
#elif   defined(Q_OS_WIN)
            if (serialPort->open(QIODevice::ReadWrite | QIODevice::Unbuffered) && serialPort->isOpen())
#endif
            {
                if (Connect(false))
                {
                    Beep();
                    result = true;
                }
                DisConnect();
            }
        }
        if (result)
        {
            serialPort->setRemote(remote);
            if (app->getGUIFactory() != 0 /*nullptr*/)
            {
                progressDialog = app->getMyProgressDialog(trUtf8("Ожидайте окончания работы фискального регистратора..."));
                progressDialog->resize(600, progressDialog->height());
            }
        }
    }
/*
    if (!result)
        app->showError(QString(failConnectErrorMessage).append(" ").append(serialPort->errorString()));
*/
    app->clearMessageOnStatusBar();
    return result;
}


void DriverFR::close()
{
    if (serialPort != 0)
    {
        if (connected)
            serialPort->close();
        delete serialPort;
    }
    connected = false;
}


bool DriverFR::Connect(bool showError)
{
    connected = false;
    bool result = false;

    if (!isLocked())
    {
        setLock(true);
        if (isLocked())
        {
            connected = true;

            if (GetECRStatus() == 0)
            {
                if (app->isDebugMode(4))
                {
                    serialPort->writeLog(QString("Режим: %1 %2").arg(fr.ECRMode).arg(fr.ECRModeDescription));
                    serialPort->writeLog(QString("Подрежим: %1 %2").arg(fr.ECRAdvancedMode).arg(fr.ECRAdvancedModeDescription));
                }
                if (showProgressBar)
                    progressDialog->show();
                result = true;
            }
            else
            {
                connected = false;
                serialPort->writeLog(QString("Ошибка соединения"));
            }
        }
    }
    if (!result && showError)
    {
        app->showError(failConnectErrorMessage);
    }
    return result;
}


void DriverFR::DisConnect()
{
    if (showProgressBar)
    {
        progressDialog->hide();
    }
    serialPort->writeLog();
    connected = false;

    setLock(false);
}


void DriverFR::setShowProgressBar(bool show)
{
    showProgressBar = show;
    if (showProgressBar)
    {
        if (!progressDialog->isVisible())
            progressDialog->show();
    }
    else
    {
        if (progressDialog->isVisible())
            progressDialog->hide();
    }
}


bool DriverFR::isLocked()
{
    if (remote)
        locked = serialPort->isLockedDriverFR();
    else
    {
        if (lockedByHost.size() > 0)        // Если фискальник как будто заблокирован
        {                                   // то сначала проверим, в сети ли хост, который его заблокировал
            app->getTcpServer()->pingClient(lockedByHost);              // Пошлем пинг
            if (!app->getTcpServer()->getPingOk())                      // Если пинг не пришел, значит клиент не в сети
            {
                setLock(false);             // то разблокируем его
            }
        }
    }
    return locked;
}


void DriverFR::setLock(bool lock, QString lockedBy)
{
    if (remote)
        locked = serialPort->setLock(lock);
    else
        lockedByHost = lockedBy;
    locked = lock;
}


void DriverFR::setProgressDialogValue(int value)
{
    if (progressDialog != 0)
        progressDialog->setValue(value);
}


int DriverFR::sendENQ()
{
    qint64 result;
    char buff[2];
    buff[0] = ENQ;
    result = serialPort->writeData(buff,1);
    return result;
}


int DriverFR::sendNAK()
{
    qint64 result;
    char buff[2];
    buff[0] = NAK;
    result = serialPort->writeData(buff, 1);
    return result;
}


int DriverFR::sendACK()
{
    qint64 result;
    char buff[2];
    buff[0] = ACK;
    result = serialPort->writeData(buff,1);
    return result;
}


short int DriverFR::readByte()
{
    short int result = -1;
    unsigned char readbuff[1] = "";
    result = readBytes(readbuff, 1);
    if (result >= 0)
    {
        result = (unsigned int) readbuff[0];
    }
    return result;
}



short int DriverFR::readBytes(unsigned char *buff, int len)
{
    short int result = -1;
    int readed = 0;
    for (int i = 0; i < len; i++)
        buff[i] = 0;
    result = serialPort->readData((char*)(buff + readed), len - readed);
    return result;
}


int DriverFR::readAnswer(answer *ans, short int byte)
{
    int result = -1;
    short int  repl = byte > 0 ? byte : readByte();
    if (repl == STX)
    {
        result = readMessage(ans);
    }
    else if (repl == ACK)
    {
        result = readMessage(ans);
    }
    return result;
}


int DriverFR::readMessage(answer *ans)
{
    int result = -1;
    short int crc;
    short int len = readByte();
    if (len > 0)
    {
        short int readedLen = readBytes(ans->buff, len);
        if (readedLen == len)
        {
            crc = readByte();
            if (crc == (LRC(ans->buff, len, 0) ^ len))
            {
                sendACK();
                ans->len = len;
                result = len;
                return result;
            }
            else
            {
                serialPort->writeLog("Не сходится контрольная сумма");
            }
        }
        else
        {
            serialPort->writeLog(QString("Прочитано %1 вместо %2 байт").arg(readedLen).arg(len));
        }
        sendNAK();
      }
    return result;
}

unsigned short int DriverFR::LRC(unsigned char *str, int len, int offset)
{
  int i;
  unsigned char *ptr;
  unsigned char ch = 0;

  ptr = str + offset;
  for(i=0; i<len; i++)ch ^= ptr[i];
  return ch;
}


int DriverFR::composeComm(command *cmd, int comm, int pass, parameter *param)
{
    int len;
    for (unsigned int i = 0; i < sizeof(cmd->buff); i++)
        cmd->buff[i] = 0;

    len = commlen[comm];
    if (param->len == 0 && len >= 5)
        param->len = len - 5;
    else
        len = param->len + 5;
    cmd->buff[0] = STX;
    cmd->buff[1] = len;
    cmd->buff[2] = comm;
    if (len >= 5)
    {
        memcpy(cmd->buff + 3, &pass, sizeof(int));
        if (param->len > 0)
            memcpy(cmd->buff + 7, param->buff, param->len);
    }
    cmd->buff[len + 2] = LRC(cmd->buff, len + 1, 1);
    cmd->len = len + 3;
    return 1;
}


int DriverFR::sendCommand(int comm, int pass, parameter *param)
{
    command cmd;
    int result = -1;
    composeComm(&cmd, comm, pass, param);

    if (serialPort->writeData((char *)cmd.buff, cmd.len) != -1)
    {
        for (int tries = 1; tries <= maxTries; tries++)    // будем в цикле передавать сообщение
        {
            short int repl = readByte();
            if (repl == ACK)                // Если сообщение получено
            {
                result = 1;                 // установим положительный результат
                break;                      // и выйдем из цикла
            }
            else if (repl == NAK)
                break;
            else if (repl == -1)
                break;
        }
    }
    return result;
}


bool DriverFR::deviceIsReady()
{
    sendENQ();
    for (int i = 0; i < maxTries; i++)
    {
        short int repl = readByte();
        if (repl == -1)
        {
            return false;
        }
        else if (repl == NAK)
        {
            return true;
        }
        else if (repl == ACK)                   // В случае, если устройство все еще пытается передать ответ
        {                                       // от предыдущей команды
            answer     a = {0, {0}};
            readAnswer(&a);
            sendENQ();
        }
    }
    return false;
}


QVariant DriverFR::getProperty(QString name)
{
    QVariant result;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    const char* propName = name.toLatin1().data();
    result = fr.property(propName);
/*
    if (result.type() == QVariant::String)
    {
        result = QTextCodec::toUnicode(result.data());
    }
*/
    QTextCodec::setCodecForLocale(app->codec());
    return result;
}


bool DriverFR::setProperty(QString name, QVariant value)
{
    bool result;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    const char* propName = name.toLatin1().data();
    result = fr.setProperty(propName, value);
    QTextCodec::setCodecForLocale(app->codec());
    return result;
}


int DriverFR::errHand(answer *a)
{
  fr.ResultCode = a->buff[1];
  fr.ResultCodeDescription = (char*)errmsg[fr.ResultCode];
  return fr.ResultCode;
}


int DriverFR::evalint(unsigned char *str, int len)
{
    int result = 0;
    while(len--)
    {
        result <<= 8;
        result += str[len];
    }
    return result;
}


int32_t DriverFR::evalint32(unsigned char *str, int len)
{
    int32_t result = 0;
    while(len--)
    {
        result <<= 8;
        result += str[len];
    }
    return result;
}


int64_t DriverFR::evalint64(unsigned char *str, int len)
{
    int64_t result = 0;
    while(len--)
    {
        result <<= 8;
        result += str[len];
    }
    return result;
}


void DriverFR::evaldate(unsigned char *str, struct tm *date)
{
  date->tm_mday = evalint(str    , 1);
  date->tm_mon  = evalint(str + 1, 1) - 1;
  date->tm_year = evalint(str + 2, 1) + 100;
  mktime(date);
}


void DriverFR::evaltime(unsigned char *str, struct tm *time)
{
  time->tm_hour = evalint(str    , 1);
  time->tm_min  = evalint(str + 1, 1);
  time->tm_sec  = evalint(str + 2, 1);
  mktime(time);
}


void DriverFR::logCommand(int comNum, QString comStr)
{
    serialPort->writeLog(QString("Команда %1 %2").arg(QString::number(comNum, 16).toUpper()).arg(comStr));
}


void DriverFR::writeLog(QString str)
{
    serialPort->writeLog(str);
}


void DriverFR::DefineECRModeDescription(void)
{
  fr.ECRMode8Status = fr.ECRMode >> 4;
  if((fr.ECRMode & 8) == 8)
  {
    fr.ECRModeDescription = (char*)ecrmode8desc[fr.ECRMode8Status];
    return;
  }
  fr.ECRModeDescription = (char*)ecrmodedesc[fr.ECRMode];
}


int DriverFR::processCommand(int command, parameter* p, answer* a)
{
    int result = -1;
    int attempts = 1;
    int timeOut = app->getConfigValue("FR_DRIVER_MAX_TIMEOUT").toInt() * 1000;
    QTime dieTime= QTime::currentTime().addMSecs(timeOut);

    if (deviceIsReady())
    {
        while (true)
        {
            sendCommand(command, fr.Password, p);
            result = readAnswer(a);
            if (result > 0)
            {
                if (a->buff[0] == command)
                {
                    if (errHand(a) != 0)
                    {
                        result = fr.ResultCode;
                    }
                    else
                        result = 0;
                }
                else
                   result = -1;
            }
            if (QTime::currentTime() >= dieTime)    // Время ожидания вышло, прекратим попытки
            {
                serialPort->writeLog(QString("Result:%1. Истек таймаут %2 мс").arg(result).arg(timeOut));
                break;
            }
            if ((/*(result < 0) || */(result == 0x50)))
            {
                attempts++;
                app->showMessageOnStatusBar(QString("Попытка %1%2/%3").arg(remote ? "удаленного соединения " : "").arg(attempts).arg(maxTries), -1);
                serialPort->writeLog();
                app->sleep(1000);
                serialPort->writeLog(QString("Result:%1. Повтор команды").arg(result));
                if (!deviceIsReady())
                {
                    break;
                }
            }
            else
            {
                if (result != 0)
                    serialPort->writeLog(QString("Result:%1").arg(result));
                break;
            }
        }
    }
    if (result > 0)
    {
        QString error(fr.ResultCodeDescription);
        for (int i = 0;referrrecomendations[i].error != 0; i++)
        {
            if (referrrecomendations[i].error == result)
            {
                error.append(". ");
                error.append(errrecomendations[referrrecomendations[i].recomendation - 1]);
                break;
            }
        }
        app->showError(error);
        serialPort->writeLog(error);
    }
    else
        fr.OperatorNumber = a->buff[2];

    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::Beep()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(BEEP, "Гудок");
        result = processCommand(BEEP, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::Buy()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;
    p.len   = 55;

    if (connected)
    {
        logCommand(BUY, "Покупка");

        int64_t quant = llround(fr.Quantity * 1000);
        int64_t price = llround(fr.Price * 100);
        memcpy(p.buff,    &quant, 5);
        memcpy(p.buff+5,  &price, 5);
        p.buff[10] = fr.Department;
        p.buff[11] = fr.Tax1;
        p.buff[12] = fr.Tax2;
        p.buff[13] = fr.Tax3;
        p.buff[14] = fr.Tax4;
        memcpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

        result = processCommand(BUY, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::CutCheck()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 1;
    int result = -1;

    if (connected)
    {
        logCommand(CUT_CHECK, "Отрезать чек");
        p.buff[0] = fr.CutType;
        result = processCommand(CUT_CHECK, &p, &a);
    }

    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::PrintString(QString str, int count)
{
    int result = 0;
    if (str.length() > 0)
    {
        result = -1;
        if (connected)
        {
            parameter  p = {0, {0}};
            answer     a = {0, {0}};
            p.len	  = str.length() + 1;

            logCommand(PRINT_STRING, "Печать строки");

            setProperty("UseReceiptRibbon", 1);
            setProperty("UseJournalRibbon", 0);
            setProperty("StringForPrinting", str);

            p.buff[0]  = (fr.UseJournalRibbon == true) ? 1 : 0;
            p.buff[0] |= (fr.UseReceiptRibbon == true) ? 2 : 0;
            memcpy((char*)&p.buff+1, (char*)fr.StringForPrinting, str.length());

            for (int i = 1; i <= count; i++)
            {
                result = processCommand(PRINT_STRING, &p, &a);
                if (result == -1)
                    return result;
            }
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::PrintWideString()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len	  = 21;
    int result = -1;

    if (connected)
    {
        logCommand(PRINT_WIDE_STRING, "Печать широкой строки");

        p.buff[0]  = (fr.UseJournalRibbon == true) ? 1 : 0;
        p.buff[0] |= (fr.UseReceiptRibbon == true) ? 2 : 0;
        memcpy((char*)&p.buff+1, (char*)fr.StringForPrinting, 20);

        result = processCommand(PRINT_WIDE_STRING, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::FeedDocument()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 2;
    int result = -1;

    if (connected)
    {
        logCommand(FEED_DOCUMENT, "Протяжка");

        p.buff[0]  = (fr.UseJournalRibbon == true) ? 1 : 0;
        p.buff[0] |= (fr.UseReceiptRibbon == true) ? 2 : 0;
        p.buff[0] |= (fr.UseSlipDocument  == true) ? 4 : 0;
        p.buff[1] = fr.StringQuantity;

        result = processCommand(FEED_DOCUMENT, &p, &a);
    }
    return result;
}


int DriverFR::FeedDocument(int count)
{
    int result = -1;
    setProperty("UseReceiptRibbon", 1);
    setProperty("UseJournalRibbon", 0);
    setProperty("StringQuantity", count);
    result = FeedDocument();
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::SetExchangeParam()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
//    p.len = 3;
    int result = -1;

    if (connected)
    {
        logCommand(SET_EXCHANGE_PARAM, "Установка параметров обмена");

        p.buff[0] = fr.PortNumber;
        p.buff[1] = fr.BaudRate;
        p.buff[2] = fr.Timeout;

//        serialPort->writeLog(QString("Порт: %1, Скорость: %2, Таймаут: %3").arg(fr.PortNumber).arg(fr.BaudRate).arg(fr.Timeout));

        result = processCommand(SET_EXCHANGE_PARAM, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetExchangeParam()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len = 1;
    int result = -1;

    if (connected)
    {
        logCommand(GET_EXCHANGE_PARAM, "Чтение параметров обмена");

        p.buff[0] = fr.PortNumber;

        result = processCommand(GET_EXCHANGE_PARAM, &p, &a);
        if (result == 0)
        {
            fr.BaudRate = a.buff[2];
            fr.Timeout  = a.buff[3];
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetShortECRStatus()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};
  int result = -1;

  if (connected)
  {
    logCommand(GET_SHORT_ECR_STATUS, "Короткий запрос состояния ФР");

    result = processCommand(GET_SHORT_ECR_STATUS, &p, &a);
    if (result == 0)
    {
        fr.OperatorNumber = a.buff[2];
        fr.ReceiptRibbonIsPresent     = (a.buff[3] &   1)  ==   1; //0
        fr.JournalRibbonIsPresent     = (a.buff[3] &   2)  ==   2; //1
        fr.SlipDocumentIsPresent      = (a.buff[3] &   4)  ==   4; //2
        fr.SlipDocumentIsMoving       = (a.buff[3] &   8)  ==   8; //3
        fr.PointPosition              = (a.buff[3] &  16)  ==  16; //4
        fr.EKLZIsPresent              = (a.buff[3] &  32)  ==  32; //5
        fr.JournalRibbonOpticalSensor = (a.buff[3] &  64)  ==  64; //6
        fr.ReceiptRibbonOpticalSensor = (a.buff[3] & 128)  == 128; //6
        fr.JournalRibbonLever         = (a.buff[4] &   1)  ==   1; //0
        fr.ReceiptRibbonLever         = (a.buff[4] &   2)  ==   2; //1
        fr.LidPositionSensor          = (a.buff[4] &   4)  ==   4; //2
        fr.ECRMode  = evalint((unsigned char*)&a.buff + 5, 1);
        DefineECRModeDescription();
        fr.ECRAdvancedMode = evalint((unsigned char*)&a.buff + 6, 1);
        fr.ECRAdvancedModeDescription = (char*)ecrsubmodedesc[fr.ECRAdvancedMode];
        fr.QuantityOfOperations = a.buff[7];

//        fr.PortNumber  = evalint((unsigned char*)&a.buff + 17, 1);
//        fr.FMSoftVersion[0] = a.buff[18];
//        fr.FMSoftVersion[1] = 0x2e;
//        fr.FMSoftVersion[2] = a.buff[19];
//        fr.FMSoftVersion[3] = 0 /*nullptr*/;
//        fr.FMBuild = evalint((unsigned char*)&a.buff + 20, 2);
//        evaldate((unsigned char*)&a.buff + 22, &fr.FMSoftDate);
//        evaldate((unsigned char*)&a.buff + 25, &fr.Date);
//        evaltime((unsigned char*)&a.buff + 28, &fr.Time);
//        fr.FM1IsPresent = (a.buff[31] & 1) == 1;
//        fr.FM2IsPresent = (a.buff[31] & 2) == 2;
//        fr.LicenseIsPresent = (a.buff[31] & 4) == 4;
//        fr.FMOverflow = (a.buff[31] & 8) == 8;
//        fr.BatteryCondition = (a.buff[31] & 16) == 16;
//        sprintf(fr.SerialNumber, "%d", evalint((unsigned char*)&a.buff + 32, 4));
//        fr.SessionNumber = evalint((unsigned char*)&a.buff + 36, 2);
//        fr.FreeRecordInFM = evalint((unsigned char*)&a.buff + 38, 2);
//        fr.RegistrationNumber = evalint((unsigned char*)&a.buff + 40, 1);
//        fr.FreeRegistration = evalint((unsigned char*)&a.buff + 41, 1);
//        sprintf(fr.INN, "%.0lg", (double)evalint64((unsigned char*)&a.buff + 42, 6));

    }
  }
  return result;
}



//-----------------------------------------------------------------------------
int DriverFR::GetECRStatus()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};
  int result = -1;

  if (connected)
  {
    logCommand(GET_ECR_STATUS, "Запрос состояния ФР");

    result = processCommand(GET_ECR_STATUS, &p, &a);
    if (result == 0)
    {
        fr.OperatorNumber = a.buff[2];
        fr.ECRSoftVersion[0] = a.buff[3];
        fr.ECRSoftVersion[1] = 0x2e;
        fr.ECRSoftVersion[2] = a.buff[4];
        fr.ECRSoftVersion[3] = 0;
        fr.ECRBuild = evalint((unsigned char*)&a.buff + 5, 2);
        evaldate((unsigned char*)&a.buff + 7, &fr.ECRSoftDate);
        fr.LogicalNumber = evalint((unsigned char*)&a.buff + 10, 1);
        fr.OpenDocumentNumber = evalint((unsigned char*)&a.buff + 11, 2);
        fr.ReceiptRibbonIsPresent     = (a.buff[13] &   1)  ==   1; //0
        fr.JournalRibbonIsPresent     = (a.buff[13] &   2)  ==   2; //1
        fr.SlipDocumentIsPresent      = (a.buff[13] &   4)  ==   4; //2
        fr.SlipDocumentIsMoving       = (a.buff[13] &   8)  ==   8; //3
        fr.PointPosition              = (a.buff[13] &  16)  ==  16; //4
        fr.EKLZIsPresent              = (a.buff[13] &  32)  ==  32; //5
        fr.JournalRibbonOpticalSensor = (a.buff[13] &  64)  ==  64; //6
        fr.ReceiptRibbonOpticalSensor = (a.buff[13] & 128)  == 128; //6
        fr.JournalRibbonLever         = (a.buff[14] &   1)  ==   1; //0
        fr.ReceiptRibbonLever         = (a.buff[14] &   2)  ==   2; //1
        fr.LidPositionSensor          = (a.buff[14] &   4)  ==   4; //2
        fr.ECRMode  = evalint((unsigned char*)&a.buff + 15, 1);
        DefineECRModeDescription();
        fr.ECRAdvancedMode = evalint((unsigned char*)&a.buff + 16, 1);
        fr.ECRAdvancedModeDescription = (char*)ecrsubmodedesc[fr.ECRAdvancedMode];
        fr.PortNumber  = evalint((unsigned char*)&a.buff + 17, 1);
        fr.FMSoftVersion[0] = a.buff[18];
        fr.FMSoftVersion[1] = 0x2e;
        fr.FMSoftVersion[2] = a.buff[19];
        fr.FMSoftVersion[3] = 0;
        fr.FMBuild = evalint((unsigned char*)&a.buff + 20, 2);
        evaldate((unsigned char*)&a.buff + 22, &fr.FMSoftDate);
        evaldate((unsigned char*)&a.buff + 25, &fr.Date);
        evaltime((unsigned char*)&a.buff + 28, &fr.Time);
        fr.FM1IsPresent = (a.buff[31] & 1) == 1;
        fr.FM2IsPresent = (a.buff[31] & 2) == 2;
        fr.LicenseIsPresent = (a.buff[31] & 4) == 4;
        fr.FMOverflow = (a.buff[31] & 8) == 8;
        fr.BatteryCondition = (a.buff[31] & 16) == 16;
        sprintf(fr.SerialNumber, "%d", evalint((unsigned char*)&a.buff + 32, 4));
        fr.SessionNumber = evalint((unsigned char*)&a.buff + 36, 2);
        fr.FreeRecordInFM = evalint((unsigned char*)&a.buff + 38, 2);
        fr.RegistrationNumber = evalint((unsigned char*)&a.buff + 40, 1);
        fr.FreeRegistration = evalint((unsigned char*)&a.buff + 41, 1);
        sprintf(fr.INN, "%.0lg", (double)evalint64((unsigned char*)&a.buff + 42, 6));
    }
  }
  return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetDeviceMetrics()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 1;
    int result = -1;

    if (connected)
    {
        logCommand(GET_DEVICE_METRICS, "Получить параметры устройства");

        result = processCommand(GET_DEVICE_METRICS, &p, &a);
        if (result == 0)
        {
            fr.UMajorType            = evalint((unsigned char*)&a.buff + 2, 1);
            fr.UMinorType            = evalint((unsigned char*)&a.buff + 3, 1);
            fr.UMajorProtocolVersion = evalint((unsigned char*)&a.buff + 4, 1);
            fr.UMinorProtocolVersion = evalint((unsigned char*)&a.buff + 5, 1);
            fr.UModel                = evalint((unsigned char*)&a.buff + 6, 1);
            fr.UCodePage             = evalint((unsigned char*)&a.buff + 7, 1);
            QByteArray data;
            data.append((char*)&a.buff+8);
            fr.UDescription = codec->toUnicode(data);
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::Test()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 1;
    int result = -1;

    if (connected)
    {
        logCommand(TEST, "Тест");
        result = processCommand(TEST, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::InterruptTest()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 1;
    int result = -1;

    if (connected)
    {
        logCommand(INTERRUPT_TEST, "Прервать тест");
        result = processCommand(INTERRUPT_TEST, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::ContinuePrint()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(CONTINUE_PRINTING, "Продолжение печати");
        result = processCommand(CONTINUE_PRINTING, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::OpenDrawer()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 1;
    int result = -1;

    if (connected)
    {
        logCommand(OPEN_DRAWER, "Открыть рисование");

        p.buff[0] = fr.DrawerNumber;

        result = processCommand(OPEN_DRAWER, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::PrintDocumentTitle()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len	  = 32;
    int result = -1;

    if (connected)
    {
        logCommand(PRINT_DOCUMENT_TITLE, "Печать заголовка документа");

        memcpy((char*)p.buff, (char*)fr.DocumentName, 30);
        memcpy((char*)p.buff+30, &fr.DocumentNumber, 2);

        result = processCommand(PRINT_DOCUMENT_TITLE, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::ResetSettings()
{
    answer     a = {0, {0}};
    parameter  p = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(RESET_SETTINGS, "Сброс настроек");
        result = processCommand(RESET_SETTINGS, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::ResetSummary()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};
  int result = -1;

  if (connected)
  {
      logCommand(RESET_SUMMARY, "Сброс сумм");
      result = processCommand(RESET_SUMMARY, &p, &a);
  }
  return result;
}


//-----------------------------------------------------------------------------
int DriverFR::ReturnBuy()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len   = 55;
    int result = -1;

    if (connected)
    {
        logCommand(RETURN_BUY, "Возврат покупки");

        int64_t quant = llround(fr.Quantity * 1000);
        int64_t price = llround(fr.Price * 100);
        memcpy(p.buff,    &quant, 5);
        memcpy(p.buff+5,  &price, 5);
        p.buff[10] = fr.Department;
        p.buff[11] = fr.Tax1;
        p.buff[12] = fr.Tax2;
        p.buff[13] = fr.Tax3;
        p.buff[14] = fr.Tax4;
        memcpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

        result = processCommand(RETURN_BUY, &p, &a);
    }
    return result;
}

//-----------------------------------------------------------------------------
int DriverFR::Sale()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;
    p.len   = 55;

    if (connected)
    {
        logCommand(SALE, QString("Продажа (%1 * %2 = %3)").arg(fr.Quantity).arg(fr.Price).arg(fr.Quantity * fr.Price));

        int64_t quant = llround(fr.Quantity * 1000);
        int64_t price = llround(fr.Price * 100);
        memcpy(p.buff,    &quant, 5);
        memcpy(p.buff+5,  &price, 5);
        p.buff[10] = fr.Department;
        p.buff[11] = fr.Tax1;
        p.buff[12] = fr.Tax2;
        p.buff[13] = fr.Tax3;
        p.buff[14] = fr.Tax4;
        memcpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

        result = processCommand(SALE, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::ReturnSale()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;
    p.len   = 55;

    if (connected)
    {
        logCommand(RETURN_SALE, QString("Возврат продажи (%1 * %2 = %3)").arg(fr.Quantity).arg(fr.Price).arg(fr.Quantity * fr.Price));

        int64_t quant = llround(fr.Quantity * 1000);
        int64_t price = llround(fr.Price * 100);
        memcpy(p.buff,    &quant, 5);
        memcpy(p.buff+5,  &price, 5);
        p.buff[10] = fr.Department;
        p.buff[11] = fr.Tax1;
        p.buff[12] = fr.Tax2;
        p.buff[13] = fr.Tax3;
        p.buff[14] = fr.Tax4;
        memcpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

        result = processCommand(RETURN_SALE, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::CancelCheck()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(CANCEL_CHECK, "Аннулирование чека");
        result = processCommand(CANCEL_CHECK, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetEKLZJournal()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len = 2;
    int result = -1;

    if (connected)
    {
        logCommand(GET_EKLZ_JOURNAL, "Запрос контрольной ленты ЭКЛЗ");

        memcpy(&p.buff, &fr.SessionNumber, 2);

        result = processCommand(GET_EKLZ_JOURNAL, &p, &a);
        if (result == 0)
        {
            QByteArray data;
            data.append((char*)&a.buff+2);
            fr.UDescription = codec->toUnicode(data);
        }
    }
  return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetEKLZData()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(GET_EKLZ_DATA, "Запрос данных отчета ЭКЛЗ");
        result = processCommand(GET_EKLZ_DATA, &p, &a);
        if (result == 0)
        {
            QByteArray data;
            data.append((char*)&a.buff+2);
            fr.EKLZData = codec->toUnicode(data);
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetEKLZCode1Report()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(GET_EKLZ_CODE1_REPORT, "Запрос состояния по коду 1 ЭКЛЗ");
        result = processCommand(GET_EKLZ_CODE1_REPORT, &p, &a);
        if (result == 0)
        {
            QByteArray data;
            fr.OperatorNumber = a.buff[2];
            fr.LastKPKDocumentResult = evalint64((unsigned char*)&a.buff+2, 5);
            fr.LastKPKDocumentResult /= 100;
            // Прочитаем дату
            data.clear();
            data.append((const char*)&a.buff+7, 3);
            fr.LastKPKDate.fromString(codec->toUnicode(data), "dd.mm.yy");
            // Прочитаем время
            data.clear();
            data.append((const char*)&a.buff+10, 2);
            fr.LastKPKTime.fromString(codec->toUnicode(data), "hh:mm");
            // Прочитаем номер КПК
            fr.LastKPKNumber = evalint32((unsigned char*)&a.buff+12, 4);
            // Прочитаем номер ЭКЛЗ
            data.clear();
            data.append((char*)&a.buff+16, 5);
            fr.EKLZNumber = codec->toUnicode(data);
            // Прочитаем флаги
            fr.EKLZFlags = a.buff[21];
        }
    }
    return result;
}


int DriverFR::EKLZInterrupt()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(EKLZ_INTERRUPT, "Прекращение ЭКЛЗ");
        result = processCommand(EKLZ_INTERRUPT, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::CashIncome()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 5;
    int result = -1;

    if (connected)
    {
        logCommand(CASH_INCOME, "Поступление наличных");

        int64_t  sum = llround(fr.Summ1 * 100);
        memcpy(p.buff, &sum, 5);

        result = processCommand(CASH_INCOME, &p, &a);
        if (result == 0)
        {
            fr.OpenDocumentNumber = evalint((unsigned char*)&a.buff+3, 2);
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::CashOutcome()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len    = 5;
    int result = -1;

    if (connected)
    {
        logCommand(CASH_OUTCOME, "Выдача наличных");

        int64_t  sum = llround(fr.Summ1 * 100);
        memcpy(p.buff, &sum, 5);

        result = processCommand(CASH_OUTCOME, &p, &a);
        if (result == 0)
        {
            fr.OpenDocumentNumber = evalint((unsigned char*)&a.buff+3, 2);
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::Charge()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};
  p.len   = 49;

  if (!connected) return -1;

  int64_t  sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  p.buff[5] = fr.Tax1;
  p.buff[6] = fr.Tax2;
  p.buff[7] = fr.Tax3;
  p.buff[8] = fr.Tax4;

  memcpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

  if (sendCommand(CHARGE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != CHARGE) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::StornoCharge()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};
  p.len   = 49;

  if (!connected) return -1;

  int64_t  sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  p.buff[5] = fr.Tax1;
  p.buff[6] = fr.Tax2;
  p.buff[7] = fr.Tax3;
  p.buff[8] = fr.Tax4;

  memcpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

  if (sendCommand(STORNO_CHARGE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != STORNO_CHARGE) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}


//-----------------------------------------------------------------------------
int DriverFR::Discount()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    p.len   = 49;
    int result = -1;

    if (connected)
    {
        logCommand(DISCOUNT, "Скидка");

        int64_t  sum = llround(fr.Summ1 * 100);
        memcpy(p.buff, &sum, 5);
        p.buff[5] = fr.Tax1;
        p.buff[6] = fr.Tax2;
        p.buff[7] = fr.Tax3;
        p.buff[8] = fr.Tax4;
        memcpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

        result = processCommand(DISCOUNT, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::StornoDiscount()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};
  p.len   = 49;

  if (!connected) return -1;

  int64_t sum = llround(fr.Summ1 * 100);

  memcpy(p.buff, &sum, 5);

  p.buff[5] = fr.Tax1;
  p.buff[6] = fr.Tax2;
  p.buff[7] = fr.Tax3;
  p.buff[8] = fr.Tax4;

  memcpy((char*)p.buff+9, (char*)fr.StringForPrinting, 40);

  if (sendCommand(STORNO_DISCOUNT, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != STORNO_DISCOUNT) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::CheckSubTotal()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(CHECK_SUBTOTAL, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != CHECK_SUBTOTAL) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.Summ1 = evalint64((unsigned char*)&a.buff+3, 5);
  fr.Summ1 /= 100;

  return 0;
}

//-----------------------------------------------------------------------------
int DriverFR::CloseCheck()
{
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        logCommand(CLOSE_CHECK, QString("Закрытие чека (%1, %2, %3, %4, %5)").arg(fr.Summ1).arg(fr.Summ2).arg(fr.Summ3).arg(fr.Summ4).arg(fr.DiscountOnCheck));

        int64_t  sum;
        p.len   = 67;
        sum = llround(fr.Summ1 * 100);
        memcpy(p.buff,    &sum, 5);			// 0-4
        sum = llround(fr.Summ2 * 100);
        memcpy(p.buff+ 5, &sum, 5);			// 5-9
        sum = llround(fr.Summ3 * 100);
        memcpy(p.buff+10, &sum, 5);			//10-14
        sum = llround(fr.Summ4 * 100);
        memcpy(p.buff+15, &sum, 5);			//15-19
        sum = llround(fr.DiscountOnCheck * 100);
        memcpy(p.buff+20, &sum, 3);			//20-22
        p.buff[23] = fr.Tax1;				//23
        p.buff[24] = fr.Tax2;				//24
        p.buff[25] = fr.Tax3;				//25
        p.buff[26] = fr.Tax4;				//26
        memcpy((char*)p.buff+27, (char*)fr.StringForPrinting, 40);
        result = processCommand(CLOSE_CHECK, &p, &a);
        if (result == 0)
        {
            fr.OperatorNumber = a.buff[2];
            fr.Change = evalint64((unsigned char*)&a.buff+3, 5);
            fr.Change /= 100;
        }
        app->sleep(400);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::Storno()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  int64_t quant = llround(fr.Quantity * 1000);
  int64_t price = llround(fr.Price * 100);
  p.len   = 55;

  memcpy(p.buff,    &quant, 5);
  memcpy(p.buff+5,  &price, 5);
  p.buff[10] = fr.Department;

  p.buff[11] = fr.Tax1;
  p.buff[12] = fr.Tax2;
  p.buff[13] = fr.Tax3;
  p.buff[14] = fr.Tax4;

  memcpy((char*)p.buff+15, (char*)fr.StringForPrinting, 40);

  if (sendCommand(STORNO, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != STORNO) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}


//-----------------------------------------------------------------------------
int DriverFR::PrintReportWithCleaning()
{
  int result = -1;

  if (connected)
  {
      parameter  p = {0, {0}};
      answer     a = {0, {0}};

      logCommand(PRINT_REPORT_WITH_CLEANING, "Снять отчет с гашением");

      result = processCommand(PRINT_REPORT_WITH_CLEANING, &p, &a);
      if (result == 0)
      {
          fr.OperatorNumber = a.buff[2];
          fr.SessionNumber = evalint((unsigned char*)&a.buff+3, 2);
      }
  }
  return result;
}


//-----------------------------------------------------------------------------
int DriverFR::PrintReportWithoutCleaning()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};
  int result = -1;

  if (connected)
  {
      logCommand(PRINT_REPORT_WITHOUT_CLEANING, "Снять отчет без гашения");

      result = processCommand(PRINT_REPORT_WITHOUT_CLEANING, &p, &a);
      if (result == 0)
      {
          fr.OperatorNumber = a.buff[2];
          fr.SessionNumber = evalint((unsigned char*)&a.buff+3, 2);
      }
  }
  return result;
}


//-----------------------------------------------------------------------------
int DriverFR::OpenSession()
{
    int result = -1;

    if (connected)
    {
        parameter  p = {0, {0}};
        answer     a = {0, {0}};
        logCommand(OPEN_SESSION, "Открыть сессию");

        result = processCommand(OPEN_SESSION, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::PrintOperationReg()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(PRINT_OPERATION_REG, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != PRINT_OPERATION_REG) return -1;

  return errHand(&a);
}


//-----------------------------------------------------------------------------
int DriverFR::DampRequest()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.DeviceCode;

  if (sendCommand(DUMP_REQUEST, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != DUMP_REQUEST) return -1;

  if (errHand(&a) != 0) return -1;

  return a.buff[2];  			//number of data blocks to return
}


//-----------------------------------------------------------------------------
int DriverFR::GetData()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(GET_DATA, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != GET_DATA) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.DeviceCode = a.buff[2];
  fr.DeviceCodeDescription = (char*)devcodedesc[fr.DeviceCode];
  fr.DataBlockNumber = evalint((unsigned char*)&a.buff+3, 2);
  memcpy(fr.DataBlock, a.buff+5, 32);

  return 0;
}


//-----------------------------------------------------------------------------
int DriverFR::InterruptDataStream()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(INTERRUPT_DATA_STREAM, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != INTERRUPT_DATA_STREAM) return -1;

  return errHand(&a);
}


//-----------------------------------------------------------------------------
int DriverFR::GetCashReg()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.RegisterNumber;

  if (sendCommand(GET_CASH_REG, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != GET_CASH_REG) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.ContentsOfCashRegister = evalint64((unsigned char*)&a.buff+3, 6);
  fr.ContentsOfCashRegister /= 100;

  return 0;
}


//-----------------------------------------------------------------------------
int DriverFR::GetOperationReg()
{
    logCommand(GET_OPERATION_REG, "Получить операционный регистр");
    parameter  p = {0, {0}};
    answer     a = {0, {0}};
    int result = -1;

    if (connected)
    {
        p.len    = 1;
        p.buff[0] = fr.RegisterNumber;

        if (sendCommand(GET_OPERATION_REG, fr.Password, &p) >= 0)
        {
            if (readAnswer(&a) >= 0)
            {
                if (a.buff[0] == GET_OPERATION_REG)
                {
                    if (errHand(&a) != 0)
                        result = fr.ResultCode;
                    else
                    {
                        fr.OperatorNumber = a.buff[2];
                        fr.ContentsOfOperationRegister = evalint((unsigned char*)&a.buff+3, 2);
                    }
                }
            }
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::SetSerialNumber()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 4;
  int num = atol(fr.SerialNumber);

  memcpy(p.buff, &num, sizeof(int));

  if (sendCommand(SET_SERIAL_NUMBER, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != SET_SERIAL_NUMBER) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::SetPointPosition()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.PointPosition;

  if (sendCommand(SET_POINT_POSITION, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != SET_POINT_POSITION) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::SetTime()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 3;

  p.buff[2] = fr.Time.tm_sec;
  p.buff[1] = fr.Time.tm_min;
  p.buff[0] = fr.Time.tm_hour;

  if (sendCommand(SET_TIME, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != SET_TIME) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::SetDate()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 3;

  p.buff[0] = fr.Date.tm_mday;
  p.buff[1] = fr.Date.tm_mon + 1;
  p.buff[2] = fr.Date.tm_year - 100;

  if (sendCommand(SET_DATE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != SET_DATE) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::ConfirmDate()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 3;

  p.buff[0] = fr.Date.tm_mday;
  p.buff[1] = fr.Date.tm_mon + 1;
  p.buff[2] = fr.Date.tm_year - 100;

  if (sendCommand(CONFIRM_DATE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != CONFIRM_DATE) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::InitTable()
{
  parameter  p = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(INIT_TABLE, fr.Password, &p) < 0) return -1;
  return 0;
}


//-----------------------------------------------------------------------------
int DriverFR::WriteTable()
{
    int result = -1;

    if (connected)
    {
        int      len;
        void    *tmp;

        GetFieldStruct();
        if (fr.FieldType == 1)
        {
           len = fr.FieldSize;
           tmp = fr.ValueOfFieldString;
        }
        else
        {
            len = fr.FieldSize;
            tmp = (void*)(&fr.ValueOfFieldInteger);
        }

        parameter  p = {0, {0}};
        answer     a = {0, {0}};

        logCommand(WRITE_TABLE, "Запись таблицы");

        p.len   = 4 + len;
        p.buff[0] = fr.TableNumber;
        memcpy(p.buff+1, &fr.RowNumber, 2);
        p.buff[3] = fr.FieldNumber;
        memcpy(p.buff+4, tmp, len);

        result = processCommand(WRITE_TABLE, &p, &a);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::ReadTable()
{
    int result = -1;

    if (connected)
    {
        logCommand(READ_TABLE, "Чтение таблицы");

        parameter  p = {0, {0}};
        answer     a = {0, {0}};

        p.len   = 4;
        p.buff[0] = fr.TableNumber;
        memcpy(p.buff+1, &fr.RowNumber, 2);
        p.buff[3] = fr.FieldNumber;

        result = processCommand(READ_TABLE, &p, &a);
        GetFieldStruct();

        if (fr.FieldType == 1)
            memcpy(fr.ValueOfFieldString, (char*)a.buff+2, 40);
        else
            fr.ValueOfFieldInteger = evalint64((unsigned char*)&a.buff+2, fr.FieldSize);
    }
    return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetFieldStruct()
{
    int result = -1;

    if (connected)
    {
        logCommand(GET_FIELD_STRUCT, "Получить структуру поля");

        parameter  p = {0, {0}};
        answer     a = {0, {0}};

        p.len    = 2;
        p.buff[0] = fr.TableNumber;
        p.buff[1] = fr.FieldNumber;

        result = processCommand(GET_FIELD_STRUCT, &p, &a);

        memcpy(fr.FieldName, (char*)&a.buff+2, 40);
        fr.FieldType = a.buff[42];
        fr.FieldSize = a.buff[43];
        fr.MINValueOfField = evalint64((unsigned char*)&a.buff+44, fr.FieldSize);
        fr.MAXValueOfField = evalint64((unsigned char*)&a.buff+44 + fr.FieldSize, fr.FieldSize);
    }

  return result;
}


//-----------------------------------------------------------------------------
int DriverFR::GetTableStruct()
{
    int result = -1;

    if (connected)
    {
        logCommand(GET_TABLE_STRUCT, "Получить структуру таблицы");

        parameter  p = {0, {0}};
        answer     a = {0, {0}};
        p.len    = 1;

        p.buff[0] = fr.TableNumber;
        result = processCommand(GET_TABLE_STRUCT, &p, &a);

        memcpy(fr.TableName, (char*)&a.buff+2, 40);
        fr.RowNumber   = evalint((unsigned char*)&a.buff+42, 2);;
        fr.FieldNumber = a.buff[44];
    }

  return result;
}


//-----------------------------------------------------------------------------
int DriverFR::WriteLicense()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  int64_t num = atoll(fr.License);
  p.len    = 5;

  memcpy(p.buff, &num, 5);

  if (sendCommand(WRITE_LICENSE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != WRITE_LICENSE) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::ReadLicense()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(READ_LICENSE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != READ_LICENSE) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  sprintf(fr.License, "%.0lg", (double)evalint64((unsigned char*)&a.buff+2, 5));

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::InitFM()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(INIT_FM, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != INIT_FM) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::Fiscalization()
{
  parameter  p = {0, {0}};
  answer 	   a;

  if (!connected) return -1;

  int     nti = fr.NewPasswordTI;
  int64_t rnm = atoll(fr.RNM);
  int64_t inn = atoll(fr.INN);
  p.len   = 14;

  memcpy(p.buff,   &nti, 4);
  memcpy(p.buff+4, &rnm, 5);
  memcpy(p.buff+9, &inn, 6);

  if (sendCommand(FISCALIZATION, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != FISCALIZATION) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.RegistrationNumber = evalint((unsigned char*)&a.buff+2, 1);
  fr.FreeRegistration   = evalint((unsigned char*)&a.buff+3, 1);
  fr.SessionNumber      = evalint((unsigned char*)&a.buff+4, 2);
  evaldate((unsigned char*)&a.buff+6, &fr.Date);

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::FiscalReportForDatesRange()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 7;

  p.buff[0] = fr.ReportType;

  p.buff[1] = fr.FirstSessionDate.tm_year - 100;
  p.buff[2] = fr.FirstSessionDate.tm_mon  + 1;
  p.buff[3] = fr.FirstSessionDate.tm_mday;

  p.buff[4] = fr.LastSessionDate.tm_year - 100;
  p.buff[5] = fr.LastSessionDate.tm_mon  + 1;
  p.buff[6] = fr.LastSessionDate.tm_mday;

  if (sendCommand(FISCAL_REPORT_FOR_DATES_RANGE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != FISCAL_REPORT_FOR_DATES_RANGE) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  evaldate((unsigned char*)&a.buff+2, &fr.FirstSessionDate);
  evaldate((unsigned char*)&a.buff+5, &fr.LastSessionDate);
  fr.FirstSessionNumber = evalint((unsigned char*)&a.buff+8, 2);
  fr.LastSessionNumber  = evalint((unsigned char*)&a.buff+10, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::FiscalReportForSessionRange()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 5;

  p.buff[0] = fr.ReportType;

  memcpy(p.buff+1, &fr.FirstSessionNumber, 2);
  memcpy(p.buff+3, &fr.LastSessionNumber, 2);

  if (sendCommand(FISCAL_REPORT_FOR_SESSION_RANGE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != FISCAL_REPORT_FOR_SESSION_RANGE) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  evaldate((unsigned char*)&a.buff+2, &fr.FirstSessionDate);
  evaldate((unsigned char*)&a.buff+5, &fr.LastSessionDate);
  fr.FirstSessionNumber = evalint((unsigned char*)&a.buff+8, 2);
  fr.LastSessionNumber  = evalint((unsigned char*)&a.buff+10, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::InterruptFullReport()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(INTERRUPT_FULL_REPORT, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != INTERRUPT_FULL_REPORT) return -1;

  return errHand(&a);
}
//-----------------------------------------------------------------------------
int DriverFR::GetFiscalizationParameters()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.RegistrationNumber;

  if (sendCommand(GET_FISCALIZATION_PARAMETERS, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != GET_FISCALIZATION_PARAMETERS) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.NewPasswordTI = evalint((unsigned char*)&a.buff+2, 4);
  sprintf(fr.RNM, "%.0lg", (double)evalint64((unsigned char*)&a.buff+6,  5));
  sprintf(fr.INN, "%.0lg", (double)evalint64((unsigned char*)&a.buff+11, 6));
  fr.SessionNumber = evalint((unsigned char*)&a.buff+17, 2);
  evaldate((unsigned char*)&a.buff+19, &fr.Date);

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::GetFMRecordsSum()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 1;

  p.buff[0] = fr.TypeOfSumOfEntriesFM;

  if (sendCommand(GET_FM_RECORDS_SUM, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != GET_FM_RECORDS_SUM) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];
  fr.Summ1 = evalint64((unsigned char*)&a.buff+3,  8);
  fr.Summ1 /= 100;
  fr.Summ2 = evalint64((unsigned char*)&a.buff+11, 6);
  fr.Summ2 /= 100;
  fr.Summ3 = evalint64((unsigned char*)&a.buff+17, 6);
  fr.Summ3 /= 100;
  fr.Summ4 = evalint64((unsigned char*)&a.buff+23, 6);
  fr.Summ4 /= 100;

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::GetLastFMRecordDate()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(GET_LAST_FM_RECORD_DATE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != GET_LAST_FM_RECORD_DATE) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber   = a.buff[2];
  fr.LastFMRecordType = a.buff[3];
  evaldate((unsigned char*)&a.buff+4, &fr.Date);

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::GetRangeDatesAndSessions()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  if (sendCommand(GET_RANGE_DATES_AND_SESSIONS, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != GET_RANGE_DATES_AND_SESSIONS) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  evaldate((unsigned char*)&a.buff+2, &fr.FirstSessionDate);
  evaldate((unsigned char*)&a.buff+5, &fr.LastSessionDate);
  fr.FirstSessionNumber = evalint((unsigned char*)&a.buff+8,  2);
  fr.LastSessionNumber  = evalint((unsigned char*)&a.buff+10, 2);

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::EKLZDepartmentReportInDatesRange(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::EKLZDepartmentReportInSessionsRange(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::EKLZJournalOnSessionNumber(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::EKLZSessionReportInDatesRange(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::EKLZSessionReportInSessionRange(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::ReadEKLZDocumentOnKPK(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::ReadEKLZSessionTotal(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::StopEKLZDocumentPrinting(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::Correction(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::DozeOilCheck(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::SummOilCheck(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::SetDozeInMilliliters(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::SetDozeInMoney(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::OilSale(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::GetLiterSumCounter(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::GetRKStatus(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::LaunchRK(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::StopRK(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::ResetRK(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::ResetAllTRK(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::SetRKParameters(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::EjectSlipDocument(){return 0;}
//-----------------------------------------------------------------------------
int DriverFR::LoadLineData()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 41;

  p.buff[0]  = fr.LineNumber;
  memcpy(p.buff + 1, fr.LineData, 40);

  if (sendCommand(LOAD_LINE_DATA, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != LOAD_LINE_DATA) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
//-----------------------------------------------------------------------------
int DriverFR::Draw()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  p.len    = 2;

  p.buff[0]  = fr.FirstLineNumber;
  p.buff[1]  = fr.LastLineNumber;

  if (sendCommand(DRAW, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != DRAW) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}


//-----------------------------------------------------------------------------
int DriverFR::PrintBarCode()
{
  parameter  p = {0, {0}};
  answer     a = {0, {0}};

  if (!connected) return -1;

  int64_t barcode = atoll(fr.BarCode);
  p.len    = 5;

  memcpy(p.buff, &barcode, 5);

  if (sendCommand(PRINT_BARCODE, fr.Password, &p) < 0) return -1;
  if (readAnswer(&a) < 0) return -1;
  if (a.buff[0] != PRINT_BARCODE) return -1;

  if (errHand(&a) != 0) return fr.ResultCode;

  fr.OperatorNumber = a.buff[2];

  return 0;
}
