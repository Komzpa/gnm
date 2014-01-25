#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>

#include "../../gnm/ogr_gnm.h"

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

    // The signal-slot pair to type the message to log.
    signals:
     void toLog(QString str);
    private slots:
     void onToLog(QString str);

     void on_pushButton_4_clicked();
     void on_pushButton_clicked();
     void on_pushButton_2_clicked();
     void on_pushButton_3_clicked();
     void on_pushButton_5_clicked();
};

#endif // WIDGET_H
