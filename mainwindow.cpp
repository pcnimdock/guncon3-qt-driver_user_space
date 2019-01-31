#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSound>
#include <QScreen>
#include <QSize>
#include "calibration.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pixmap_mirilla->hide();
    ui->cb_guncon->hide();
    calibracion=0;
    uidev_opened=0;
    cal.reset();

    gcon3 = new guncon_usb();
    timer = new QTimer;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_data(QByteArray &data)
{
    static unsigned char apply_event;
    if(data.isEmpty())
    {
        timer->start();
        return;
    }
    if(data.size()<15)
    {return;}
    QByteArray dec_data = gcon3->decode(data);

 //   qDebug() << data.toHex().data();
 //   qDebug() << dec_data.toHex().data();
    if(dec_data.size()<13)
    {return;}
    //transform decode data to buttons
    QList <quint16> n_event;
    qint16 abs_x,abs_y,abs_z;
    quint8 abs_ry,abs_rx,abs_hat0y,abs_hat0x;
    bool btn_trigger,btn_0,btn_1,btn_2,btn_3,btn_4,btn_5,btn_6,btn_7,btn_8,btn_9;
    abs_ry=dec_data.at(0);//ABS_RY
    abs_rx=dec_data.at(1);//ABS_RX
    abs_hat0y=(dec_data.at(2)); //ABS_HAT0Y
    abs_hat0x=(dec_data.at(3)); //ABS_HAT0X
    abs_z=(dec_data.at(4)*256+dec_data.at(5)); //Z
    abs_y=(dec_data.at(6)*256+dec_data.at(7)); //ABS_Y
    abs_x=(dec_data.at(8)*256+dec_data.at(9)); //ABS_X
    btn_trigger=((dec_data.at(11) & 0x20)?1:0); //BTN_TRIGGER
    btn_0=((dec_data.at(12) & 0x04)?1:0);  //BTN 0
    btn_1=((dec_data.at(12) & 0x02)?1:0);  //1
    btn_2=((dec_data.at(11) & 0x04)?1:0);  //2
    btn_3=((dec_data.at(11) & 0x02)?1:0);  //3
    btn_4=((dec_data.at(11) & 0x80)?1:0);  //4
    btn_5=((dec_data.at(12) & 0x08)?1:0);  //5
    btn_6=((dec_data.at(10) & 0x80)?1:0);  //6
    btn_7=((dec_data.at(10) & 0x40)?1:0);  //7
    btn_8=((dec_data.at(11) & 0x10)?1:0);  // only one led reference
    btn_9=((dec_data.at(11) & 0x08)?1:0);  // out of range
    //comprobar si está en modo de calibración


    ui->lbl->setText(QString::number(abs_rx));
    ui->lbl_absx->setText(QString::number(abs_x));
    ui->lbl_absy->setText(QString::number(abs_y));
    //qDebug() << abs_rx << abs_ry << abs_hat0x << abs_hat0y;
    if(cal.isKsetted())
    {
       short int x,y;
        x=abs_x;
        y=abs_y;
        cal.Do_Calibration(&x,&y);
        ui->lbl_absx_2->setText(QString::number(x));
        ui->lbl_absy_2->setText(QString::number(y));
        if(uidev_opened&&((apply_event++)&0x01))
        {
            qDebug() << time_elapsed.elapsed();
            time_elapsed.start();
            //set values to virtual joystick
            events.set_axis(ABS_X,x); //calibrated absx
            events.set_axis(ABS_Y,y); //calibrated absy
            events.set_axis(ABS_RX,abs_rx);
            events.set_axis(ABS_RY,abs_ry);
            events.set_axis(ABS_HAT0X,abs_hat0x);
            events.set_axis(ABS_HAT0Y,abs_hat0y);

            events.set_button(BTN_TRIGGER,(int)btn_trigger);
            events.set_button(BTN_0,(int)btn_0);
            events.set_button(BTN_1,(int)btn_1);
            events.set_button(BTN_2,(int)btn_2);
            events.set_button(BTN_3,(int)btn_3);
            events.set_button(BTN_4,(int)btn_4);
            events.set_button(BTN_5,(int)btn_5);
            events.set_button(BTN_6,(int)btn_6);
            events.set_button(BTN_7,(int)btn_7);
            events.set_button(BTN_8,(int)btn_8);
            events.syn_report();
        }

    }
    if(calibracion)
    {
        //se ha activado el botón de calibracion
        //ver si se activa el trigger,obtener el valor y añadir
        //si ha sido activado, no volver a almacenar hasta que se vuelva a activar
        if(btn_trigger!=trigger_act)
        {
            //hay un evento nuevo
            if(btn_trigger)
            {
                //hay un punto nuevo
            QSound::play(":/audio/pum.wav");
            cal.append_sample_point(abs_x,abs_y);
            num_samples++;

            if(num_samples>8)
            {
                //se han obtenido todos los samples
                //salir de calibracion
                //salir de la pantalla completa y mostrar lo anterior
                ui->btn_calibration->show();
                ui->btn_connect->show();
                ui->btn_run->show();
                ui->cb_guncon->show();
                ui->lbl->show();
                ui->lbl_2->show();
                ui->lbl_3->show();
                ui->pixmap_mirilla->hide();
                this->showNormal();
                ui->btn_calibration->setText("Calibration");
                calibracion=0;
            }
            else
            {
                ui->pixmap_mirilla->move(pixmap_points[num_samples][0],pixmap_points[num_samples][1]);
            }
            }
            trigger_act=btn_trigger;
        }
    }

    timer->start();
}


void MainWindow::on_btn_connect_clicked()
{

    if(!(gcon3->isOpen()))
   {
        qDebug() << "Device isn't open";
        qDebug() << "Try to open";
        gcon3->openDevice();
        return;
    }

    connect(timer,SIGNAL(timeout()),gcon3,SLOT(new_read()));
    connect(gcon3,SIGNAL(new_readed_data(QByteArray&)),this,SLOT(update_data(QByteArray&)));
    timer->setInterval(5);
    timer->setSingleShot(true);
    QString value = "01126F3224601721";
    QByteArray array2 = QByteArray::fromHex(value.toLatin1());
    gcon3->write(&array2);
    timer->start();

}

void MainWindow::on_btn_calibration_clicked()
{
 QSound::play(":/audio/pum.wav");
 if(!calibracion)
    {

    //ocultar botones

    ui->btn_connect->hide();
    ui->btn_calibration->setText("Exit");
    ui->btn_run->hide();
    ui->cb_guncon->hide();
    //mostrar frame de mirilla
    ui->pixmap_mirilla->show();
    //obtener tamaño de la pantalla
    QList <QScreen *> pantalla = QApplication::screens();
    QSize *resolucion = new QSize();
    *resolucion = pantalla.at(0)->size();
    qint32 height_screen,width_screen;

    height_screen= resolucion->height();
    width_screen = resolucion->width();
    delete resolucion;
    int height_pix,width_pix;
    height_pix=height_screen/5;
    width_pix=height_pix;
    //el tamaño de la imagen de la mirilla será un quinto de la resolucion vertical
    ui->pixmap_mirilla->resize(width_pix,height_pix);

    //calcular los puntos
    ref_points[0][0]=height_pix/2;              ref_points[0][1]=height_pix/2;
    ref_points[1][0]=width_screen/2;            ref_points[1][1]=height_pix/2;
    ref_points[2][0]=width_screen-height_pix/2; ref_points[2][1]=height_pix/2;

    ref_points[3][0]=height_pix/2;              ref_points[3][1]=height_screen/2;
    ref_points[4][0]=width_screen/2;            ref_points[4][1]=height_screen/2;
    ref_points[5][0]=width_screen-height_pix/2; ref_points[5][1]=height_screen/2;

    ref_points[6][0]=height_pix/2;              ref_points[6][1]=height_screen-height_pix/2;
    ref_points[7][0]=width_screen/2;            ref_points[7][1]=height_screen-height_pix/2;
    ref_points[8][0]=width_screen-height_pix/2; ref_points[8][1]=height_screen-height_pix/2;

    pixmap_points[0][0]=0;                          pixmap_points[0][1]=0;
    pixmap_points[1][0]=width_screen/2-height_pix/2;pixmap_points[1][1]=0;
    pixmap_points[2][0]=width_screen-height_pix;    pixmap_points[2][1]=0;

    pixmap_points[3][0]=0;                          pixmap_points[3][1]=height_screen/2-height_pix/2;
    pixmap_points[4][0]=width_screen/2-height_pix/2;pixmap_points[4][1]=height_screen/2-height_pix/2;
    pixmap_points[5][0]=width_screen-height_pix;    pixmap_points[5][1]=height_screen/2-height_pix/2;

    pixmap_points[6][0]=0;                          pixmap_points[6][1]=height_screen-height_pix;
    pixmap_points[7][0]=width_screen/2-height_pix/2;pixmap_points[7][1]=height_screen-height_pix;
    pixmap_points[8][0]=width_screen-height_pix;    pixmap_points[8][1]=height_screen-height_pix;
    //almacenar posiciones
    for(int i=0;i<9;i++)
    {cal.append_reference_point(ref_points[i][0],ref_points[i][1]);} //primer punto, arriba izq pixmap en 0,0

   ui->pixmap_mirilla->move(pixmap_points[0][0],pixmap_points[0][1]);
    ui->pixmap_mirilla->show();
    num_samples=0;
    trigger_act=0;
    calibracion=1;
    this->showFullScreen();
    }
 else
    {
        //se ha activado mientras se calibraba
        //reiniciar todo
     cal.reset();
     ui->pixmap_mirilla->hide();
     ui->btn_calibration->setText("Calibration");
     ui->btn_connect->show();
     ui->btn_run->show();
     calibracion=0;
     num_samples=0;
     trigger_act=0;
     this->showNormal();
 }
}

void MainWindow::on_btn_run_clicked()
{
    //crear udev con maximos y mínimos 0-screenwidth 0-screenheight
   uidev_opened=0;
    if(cal.isKsetted()==0)
   {
   //error, cal not setted
       return;
   }

       if(events.init_events_joy(QApplication::screens().at(0)->size().width(),
                                 QApplication::screens().at(0)->size().height(),ui->checkBox->isChecked())<0)
    {
        //error, no se ha abiero uidev
        uidev_opened=0;
        return;
    }
    uidev_opened=1;

}
