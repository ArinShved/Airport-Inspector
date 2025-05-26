#include "airport_database.h"

airportDataBase::airportDataBase(QObject *parent)
    : QObject{parent}
{ 
    dataBase = new QSqlDatabase();
}

airportDataBase::~airportDataBase(){
    delete dataBase;
}

void airportDataBase::AddDataBase(QString driver)
{

    *dataBase = QSqlDatabase::addDatabase(driver);

}

void airportDataBase::ConnectToDataBase()
{

    dataBase->setHostName(hostName);
    dataBase->setDatabaseName(dbName);
    dataBase->setUserName(login);
    dataBase->setPassword(pass);
    dataBase->setPort(port);

    bool status;
    status = dataBase->open( );
    emit sig_SendStatusConnection(status);

}

void airportDataBase::RequestToDB(QString request)
{
    this->request = request;
    *simpleQuery = QSqlQuery(*dataBase);
    simpleQuery->exec(request);

    emit sig_SendStatusRequest(simpleQuery->lastError( ));
}

QList<QPair<QString, QString>> airportDataBase::fetchAirports()
{
    QList<QPair<QString, QString>> airports;
    QSqlQuery query("SELECT airport_name->>'ru' as \"airportName\", airport_code FROM bookings.airports_data");

    while (query.next())
    {
        QString name = query.value(0).toString();
        QString code = query.value(1).toString();

        airports.append(QPair<QString, QString>(code, name));
    }
    return airports;
}

QList<QMap<QString, QString>> airportDataBase::fetchArrivals(const QString &airportCode, const QString &date)
{
    QList<QMap<QString, QString>> flights;
    QSqlQuery query;
    query.prepare(" SELECT flight_no, scheduled_arrival, ad.airport_name->>'ru' as \"Name\" "
                     " FROM bookings.flights f "
                     " JOIN bookings.airports_data ad ON ad.airport_code = f.departure_airport "
                     " WHERE f.arrival_airport = :airportCode"
                     " AND DATE(f.scheduled_arrival) = :date "
                     " ORDER BY f.scheduled_arrival");

    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date);

    if (!query.exec())
    {
        qDebug() << "Query error: " << query.lastError().text();
        return flights;
    }

    while (query.next())
    {
        QMap<QString, QString> flight;
        flight["flight_no"] = query.value(0).toString();
        flight["scheduled_arrival"] = query.value(1).toString();
        flight["airport_name"] = query.value(2).toString();
        flights.append(flight);
    }
    return flights;
}

QList<QMap<QString, QString>> airportDataBase::fetchDepartures(const QString &airportCode, const QString &date)
{
    QList<QMap<QString, QString>> flights;
    QSqlQuery query;
    query.prepare(" SELECT flight_no, scheduled_departure, ad.airport_name->>'ru' as \"Name\" "
                  " FROM bookings.flights f "
                  " JOIN bookings.airports_data ad ON ad.airport_code = f.arrival_airport "
                  " WHERE f.departure_airport = :airportCode"
                  " AND DATE(f.scheduled_departure) = :date "
                  " ORDER BY f.scheduled_departure");

    query.bindValue(":airportCode", airportCode);
    query.bindValue(":date", date);


    if (!query.exec())
    {
        qDebug() << "Query error: " << query.lastError().text();
        return flights;
    }

    while (query.next())
    {
        QMap<QString, QString> flight;
        flight["flight_no"] = query.value(0).toString();
        flight["scheduled_departure"] = query.value(1).toString();
        flight["airport_name"] = query.value(2).toString();
        flights.append(flight);
    }
    return flights;
}

QList<QPair<QString, int>> airportDataBase::fetchYearlyStatistics(const QString &airportCode)
{
    QList<QPair<QString, int>> yearlyData;
    QSqlQuery query;
    query.prepare(" SELECT count(flight_no), date_trunc('month', scheduled_departure) as \"Month\" "
                  " FROM bookings.flights f "
                  " WHERE (scheduled_departure::date > date('2016-08-31') AND scheduled_departure::date <= date('2017-08-31')) "
                  " AND (departure_airport = :airportCode OR arrival_airport = :airportCode) "
                  " GROUP BY \"Month\"");

    query.bindValue(":airportCode", airportCode);

    if (!query.exec())
    {
        qDebug() << "Query error: " << query.lastError().text();
        return yearlyData;
    }

    while (query.next())
    {
        QString month = query.value(1).toString();
        int count = query.value(0).toInt();
        yearlyData.append(QPair<QString, int>(month, count));
    }

    return yearlyData;
}


QList<QPair<QString, int>> airportDataBase::fetchMonthlyStatistics(const QString &airportCode, const QString &month)
{
    QList<QPair<QString, int>> monthlyData;
    QSqlQuery query;
    query.prepare(" SELECT count(flight_no), date_trunc('day', scheduled_departure) as \"Day\" "
                  " FROM bookings.flights f "
                  " WHERE (scheduled_departure::date >= date('2016-08-31') AND scheduled_departure::date <= date('2017-08-31')) "
                  " AND (departure_airport = :airportCode OR arrival_airport = :airportCode) "
                  " AND date_trunc('month', scheduled_departure) = date_trunc('month', :month::date) "
                  " GROUP BY \"Day\"");

    query.bindValue(":airportCode", airportCode);
    query.bindValue(":month", month);



    if (!query.exec())
    {
        qDebug() << "Query error: " << query.lastError().text();
        return monthlyData;
    }

    while (query.next())
    {
        QString day = query.value(1).toString();
        int count = query.value(0).toInt();

        monthlyData.append(QPair<QString, int>(day, count));
    }

    return monthlyData;
}

QString airportDataBase::returnName(const QString &airportCode)
{
    QSqlQuery query;
    query.prepare("SELECT (airport_name->>'ru') as airportName FROM bookings.airports_data WHERE airport_code = :airportCode ");
    query.bindValue(":airportCode", airportCode);

    if (!query.exec())
    {
        qDebug() << "Query error: " << query.lastError().text();
        return "Аэропорт";
    }

    if(query.next())
    {
        return query.value("airportName").toString();
    }
    else
    {
        return "Аэропорт";
    }
}
