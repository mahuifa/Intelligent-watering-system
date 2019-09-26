#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mqtt/qmqtt.h"
#include "qcustomplot.h"
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int data1 ;
    int data2;

private slots:
    void onMQTT_Received(const QMQTT::Message &message);
    void onMQTT_Connected();
    void onMQTT_Subscribed(const QString &topic);
    void onMQTT_Subscribe();

private:
    Ui::MainWindow *ui;
    QMQTT::Client *client;
    int randInt(int low, int high);
};

#endif // MAINWINDOW_H
