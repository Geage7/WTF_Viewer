#include "graphdialog.h"
#include "ui_graphdialog.h"

#include <QDebug>

GraphDialog::GraphDialog(Frames *frames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphDialog)
{
    ui->setupUi(this);

//    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->DataY->addItem("All ");
    foreach(quint16 value, frames->getClustersLenghtList())
        ui->DataY->addItem(QString::number(value));

//    QApplication::restoreOverrideCursor();

    connect(ui->DataX, SIGNAL(currentTextChanged(QString)), this, SLOT(slotSelectDataX(QString)));

    ui->windowGraph->addItem(NEW_WINDOW);
}

GraphDialog::~GraphDialog()
{
    delete ui;
}

quint16 GraphDialog::getCurrentClusterLenght()
{
    return quint16(ui->DataY->currentText().toInt());
}

QString GraphDialog::getCurrentX()
{
    return ui->DataX->currentText();
}

QString GraphDialog::getCurrentY()
{
    return ui->DataY->currentText();
}

QString GraphDialog::getCurrentWindowGraph()
{
    return  ui->windowGraph->currentText();
}

void GraphDialog::selectLastWindow()
{
    ui->windowGraph->setCurrentIndex(ui->windowGraph->count() - 1);
}

void GraphDialog::clearWindow()
{
    ui->windowGraph->clear();
    ui->windowGraph->addItem(NEW_WINDOW);
}

void GraphDialog::appendWindow(QString value)
{
    ui->windowGraph->addItem(value);
}

void GraphDialog::slotSelectDataX(QString value)
{
    if(value == ui->DataX->itemText(0))
    {
        ui->DataY->setEnabled(true);
        ui->coefficients->setDisabled(true);
    }
    else if(value == ui->DataX->itemText(1))
    {
        ui->DataY->setDisabled(true);
        ui->coefficients->setDisabled(true);
    }
    else if(value == ui->DataX->itemText(2))
    {
        ui->DataY->setEnabled(true);
        ui->coefficients->setEnabled(true);
    }

    emit signalDataXChanged(value);
}
