#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>

#include "../../src/network_manager.h"

namespace Ui
{
    class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

    private:
     Ui::Widget *ui;
    
    public:
     explicit Widget(QWidget *parent = 0);
     ~Widget();

     GnmNetwork *network;

    //пара сигнал-слот для отправки строки в лог
    signals:
     void toLog(QString str);
    private slots:
     void onToLog(QString str);

     void on_pushButton_clicked();
     void on_pushButton_3_clicked();
     void on_pushButton_2_clicked();
};

#endif // WIDGET_H
