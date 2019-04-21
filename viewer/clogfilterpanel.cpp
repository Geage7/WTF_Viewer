#include "clogfilterpanel.h"
#include "ui_clogfilterpanel.h"

#include <QKeyEvent>
#include <QDebug>

ClogFilterPanel::ClogFilterPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClogFilterPanel)
{
    ui->setupUi(this);

    connect(ui->clusterRangeGroup,  SIGNAL(clicked(bool)), this, SLOT(slotEnableRange()));
    connect(ui->totRangeGroup,      SIGNAL(clicked(bool)), this, SLOT(slotEnableRange()));

    connect(ui->apply,              SIGNAL(clicked()),          this, SLOT(slotApplyFilter()) );
}

ClogFilterPanel::~ClogFilterPanel()
{
    delete ui;
}

void ClogFilterPanel::setClusterRange(QVector<int> vector)
{
    disconnectSignals();

    ui->clusterRangeBegin->clear();
    ui->clusterRangeEnd->clear();

    clusterListModel.clear();
    clusterListBegin.clear();
    clusterListEnd.clear();

    foreach(int value, vector)
        clusterListModel << QString::number(value);

    ui->clusterRangeBegin->addItems(clusterListModel);

    if(ui->clusterRangeGroup->isChecked())
        ui->clusterRangeBegin->setCurrentText(clusterBeginLast);
    else
        ui->clusterRangeBegin->setCurrentText(clusterListModel.first());

    int startFrom = 0;
    for (int i = 0; i < clusterListModel.length(); ++i)
        if(clusterListModel.at(i) == clusterBeginLast)
        {
            startFrom = i;
            break;
        }

    for (int i = startFrom; i < clusterListModel.length(); ++i)
        clusterListEnd << clusterListModel.at(i);

    ui->clusterRangeEnd->clear();
    ui->clusterRangeEnd->addItems(clusterListEnd);

    if(clusterListEnd.contains(clusterEndLast) && ui->clusterRangeGroup->isChecked())
        ui->clusterRangeEnd->setCurrentText(clusterEndLast);
    else
        ui->clusterRangeEnd->setCurrentText(clusterListModel.last());

    connectSignals();

}
void ClogFilterPanel::slotDates(QString value)
{
    if(value == "")
        return;

    QComboBox* pCB = static_cast<QComboBox*>(sender());
    QString currentText;
    if(pCB->objectName() == "clusterRangeBegin")
    {
        disconnectSignals();

        clusterListEnd.clear();

        int startFrom = 0;
        for (int i = 0; i < clusterListModel.length(); ++i)
            if(clusterListModel.at(i) == value)
            {
                startFrom = i;
                break;
            }

        for (int i = startFrom; i < clusterListModel.length(); ++i)
            clusterListEnd << clusterListModel.at(i);

        ui->clusterRangeEnd->clear();
        ui->clusterRangeEnd->addItems(clusterListEnd);

        if(clusterListEnd.contains(clusterEndLast))
            ui->clusterRangeEnd->setCurrentText(clusterEndLast);
        else
            ui->clusterRangeEnd->setCurrentText(clusterListModel.last());

        if(ui->clusterRangeGroup->isChecked())
            clusterBeginLast = value;

        connectSignals();
    }
    else if(pCB->objectName() == "clusterRangeEnd")
    {
        disconnectSignals();

        if(ui->clusterRangeGroup->isChecked())
            clusterEndLast = value;

        connectSignals();
    }
    else if(pCB->objectName() == "totRangeBegin")
    {
        disconnectSignals();

        totListEnd.clear();

        int startFrom = 0;
        for (int i = 0; i < totListModel.length(); ++i)
            if(totListModel.at(i) == value)
            {
                startFrom = i;
                break;
            }

        for (int i = startFrom; i < totListModel.length(); ++i)
            totListEnd << totListModel.at(i);

        ui->totRangeEnd->clear();
        ui->totRangeEnd->addItems(totListEnd);

        if(totListEnd.contains(totEndLast))
            ui->totRangeEnd->setCurrentText(totEndLast);
        else
            ui->totRangeEnd->setCurrentText(totListModel.last());

        if(ui->totRangeGroup->isChecked())
            totBeginLast = value;

        connectSignals();
    }
    else if(pCB->objectName() == "totRangeEnd")
    {
        disconnectSignals();

        if(ui->totRangeGroup->isChecked())
            totEndLast = value;

        connectSignals();
    }
}

void ClogFilterPanel::slotApplyFilter()
{
    if(ui->clusterRangeGroup->isChecked())
    {
        clusterBeginLast = ui->clusterRangeBegin->currentText();
        clusterEndLast   = ui->clusterRangeEnd->currentText();
    }
    if(ui->totRangeGroup->isChecked())
    {
        totBeginLast = ui->totRangeBegin->currentText();
        totEndLast   = ui->totRangeEnd->currentText();
    }

    emit signalApplyFilter();
}

void ClogFilterPanel::setTotRange(QVector<int> vector)
{
    disconnectSignals();

    ui->totRangeBegin->clear();
    ui->totRangeEnd->clear();

    totListModel.clear();
    totListBegin.clear();
    totListEnd.clear();

    foreach(int value, vector)
        totListModel << QString::number(value);

    ui->totRangeBegin->addItems(totListModel);

    if(ui->totRangeGroup->isChecked())
        ui->totRangeBegin->setCurrentText(totBeginLast);
    else
        ui->totRangeBegin->setCurrentText(totListModel.first());

    int startFrom = 0;
    for (int i = 0; i < totListModel.length(); ++i)
        if(totListModel.at(i) == totBeginLast)
        {
            startFrom = i;
            break;
        }

    for (int i = startFrom; i < totListModel.length(); ++i)
        totListEnd << totListModel.at(i);

    ui->totRangeEnd->clear();
    ui->totRangeEnd->addItems(totListEnd);

    if(totListEnd.contains(totEndLast) && ui->totRangeGroup->isChecked())
        ui->totRangeEnd->setCurrentText(totEndLast);
    else
        ui->totRangeEnd->setCurrentText(totListModel.last());

    connectSignals();
}

int ClogFilterPanel::getClusterBegin() const
{
    return int(ui->clusterRangeBegin->currentText().toInt());
}

int ClogFilterPanel::getClusterEnd() const
{
    return int(ui->clusterRangeEnd->currentText().toInt());
}

int ClogFilterPanel::getTotBegin() const
{
    return int(ui->totRangeBegin->currentText().toInt());
}

int ClogFilterPanel::getTotEnd() const
{
    return int(ui->totRangeEnd->currentText().toInt());
}

bool ClogFilterPanel::isClusterEnable()
{
    return ui->clusterRangeGroup->isChecked();
}

bool ClogFilterPanel::isTotEnable()
{
    return ui->totRangeGroup->isChecked();
}

bool ClogFilterPanel::isAllTotInCluster()
{
    return ui->allTotInCluster->isChecked();
}

bool ClogFilterPanel::isMediPix()
{
    return ui->midiPixRadioButton->isChecked();
}

void ClogFilterPanel::disconnectSignals()
{
    disconnect(ui->clusterRangeBegin,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
    disconnect(ui->clusterRangeEnd,    SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));

    disconnect(ui->totRangeBegin,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
    disconnect(ui->totRangeEnd,    SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
}

void ClogFilterPanel::connectSignals()
{
    connect(ui->clusterRangeBegin,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
    connect(ui->clusterRangeEnd,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));

    connect(ui->totRangeBegin,  SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
    connect(ui->totRangeEnd,    SIGNAL(currentTextChanged(QString)), this, SLOT(slotDates(QString)));
}

void ClogFilterPanel::slotEnableRange()
{
    QGroupBox* groupBox = static_cast<QGroupBox*>(sender());

    if(groupBox->objectName() == "clusterRangeGroup")
    {
        if(groupBox->isChecked())
        {
            ui->clusterRangeBegin->setEnabled(true);
            ui->clusterRangeEnd->setEnabled(true);

            ui->clusterRangeBegin->setCurrentText(clusterBeginLast);
            ui->clusterRangeEnd->setCurrentText(clusterEndLast);
        }
        else
        {
            ui->clusterRangeBegin->setEnabled(false);
            ui->clusterRangeEnd->setEnabled(false);

            ui->clusterRangeBegin->setCurrentText(clusterListModel.first());
            ui->clusterRangeEnd->setCurrentText(clusterListModel.last());
        }
    }
    else if(groupBox->objectName() == "totRangeGroup")
    {
        if(groupBox->isChecked())
        {
            ui->totRangeBegin->setEnabled(true);
            ui->totRangeEnd->setEnabled(true);

            ui->totRangeBegin->setCurrentText(totBeginLast);
            ui->totRangeEnd->setCurrentText(totEndLast);

            ui->allTotInCluster->setEnabled(true);
        }
        else
        {
            ui->totRangeBegin->setEnabled(false);
            ui->totRangeEnd->setEnabled(false);

            ui->totRangeBegin->setCurrentText(totListModel.first());
            ui->totRangeEnd->setCurrentText(totListModel.last());

            ui->allTotInCluster->setEnabled(false);
            if(!ui->allTotInCluster->isChecked())
                ui->allTotInCluster->toggle();
        }
    }
}

void ClogFilterPanel::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        emit signalApplyFilter();
}

bool ClogFilterPanel::event(QEvent *event)
{

//  qDebug() << event;

    return QWidget::event(event);
}

//bool ClogFilterPanel::event(QEvent *event)
//{
//    if(event->type() == QEvent::KeyRelease)
//    {
//        QKeyEvent* kEv = static_cast<QKeyEvent*>(event);
//        if(kEv->key() == Qt::Key_Enter)
//            emit signalApplyFilter();
//    }

//    return QWidget::event(event);
//}
