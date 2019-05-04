#include "centralwidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QStatusBar>
#include <limits>

#include <QApplication>

CentralWidget::CentralWidget(QWidget *parent) : QMainWindow(parent)
{
    panelWidget.setParent(this);

    createMenu();

    statusBar()->insertWidget(0, &statusBarWidget);

    XYDefault();

    QFont font = chart.titleFont();
    font.setPixelSize(18);
    chart.setTitleFont(QFont(font));

    createAxes();

    chart.axisX()->setTitleText("X");
    chart.axisY()->setTitleText("Y");
    chart.setDropShadowEnabled(true);

    chartView.setChart(&chart);

    layout.addWidget(&chartView);
    layout.addWidget(&panelWidget);

    layout.setStretch(0, 1);
    centralWidget.setLayout(&layout);
    setCentralWidget(&centralWidget);
    chartView.setFocus();

    connect(&panelWidget,    SIGNAL(signalChangeTheme(int)), this,
                             SLOT(slotSetTheme(int)));

    connect(&panelWidget,    SIGNAL(signalSetLegendPosition(int)), this,
                             SLOT(slotSetLegentPosition(int)));

    connect(&panelWidget,    SIGNAL(signalSetTitile(QString)), this,
                             SLOT(slotSetTitle(QString)));

    connect(&panelWidget,    SIGNAL(signalAnimation(bool)), this,
                             SLOT(slotAnimation(bool)));

    connect(&panelWidget,    SIGNAL(signalAntialiasing(bool)), this,
                             SLOT(slotAntialiasing(bool)));

    connect(&panelWidget,    SIGNAL(signalTickCountChangeX(int)), this,
                             SLOT(slotSetTcickCountX(int)));
    connect(&panelWidget,    SIGNAL(signalTickCountChangeY(int)), this,
                             SLOT(slotSetTcickCountY(int)));

    connect(&panelWidget,    SIGNAL(signalSeriesDeleted()), this,
                             SLOT(slotReRange()));

    connect(&panelWidget,    SIGNAL(signalRubberMode(QChartView::RubberBand)), this,
                             SLOT(slotSetRubberMode(QChartView::RubberBand)));

    connect(&panelWidget,    SIGNAL(signalAxisXRangeChanged(qreal, qreal)), this,
                             SLOT(slotRangeXSet(qreal, qreal)));

    connect(&panelWidget,    SIGNAL(signalAxisYRangeChanged(qreal, qreal)), this,
                             SLOT(slotRangeYSet(qreal, qreal)));

    connect(&panelWidget,    SIGNAL(signalSeriesTypeChange()), this,
                             SLOT(slotSeriesTypeChanged()) );


    connect(&chartView,      SIGNAL(signalMousePosition(QPointF)), this,
                             SLOT(slotViewXYCoordinate(QPointF)));
}

CentralWidget::~CentralWidget()
{
//    delete pMenuFile;
}

void CentralWidget::addSeries(QVector<QPointF> pointVector, QXYSeries::SeriesType type, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    QXYSeries* series = nullptr;

    switch (type) {
    case QXYSeries::SeriesTypeLine:
        series = new QLineSeries();
        break;
    case QXYSeries::SeriesTypeSpline:
        series = new QSplineSeries();
        break;
    case QXYSeries::SeriesTypeScatter:
        series = new QScatterSeries();
        break;
    default:
        break;
    }

    foreach (QPointF point, pointVector)
    {
        *series << point;

        if(point.x() > maxX) maxX = point.x();
        if(point.y() > maxY) maxY = point.y();

        if(point.x() < minX) minX = point.x();
        if(point.y() < minY) minY = point.y();
    }

    series->setName(legendTitle);
    chart.addSeries(series);
    panelWidget.addSeriesList(series);

    if(chart.series().length() > 2) //1 - axis x, 2 - axis y
    {
        QXYSeries* ser = static_cast<QXYSeries*>(chart.series().last());
        QColor newColor = series->color();
        QPen pen = ser->pen();
        pen.setColor(newColor);
        series->setPen(pen);
        if(series->type() == QAbstractSeries::SeriesTypeScatter)
        {
            QScatterSeries* scatSer = static_cast<QScatterSeries*>(series);
            scatSer->setMarkerSize(pen.width());
            scatSer->setBorderColor(newColor);
        }
    }

//Что-бы правильно работало нажатие Esc >>
    chartView.rangeX.min = minX;
    chartView.rangeX.max = maxX;

    chartView.rangeY.min = minY;
    chartView.rangeY.max = maxY;
//Что-бы правильно работало нажатие Esc <<

    createAxes();

    chart.axisX()->setRange(minX, maxX);
    chart.axisX()->setTitleText(axsisX_Title);
    chart.axisY()->setRange(minY, maxY);
    chart.axisY()->setTitleText(axsisY_Title);
}

void CentralWidget::addSeries(QVector<QPointF> pointVector, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    addSeries(pointVector, QXYSeries::SeriesType(panelWidget.getSeriesType()), legendTitle, axsisX_Title, axsisY_Title);
}

void CentralWidget::setTitle(QString title)
{
    chart.setTitle(title);
    panelWidget.setTitle(chart.title());
}

QString CentralWidget::getTitle()
{
    return chart.title();
}

QString CentralWidget::getDataXType()
{
    return chart.axisX()->titleText();
}
void CentralWidget::XYDefault()
{
    lineSeriesX.setPen(QPen(QColor(Qt::black)));
    lineSeriesX << QPointF(0, 0) << QPointF(chart.maximumWidth(), 0);
    lineSeriesX.setName("X");
    chart.addSeries(&lineSeriesX);

    lineSeriesY.setPen(QPen(QColor(Qt::black)));
    lineSeriesY << QPointF(0, 0) << QPointF(0, chart.maximumWidth());
    lineSeriesY.setName("Y");

    chart.addSeries(&lineSeriesY);
}

void CentralWidget::createMenu()
{
    menuFile.setTitle("File");
    menuFile.addAction(QIcon(":/save_as"), "save as BMP", this, SLOT(slotSaveBMP()));

    menuView.setTitle("View");
    menuView.addAction(QIcon(":/reset"), "Reset zoom and position(Esp)", this, SLOT(slotResetZoomAndPosition()));

    this->menuBar()->addMenu(&menuFile);
    this->menuBar()->addMenu(&menuView);
}

double CentralWidget::findMaxX(QXYSeries *series)
{
    double maxX = 0;

    foreach (QPointF point, series->points())
        if(point.x() > maxX) maxX = point.x();

    return maxX;
}

double CentralWidget::findMinX(QXYSeries *series)
{
    double minX = std::numeric_limits<double>::max();

    foreach (QPointF point, series->points())
        if(point.x() < minX) minX = point.x();

    return minX;
}

double CentralWidget::findMaxY(QXYSeries *series)
{
    double maxY = 0;

    foreach (QPointF point, series->points())
        if(point.y() > maxY) maxY = point.y();

    return maxY;
}

double CentralWidget::findMinY(QXYSeries *series)
{
    double minY = std::numeric_limits<double>::max();

    foreach (QPointF point, series->points())
        if(point.y() < minY) minY = point.y();

    return minY;
}

void CentralWidget::createAxes()
{
//    disconnect(pAxisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(slotRangeChanged(qreal, qreal)));

    chart.createDefaultAxes();

    pAxisX = qobject_cast<QValueAxis*>(chart.axisX());
    pAxisY = qobject_cast<QValueAxis*>(chart.axisY());

    connect(pAxisX, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(slotRangeXChanged(qreal, qreal)));
    connect(pAxisY, SIGNAL(rangeChanged(qreal, qreal)), this, SLOT(slotRangeYChanged(qreal, qreal)));
}

void CentralWidget::slotSetTheme(int theme)
{
    chartView.chart()->setTheme(QChart::ChartTheme(theme));
    lineSeriesX.setPen(QPen(QColor(Qt::black)));
    lineSeriesY.setPen(QPen(QColor(Qt::black)));
    chart.setDropShadowEnabled(true);
}

void CentralWidget::slotSetLegentPosition(int position)
{
    switch (position) {
    case 0:
        chart.legend()->setAlignment(Qt::AlignTop);
        break;
    case 1:
        chart.legend()->setAlignment(Qt::AlignRight);
        break;
    case 2:
        chart.legend()->setAlignment(Qt::AlignBottom);
        break;
    case 3:
        chart.legend()->setAlignment(Qt::AlignLeft);
        break;
    default:
        break;
    }
}

void CentralWidget::slotSetTitle(QString title)
{
    chart.setTitle(title);
}

void CentralWidget::slotAnimation(bool value)
{
    if(value)
        chart.setAnimationOptions(QChart::AllAnimations);
    else
        chart.setAnimationOptions(QChart::NoAnimation);
}

void CentralWidget::slotAntialiasing(bool value)
{
    if(value)
        chartView.setRenderHint(QPainter::HighQualityAntialiasing);
    else
        chartView.setRenderHint(QPainter::HighQualityAntialiasing, false);
}

void CentralWidget::slotSetTcickCountY(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(chart.axisY());
    axis->setTickCount(value);
}

void CentralWidget::slotSaveBMP()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save as BMP"),
                                                    QDir::rootPath() + chart.title(),
                                                    "Images (*.bmp);;All files (*.*)");
    if(fileName != "")
    {
        QPixmap pix = chartView.grab();
        pix.toImage().save(fileName, "BMP");
    }
}

void CentralWidget::slotReRange()
{
    maxX = 0;
    minX = std::numeric_limits<double>::max();
    maxY = 0;
    minY = std::numeric_limits<double>::max();

    for (int i = 2; i < chart.series().count(); ++i)
    {
        QXYSeries* series = static_cast<QXYSeries*>(chart.series().at(i));
        if(maxX < findMaxX(series)) maxX = findMaxX(series);
        if(minX > findMinX(series)) minX = findMinX(series);
        if(maxY < findMaxY(series)) maxY = findMaxY(series);
        if(minY > findMinY(series)) minY = findMinY(series);
    }
//Что-бы правильно работало нажатие Esc >>
    chartView.rangeX.min = minX;
    chartView.rangeX.max = maxX;

    chartView.rangeY.min = minY;
    chartView.rangeY.max = maxY;
//Что-бы правильно работало нажатие Esc <<

    QString axisX_Title = chart.axisX()->titleText();
    QString axisY_Title = chart.axisY()->titleText();

    createAxes();
    chart.axisX()->setRange(minX, maxX);
    chart.axisX()->setTitleText(axisX_Title);
    chart.axisY()->setRange(minY, maxY);
    chart.axisY()->setTitleText(axisY_Title);
}

void CentralWidget::slotResetZoomAndPosition()
{
    chartView.slotResetZoomAndPosition();
}

void CentralWidget::slotViewXYCoordinate(QPointF point)
{
    statusBarWidget.setText("X: " + QString::number(point.x()) +
                            "   Y: " + QString::number(point.y()));
}

void CentralWidget::slotSetRubberMode(QChartView::RubberBand mode)
{
    chartView.setRubberBand(mode);
}

void CentralWidget::slotRangeXChanged(qreal min, qreal max)
{
    panelWidget.setRangeAxisX(min, max);
}

void CentralWidget::slotRangeYChanged(qreal min, qreal max)
{
    panelWidget.setRangeAxisY(min, max);
}

void CentralWidget::slotRangeXSet(qreal min, qreal max)
{
    pAxisX->setRange(min, max);
}

void CentralWidget::slotRangeYSet(qreal min, qreal max)
{
    pAxisY->setRange(min, max);
}

void CentralWidget::slotSeriesTypeChanged()
{
    QString axisX_Title = chart.axisX()->titleText();
    QString axisY_Title = chart.axisY()->titleText();

    createAxes();

    chart.axisX()->setTitleText(axisX_Title);
    chart.axisY()->setTitleText(axisY_Title);
}

void CentralWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit signalCloseWindow(this);
}

void CentralWidget::slotSetTcickCountX(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(chart.axisX());
    axis->setTickCount(value);
}
