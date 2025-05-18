#ifndef STATISTICS_H
#define STATISTICS_H

#include <QDialog>
#include <QWidget>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QChartView>
#include <QLineSeries>
#include <QDateTime>
#include "airport_database.h"

namespace Ui {
class Statistics;
}

class Statistics : public QDialog
{
    Q_OBJECT

public:
    explicit Statistics(const QString &_airportCode, const QString &_date, QWidget *parent = nullptr);
    ~Statistics();
    void updateMonthChart(int i);


private:
    void tabSetup();
    void loadData();
    void createMonthChart(const QList<QPair<QString, int>> &data);
    void createYearChart(const QList<QPair<QString, int>> &data);

    Ui::Statistics *ui;

    const QString airportCode;
    const QString date;
    airportDataBase* dataBase;

    QChartView* yearChartView;
    QChartView* monthChartView;
};

#endif // STATISTICS_H
