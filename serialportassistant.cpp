#include "serialportassistant.h"
#include "ui_serialportassistant.h"

SerialPortAssistant::SerialPortAssistant(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SerialPortAssistant)
{
    ui->setupUi(this);
    port = new QSerialPort(this);
    helpDialog = new HelpDialog(this);
    timer = new QTimer(this);
    ui->send->setEnabled(false);
    ui->sendFile->setEnabled(false);
    ui->delay->setValidator(new QIntValidator(0, 10000, this));
    ui->times->setValidator(new QIntValidator(0, 10000, this));
    initSerialPortSetting();
    connections();

#ifdef DEBUG
    test();
#endif
}

SerialPortAssistant::~SerialPortAssistant()
{
    delete ui;
}


/* Initialize the serial port setting UI. */
void SerialPortAssistant::initSerialPortSetting(void)
{
    /* Insert the available serial ports into QComboBox. Keep infolist and ui->serialPortNumber in same order.*/
    infolist = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo& info,infolist)
    {
        ui->serialPortNumber->addItem(info.portName());
    }
    if(ui->serialPortNumber->count() == 0)
    {
        ui->serialPortNumber->addItem(tr("NULL"));
        ui->serialPortSwitch->setEnabled(false);
    }

    /* Insert choices of baud rate into QComboBox. Keep this->baudRate and baudRate in same order. */
    this->baudRate << QSerialPort::Baud115200 << QSerialPort::Baud57600 << QSerialPort::Baud38400
                   << QSerialPort::Baud19200 << QSerialPort::Baud9600 << QSerialPort::Baud4800
                   << QSerialPort::Baud2400 << QSerialPort::Baud1200;
    QStringList baudRate;
    baudRate << tr("115200") << tr("57600") << tr("38400")
             << tr("19200") << tr("9600") << tr("4800")
             << tr("2400") << tr("1200");
    ui->baudRate->addItems(baudRate);

    /* Insert choices of data bits into QComboBox. Keep this->dataBits and dataBits in same order. */
    this->dataBits << QSerialPort::Data8 << QSerialPort::Data7
                   << QSerialPort::Data6 << QSerialPort::Data5;
    QStringList dataBits;
    dataBits << tr("8") << tr("7") << tr("6") << tr("5");
    ui->dataBits->addItems(dataBits);

    /* Insert choices of parity into QComboBox. Keep this->parity and parity in same order. */
    this->parity << QSerialPort::NoParity << QSerialPort::OddParity << QSerialPort::EvenParity
                 << QSerialPort::SpaceParity << QSerialPort::MarkParity;
    QStringList parity;
    parity << tr("无校验") << tr("奇校验") << tr("偶校验")
           << tr("空校验") << tr("标记校验");
    ui->parity->addItems(parity);

    /* Insert choices of stop bits into QComboBox. Keep this->stopBits and stopBits in same order. */
    this->stopBits << QSerialPort::OneStop << QSerialPort::OneAndHalfStop << QSerialPort::TwoStop;
    QStringList stopBits;
    stopBits << tr("1") << tr("1.5") << tr("2");
    ui->stopBits->addItems(stopBits);

}


/* Connect all signals and slots. */
void SerialPortAssistant::connections(void)
{
    /* Close */
    connect(ui->actionExit,&QAction::triggered,this,&SerialPortAssistant::close);

    /* Show about. */
    connect(ui->actionAbout,&QAction::triggered,[this]{QMessageBox::information(this,tr("关于"),
                                                       tr("Copyright (C) hubenchang0515@outlook.com\n"
                                                          "http://www.kurukurumi.com\n"
                                                          "https://github.com/hubenchang0515/SerialPortAssistant\n"));});

    /* Show about Qt. */
    connect(ui->actionQt,&QAction::triggered,[this]{QMessageBox::aboutQt(this);});

    /* Open or close serial port. */
    connect(ui->serialPortSwitch,&QPushButton::clicked,this,&SerialPortAssistant::switchSerialPort);

    /* Show help */
    connect(ui->actionHelp,&QAction::triggered,helpDialog,&QWidget::show);

    /* Open and save file. */
    connect(ui->actionOpenFile,&QAction::triggered,this,&SerialPortAssistant::openFile);
    connect(ui->actionSaveFile,&QAction::triggered,this,&SerialPortAssistant::saveAs);

    /* Clear */
    connect(ui->clear,&QPushButton::clicked,this,&SerialPortAssistant::clear);

    /* Receive */
    connect(port,&QSerialPort::readyRead,this,&SerialPortAssistant::receive);

    /* Start send. */
    connect(ui->send,&QPushButton::clicked,this,&SerialPortAssistant::send);

    /* Transmit file. */
    connect(ui->sendFile,&QPushButton::clicked,this,&SerialPortAssistant::transmitFile);

    /* Transmit data. */
    connect(timer,&QTimer::timeout,this,&SerialPortAssistant::transmit);

    /* Stop transmit loop */
    connect(timer,&QTimer::timeout,[&]{looptimes--;if(looptimes<=0) timer->stop();});
}

/* Insert text into ui->dataDisplay with color(default black). */
void SerialPortAssistant::insertDataDisplay(const QString& text, const QColor& color)
{
    QTextCharFormat fmt;
    fmt.setForeground(color);
    ui->dataDisplay->setCurrentCharFormat(fmt);
    ui->dataDisplay->insertPlainText(text);
    QScrollBar* scroll = ui->dataDisplay->verticalScrollBar();
    scroll->setSliderPosition(scroll->maximum());
}


/* Open or close the serial port. */
void SerialPortAssistant::switchSerialPort(void)
{
    if(isPortOpen)
    {
        /* If serial port is open,close it. */
        port->close();
        ui->serialPortSwitch->setText(tr("打开串口"));
        isPortOpen = false;

        /* Enable all QComboBox.*/
        ui->serialPortNumber->setEnabled(true);
        ui->baudRate->setEnabled(true);
        ui->dataBits->setEnabled(true);
        ui->parity->setEnabled(true);
        ui->stopBits->setEnabled(true);
    }
    else
    {
        /* If serial port is closed,open it. */

        /* List and QComboBox are in same order,so they can use the same index. */
        port->setPort(infolist[ui->serialPortNumber->currentIndex()]);
        port->setBaudRate(baudRate[ui->baudRate->currentIndex()]);
        port->setDataBits(dataBits[ui->dataBits->currentIndex()]);
        port->setParity(parity[ui->parity->currentIndex()]);
        port->setStopBits(stopBits[ui->stopBits->currentIndex()]);
        if(port->open(QSerialPort::ReadWrite))
        {
            ui->serialPortSwitch->setText(tr("关闭串口"));
            isPortOpen = true;
            /* Disable all QComboBox.*/
            ui->serialPortNumber->setEnabled(false);
            ui->baudRate->setEnabled(false);
            ui->dataBits->setEnabled(false);
            ui->parity->setEnabled(false);
            ui->stopBits->setEnabled(false);
        }
        else
        {
            QString message = "Connect to " + ui->serialPortNumber->currentText() + " failed : " + port->errorString();
            statusBar()->showMessage(message,5000);
        }
    }

    /* Enable or disable ui->send QPushButton. */
    ui->send->setEnabled(isPortOpen);
    ui->sendFile->setEnabled(isPortOpen);
}

/* Receive data from serial port. */
void SerialPortAssistant::receive(void)
{
    /* Receive data. */
    QByteArray data = port->readAll();
    QString display;

    /* Convert data to hexadecimal. */
    if(ui->hexadecimaleReceive->isChecked())
    {
        QString temp;
        for(int i=0; i < data.size(); i++)
        {
            temp.sprintf("%02x ",(unsigned char)data.at(i));
            display += temp;
        }
    }

    /* Add time to show. */
    if(ui->showTime->isChecked())
    {
        QDateTime time = QDateTime::currentDateTime();
        display = time.toString("yyyy-MM-dd hh:mm:ss") + " : " + display;
    }
    /* Add newline to show. */
    if(ui->autoNewLine->isChecked())
    {
        display += "\n";
    }

    /* Show the data received. */
    insertDataDisplay(display,ui->doubleColor->isChecked() ? Qt::blue : Qt::black);
}

/* Start send. */
void SerialPortAssistant::send(void)
{
    if(ui->loop->isChecked())
    {
        transmitCircularly();//send some times.
    }
    else
    {
        transmit();  //send once.
    }

}

/* Transmit data from serial port. */
void  SerialPortAssistant::transmit()
{
    if(ui->hexadecimalSend->isChecked())
    {
        transmitHexadecimal();
    }
    else
    {
        transmitString();
    }
}

/* Transmit String from serial port. */
void SerialPortAssistant::transmitString(void)
{
    /* Transmit data. */
    QString data = ui->dataToSend->toPlainText();
    if(port->write(data.toStdString().c_str()) == -1)
    {
        statusBar()->showMessage("Send data failed : "+ port->errorString());
        return;
    }

    /* Add time to show. */
    if(ui->showTime->isChecked())
    {
        QDateTime time = QDateTime::currentDateTime();
        data = time.toString("yyyy-MM-dd hh:mm:ss") + " : " + data;
    }
    /* Add newline to show. */
    if(ui->autoNewLine->isChecked())
    {
        data += "\n";
    }

    /* Show the date sended. */
    insertDataDisplay(data,ui->doubleColor->isChecked() ? Qt::green : Qt::black);
}

/* Transmit hexadecimal number from serial port. */
void SerialPortAssistant::transmitHexadecimal(void)
{
    /* Check if the data is splited by withspace every 2 characters. */
    QString data = ui->dataToSend->toPlainText();
    QRegExp regExp(" *([0-9A-Fa-f]{2} +)+[0-9A-Fa-f]{2} *");
    if(regExp.exactMatch(data))
    {
        /* Convert every 2 characters to hexadecimal. */
        QStringList dataList = data.split(QRegExp(" +"));
        QString newData,showData;
        foreach(const QString& i, dataList)
        {
            showData += i + " ";
            int n = i.toInt(0,16);
            newData += data.sprintf("%c",static_cast<char>(n));
        }

        /* Transmit data. */
        if(port->write(newData.toStdString().c_str()) == -1)
        {
            statusBar()->showMessage("Send data failed : "+ port->errorString());
            return;
        }

        /* Add time to show. */
        if(ui->showTime->isChecked())
        {
            QDateTime time = QDateTime::currentDateTime();
            showData = time.toString("yyyy-MM-dd hh:mm:ss") + " : " + showData;
        }
        /* Add newline to show. */
        if(ui->autoNewLine->isChecked())
        {
            showData += "\n";
        }
        /* Show data. */
        insertDataDisplay(showData,ui->doubleColor->isChecked() ? Qt::green : Qt::black);
    }
    else
    {
        statusBar()->showMessage("Data format is error",5000);
    }
}

/* Transmit data circularly by setting. */
void SerialPortAssistant::transmitCircularly(void)
{
    delayms = ui->delay->text().toInt();
    looptimes = ui->times->text().toInt();
    if(looptimes>0)
    {
        timer->start(delayms);
    }
}

/* Transmit file from serial port. */
void SerialPortAssistant::transmitFile(void)
{
    QString filename = QFileDialog::getOpenFileName(this,tr("打开"));
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        statusBar()->showMessage("Open file failed",5000);
    }
    else if(port->write(file.readAll()) == -1)
    {
        statusBar()->showMessage("Send data failed : "+ port->errorString(),5000);
    }
    else
    {
        QString message = "[Sended file <" + filename +">]\n";
        insertDataDisplay(message,Qt::red);
    }
}

/* Read file's content to ui->dataToSend. */
void SerialPortAssistant::openFile(void)
{
    QString filename = QFileDialog::getOpenFileName(this,tr("打开"));
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        statusBar()->showMessage("Open file failed",5000);
        return;
    }
    ui->dataToSend->setPlainText(file.readAll());
    file.close();
}


/* Save content of ui->dataDisplay into file. */
void SerialPortAssistant::saveAs(void)
{
    QString filename = QFileDialog::getSaveFileName(this,tr("保存"));
    QFile file(filename);
    if(!file.open(QFile::WriteOnly))
    {
        statusBar()->showMessage("Save file failed",5000);
        return;
    }
    QString text = ui->dataDisplay->toPlainText();
    file.write(text.toStdString().c_str());
    file.close();
}


/* Clear ui->dataDisplay */
void SerialPortAssistant::clear(void)
{
    ui->dataDisplay->clear();
}

#ifdef DEBUG
/* Just have a test while debug. */
void SerialPortAssistant::test(void)
{
    QString text;
    text += "Welcome to use this software.\n"
            "Copyright (C) hubenchang0515@outlook.com\n"
            "http://www.kurukurumi.com\n"
            "https://github.com/hubenchang0515/SerialPortAssistant\n";
    insertDataDisplay(text,Qt::gray);
}

#endif
