#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "guncon_usb.h"
#include "calibration.h"
#include "events.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void update_data(QByteArray &data);
    void on_btn_connect_clicked();

    void on_btn_calibration_clicked();

    void on_btn_run_clicked();

private:
    Ui::MainWindow *ui;
    guncon_usb *gcon3;
    QTimer *timer;
    bool calibracion;
    double KX1, KX2, KX3, KY1, KY2, KY3;        // coefficients for calibration algorithm
    Calibration cal;
    int pixmap_points[9][2];
    int ref_points[9][2];
    quint8 num_samples;
    bool trigger_act;
    bool uidev_opened;
    Events events;
};

#endif // MAINWINDOW_H
