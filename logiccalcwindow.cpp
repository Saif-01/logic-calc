#include "logiccalcwindow.h"
#include "ui_logiccalcwindow.h"

LogicCalcWindow::LogicCalcWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogicCalcWindow)
{
    ui->setupUi(this);

    connect(ui->expLineEdit, SIGNAL(returnPressed()), this, SLOT(lineEntered()));
    connect(ui->expLineEdit_2, SIGNAL(returnPressed()), this, SLOT(lineEntered()));
}

LogicCalcWindow::~LogicCalcWindow()
{
    delete ui;
}

void LogicCalcWindow::checkExpressionsEntered()
{
    if (!ui->expLineEdit->text().isEmpty() && ui->expLineEdit_2->text().isEmpty())
    {
        expEntered(ui->expLineEdit->text(), 1);
    }

    else if (ui->expLineEdit->text().isEmpty() && !ui->expLineEdit_2->text().isEmpty())
    {
        expEntered(ui->expLineEdit_2->text(), 2);
    }

    else if (!ui->expLineEdit->text().isEmpty() && !ui->expLineEdit_2->text().isEmpty())
    {
        expEntered(ui->expLineEdit->text(), 1);
        expEntered(ui->expLineEdit_2->text(), 2);
    }

    else
    {
        throw ZERO_EXPRESSIONS_ENTERED;
    }
}

void LogicCalcWindow::lineEntered()
{
    try
    {
        checkExpressionsEntered();
    }

    catch (errors error)
    {
        if (error == ZERO_EXPRESSIONS_ENTERED)
        {
            QMessageBox::warning(this, QString::fromUtf8("Логический калькулятор"), QString::fromUtf8("Выражения не введены"), QMessageBox::Ok);
        }
    }

}

void LogicCalcWindow::expEntered(const QString& enteredExpr, int numberOfExpression)
{ // X -> Y -> Z
    try
    {
        QList<int> results;
        QStringList TTinput, splittedExpression;
        QString expr = enteredExpr;

        expr.remove(QRegularExpression("[ ]+"));

        splitExpression(expr, splittedExpression, lexerErrorString);

        generateTTInput(TTinput);

        calculate(splittedExpression, TTinput, results);

        fillTruthTableWidget(TTinput, results, numberOfExpression);


        FunctionType type = functionType(results, numberOfExpression);

        if (type == ALWAYSTRUE)
        {
            generateDesjunctiveNormalForm(TTinput, results, numberOfExpression);
            if (numberOfExpression == 1)
            {
                ui->conjLabel->setText("1");
                ui->MCNF1lineEdit->setText("1");
                ui->MDNF1lineEdit->setText("1");
            }
            else if (numberOfExpression == 2)
            {
                ui->conjLabel_2->setText("1");
                ui->MCNF2lineEdit->setText("1");
                ui->MDNF2lineEdit->setText("1");
            }
        }
        else if (type == ALWAYSFALSE)
        {
            generateConjuctiveNormalForm(TTinput, results, numberOfExpression);
            if (numberOfExpression == 1)
            {
                ui->desjLabel->setText("0");
                ui->MDNF1lineEdit->setText("0");
                ui->MCNF1lineEdit->setText("0");
            }
            else if (numberOfExpression == 2)
            {
                ui->desjLabel_2->setText("0");
                ui->MDNF2lineEdit->setText("0");
                ui->MCNF2lineEdit->setText("0");
            }
        }
        else if (type == DOABLE)
        {
            generateDesjunctiveNormalForm(TTinput, results, numberOfExpression);
            generateConjuctiveNormalForm(TTinput, results, numberOfExpression);

            minimizeDesjunctiveNormalForm(TTinput, results, numberOfExpression);
            minimizeConjunctiveNormalForm(TTinput, results, numberOfExpression);
        }

        checkFunctionsForDuality();
    }

    catch (errors error)
    {
        switch (error)
        {
            case EMPTY_STRING:
            {
                QMessageBox::warning(this, QString::fromUtf8("Логический калькулятор"), QString::fromUtf8("Введено пустое выражение"), QMessageBox::Ok);
                break;
            }

            case BAD_TOKEN:
            {
                QMessageBox::warning(this, QString::fromUtf8("Логический калькулятор"), QString::fromUtf8("Введены недопустимые символы: ") + lexerErrorString, QMessageBox::Ok);
                break;
            }
            case NONE_FUNCTION_TYPE:
            {
                QMessageBox::warning(this, QString::fromUtf8("Логический калькулятор"), QString::fromUtf8("Невозможно установить тип функции"), QMessageBox::Ok);
                break;
            }
        }
    }

    catch (const char *e)
    {
        QMessageBox::warning(this, QString::fromUtf8("Логический калькулятор"), QString::fromUtf8("Ошибка парсера: ") + QString(e), QMessageBox::Ok);
    }

    catch (...)
    {
        QMessageBox::warning(this, QString::fromUtf8("Логический калькулятор"), QString::fromUtf8("Неизвестная ошибка"), QMessageBox::Ok);
    }
}

void LogicCalcWindow::checkFunctionsForDuality()
{
    ui->dualityFunction1Label->clear();
    ui->dualityFunction2Label->clear();
    ui->dualityLabel->clear();

    QList<int> firstFunctionResults, secondFunctionResults, firstFunctionResultsReversed, secondFunctionResultsReversed;


    for (int i = 0; i < ui->truthTableWidget->rowCount(); i++)
    {
        int value = ui->truthTableWidget->item(i, ui->truthTableWidget->columnCount() - 1)->text().toInt();
        firstFunctionResults.append(value);
    }

    for (int i = 0; i < ui->truthTableWidget_2->rowCount(); i++)
    {
        int value = ui->truthTableWidget_2->item(i, ui->truthTableWidget_2->columnCount() - 1)->text().toInt();
        secondFunctionResults.append(value);
    }

    firstFunctionResultsReversed = firstFunctionResults;
    secondFunctionResultsReversed = secondFunctionResults;

    invertColumn(firstFunctionResultsReversed);
    reverseColumn(firstFunctionResultsReversed);

    invertColumn(secondFunctionResultsReversed);
    reverseColumn(secondFunctionResultsReversed);

    if (!ui->expLineEdit->text().isEmpty() && !ui->expLineEdit_2->text().isEmpty() && ui->truthTableWidget->rowCount() > 0 && ui->truthTableWidget_2->rowCount() > 0)
    {
        if (firstFunctionResultsReversed == secondFunctionResults)
            ui->dualityLabel->setText("Функции двойственны");
        else
            ui->dualityLabel->setText("Функции недвойственны");

        if (firstFunctionResultsReversed == firstFunctionResults)
        {
            ui->dualityFunction1Label->setText("Функция самодвойственна");
        }
        else
        {
            ui->dualityFunction1Label->setText("Функция несамодвойственна");
        }

        //(x&y)|(x&z)|(y&z)
        if (secondFunctionResultsReversed == secondFunctionResults)
        {
            ui->dualityFunction2Label->setText("Функция самодвойственна");
        }
        else
        {
            ui->dualityFunction2Label->setText("Функция несамодвойственна");
        }
    }

    else if (!ui->expLineEdit->text().isEmpty() && ui->expLineEdit_2->text().isEmpty())
    {

        if (firstFunctionResultsReversed == firstFunctionResults)
        {
            ui->dualityFunction1Label->setText("Функция самодвойственна");
        }
        else
        {
            ui->dualityFunction1Label->setText("Функция несамодвойственна");
        }
    }

    else if (ui->expLineEdit->text().isEmpty() && !ui->expLineEdit_2->text().isEmpty())
    {
        if (secondFunctionResultsReversed == secondFunctionResults)
        {
            ui->dualityFunction2Label->setText("Функция самодвойственна");
        }
        else
        {
            ui->dualityFunction2Label->setText("Функция несамодвойственна");
        }
    }
}

void LogicCalcWindow::calculate(const QStringList& expr, const QStringList& input, QList<int>& results)
{
    for (int i = 0; i < input.size(); i++)
    {
        QStringList exprToBeParsed = expr;

        for (int j = 0; j < vars.size(); j++)
        {
            int from = 0;
            while (exprToBeParsed.indexOf(vars.at(j), from) != -1)
            {
                exprToBeParsed.replace(exprToBeParsed.indexOf(vars.at(j)), input.at(i).at(j));
                from++;
            }
        }

        results.append(parseExpression(exprToBeParsed.join("")));
    }
}

void LogicCalcWindow::generateDesjunctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression)
{
    int from = 0;
    int indexOfTrue;
    QString desjunctiveFormString;
    bool ok;

    if (numberOfExpression == 1)
        ui->desjLabel->clear();
    else if (numberOfExpression == 2)
        ui->desjLabel_2->clear();

    indexOfTrue = results.indexOf(1, from);

    while (indexOfTrue != -1)
    {
        for (int i = 0; i < vars.size(); i++)
        {
            int variableValue = QString(input.at(indexOfTrue).at(i)).toInt(&ok);
            if (variableValue)
            {
                if (i != vars.size() - 1)
                    desjunctiveFormString.append(vars.at(i) + " & ");
                else
                    desjunctiveFormString.append(vars.at(i));
            }
            else
            {
                if (i != vars.size() - 1)
                    desjunctiveFormString.append("!" + vars.at(i) + " & ");
                else
                    desjunctiveFormString.append("!" + vars.at(i));
            }
        }

        from = indexOfTrue + 1;
        indexOfTrue = results.indexOf(1, from);
        if (indexOfTrue != -1)
            desjunctiveFormString.append(" | ");
    }

    if (numberOfExpression == 1)
        ui->desjLabel->setText(desjunctiveFormString);
    else if (numberOfExpression == 2)
        ui->desjLabel_2->setText(desjunctiveFormString);
}

void LogicCalcWindow::generateConjuctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression)
{
    int from = 0;
    int indexOfFalse;
    QString conjuctiveFormString;
    bool ok;

    if (numberOfExpression == 1)
        ui->conjLabel->clear();
    else if (numberOfExpression == 2)
        ui->conjLabel_2->clear();


    indexOfFalse = results.indexOf(0, from);

    while (indexOfFalse != -1)
    {
        conjuctiveFormString.append("(");
        for (int i = 0; i < vars.size(); i++)
        {
            int variableValue = QString(input.at(indexOfFalse).at(i)).toInt(&ok);
            if (!variableValue)
            {
                if (i != vars.size() - 1)
                    conjuctiveFormString.append(vars.at(i) + " | ");
                else
                    conjuctiveFormString.append(vars.at(i));
            }
            else
            {
                if (i != vars.size() - 1)
                    conjuctiveFormString.append("!" + vars.at(i) + " | ");
                else
                    conjuctiveFormString.append("!" + vars.at(i));
            }
        }
        conjuctiveFormString.append(")");
        from = indexOfFalse + 1;
        indexOfFalse = results.indexOf(0, from);
        if (indexOfFalse != -1)
            conjuctiveFormString.append(" & ");
    }

    if (numberOfExpression == 1)
        ui->conjLabel->setText(conjuctiveFormString);
    else if (numberOfExpression == 2)
        ui->conjLabel_2->setText(conjuctiveFormString);
}

void LogicCalcWindow::minimizeDesjunctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression)
{
    // sort contituents by count of ones
    QVector<QStringList> sortedGroups = createSortedGroups(input, results, 1);

    // glue
    QStringList gluedConstituents = glue(sortedGroups);

    // concatenate vectors
    QStringList notGluedAndGluedConstituents = concatenateVectors(sortedGroups, gluedConstituents);

    // absrob
    QStringList constituentsLeftAfterAbsorb = absorb(notGluedAndGluedConstituents);

    // find minimized form
    QStringList mf = getMinimizedFunction(sortedGroups, constituentsLeftAfterAbsorb);

    // transform to expression form
    QString expressionMDNF = generateExpressionForm(mf, "&");

    if (numberOfExpression == 1)
        ui->MDNF1lineEdit->setText(expressionMDNF);
    else if (numberOfExpression == 2)
        ui->MDNF2lineEdit->setText(expressionMDNF);
}

void LogicCalcWindow::minimizeConjunctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression)
{
    // sort contituents by count of ones
    QVector<QStringList> sortedGroups = createSortedGroups(input, results, 0);

    // glue
    QStringList gluedConstituents = glue(sortedGroups);

    // concatenate vectors
    QStringList notGluedAndGluedConstituents = concatenateVectors(sortedGroups, gluedConstituents);

    // absrob
    QStringList constituentsLeftAfterAbsorb = absorb(notGluedAndGluedConstituents);

    // find minimized form
    QStringList mf = getMinimizedFunction(sortedGroups, constituentsLeftAfterAbsorb);

    // transform to expression form
    QString expressionMCNF = generateExpressionForm(mf, "|");

    if (numberOfExpression == 1)
        ui->MCNF1lineEdit->setText(expressionMCNF);
    else if (numberOfExpression == 2)
        ui->MCNF2lineEdit->setText(expressionMCNF);
}

QVector<QStringList> LogicCalcWindow::createSortedGroups(const QStringList& input, const QList<int>& results, const int type)
{
    QVector<QStringList> sortedGroups(vars.size() + 1);

    if (type == 1)
    {
        for (int i = 0; i < results.size(); i++)
        {
            if (results.at(i) == 1)
            {
                int countOfOnes = input.at(i).count("1");
                sortedGroups[countOfOnes].append(input.at(i));
            }
        }
    }

    else if (type == 0)
    {
        for (int i = 0; i < results.size(); i++)
        {
            if (results.at(i) == 0)
            {
                int countOfOnes = input.at(i).count("1");
                sortedGroups[countOfOnes].append(input.at(i));
            }
        }
    }

    return sortedGroups;
}

QStringList LogicCalcWindow::glue(const QVector<QStringList>& sortedGroups)
{
    QStringList gluedConstituentsFinal;
    QString gluedConstituent;

    // Первый проход склеивания
    for (int i = 0; i < sortedGroups.size() - 1; i++)
    {
        int firstGroupSize = sortedGroups.at(i).size();
        int secondGroupSize = sortedGroups.at(i+1).size();

        for (int j = 0; j < firstGroupSize; j++)
        {
            for (int k = 0; k < secondGroupSize; k++)
            {
                QString firstConstituent = sortedGroups.at(i).at(j);
                QString secondConstituent = sortedGroups.at(i+1).at(k);

                if (glueTwoConstituents(firstConstituent, secondConstituent, gluedConstituent))
                {
                    gluedConstituentsFinal.append(gluedConstituent);
                }

            }
        }
    }

    // Последующие проходы склеивания; до невозможности склеивания
    int countGlues;
    QStringList gluedConstituents;
    QStringList gluedConstituentsFinalCopy = gluedConstituentsFinal;

    do {

        countGlues = 0;

        for (int i = 0; i < gluedConstituentsFinalCopy.size() - 1; i++)
        {
            QString firstConstituent = gluedConstituentsFinalCopy.at(i);

            for (int j = i + 1; j < gluedConstituentsFinalCopy.size(); j++)
            {
                QString secondConstituent = gluedConstituentsFinalCopy.at(j);
                if (glueTwoConstituents(firstConstituent, secondConstituent, gluedConstituent))
                {
                    gluedConstituents.append(gluedConstituent);
                    countGlues++;
                }
            }
        }

        if (countGlues != 0)
        {
            gluedConstituents.removeDuplicates();

            gluedConstituentsFinal.append(gluedConstituents);

            gluedConstituentsFinalCopy = gluedConstituents;

            gluedConstituents.clear();
        }

    } while (countGlues > 0);



    return gluedConstituentsFinal;

}

bool LogicCalcWindow::glueTwoConstituents(QString first, const QString &second, QString& gluedConstituent)
{
    int diffs = 0, diffPos;
    for (int i = 0; i < first.size(); i++)
    {
        if (first[i] != second[i])
        {
            diffs++;
            diffPos = i;
        }
    }

    if (diffs == 1)
    {
        gluedConstituent = first.replace(diffPos, 1, "X");
        return true;
    }

    return false;
}

QStringList LogicCalcWindow::concatenateVectors(const QVector<QStringList>& sortedGroups, const QStringList& gluedConstituents)
{
    QStringList notGluedConstituents;
    for (auto group : sortedGroups)
    {
        if (!group.isEmpty())
        {
            for (auto constituent : group)
                notGluedConstituents.append(constituent);
        }
    }
    return notGluedConstituents + gluedConstituents;
}

QStringList LogicCalcWindow::absorb(QStringList& constituents)
{
    QStringList constituentsLeftAfterAbsorb;
    int i = constituents.size() - 1;
    do
    {

        QString firstConstituent = constituents.at(i);
        firstConstituent.replace("X", ".");
        QRegExp rx(firstConstituent);

        for (int j = i - 1; j >= 0; j--)
        {
            QString secondConstituent = constituents.at(j);

            if (rx.exactMatch(secondConstituent))
            {
                constituents.removeAt(j);
            }
        }
        constituentsLeftAfterAbsorb << constituents.takeLast();
        i = constituents.size() - 1;
    } while (i > 0);

    return constituentsLeftAfterAbsorb;
}
// x & y & !z | x & !y & z | x & !y & !z | !x & !y & z
QStringList LogicCalcWindow::getMinimizedFunction(const QVector<QStringList>& sortedGroups, const QStringList& constituentsLeftAfterAbsorb)
{
    // create table
    // fill headers
    QStringList headers;
    for (auto group : sortedGroups)
    {
        if (!group.isEmpty())
        {
            headers.append(group);
        }
    }

    // fill rows
    QStringList rows = constituentsLeftAfterAbsorb;

    // fill table
    QVector<QVector<QString>> table(rows.size());

    for (int i = 0; i < rows.size(); i++)
    {
        table[i].resize(headers.size());
        QString tempRow = rows[i];
        tempRow.replace("X", ".");
        QRegExp rx(tempRow);

        for (int j = 0; j < headers.size(); j++)
        {
            if (rx.exactMatch(headers[j]))
            {
                table[i][j] = "V";
            }
        }
    }

    QStringList kernel;
    int countVs;
    QVector<int> rowsWithV, colsWithV;

    // find kernel
    for (int i = 0; i < headers.size(); i++)
    {
        countVs = 0;
        int rowWithVIndex, colWithVIndex;
        for (int j = 0; j < rows.size(); j++)
        {
            if (table[j][i] == "V")
            {
                countVs++;
                rowWithVIndex = j;
                colWithVIndex = i;
            }
        }
        if (countVs == 1)
        {
            if (!kernel.contains(rows[rowWithVIndex]))
                kernel.append(rows[rowWithVIndex]);
            if (!rowsWithV.contains(rowWithVIndex))
                rowsWithV.append(rowWithVIndex);
        }
        colsWithV.append(colWithVIndex);
    }

    deleteRowsAndCols(rowsWithV, colsWithV, table, rows, headers);

    /*while (!table.isEmpty())
    {
    }*/

    return kernel;
}

void LogicCalcWindow::deleteRowsAndCols(QVector<int>& rowsWithV, const QVector<int>& colsWithV, QVector<QVector<QString>>& table, QStringList& rows, QStringList& headers)
{
    qSort(rowsWithV.begin(), rowsWithV.end(), qGreater<int>());
    // delete columns from table
    for (int i = 0; i < table.size(); i++)
    {
        for (int j = colsWithV.size() - 1; j >= 0; j--)
        {
            table[i].removeAt(colsWithV.at(j));
        }
    }

    // delete rows from table and rows labels from list
    for (int i = 0; i < rowsWithV.size(); i++)
    {
        table.removeAt(rowsWithV.at(i));
        rows.removeAt(rowsWithV.at(i));
    }


    // delete column labels (headers) from list
    for (int i = colsWithV.size() - 1; i >= 0; i--)
    {
        headers.removeAt(colsWithV.at(i));
    }

    // delete empty rows from table
    QMutableVectorIterator<QVector<QString>> it(table);
    while (it.hasNext())
    {
        if (it.next().isEmpty())
            it.remove();
    }
}

QString LogicCalcWindow::generateExpressionForm(const QStringList& mf, const QString& delim)
{
    QStringList resultExpression;

    if (delim == "&")
    {
        for (QString term : mf)
        {
            QString printableTerm;
            for (int i = 0; i < term.size(); i++)
            {
                if (term.at(i) == 'X')
                    continue;
                else if (term.at(i) == '1')
                    printableTerm.append(vars[i] + " " + delim + " ");

                else if (term.at(i) == '0')
                    printableTerm.append("!" + vars[i] + " " + delim + " ");
            }
            printableTerm.chop(3);
            resultExpression.append(printableTerm);
        }
        return resultExpression.join(" | ");
    }

    else if (delim == "|")
    {
        for (QString term : mf)
        {
            QString printableTerm;
            for (int i = 0; i < term.size(); i++)
            {
                if (term.at(i) == 'X')
                    continue;
                else if (term.at(i) == '0')
                    printableTerm.append(vars[i] + " " + delim + " ");

                else if (term.at(i) == '1')
                    printableTerm.append("!" + vars[i] + " " + delim + " ");
            }
            printableTerm.chop(3);
            resultExpression.append(printableTerm);
        }
        return "(" + resultExpression.join(") & (") + ")";
    }

    return "";
}
// ∧ ∨
enum FunctionType LogicCalcWindow::functionType(const QList<int>& results, int numberOfExpression)
{
    if (results.indexOf(0) == -1)
    {
        if (numberOfExpression == 1)
            ui->functionTypeLabel->setText(QString::fromUtf8("Выполнимая, тождественно истинная"));
        else if (numberOfExpression == 2)
            ui->functionTypeLabel_2->setText(QString::fromUtf8("Выполнимая, тождественно истинная"));
        return ALWAYSTRUE;
    }
    else if (results.indexOf(1) == -1)
    {
        if (numberOfExpression == 1)
            ui->functionTypeLabel->setText(QString::fromUtf8("Опровержимая, тождественно ложная"));
        else if (numberOfExpression == 2)
            ui->functionTypeLabel_2->setText(QString::fromUtf8("Опровержимая, тождественно ложная"));

        return ALWAYSFALSE;
    }
    else
    {
        if (numberOfExpression == 1)
            ui->functionTypeLabel->setText(QString::fromUtf8("Выполнимая, опровержимая"));
        else if (numberOfExpression == 2)
            ui->functionTypeLabel_2->setText(QString::fromUtf8("Выполнимая, опровержимая"));
        return DOABLE;
    }
    throw NONE_FUNCTION_TYPE;
}

void LogicCalcWindow::splitExpression(const QString& expr, QStringList& splittedExpression, QString& errorString)
{

    if (expr.isEmpty())
        throw EMPTY_STRING;

    YY_BUFFER_STATE buf;
    QStringList errorTokens;
    int token = 1;

    vars.clear();
    lexemas.clear();
    buf = yy_scan_string(expr.toStdString().c_str());
    while (token != 0)
    {
        token = yylex();
        if (token != yytokentype::ERR && token != 0)
        {
            QPair<yytokentype, QString> lexema((yytokentype)token, yytext);
            lexemas.append(lexema);
            splittedExpression.append(yytext);

            if (token == yytokentype::VAR)
            {
                vars.append(yytext);
            }
        }
        else if (token == yytokentype::ERR && yytext != "")
        {
            errorTokens.append(yytext);
        }

    }
    if (!errorTokens.isEmpty())
    {
        errorString = errorTokens.join(" ");
        throw BAD_TOKEN;
    }

    vars.removeDuplicates();
    yy_delete_buffer(buf);
}

int LogicCalcWindow::parseExpression(const QString& expression)
{
    bool result;
    YY_BUFFER_STATE buf;
    buf = yy_scan_string(expression.toStdString().c_str());

    int status = yyparse(&result);
    yy_delete_buffer(buf);

    return result;
}

void LogicCalcWindow::fillTruthTableWidget(const QStringList& input, const QList<int>& results, int numberOfExpression)
{
    if (numberOfExpression == 1)
    {
        ui->truthTableWidget->setRowCount(qPow(2, vars.size()));
        ui->truthTableWidget->setColumnCount(vars.size() + 1);
        ui->truthTableWidget->setHorizontalHeaderLabels(vars);
        ui->truthTableWidget->setHorizontalHeaderItem(vars.size(), new QTableWidgetItem("Result"));

        for (int i = 0; i < input.size(); i++)
        {
            for (int j = 0; j < vars.size(); j++)
            {
                ui->truthTableWidget->setItem(i, j, new QTableWidgetItem(input.at(i).at(j)));
            }
            ui->truthTableWidget->setItem(i, vars.size(), new QTableWidgetItem(QString::number(results.at(i))));
        }
    }
    else if (numberOfExpression == 2)
    {
        ui->truthTableWidget_2->setRowCount(qPow(2, vars.size()));
        ui->truthTableWidget_2->setColumnCount(vars.size() + 1);
        ui->truthTableWidget_2->setHorizontalHeaderLabels(vars);
        ui->truthTableWidget_2->setHorizontalHeaderItem(vars.size(), new QTableWidgetItem("Result"));

        for (int i = 0; i < input.size(); i++)
        {
            for (int j = 0; j < vars.size(); j++)
            {
                ui->truthTableWidget_2->setItem(i, j, new QTableWidgetItem(input.at(i).at(j)));
            }
            ui->truthTableWidget_2->setItem(i, vars.size(), new QTableWidgetItem(QString::number(results.at(i))));
        }
    }
}

void LogicCalcWindow::generateTTInput(QStringList& input)
{
    for (int i = 0; i < qPow(2, vars.size()); i++)
    {
        QString binaryInput = QString::number(i, 2);

        while (binaryInput.length() < vars.size())
        {
            binaryInput.prepend("0");
        }
        input.append(binaryInput);
    }
}

void LogicCalcWindow::invertColumn(QList<int>& resultsColumn)
{
    for (int i = 0; i < resultsColumn.size(); i++)
    {
        if (resultsColumn[i] == 0)
            resultsColumn[i] = 1;
        else
            resultsColumn[i] = 0;
    }
}

void LogicCalcWindow::reverseColumn(QList<int>& resultsColumn)
{
    std::list<int> firstFunctionresultsSTD = resultsColumn.toStdList();
    std::reverse(firstFunctionresultsSTD.begin(), firstFunctionresultsSTD.end());
    resultsColumn = QList<int>::fromStdList(firstFunctionresultsSTD);
}
