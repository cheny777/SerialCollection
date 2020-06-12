#ifndef DRAWLINE_H
#define DRAWLINE_H

#include <QWidget>
#include <QPainter>
#include <queue>
#include <QQueue>
#define MAXNUM 1000



using namespace std;
class DrawLine :public QWidget
{
    Q_OBJECT
public:
    DrawLine(QWidget *parent = NULL);
    void setData(double num);

    void test();
    int getcha();
protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    /*最大*/
    double m_max,m_max2;
    /*最小*/
    double m_min,m_min2;
    /*平均*/
    double mean;
    /*数据的个数*/
    int datanum;
    /*数据容器*/
    QQueue<double> mquevedouble;
    /*数据总和*/
    double m_total;

    /*宽高*/
    int mwidth;
    int mheight;

};

#endif // DRAWLINE_H
