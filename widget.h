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


    /*标定容器*/
    QList<double> biaodingKList;
    QList<double> biaodingZList;

    bool m_bbiaoding;
    int m_Btype;

    /*标定零位*/
    double m_Zero;
    double m_biaoK;
    /*采集点数*/
    int collectionNUM;

private slots:
    void slotSerialMessgae();
    void slotopenSerial();
    void slotreadSerial();
    void slotstopSerial();



    void on_collectionbtn_clicked();
    void on_stopCollectionbtn_clicked();
    void on_biaodingBrn_clicked();
    void on_CollectionCombox_currentIndexChanged(int index);
};

#endif // WIDGET_H
