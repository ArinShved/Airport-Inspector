#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "statistics.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataBase = new airportDataBase(this);
    dataBase->AddDataBase(POSTGRE_DRIVER);
    connect(dataBase, &airportDataBase::sig_SendStatusConnection, this, &MainWindow::ConnectionToDB);

    setupUI_1();
    dataBase->ConnectToDataBase();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dataBase;
}

void MainWindow::setupUI_1()
{
    ui->rb_arrival->setChecked(true);

    ui->de_flightDate->setDateRange(QDate(2016, 8, 15), QDate(2017, 9, 14));
    ui->de_flightDate->setDate(QDate(2016, 8, 15));

    ui->pb_flyList->setEnabled(false);
    ui->pb_flyList->setText("Показать список рейсов");

    ui->pb_showStatistics->setEnabled(false);
}

void MainWindow::showAirports()
{
    ui->cb_airports->clear();
    QList<QPair<QString, QString>> airports = dataBase->fetchAirports();
    if (airports.isEmpty())
    {
        QMessageBox::warning(this, "Аэропорты", "Отсутствует список рейсов");
    }
    for (const auto &airport : airports)
    {
        ui->cb_airports ->addItem(airport.second, airport.first);
    }
}

void MainWindow::ConnectionToDB(bool status)
{
    if(status)
    {
        ui->l_status->setText("Подключено");
        ui->l_status->setStyleSheet("color:green");
        ui->pb_flyList->setEnabled(true);
        ui->pb_showStatistics->setEnabled(true);
        showAirports();

    }
    else
    {
        ui->l_status->setText("Отключено");
        ui->l_status->setStyleSheet("color:red");
        ui->pb_flyList->setEnabled(false);
        ui->pb_showStatistics->setEnabled(false);

        QMessageBox::critical(this, "Ошибка подключения", "Не удалось подключиться к базе данных.");

        QTimer::singleShot(5000, this, [this]()
        {
            dataBase->ConnectToDataBase();
        });
    }
}

void MainWindow::showFlights()
{
    QString airportCode = ui->cb_airports->currentData().toString();
    QString date = ui->de_flightDate->date().toString();
    bool flightType = ui->rb_arrival->isChecked();

    ui->tw_main->clearContents();
    ui->tw_main->setRowCount(0);
    ui->tw_main->setColumnCount(3);

    QList<QMap<QString, QString>> flights;
    if (flightType)
    {
        flights = dataBase->fetchArrivals(airportCode, date);
        ui->tw_main->setHorizontalHeaderLabels({"Номер рейса", "Время вылета", "Аэропорт прибытия"});
    }
    else
    {
        flights = dataBase->fetchDepartures(airportCode, date);
        ui->tw_main->setHorizontalHeaderLabels({"Номер рейса", "Время вылета", "Аэропорт вылета"});
    }

    ui->tw_main->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tw_main->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    int row = 0;
    for (const auto &flight : flights)
    {
        ui->tw_main->insertRow(row);
        ui->tw_main->setItem(row, 0, new QTableWidgetItem(flight["flight_no"]));
        QString type = flightType ? flight["scheduled_arrival"] : flight["scheduled_departure"];
        ui->tw_main->setItem(row, 1, new QTableWidgetItem(type.mid(11, 5)));
        ui->tw_main->setItem(row, 2, new QTableWidgetItem(flight["airport_name"]));
        row++;
    }
}

void MainWindow::on_pb_flyList_clicked()
{
    showFlights();
}


void MainWindow::on_pb_showStatistics_clicked()
{
    QString airportCode = ui->cb_airports->currentData().toString();
    QString date = ui->de_flightDate->date().toString();
    Statistics *newWindow = new Statistics(airportCode, date, this);
    newWindow->setModal(true);
    newWindow->exec();
}

