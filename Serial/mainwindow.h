#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include "graphics.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ConnectBtn_clicked();
    void readSerial();

    void on_dataGet_clicked();

    void on_dataSend_clicked();

    void on_updateBtn_clicked();

    void on_SaveFileBtn_clicked();

    void on_createGraph_clicked();

signals:
    void arduinoReady(bool b);

private:
    void searchPorts();
    void setBaudRatesList();
    void serialBegin();
    void getItems(QString s);
    void updateTable(int itemIndex, QString val);
    //----------------------
    Ui::MainWindow *ui;
    graphics *nw;
    QSerialPort *serial;
    //----------------------
    QString serialName;
    bool serialInitialized = false;
    bool itemsRegistered = false;
    int dataItems = 0;
    int graphWindowOpened = 0;
    //----------------------
    bool flagGetall = false;
    int itemNumberGet = -1;



};
#endif // MAINWINDOW_H
