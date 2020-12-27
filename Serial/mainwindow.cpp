#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    searchPorts();
    setBaudRatesList();
    serialName = "";
    serial = new QSerialPort(this);

    //VENDOR Y PRODUCT ID CODE
    /*qDebug() << "available ports :" << QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << serialPortInfo.portName();
        if(serialPortInfo.hasVendorIdentifier()){
           qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
         }
        if(serialPortInfo.hasProductIdentifier()){
            qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
        }
    }*/

}

MainWindow::~MainWindow()
{
    if(serial->isOpen()) serial->close();
    delete ui;
}

void MainWindow::searchPorts()
{
    QList <QSerialPortInfo> list;
    list = QSerialPortInfo::availablePorts();
    for (int i = 0; i < list.size(); i++) {
        ui->ComList->addItem(list[i].portName());
    }
}

void MainWindow::setBaudRatesList()
{
    ui->BaudList->addItem(QString::number(9600));
    ui->BaudList->addItem(QString::number(115200));

}

void MainWindow::serialBegin()
{
    serialInitialized = true;
    qDebug() << "connected!";

}

void MainWindow::getItems(QString s)
{

    if(!dataItems){
        try {
            dataItems = s.split(" ")[0].toInt();
        } catch(int e) {
             QMessageBox::warning(this, "connection error", "no se recivi{o un número de datos");
        }
    }
    else {
        if(ui->varList->count() < dataItems){
            QString itemName = s.split(":")[0];
            ui->varList->addItem(itemName);
            ui->dataTable->setColumnCount(2);
            ui->dataTable->setRowCount(dataItems);
            updateTable(ui->varList->count()-1, "0");
        }
        else{
            itemsRegistered = true;
            ui->ComList->setEnabled(false);
            ui->BaudList->setEnabled(false);
            ui->ConnectBtn->setText("Desconectar");
            ui->dataBox->setEnabled(true);
            ui->dataDisplayBox->setEnabled(true);
            ui->GraphBox->setEnabled(true);
        }
    }

}

void MainWindow::updateTable(int itemIndex, QString val)
{
    if(ui->dataTable->item(itemIndex, 0)){
         ui->dataTable->item(itemIndex, 1)->setText(val);
         ui->dataTable->update();
    }
    else{
        QTableWidgetItem *itemName = new QTableWidgetItem(ui->varList->itemText(itemIndex));
        QTableWidgetItem *itemVal = new QTableWidgetItem(val);
        ui->dataTable->setItem(itemIndex, 0, itemName);
        ui->dataTable->setItem(itemIndex, 1, itemVal);
    }

}


void MainWindow::on_ConnectBtn_clicked()
{
    if(ui->ConnectBtn->text() == "Conectar"){
        if(!serial->isOpen()){
            serialName = ui->ComList->currentText();
            qint32 baud = QSerialPort::Baud9600;
            if(ui->BaudList->currentText() == "9600") baud = QSerialPort::Baud9600;
            else if (ui->BaudList->currentText() == "115200") baud = QSerialPort::Baud115200;


            qDebug()<<serialName;
            serial->setPortName(serialName);
            serial->open(QSerialPort::ReadWrite);
            serial->setBaudRate(baud);

            serial->setDataBits(QSerialPort::Data8);
            serial->setParity(QSerialPort::NoParity);
            serial->setStopBits(QSerialPort::OneStop);
            serial->setFlowControl(QSerialPort::NoFlowControl);
            QString a = "12345";
            serial->write(a.toStdString().c_str());
            connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerial);
        }else{
            //error
            QMessageBox::warning(this, "port error", "el puerto ya fue abierto");
        }
    }else{
        if(serial->isOpen()) serial->close();
        itemsRegistered = false;
        serialInitialized = false;
        ui->ComList->setEnabled(true);
        ui->BaudList->setEnabled(true);
        ui->ConnectBtn->setText("Conectar");
        ui->dataBox->setEnabled(false);
        ui->dataDisplayBox->setEnabled(false);
        ui->GraphBox->setEnabled(false);
    }


}

void MainWindow::readSerial()
{
    if(!serialInitialized || !itemsRegistered ||ui->ContGet->isChecked() || (itemNumberGet  != -1) || flagGetall || graphWindowOpened){
        QByteArray serialData = serial->readLine();
        QString dataString = QString::fromStdString(serialData.toStdString());
        dataString.remove(QChar('\r'));
        dataString.remove(QChar('\n'));

        if(!serialInitialized  &&  (dataString == "OK")) serialBegin();
        else if(!itemsRegistered) getItems(serialData);
        else if(itemNumberGet  != -1){
            if(dataString[0] == ui->varList->itemText(itemNumberGet)){
                qDebug() << dataString.split(":");
                updateTable(itemNumberGet, dataString.split(":")[1]);
                itemNumberGet = -1;
            }
        }
        else if(ui->ContGet->isChecked()){
            if(dataString[0] == ui->varList->currentText()){
                qDebug() << dataString.split(":");
                updateTable(ui->varList->currentIndex(), dataString.split(":")[1]);
                itemNumberGet = -1;
            }
        }
        else if(flagGetall){
            static int counter = -1;
            counter ++;
            if((counter >= 0) && (counter < dataItems+1)){
                QStringList data = dataString.split(":");
                qDebug() << data;
                int pos = ui->varList->findText(data[0]);
                updateTable(pos, data[1]);
            }
            else if(counter == dataItems+1){
                counter = 0;
                flagGetall = false;
            }
        }
        else if(graphWindowOpened){
            /*for(int i =0; i < graphWindowOpened; i ++){
                nw->addPoint(i, i);
            }
            graphWindowOpened = 0;*/
            static double x = -1, y = -1;
            if(y == -1){
                if(x == -1){
                    if(dataString[0] == 't')
                        x = dataString.split(":")[1].toDouble();
                }
                else{
                    if(dataString[0] == 's')
                        y = dataString.split(":")[1].toDouble();


                }
            }
            else{
                nw->addPoint(x, y);
                qDebug() << x << " de x   "<< y << " de y";
                x = -1;
                y = -1;
                graphWindowOpened --;
            }

        }
    }
    else serial->clear();


}

void MainWindow::on_dataGet_clicked()
{
    int index = ui->varList->currentIndex();
    itemNumberGet = index;
}

void MainWindow::on_dataSend_clicked()
{
    if(ui->dataSendInput->text() != ""){
        QString out = ui->varList->currentText() + ":" + ui->dataSendInput->text();
        serial->write(out.toStdString().c_str());
        flagGetall = true;
    }
}

void MainWindow::on_updateBtn_clicked()
{
    flagGetall = true;

}

void MainWindow::on_SaveFileBtn_clicked()
{
    QFile csv("../log_activity.csv");


    if(!csv.open(QFile::WriteOnly))
        return;

    QTextStream out(&csv);
    qDebug() << "guardando...";

    for(int i = 0; i < ui->dataTable->rowCount(); ++i){
        for (int j = 0; j < 2; j++)
            out<<ui->dataTable->item(i, j)->text() << ";";
        out << "\n";
    }
    csv.close();
}

void MainWindow::on_createGraph_clicked()
{
    nw = new graphics();
    nw->window();
    nw->show();
    graphWindowOpened = 80;
}
