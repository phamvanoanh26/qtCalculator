#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

class Calculator : public QMainWindow
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();

private:
    Ui::Calculator *ui;

private slots:
    void numberPress();
    bool calculate(double nextOperand, const QString &pendingOperator);
    void changeNumberSignPress();
    void unOperatorPress();
    void equalPress();
    void poitPress();
    void clearPress();
    void showError();
    void clearAll();
    void backPress();
    void addSubPress();
    void mulDivPress();
};
#endif // CALCULATOR_H
