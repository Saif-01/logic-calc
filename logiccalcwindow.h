#ifndef LOGICCALCWINDOW_H
#define LOGICCALCWINDOW_H

#include <QMainWindow>
#include <qmath.h>
#include <QDebug>
#include <QMessageBox>
#include "lexer.hpp"
#include "parser.hpp"
#include "enums.h"


namespace Ui {
class LogicCalcWindow;
}


class LogicCalcWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogicCalcWindow(QWidget *parent = 0);
    ~LogicCalcWindow();


private slots:
    void lineEntered();

private:

    void splitExpression(const QString& expr, QStringList& splittedExpression, QString& errorString);

    void fillTruthTableWidget(const QStringList& input, const QList<int>& results, int numberOfExpression);

    void generateTTInput(QStringList& input);

    void calculate(const QStringList& expr, const QStringList& input, QList<int>& results);

    int parseExpression(const QString& expression);

    enum FunctionType functionType(const QList<int>& results, int numberOfExpression);

    void generateDesjunctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression);

    void generateConjuctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression);

    void expEntered(const QString& enteredExpr, int numberOfExpression);

    void checkExpressionsEntered();

    void checkFunctionsForDuality();

    void invertColumn(QList<int>& resultsColumn);

    void reverseColumn(QList<int>& resultsColumn);

    void minimizeDesjunctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression);

    void minimizeConjunctiveNormalForm(const QStringList& input, const QList<int>& results, const int numberOfExpression);

    QVector<QStringList> createSortedGroups(const QStringList& input, const QList<int>& results, const int type);

    QStringList glue(const QVector<QStringList>& sortedGroups);

    bool glueTwoConstituents(QString first, const QString &second, QString& gluedConstituent);

    QStringList concatenateVectors(const QVector<QStringList>& sortedGroups, const QStringList& gluedConstituents);

    QStringList absorb(QStringList& constituents);

    QStringList getMinimizedFunction(const QVector<QStringList>& sortedGroups, const QStringList& constituentsLeftAfterAbsorb);

    void deleteRowsAndCols(QVector<int>& rowsWithV, const QVector<int>& colsWithV, QVector<QVector<QString> >& table, QStringList& rows, QStringList& headers);

    QString generateExpressionForm(const QStringList& mf, const QString& delim);

    Ui::LogicCalcWindow *ui;

    QList<QPair<yytokentype, QString> > lexemas;
    QStringList vars;
    QString lexerErrorString;
};

#endif // LOGICCALCWINDOW_H
