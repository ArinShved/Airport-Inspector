#ifndef AIRPORT_DATABASE_H
#define AIRPORT_DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#define POSTGRE_DRIVER "QPSQL"
//#define DB_NAME "demo"

class airportDataBase : public QObject
{
    Q_OBJECT
public:
    explicit airportDataBase(QObject *parent = nullptr);
    ~airportDataBase();

    void RequestToDB(QString request);
    void AddDataBase(QString driver);
    void ConnectToDataBase();
    QList<QPair<QString, QString>> fetchAirports();
    QList<QMap<QString, QString>> fetchDepartures(const QString &airportCode, const QString &date);
    QList<QMap<QString, QString>> fetchArrivals(const QString &airportCode, const QString &date);
    QList<QPair<QString, int>> fetchYearlyStatistics(const QString &airportCode);
    QList<QPair<QString, int>> fetchMonthlyStatistics(const QString &airportCode, const QString &month);
    QString returnName(const QString &airportCode);

signals:
    void sig_SendStatusConnection(bool status);
    void sig_SendStatusRequest(QSqlError);


private:
    QSqlDatabase* dataBase;

    QString hostName = "981757-ca08998.tmweb.ru";
    QString dbName = "demo";
    QString login = "netology_usr_cpp";
    QString pass = "CppNeto3";
    int port = 5432;

    QSqlQuery* simpleQuery;


    QString request;

};

#endif // AIRPORT_DATABASE_H
