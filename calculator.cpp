#include "calculator.h"
#include "ui_calculator.h"
#include <math.h>

double valueCal = 0.0;
double priorValue = 0.0;
bool waitingForOperand = true;
QString pendingAddSub;
QString pendingMulDiv;

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{

    ui->setupUi(this);

    ui->screen->setText("0");
    ui->screen->setReadOnly(true);
    ui->screen->setMaxLength(16);
    ui->screen->setText(QString::number(valueCal));

    QPushButton *numButton[10];
    for(int i=0; i<10;i++){
        QString btnName = "Button" + QString::number(i);
        numButton[i] = Calculator::findChild<QPushButton *>(btnName);
        connect(numButton[i], SIGNAL(released()), this, SLOT(numberPress()));
    }
    connect(ui->ButtonClear, SIGNAL(released()), this, SLOT(clearPress()));
    connect(ui->ButtonPoint, SIGNAL(released()), this, SLOT(poitPress()));
    connect(ui->ButtonChangeSign, SIGNAL(released()), this, SLOT(changeNumberSignPress()));
    connect(ui->ButtonReciprocal, SIGNAL(released()), this, SLOT(unOperatorPress()));
    connect(ui->ButtonSquare, SIGNAL(released()), this, SLOT(unOperatorPress()));
    connect(ui->ButtonSqrt, SIGNAL(released()), this, SLOT(unOperatorPress()));
    connect(ui->ButtonSin, SIGNAL(released()), this, SLOT(unOperatorPress()));
    connect(ui->ButtonCos, SIGNAL(released()), this, SLOT(unOperatorPress()));
    connect(ui->ButtonTag, SIGNAL(released()), this, SLOT(unOperatorPress()));
    connect(ui->ButtonBack, SIGNAL(released()), this, SLOT(backPress()));
    connect(ui->ButtonEqual, SIGNAL(released()), this, SLOT(equalPress()));
    connect(ui->ButtonAdd, SIGNAL(released()), this, SLOT(addSubPress()));
    connect(ui->ButtonSubtract, SIGNAL(released()), this, SLOT(addSubPress()));
    connect(ui->ButtonDivide, SIGNAL(released()), this, SLOT(mulDivPress()));
    connect(ui->ButtonMultiply, SIGNAL(released()), this, SLOT(mulDivPress()));
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::numberPress()
{
    QPushButton *clickedButton = (QPushButton *)sender();
    int digitValue = clickedButton->text().toInt();
    if (ui->screen->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        ui->screen->clear();
        waitingForOperand = false;
    }
    ui->screen->setText(ui->screen->text() + QString::number(digitValue));
}

void Calculator::clearPress()
{
    clearAll();
    ui->screen->setText("0");

}

void Calculator::poitPress()
{
    if (waitingForOperand)
        ui->screen->setText("0");
    if (!ui->screen->text().contains("."))
        ui->screen->setText(ui->screen->text() + tr("."));
    waitingForOperand = false;
}

void Calculator::changeNumberSignPress()
{
    QString text = ui->screen->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    ui->screen->setText(text);
}

void Calculator::unOperatorPress()
{
    QPushButton *clickedButton = (QPushButton *)sender();
    QString textOperator = clickedButton->text();
    double operand = ui->screen->text().toDouble();
    double result = 0.0;

    if (textOperator == tr("sqrt")) {
        if (operand < 0.0) {
            showError();
            return;
        }
        result = sqrt(operand);
    } else if (textOperator == tr("x2")) {
        result = pow(operand, 2.0);
    } else if (textOperator == tr("1/x")) {
        if (operand == 0.0) {
            showError();
            return;
        }
        result = 1.0 / operand;
    } else if (textOperator == tr("sin(x)")){
        result = sin(operand * M_PI / 180);
    } else if (textOperator == tr("cos(x)")){
        result = cos(operand * M_PI / 180);
    } else if (textOperator == tr("tag(x)")){
        result = tan(operand * M_PI / 180);
    }
    ui->screen->setText(QString::number(result));
    waitingForOperand = true;
}

void Calculator::showError()
{
    clearAll();
    ui->screen->setText("Error");
}

void Calculator::clearAll(){
    priorValue = 0.0;
    valueCal = 0.0;
    waitingForOperand = true;
}
void Calculator::backPress()
{
    if (waitingForOperand)
        return;

    QString text = ui->screen->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    ui->screen->setText(text);
}

void Calculator::addSubPress()
{
    QPushButton *clickedButton = (QPushButton *)sender();
    QString textOperator = clickedButton->text();
    double operand = ui->screen->text().toDouble();

    if (!pendingMulDiv.isEmpty()) {
        if (!calculate(operand, pendingMulDiv)) {
            showError();
            return;
        }
        ui->screen->setText(QString::number(priorValue));
        operand = priorValue;
        priorValue = 0.0;
        pendingMulDiv.clear();
    }

    if (!pendingAddSub.isEmpty()) {
        if (!calculate(operand, pendingAddSub)) {
            showError();
            return;
        }
        ui->screen->setText(QString::number(valueCal));
    } else {
        valueCal = operand;
    }

    pendingAddSub = textOperator;
    waitingForOperand = true;
}

void Calculator::mulDivPress()
{
    QPushButton *clickedButton = (QPushButton *)sender();
    QString textOperator = clickedButton->text();
    double operand = ui->screen->text().toDouble();

    if (!pendingMulDiv.isEmpty()) {
        if (!calculate(operand, pendingMulDiv)) {
            showError();
            return;
        }
        ui->screen->setText(QString::number(priorValue));
    } else {
        priorValue = operand;
    }

    pendingMulDiv = textOperator;
    waitingForOperand = true;
}

bool Calculator::calculate(double nextOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        valueCal += nextOperand;
    } else if (pendingOperator == tr("-")) {
        valueCal -= nextOperand;
    } else if (pendingOperator == tr("x")) {
        priorValue *= nextOperand;
    } else if (pendingOperator == tr("÷")) {
        if (nextOperand == 0.0)
            return false;
        priorValue /= nextOperand;
    }
    return true;
}

void Calculator::equalPress()
{
    double operand = ui->screen->text().toDouble();

    if (!pendingMulDiv.isEmpty()) {
        if (!calculate(operand, pendingMulDiv)) {
            showError();
            return;
        }
        operand = priorValue;
        priorValue = 0.0;
        pendingMulDiv.clear();
    }
    if (!pendingAddSub.isEmpty()) {
        if (!calculate(operand, pendingAddSub)) {
            showError();
            return;
        }
        pendingAddSub.clear();
    } else {
        valueCal = operand;
    }

    ui->screen->setText(QString::number(valueCal));
    valueCal = 0.0;
    waitingForOperand = true;
}

