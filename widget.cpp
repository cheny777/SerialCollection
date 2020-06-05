#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QString>

#include "smoothing.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->resize(1200,500);
    //查找可用的串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox->addItem(serial.portName());
            serial.close();
        }
    }
    /*设置波特率*/
    QStringList strbaud;
    strbaud<<"9600"<<"19200"<<"38400"<<"57600"<<"115200";
    for(int i =0;i<strbaud.size();i++)
    {
        ui->comboBox_2->addItem(strbaud[i]);
    }
    ui->comboBox_2->setCurrentIndex(4);

    ui->comboBox_3->addItem(QString::number(8));

    ui->comboBox_4->addItem(QString::number(0));

    ui->comboBox_5->addItem(QString::number(0));
    ui->comboBox_5->addItem(QString::number(1));
    ui->comboBox_5->addItem(QString::number(2));

    for(int i =0 ;i<6;i++)
    {
        ui->comboBox_6->addItem(QString(65+i));
    }
    /*QSerialPortInfo：提供系统中存在的串口的信息
    接下来需要创建一个QSerialPort的对象，对串口的名称、
    波特率、数据位、校验位、停止位等参数进行设置，然后才进行串口读写操作。
    */
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(slotstopSerial()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(slotopenSerial()));

    ui->textEdit->setReadOnly(true);

    mdrawLine = new DrawLine(ui->widget);
    mdrawLine->setMinimumHeight(300);
    QHBoxLayout *hoxlayout = new QHBoxLayout(ui->widget);
    hoxlayout->addWidget(mdrawLine);
    hoxlayout->setSpacing(0);
    hoxlayout->setMargin(0);

    QStringList FilterList;
    FilterList<<"低通"<<"均值"<<"低通加均值"<<"不滤波";
    for(int i = 0 ;i<FilterList.size();i++)
    {
        ui->comboBox_7->addItem(FilterList[i]);
    }

    /*设置滤波参数*/
    int FilterPoints = 3;
    int FilterRange = 10;
    m_nfilter = pulse_filter_create(FilterPoints, FilterRange, 1);


    /*巴特沃斯滤波*/
    lowFilter = new math::LowPassFilter2p(0.01,1);
}

Widget::~Widget()
{
    delete ui;
}

QList<double> Widget::StrSplit(QString str)
{
    QString nums;
    QList<double> doublelist;
    for(int i =0;i<str.size();i++)
    {

        if(str[i] == 'A'||str[i] == 'B'||str[i] == 'C'||str[i] == 'D'||
                str[i] == 'E'||str[i] == 'F'||str[i] == 'G'||
                str[i] == 'H'||str[i] == 'I')
        {
            bool bbl;
            double nn = nums.toDouble(&bbl);
            if(bbl)
            {
                doublelist.push_back(nn);
            }
            nums.clear();
        }
        else
        {
            nums+=str[i];
        }
    }
    return doublelist;
}

void Widget::slotSerialMessgae()
{

}

void Widget::slotopenSerial()
{
    ui->textEdit->clear();

    serial = new QSerialPort();

    //设置串口名
    serial->setPortName(ui->comboBox->currentText());
    //打开串口
    serial->open(QIODevice::ReadWrite);
    //设置波特率
    serial->setBaudRate(ui->comboBox_2->currentText().toInt());
    //设置数据位数
    switch(ui->comboBox_3->currentText().toInt())
    {
    case 8: serial->setDataBits(QSerialPort::Data8); break;
    default: break;
    }
    //设置奇偶校验
    switch(ui->comboBox_4->currentText().toInt())
    {
    case 0: serial->setParity(QSerialPort::NoParity); break;
    default: break;
    }
    //设置停止位
    switch(ui->comboBox_5->currentIndex())
    {
    case 1: serial->setStopBits(QSerialPort::OneStop); break;
    case 2: serial->setStopBits(QSerialPort::TwoStop); break;
    default: break;
    }
    //设置流控制
    serial->setFlowControl(QSerialPort::NoFlowControl);
    connect(serial,SIGNAL(readyRead()),SLOT(slotreadSerial()));
}

void Widget::slotreadSerial()
{
    QByteArray buf;
    buf = serial->readAll();
    if(!buf.isEmpty())
    {
        if(ui->lineEdit->text().isEmpty())
        {
            ui->textEdit->append(tr(buf));
        }
        else
        {
            strTemporary+=buf;
            if(strTemporary.endsWith(ui->lineEdit->text()))
            {
                ui->textEdit->append(strTemporary);
                QList<double> blist = StrSplit(strTemporary);
                /*中值滤波*/
                //mdrawLine->setData(Ffilter(blist[ui->comboBox_6->currentIndex()]));
                /*不滤波*/
                //mdrawLine->setData(blist[ui->comboBox_6->currentIndex()]);

                /*不明滤波*/
                //int output = 0;
                //pulse_filter_write(m_nfilter, LowPassFilter_Average(blist[ui->comboBox_6->currentIndex()]));
                //output = pulse_filter_read(m_nfilter);
                //mdrawLine->setData(output);

                /*均值滤波*/
                //mdrawLine->setData(LowPassFilter_Average(blist[ui->comboBox_6->currentIndex()]));

                /*巴特沃斯*/
                //mdrawLine->setData(lowFilter->apply(blist[ui->comboBox_6->currentIndex()]));
                /*一阶低通滤波*/
                //mdrawLine->setData(Filter1jie(blist[ui->comboBox_6->currentIndex()]));
                /*低通滤波2*/
                //mdrawLine->setData(FilterDitong2(blist[ui->comboBox_6->currentIndex()]));

                /*低通+均值*/
                //mdrawLine->setData(LowPassFilter_Average(FilterDitong2(blist[ui->comboBox_6->currentIndex()])));

                //mdrawLine->setData(FilterDitong2(LowPassFilter_Average(blist[ui->comboBox_6->currentIndex()])));

                switch (ui->comboBox_7->currentIndex()) {
                case 0:
                    mdrawLine->setData(FilterDitong2(blist[ui->comboBox_6->currentIndex()]));
                    break;
                case 1:
                    mdrawLine->setData(LowPassFilter_Average(blist[ui->comboBox_6->currentIndex()]));
                    break;
                case 2:
                    mdrawLine->setData(LowPassFilter_Average(FilterDitong2(blist[ui->comboBox_6->currentIndex()])));
                    break;
                case 3:
                    mdrawLine->setData(blist[ui->comboBox_6->currentIndex()]);
                    break;
                default:
                    break;
                }


                strTemporary.clear();

            }
        }
    }
    buf.clear();
}

void Widget::slotstopSerial()
{
    //关闭串口
    serial->clear();
    serial->close();
    serial->deleteLater();
}
