var currentDate = new Date();
var printCheckBody = app.getConst("ККМпечатьТелаЧека");
var checkNumber = 0;
var attempts = 5;
var result = 0;

if (checkType != 0 && 					// Если правильный тип чека
    !documents.getValue("ЧЕКОТБИТ") && 			// Если чек еще не отбит
    documents.getValue("СУММА") > 0 &&
//    (documents.getValue('ДАТА').toLocaleDateString() == currentDate.toLocaleDateString()) && 	// Если мы пытаемся отбить сегодняшний чек
    app.drvFRisValid())					// И драйвер фискального регистратора доступен
{
	var exit = false;
	while (!exit && attempts > 0)
	{
		drvFR.setShowProgressBar(true);
		if (drvFR.Connect())
		{
		      var mode = drvFR.getProperty("ECRMode");
		      if (mode == 2 || mode == 4)
		      {
				drvFR.setProgressDialogValue(0);
//				result = drvFR.GetEKLZCode1Report();
				result = 0;
				if (result == 0)
				{
				  checkNumber = drvFR.getProperty("OpenDocumentNumber") + 1;
				
				  // Проверим, не существует ли уже такой чек (если чек не допечатался по какой-либо причине, но успел отметиться в базе)
				  var type = "";
				  var docId = 0;
				  if (checkType == 1)
				  {
				    docId = db.getValue("SELECT КОД FROM докатрибуты1 WHERE НОМЕРЧЕКА = " + checkNumber + " AND КОД > " + lastMaxDocId);
				    if (docId > 0)
				    {
				      type = "продажи";
				      db.exec("UPDATE докатрибуты1 SET ЧЕКОТБИТ = false, НОМЕРЧЕКА = 0 WHERE КОД = " + docId);
				    }
				  }
				  if (checkType == 2)
				  {
				    docId = db.getValue("SELECT КОД FROM докатрибуты71 WHERE НОМЕРЧЕКА = " + checkNumber + " AND КОД > " + lastMaxDocId);
				    if (docId > 0)
				    {
				      type = "возврата";
				      db.exec("UPDATE докатрибуты71 SET ЧЕКОТБИТ = false, НОМЕРЧЕКА = 0 WHERE КОД = " + docId);
				    }
				  }
				  if (docId > 0)
				  {
				    var number = db.getValue("SELECT НОМЕР FROM документы WHERE КОД = " + docId);
				    QMessageBox.warning(form, "Внимание!", "Возможно при печати чека " + type + " произошел сбой. Отпечатайте повторно чек для документа номер " + number + ".");
				  }
//				  db.exec("UPDATE докатрибуты1 SET ЧЕКОТБИТ = CASE WHEN НОМЕРЧЕКА > 0 AND НОМЕРЧЕКА < " + checkNumber + " THEN true ELSE false END;");
//				  db.exec("UPDATE докатрибуты1 SET ЧЕКОТБИТ = false WHERE НОМЕРЧЕКА = 0;");
//				  db.exec("UPDATE докатрибуты1 SET ЧЕКОТБИТ = true WHERE НОМЕРЧЕКА > 0 AND НОМЕРЧЕКА < " + checkNumber);								  
				
				  drvFR.writeLog("*** Начало чека. Номер: " + checkNumber + "  ***");
				
				  drvFR.setProperty("UseReceiptRibbon", 1);
				  drvFR.setProperty("UseJournalRibbon", 0);

				  result = drvFR.PrintString(app.getConst("ККМклише1"));
				  if (result == 0)
				    result = drvFR.PrintString(app.getConst("ККМклише2"));
				  if (result == 0)
				    result = drvFR.PrintString(app.getConst("ККМклише3"));
				  if (result == 0)
				    result = drvFR.PrintString(app.getConst("ККМклише4"));
				  if (result == 0)
				    result = drvFR.PrintString(app.getConst("ККМклише5"));
				  if (result == 0)
				    result = drvFR.PrintString(app.getConst("ККМклише6"));
				  if (result == 0)
				    result = drvFR.PrintString(" ");

				  drvFR.setProgressDialogValue(25);			// Отпечатали 1/4 чека

				  if (result == 0)
				  {
				    // Отпечатаем список продаж
				    var goodsList = [];					// Здесь сохраним список продаж на случай ошибки
				    var totSum = 0;
				
				    for (var i = 0; i < getRowCount(); i++)
				    {
				      var quan = getValue("P1__КОЛ", i);
					  if (quan > 0)
					  { 
						var name = getValue("ТОВАР__ИМЯ", i);
						var otdel =  tableGroup.getValue("ОТДЕЛ", tableGroup.locateId(getValue("ТОВАР__КОД_ГРУППЫ", i)));
						var price = getValue("P1__ЦЕНА", i);
						
						goodsList.push("<" + name + "> " + quan + "*" + price);
						totSum = totSum + Math.round(quan * price * 100) / 100;
						
						drvFR.setProperty("StringForPrinting", name);
						drvFR.setProperty("Quantity", quan);
						drvFR.setProperty("Price", price);
						drvFR.setProperty("Department", otdel);
						drvFR.setProperty("Tax1", 0);
						drvFR.setProperty("Tax2", 0);
						drvFR.setProperty("Tax3", 0);
						drvFR.setProperty("Tax4", 0);
						if (checkType == 1)
							result = drvFR.Sale();
						else if (checkType == 2)
							result = drvFR.ReturnSale();
						if (result != 0)
						  break;
					}
				    }
				  }
				  drvFR.setProgressDialogValue(50);			// Отпечатали 2/4 чека

				  if (result == 0)
				  {
				    var nalich = documents.getValue("СУММА");

				    if (checkType == 1)
				    {	
					//Отпечатаем скидку, если она есть
					var discount = getValue("P3__СУММА");
					if (discount > 0)
					{
						discount = Math.round(discount * 100) / 100;
						goodsList.push("Всего : " + totSum);
						goodsList.push("Скидка: " + discount);
						totSum = totSum - discount;
						totSum = Math.round(totSum * 100) / 100;
						
//						var discountStr = documents.getValue("СКИДКА") + "%";
						drvFR.setProperty("DiscountOnCheck", documents.getValue("СКИДКА"));
						drvFR.setProperty("StringForPrinting", "");
						drvFR.setProperty("Summ1", discount);
						drvFR.setProperty("Tax1", 0);
						drvFR.setProperty("Tax2", 0);
						drvFR.setProperty("Tax3", 0);
						drvFR.setProperty("Tax4", 0);
						result = drvFR.Discount();
					}
					
					// Отпечатаем итог, полученные наличные, сдачу и закроем чек
					if (typeof nal != 'undefined')
					{
						if (nal > totSum)
						  nalich = nal;
					}
				    }
				  }
				  if (result == 0)
				  {
				    if (totSum > 0)
				    {
				      drvFR.setProperty("StringForPrinting", "");
				      drvFR.setProperty("Summ1", nalich);
				      drvFR.setProperty("Summ2", 0);
				      drvFR.setProperty("Summ3", 0);
				      drvFR.setProperty("Summ4", 0);
				      drvFR.setProperty("DiscountOnCheck", 0);
				      drvFR.setProperty("Tax1", 0);
				      drvFR.setProperty("Tax2", 0);
				      drvFR.setProperty("Tax3", 0);
				      drvFR.setProperty("Tax4", 0);
				      result = drvFR.CloseCheck();
				      if (result == 0)
				      {
					documents.setValue("НОМЕРЧЕКА", checkNumber);
					documents.setValue("ЧЕКОТБИТ", true);
					document.saveChanges();
					
					drvFR.setProgressDialogValue(75);			// Отпечатали 3/4 чека
					
					// Отпечатаем рекламу
					result = drvFR.PrintString(app.getConst("ККМреклама1"));
					if (result == 0)
					    result = drvFR.PrintString(app.getConst("ККМреклама2"));
					if (result == 0)
					    result = drvFR.PrintString(app.getConst("ККМреклама3"));
					if (result == 0)
					{
					  // Отпечатаем 3 пустых строки
					  drvFR.setProperty("StringQuantity", 3);
					  drvFR.setProperty("UseReceiptRibbon", 1);
					  result = drvFR.FeedDocument();
					}
					drvFR.writeLog("*** Конец чека. Номер: " + checkNumber + " ***");
				      }
				      else
				      {
					drvFR.CancelCheck();
				      }
				    }
				  }
				}
				if (result != 0)
				{
					drvFR.writeLog(result);			// Запишем в журнал сообщение об ошибке
					ShowErrorMessage(result);
				}

				drvFR.setProgressDialogValue(100);			// Отпечатали 4/4 чека
			}
			else
			{
				if (mode == 8)
       					QMessageBox.warning(form, "Внимание!", "Нельзя печатать чек, т.к. не закрыт предыдущий.");
				else	
       					QMessageBox.warning(form, "Внимание!", "Нельзя печатать чек. Возможно нужно снять отчет с гашением.");
			}
			drvFR.DisConnect();
			exit = true;
	  	}
		else
		{
			documents.updateCurrentRow();	// Обновим информацию о документ, вдруг чек отбит
			if (documents.getValue("ЧЕКОТБИТ"))
			{
				QMessageBox.warning(form, "Внимание!", "Чек уже отбит.");
				exit = true;
			}
			attempts--;
			app.timeOut(1000);
		}
	}
}

function ShowErrorMessage(result)
{
	if (result > 0)
		app.print("Ошибка : " + drvFR.getProperty("ResultCodeDescription"));
	else if (result < 0)
		app.showMessageOnStatusBar("Фискальный регистратор не откликается");
}

