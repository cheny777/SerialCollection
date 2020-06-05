#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "drawline.h"
#include <QList>
#include "lowpassfilter2p.h"

/*串口采集程序*/
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

    QSerialPort *serial;

    QString strTemporary;

    DrawLine *mdrawLine;

    /*字符串拆分*/
    QList<double> StrSplit(QString str);

    /*滤波器钥匙*/
    int m_nfilter;

    /*巴特沃斯低通滤波器*/
    math::LowPassFilter2p *lowFilter;
private slots:
    void slotSerialMessgae();
    void slotopenSerial();
    void slotreadSerial();
    void slotstopSerial();



};

#endif // WIDGET_H
