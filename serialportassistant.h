#ifndef SERIALPORTASSISTANT_H
#define SERIALPORTASSISTANT_H

#define DEBUG

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QTimer>
#include <QScrollBar>
#include "helpdialog.h"

#ifdef DEBUG
#include <QDebug>
#endif

namespace Ui {
class SerialPortAssistant;
}

class SerialPortAssistant : public QMainWindow
{
    Q_OBJECT

public:
    QList<QSerialPort::BaudRate> baudRate;
    QList<QSerialPort::DataBits> dataBits;
    QList<QSerialPort::Parity> parity;
    QList<QSerialPort::StopBits> stopBits;

    explicit SerialPortAssistant(QWidget *parent = 0);
    ~SerialPortAssistant();
    void insertDataDisplay(const QString& text,const QColor& color=Qt::black);

public slots:
    void switchSerialPort(void);
    void receive(void);
    void send(void);
    void transmit(void);
    void transmitString(void);
    void transmitHexadecimal(void);
    void transmitCircularly(void);
    void transmitFile(void);
    void openFile(void);
    void saveAs(void);
    void clear(void);

private:
    Ui::SerialPortAssistant *ui;
    HelpDialog* helpDialog;
    QSerialPort* port;
    QTimer* timer;
    QList<QSerialPortInfo> infolist;
    bool isPortOpen = false;
    int delayms;
    int looptimes;
    void initSerialPortSetting(void);
    void connections(void);


#ifdef DEBUG
    void test(void);
#endif
};

#endif // SERIALPORTASSISTANT_H
