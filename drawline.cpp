#include "drawline.h"
#include <QDebug>
#include <list>
#include <vector>

DrawLine::DrawLine(QWidget *parent)
    :QWidget(parent)
{
    datanum = 0;
    m_total = 0;
    mean = 0;

    m_max = 0;
    m_min = 0;
}

void DrawLine::setData(double num)
{
    if(datanum<MAXNUM)
    {
        datanum++;
        mquevedouble.enqueue(num);
        m_total+=num;

        mean =m_total/datanum;

    }
    else
    {
        m_total-=mquevedouble.front();
        m_total+=num;
        mquevedouble.dequeue();
        mquevedouble.enqueue(num);

        mean =m_total/datanum;

    }
    auto smallestmin = min_element(mquevedouble.begin(),mquevedouble.end());
    auto smallestmax = max_element(mquevedouble.begin(),mquevedouble.end());

    m_max = *smallestmax;
    m_min = *smallestmin;
    update();
}

void DrawLine::test()
{
    mquevedouble.enqueue(1);
    mquevedouble.enqueue(2);
    mquevedouble.enqueue(3);
    mquevedouble.enqueue(4);


    qDebug()<<mquevedouble.size();

    for(int i =0 ;i <mquevedouble.size();i++)
    {
        qDebug()<<mquevedouble[i];
    }

    mquevedouble.dequeue();
    mquevedouble.enqueue(5);
    for(int i =0 ;i <mquevedouble.size();i++)
    {
        qDebug()<<mquevedouble[i];
    }

    auto smallest = min_element(mquevedouble.begin(),mquevedouble.end());

    qDebug()<<*smallest;


}

int DrawLine::getcha()
{
    return m_max-m_min;
}

void DrawLine::paintEvent(QPaintEvent *e)
{
    double ky = mheight/(double)(m_max-m_min);
    //int ky = mheight/(double)(m_max);
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);

    for(int i=0;i<mquevedouble.size()-1;i++)
    {
        //由于y轴是倒着的，所以y轴坐标要pointy-a[i]*ky 其中ky为比例系数
        painter.setPen(pen);//黑色笔用于连线
        painter.drawLine(i,
                         mheight-(mquevedouble[i]-m_min)*ky,
                         i+1,
                         mheight-(mquevedouble[i+1]-m_min)*ky
                );

        if(mquevedouble[i] >= m_max-0.00001)
        {
            m_max2 = i;
        }
        if(mquevedouble[i] <= m_min+0.00001)
        {
            m_min2 = i;
        }
    }

    QPen penAve;

    penAve.setColor(Qt::red);//选择红色
    penAve.setWidth(2);
    penAve.setStyle(Qt::DotLine);//线条类型为虚线
    painter.setPen(penAve);

    painter.drawLine(0,mheight-(mean-m_min)*ky,mwidth,mheight-(mean-m_min)*ky);
    painter.drawText(mwidth-100,mheight-(mean-m_min)*ky,"平均值"+QString::number(mean));

    QPen penAvem;

    penAvem.setColor(Qt::blue);//选择红色
    penAvem.setWidth(2);
    penAvem.setStyle(Qt::DotLine);//线条类型为虚线
    painter.setPen(penAvem);

    painter.drawLine(0,mheight-(m_max-m_min)*ky,mwidth,mheight-(m_max-m_min)*ky);
    painter.drawLine(0,mheight,mwidth,mheight);
    painter.drawText(m_max2,20,"最大值"+QString::number(m_max));
    painter.drawText(m_min2,mheight-20,"最小值"+QString::number(m_min));


    QPen penMaxMin;
    penMaxMin.setColor(Qt::red);
    penMaxMin.setWidth(7);
    painter.setPen(penMaxMin);
    painter.drawPoint(m_max2,mheight-(m_max-m_min)*ky-2);//标记最大值点
    painter.drawPoint(m_min2,mheight-2);//标记最小值点
}

void DrawLine::resizeEvent(QResizeEvent *e)
{
    mwidth = this->width();
    mheight = this->height();
}
