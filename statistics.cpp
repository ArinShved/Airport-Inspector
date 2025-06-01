#include "statistics.h"
#include "qboxlayout.h"
#include "ui_form.h"

Statistics::Statistics(const QString &_airportCode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Statistics),
    airportCode(_airportCode)
{
    ui->setupUi(this);
    connect(ui->pb_close, &QPushButton::clicked, [this]() {
        this->close();
    });

    tabSetup();

    dataBase = new airportDataBase(this);
    dataBase->ConnectToDataBase();
    loadData();

    ui->l_airportName->setText(dataBase->returnName(_airportCode));

}

Statistics::~Statistics()
{


    delete yearChartView;
    delete monthChartView;

    delete dataBase;
    delete ui;
}

void Statistics::tabSetup()
{
    ui->pb_close->setText("Закрыть");

    QStringList months = {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                             "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
    ui->cb_months->addItems(months);

    yearChartView = new QChartView(ui->wid_yearlyStatistic);
    monthChartView = new QChartView(ui->wid_monthlyStatistic);

    QVBoxLayout *yearLayout = new QVBoxLayout(ui->wid_yearlyStatistic);
    QVBoxLayout *monthLayout = new QVBoxLayout(ui->wid_monthlyStatistic);

    yearLayout->addWidget(yearChartView);
    monthLayout->addWidget(monthChartView);

    yearChartView->setRenderHint(QPainter::Antialiasing);
    monthChartView->setRenderHint(QPainter::Antialiasing);

}


void Statistics::loadData()
{
    QList<QPair<QString, int>> yearlyData = dataBase->fetchYearlyStatistics(airportCode);
    QList<QPair<QString, int>> monthlyData = dataBase->fetchMonthlyStatistics(airportCode, "2017.01.01");

    createYearChart(yearlyData);
    createMonthChart(monthlyData);
    connect(ui->cb_months, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Statistics::updateMonthChart);
}


void Statistics::updateMonthChart(int i)
{
    QString selectedMonth = ui->cb_months->itemText(i);
    QString monthDate;

    if(i <8)
    {
       monthDate = "2017." + QString::number(i + 1) + ".01";
    }
    else
    {
       monthDate = "2016." + QString::number(i + 1) + ".01";
    }

    QList<QPair<QString, int>> monthlyData = dataBase->fetchMonthlyStatistics(airportCode, monthDate);
    createMonthChart(monthlyData);
}


void Statistics::createYearChart(const QList<QPair<QString, int>> &data)
{
    QChart *oldChart = yearChartView->chart();


    QChart *yearlyChart = new QChart();

    yearlyChart->setTitle("Загруженность аэропорта за год");

    QBarSeries *series = new QBarSeries(yearlyChart);
    QBarSet *set = new QBarSet("Количество рейсов", series);

    QStringList type;

    for (const auto &i : data)
    {
        type << i.first.mid(5,2);
        *set << i.second;

    }
    series->append(set);


    QStringList months = {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь",
                             "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};

    QStringList categories;

    for (const auto &i : type)
    {
        categories << months[i.toInt()-1];
    }

    yearlyChart->addSeries(series);
    yearlyChart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis(yearlyChart);
    axisX->append(categories);
    yearlyChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis(yearlyChart);
    yearlyChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);



    yearChartView->setChart(yearlyChart);

    if(oldChart)
    {
        deleteChart(oldChart);
    }


}

void Statistics::createMonthChart(const QList<QPair<QString, int>> &data)
{
    QChart *oldChart = monthChartView->chart();

    QChart *monthChart = new QChart();


    monthChart->setTitle("Загруженность аэропорта за месяц");

    QLineSeries *series = new QLineSeries(monthChart);
    series->setName("Количество рейсов");

    QStringList type;
    int max = 0;

    for (const auto &i: data) {
        QDateTime dateTime = QDateTime::fromString(i.first, Qt::ISODate);

        if (dateTime.isValid()) {
            int day = dateTime.date().day();
            series->append(day, i.second);
            max = qMax(max, i.second);
        }
    }

    monthChart->addSeries(series);
    monthChart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis *axisX = new QValueAxis(monthChart);
    axisX->setRange(1, 31);
    axisX->setTitleText("День месяца");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(11);
    monthChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis(monthChart);
    axisY->setTitleText("Количество рейсов");
    axisY->setRange(0, max * 1.1);
    monthChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);


    monthChartView->setChart(monthChart);

    if(oldChart)
    {
        deleteChart(oldChart);
    }

}

void Statistics::deleteChart(QChart* chart) {
    if (!chart) return;

    for (auto* series : chart->series())
    {
        chart->removeSeries(series);
        delete series;
    }

    for (auto* axis : chart->axes()) {
        chart->removeAxis(axis);
        delete axis;
    }

    delete chart;
}
