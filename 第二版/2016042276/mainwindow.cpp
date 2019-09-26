#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>

#include <QVector>
#include <QTimer>

#include <QDateTime>
#include <QRandomGenerator>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new QMQTT::Client(QHostAddress("222.198.208.190"), 1883);
    client->connectToHost();



    connect(client,SIGNAL(connected()),this,SLOT(onMQTT_Connected()));
    connect(client,SIGNAL(connected()),this,SLOT(onMQTT_Subscribe()));
    connect(client, SIGNAL(subscribed(const QString &)), this, SLOT(onMQTT_Subscribed(QString)));
    connect(client,SIGNAL(received(const QMQTT::Message &)),this,SLOT(onMQTT_Received(QMQTT::Message)));


    this-> showMaximized();//最大化
    this-> setWindowTitle("马慧发：2016042276");  //窗口名称

    ui->widget->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(
    ui->widget, "光照&温度:2016042276", QFont("sans", 14, QFont::Bold));
    ui->widget->plotLayout()->addElement(0, 0, title);

    ui->widget->setOpenGl(true);


    QPen linePen;
    linePen.setWidth(3);
    linePen.setColor(QColor(255, 0, 0));  // 红线
    ui->widget->addGraph();
    ui->widget->graph(0)->setPen(linePen);
    ui->widget->graph(0)->setLineStyle(QCPGraph::LineStyle::lsLine);
    ui->widget->graph(0)->setName(QString(" LX"));

    ui->widget->addGraph();
    linePen.setColor(QColor(0, 0, 255));  // 蓝线
    ui->widget->graph(1)->setPen(linePen);
    ui->widget->graph(1)->setLineStyle(QCPGraph::LineStyle::lsLine);
    ui->widget->graph(1)->setName(QString(" ℃"));
    ui->widget->axisRect()->setupFullAxesBox();


    ui->widget->yAxis->setRange(0, 255);    //纵坐标轴数值范围
    ui->widget->yAxis2->setRange(0, 255);

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(10);            //setTickStep(double step);设置刻度间距  
    ui->widget->yAxis->setTicker(fixedTicker);
    ui->widget->yAxis2->setTicker(fixedTicker);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");  //设置横坐标时间轴
    timeTicker->setTickCount(10);
    ui->widget->xAxis->setTicker(timeTicker);

    ui->widget->yAxis->setLabel("温度");    //设置左坐标轴标签
    QFont axisFont = font();     //字体设置
    axisFont.setPointSize(16);
    axisFont.setBold(true);
    ui->widget->yAxis->setLabelFont(axisFont);
    ui->widget->yAxis2->setLabel("光照");   //设置右坐标轴标签
    ui->widget->yAxis->setLabelFont(axisFont);
    ui->widget->yAxis->setTickLabelFont(axisFont);
    ui->widget->yAxis2->setLabelFont(axisFont);
    ui->widget->yAxis2->setTickLabelFont(axisFont);
    ui->widget->xAxis->setTickLabelFont(axisFont);
    ui->widget->xAxis->setTickLabelRotation(-30);  //将标签旋转30度

    ui->widget->yAxis->setTicks(true);  //y轴显示刻度
    ui->widget->yAxis->setTickLabels(true);//y轴显示文本
    ui->widget->yAxis2->setTicks(true);
    ui->widget->yAxis2->setTickLabels(true);
    ui->widget->xAxis2->setTicks(false);

    ui->widget->xAxis->setTickLength(8, 0);  //设置刻度线
    ui->widget->xAxis->setSubTickLength(4, 0);
    ui->widget->yAxis->setTickLength(8, 0);
    ui->widget->yAxis->setSubTickLength(4, 0);
    ui->widget->yAxis2->setTickLength(8, 0);
    ui->widget->yAxis2->setSubTickLength(4, 0);

    QPen tickPen;
    tickPen.setWidth(2);   //设置坐标轴线条粗细
    ui->widget->xAxis->setBasePen(tickPen);
    ui->widget->xAxis2->setBasePen(tickPen);
    ui->widget->yAxis->setBasePen(tickPen);
    ui->widget->yAxis2->setBasePen(tickPen);
    ui->widget->xAxis->setTickPen(tickPen);
    ui->widget->xAxis->setSubTickPen(tickPen);
    ui->widget->yAxis->setTickPen(tickPen);
    ui->widget->yAxis->setSubTickPen(tickPen);
    ui->widget->yAxis2->setTickPen(tickPen);
    ui->widget->yAxis2->setSubTickPen(tickPen);

    QFont legendFont = font();
    legendFont.setPointSize(12);   //设定左上角图形标注的字体
    ui->widget->legend->setVisible(true);
    ui->widget->legend->setFont(legendFont);
    ui->widget->legend->setSelectedFont(legendFont);
    ui->widget->legend->setSelectableParts(QCPLegend::spItems);
    ui->widget->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignTop | Qt::AlignLeft);

    //使左轴和下轴将其范围转移到右轴和上轴：
    connect(ui->widget->xAxis, SIGNAL(rangeChanged(QCPRange)),
            ui->widget->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widget->yAxis, SIGNAL(rangeChanged(QCPRange)),
            ui->widget->yAxis2, SLOT(setRange(QCPRange)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onMQTT_Connected()
{
    qDebug()<<"连接成功！！！";
}

void MainWindow::onMQTT_Subscribe()
{
    QString topicSub = "data1";
    quint8 qosSub = 0;
    client->subscribe(topicSub, qosSub);  //订阅
}

void MainWindow::onMQTT_Subscribed(const QString &topic)
{
    qDebug()<<"已订阅";
}

void MainWindow::onMQTT_Received(const QMQTT::Message &message)   //显示
{

    QString str = message.payload();
    QStringList list = str.split(".");    //拆分字符串
    if (list[0] == "0x01"){
         qDebug()<<"光照:"<<list[1]<<list[2];  //debug输出信息
         data1 = list[2].toInt();
    }
    else {
        qDebug()<<"温度:"<<list[1]<<list[2];
        data2 = list[2].toInt();
    }


    QTime time = QTime::currentTime();
      int seconds = QTime(0, 0).secsTo(time);  //时间

      // 在顶部添加文本标签：
      static QCPItemText *textLabel = new QCPItemText(ui->widget);
      //    textLabel->setPositionAlignment(Qt::AlignLeft);
      textLabel->setTextAlignment(Qt::AlignLeft);
      // 将位置置于轴的中心/顶部
      textLabel->position->setCoords(seconds - 20, 50);
      textLabel->setText("L: " + QString::number(data1) + " LX" +
                         "\nT: " + QString::number(data2) + " ℃");
      textLabel->setFont(QFont(font().family(), 16));  // 使字体大一点
      textLabel->setPen(QPen(Qt::NoPen));  // 在文本周围显示黑色边框

      // 添加箭头：
      QPen arrowLinePen;
      arrowLinePen.setWidth(2);
      arrowLinePen.setColor(Qt::black);
      arrowLinePen.setStyle(Qt::DashDotLine);
      static QCPItemLine *arrowT = new QCPItemLine(ui->widget);
      arrowT->start->setCoords(seconds, data2);
      arrowT->end->setParentAnchor(textLabel->bottomRight);
      arrowT->setHead(QCPLineEnding::esSpikeArrow);
      arrowT->setPen(arrowLinePen);

      //添加箭头：
      static QCPItemLine *arrowH = new QCPItemLine(ui->widget);
      arrowH->start->setCoords(seconds, data1);
      arrowH->end->setParentAnchor(textLabel->topRight);
      arrowH->setHead(QCPLineEnding::esSpikeArrow);
      arrowH->setPen(arrowLinePen);

      //向行中添加数据：
      ui->widget->graph(0)->addData(seconds, data1);
      ui->widget->graph(1)->addData(seconds,data2);

      // 使键轴范围随数据滚动（范围大小恒定为8）：
      ui->widget->xAxis->setRange(seconds, 100, Qt::AlignRight);
      ui->widget->replot();
}

