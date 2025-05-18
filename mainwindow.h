#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "airport_database.h"
#include <QTimer>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void ConnectionToDB(bool status);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showAirports();
    void showFlights();


private slots:
    void on_pb_flyList_clicked();

    void on_pb_showStatistics_clicked();

private:
    void setupUI_1();

    Ui::MainWindow *ui;
    airportDataBase* dataBase;
};
#endif // MAINWINDOW_H
