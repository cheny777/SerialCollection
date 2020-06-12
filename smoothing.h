#ifndef SMOOTHING_H
#define SMOOTHING_H


//1、限副滤波
/*  A值可根据实际情况调整
value为有效值，new_value为当前采样值
滤波程序返回有效的实际值  */
#define A 10
double value;
double filter(double newd)
{
    double  new_value;
    new_value = newd;
    if ( ( new_value - value > A ) || ( value - new_value > A ))
        return value;
    return new_value;

}

//2、中位值滤波法
/*  N值可根据实际情况调整
排序采用冒泡法*/
#define N  5
double value_buf[N];
int jishu = 0;
double Ffilter(double newd)
{
    if(jishu<N)
    {
        value_buf[jishu] = newd;
        jishu++;
    }
    else
    {
        for(int i = 0;i<N-1;i++)
        {
            value_buf[i] = value_buf[i+1];
        }
        value_buf[N-1] = newd;
    }

    int i,j;
    double temp;
    for (j=0;j<=jishu;j++)
    {
        for (i=0;i<=jishu;i++)
        {
            if ( value_buf[i]>value_buf[i+1] )
            {
                temp = value_buf[i];
                value_buf[i] = value_buf[i+1];
                value_buf[i+1] = temp;
            }
        }
    }
    return value_buf[(jishu-1)/2];
}

/*均值滤波*/
#define AV 10
double Average_buf[AV];
int junzhi = 0;

double LowPassFilter_Average(double data)
{
    double add=0;
    double  result;

    if(junzhi<AV)
    {
        Average_buf[junzhi] = data;
        junzhi++;
    }
    else
    {
        for(int i =0 ;i<AV ;i++)
        {
            Average_buf[i] = Average_buf[i+1];
        }
        Average_buf[AV-1] = data;
    }

    for(int i=0;i<junzhi+1;i++)
    {
        add += Average_buf[i];
    }
    result=add/double(junzhi+1);
    return result;
}

//data[]放入一段时间里的数值，length：data数组的长度


/******************滤波**********************/

typedef struct
{
    // 过滤连续脉冲数
    int num_filter_pulse;
    // 周期
    int cycle;
    // 最大偏差值
    int max_delta;
    // 缓存节点数
    int num_node;
    // key数组.基于key进行滤波
    int *key;
    // 数据有效标志数组
    bool *valid;
} Filter;


int pulse_filter_create(int num_filter_pulse, int max_delta, int cycle);
/**
* @brief 写入数据
* @note 针对本项目进行定制.输入的数是一个周期数
* @param key: 键值
* @param cycle: 周期
*/
void pulse_filter_write(int filter_index, int key);
/**
* @brief 读取数据
*/
int pulse_filter_read(int filter_index);
/**
* @brief 是否包含数据
* @retval true:有.false:没有
*/
bool pulse_filter_is_contain_data(int filter_index);
/**
* @brief 打印过滤器
*/
void pulse_filter_print(int filter_index);



/*滤波函数*/

/**
* @brief 得到最近的有效数据下表
* @note 不考虑第一个节点.因为第一个节点需要被删除
* @param filter: 滤波器
* @return -1:无有效数据.其他:下标
*/
static int get_newest_valid_index(Filter *filter)
{
    for (int i = 0; i < filter->num_node; i++)
    {
        if (filter->valid[filter->num_node - 1 - i])
        {
            return filter->num_node - 1 - i;
        }
    }
    return -1;
}

static void pop_first(Filter *filter)
{
    int item_num = filter->num_filter_pulse + 2;
    for (int i = 0; i < item_num - 1; i++)
    {
        filter->key[i] = filter->key[i + 1];
        filter->valid[i] = filter->valid[i + 1];
    }
    filter->num_node--;
}
static void append(Filter *filter, int key, bool is_valid)
{
    int item_num = filter->num_filter_pulse + 2;
    if (filter->num_node < item_num)
    {
        filter->key[filter->num_node] = key;
        filter->valid[filter->num_node] = is_valid;
        filter->num_node++;
        return;
    }
}

static void set_all_node_valid(Filter *filter)
{
    int item_num = filter->num_filter_pulse + 2;
    for (int i = 0; i < item_num; i++)
    {
        filter->valid[i] = true;
    }
}
/**
* @brief 计算两个周期数的最小差值
* @note 比如两个数1, 9.都是在周期10以内循环的。所以最小差值应该为2而不是8
* @param x: 数1
* @param y: 数2
* @return 差值
*/
int calc_min_delta_in_cycle(int x, int y, int cycle)
{
    if (cycle == 0)
    {
        if (x > y)
        {
            return x - y;
        }
        else
        {
            return y - x;
        }
    }

    int a = (x - y + cycle) % cycle;
    int b = cycle - a;
    if (a > b)
    {
        return b;
    }
    else
    {
        return a;
    }
}
static bool key_is_valid(Filter *filter, int valid_index, int key)
{
    return (calc_min_delta_in_cycle(filter->key[valid_index], key, filter->cycle) <= filter->max_delta);
}



/*创建*/
/**
* @brief 创建脉冲滤波器
* @param num_filter_pulse: 过滤的脉冲数
* @param max_delta: 最大偏差值
* @param cycle: 键值周期.如果为0表示没有周期
* @return 过滤器索引
*/
int pulse_filter_create(int num_filter_pulse, int max_delta, int cycle)
{
    Filter *filter = (Filter *)malloc(sizeof(Filter));
    filter->num_filter_pulse = num_filter_pulse;
    filter->max_delta = max_delta;
    filter->cycle = cycle;

    int item_num = filter->num_filter_pulse + 2;
    filter->key = (int *)malloc(item_num * sizeof(int));
    filter->valid = (bool *)malloc(item_num * sizeof(bool));

    filter->num_node = 0;
    return (int)filter;
}
/**
* @brief 写入数据
* @note 针对本项目进行定制.输入的数是一个周期数
* @param key: 键值
* @param cycle: 周期
*/
void pulse_filter_write(int filter_index, int key)
{
    Filter *filter = (Filter *)filter_index;
    int item_num = filter->num_filter_pulse + 2;
    if (filter->num_node < item_num)
    {
        append(filter, key, true);
        return;
    }
    pop_first(filter);
    int index = get_newest_valid_index(filter);
    if (index == -1)
    {
        append(filter, key, true);
        set_all_node_valid(filter);
        return;
    }
    append(filter, key, key_is_valid(filter, index, key));
}




/**
* @brief 读取数据
*/
int pulse_filter_read(int filter_index)
{
    Filter *filter = (Filter *)filter_index;

    if (filter->num_node == 0)
    {
        return 0;
    }
    int index = get_newest_valid_index(filter);
    if (index == -1)
    {
        return 0;
    }
    return filter->key[index];
}

/**
* @brief 是否包含数据
* @retval true:有.false:没有
*/
bool pulse_filter_is_contain_data(int filter_index)
{
    Filter *filter = (Filter *)filter_index;
    return (filter->num_node > 0);
}

/**
* @brief 打印过滤器
*/
void pulse_filter_print(int filter_index)
{
    Filter *filter = (Filter *)filter_index;
    printf("--------------\n");
    for (int i = 0; i < filter->num_node; i++)
    {
        printf("key = %d valid = %d\n", filter->key[i], filter->valid[i]);
    }
}
/******************************************************************************/



// 一阶滞后滤波法
/*
取a=0-1，本次滤波结果=(1-a)本次采样值+a上次滤波结果。
B、优点：
对周期性干扰具有良好的抑制作用；
适用于波动频率较高的场合。
C、缺点：
相位滞后，灵敏度低；
滞后程度取决于a值大小；
不能消除滤波频率高于采样频率1/2的干扰信号。*/

#define FILTER_A 0.1
double Value;
double Filter1jie(double newdata)
{
    Value = (int)((float)newdata * FILTER_A + (1.0 - FILTER_A) * (float)Value);
    return Value;
}

/*低通滤波2*/
int num_x = 0;      //一个过渡值，对于同方向变化量大的数据，num_x越大
float k_x = 0.3;    //表示对新读入的数据的信任度，取值范围0-1
int old_flag = 0;   //表示第n-2个数据到第n-1个数据的变化趋势，加为1，减为0
int new_flag = 0;   //表示第n-1个数据到第n个数据的变化趋势，加为1，减为0
float old_data = 0;     //第n-1次的答案
float new_data = 0;     //第n次的输入值
#define Threshold_1 0.5    //临界点1，当数据变化值超过这个值时开始改变num_x
#define Threshold_2 1   //临界点2，当数据变化值超过这个值时开始改变k_x

double FilterDitong2(double new_data1)
{
        new_data = new_data1;
        if (new_data - old_data > 0 )       //计算方向
            new_flag = 1;
        else new_flag = 0;
        if (new_flag == old_flag)           //变化同向
        {
            if (abs (new_data - old_data) > Threshold_1)    //变化很大则num增大
                num_x += 5;
            if (num_x >= Threshold_2)       //变化持续同向且一直变化很大则k_x增大
                k_x += 0.1;
        }
        else                //变化反向
        {
            num_x = 0;
            k_x = 0.1;
            old_flag = new_flag;
        }
        if (k_x > 0.95)  k_x = 0.95;    //系数限幅
        new_data = (1-k_x) * old_data + k_x * new_data;   //计算答案
        old_data = new_data;        //更新old_data
        return old_data;
}




#endif // SMOOTHING_H
