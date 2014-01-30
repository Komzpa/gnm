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

     // Currently opened network:
     OGRSFDriver *poDriver;
     OGRDataSource *poDataSource;
     bool wasDSClosed;

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
     void on_pushButton_6_clicked();
     void on_pushButton_8_clicked();
     void on_pushButton_7_clicked();
};

#endif // WIDGET_H
