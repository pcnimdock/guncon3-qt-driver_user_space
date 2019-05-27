#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSound>
#include <QScreen>
#include <QSize>
#include <QMessageBox>
#include <QSerialPortInfo>

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
    send_guncon2=0;
    var_offset_x=0;
    QList <QSerialPortInfo> lista;
    QSerialPortInfo *serial_info;
    serial_info = new QSerialPortInfo();
    lista = serial_info->availablePorts();
    int i;
    //this->mantener_pulsado=0;

    if(lista.size()>0)
    {
        for(i=0;i<lista.size();i++)
        {ui->cb_serial->addItem(lista.at(i).portName());}
    }
    delete(serial_info);


    gcon3 = new guncon_usb();
    timer = new QTimer;
}

MainWindow::~MainWindow()
{
    delete ui;
}

//GCON2 defines
#define BUTTON_TRIGGER	0x2000
#define BUTTON_A		0x0008
#define BUTTON_B		0x0004
#define BUTTON_C		0x0002
#define BUTTON_SELECT	0x4000
#define BUTTON_START	0x8000
#define DPAD_UP      	0x0010
#define DPAD_DOWN		0x0040
#define DPAD_LEFT 		0x0080
#define DPAD_RIGHT      0x0020

/*
jstest
button0 trigger
1 a1
2 a2
3 b1
4 b2
5 c1
6 c2
7 stick a button
8 stick b button
9
*/
quint8 toongle_trigger=0;
void MainWindow::update_data(QByteArray &data)
{
    static unsigned char apply_event;
    static unsigned char key_pressed;
     static unsigned char btn0_pressed;
    if(data.isEmpty())
    {
        serial.write(mandar);
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
    btn_0=((dec_data.at(12) & 0x04)?1:0);  //A1
    btn_1=((dec_data.at(12) & 0x02)?1:0);  //A2
    btn_2=((dec_data.at(11) & 0x04)?1:0);  //B1
    btn_3=((dec_data.at(11) & 0x02)?1:0);  //B2
    btn_4=((dec_data.at(11) & 0x80)?1:0);  //C1
    btn_5=((dec_data.at(12) & 0x08)?1:0);  //C2
    btn_6=((dec_data.at(10) & 0x80)?1:0);  //A STICK BUTTON
    btn_7=((dec_data.at(10) & 0x40)?1:0);  //B STICK BUTTON
    btn_9=((dec_data.at(11) & 0x10)?1:0);  // out of range
    btn_8=((dec_data.at(11) & 0x08)?1:0);  // only one led reference ¿?
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
            if(ui->checkbox_mouse->isChecked())
            {
                events.set_axis(ABS_X,x); //calibrated absx
                events.set_axis(ABS_Y,y); //calibrated absy
                events.syn_report();
                if(key_pressed!=btn_trigger)
                {
                    key_pressed=btn_trigger;
               if(btn_trigger)
               {

                events.set_button(BTN_TOUCH,(int)btn_trigger);
                events.syn_report();
               }
               else
               {
                   events.set_button(BTN_TOUCH,(int)btn_trigger);
                   events.syn_report();

                 }

                }

            }
            else
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
            events.set_button(BTN_0,(int)btn_0); //A1
            events.set_button(BTN_1,(int)btn_1); //A2
            events.set_button(BTN_2,(int)btn_2); //B1
            events.set_button(BTN_3,(int)btn_3); //B2
            events.set_button(BTN_4,(int)btn_4); //C1
            events.set_button(BTN_5,(int)btn_5); //C2
            events.set_button(BTN_6,(int)btn_6); //A STICK BUTTON
            events.set_button(BTN_7,(int)btn_7); //B STICK BUTTON
            events.set_button(BTN_8,(int)btn_8); //undefined
            events.syn_report();

            }
        }

        if(ui->checkBox_guncon2->isChecked()&&send_guncon2)
        {
            //hacer transformación de valores para guncon2
            if(!serial.isOpen())
            { //serialport no abierto
                return;
            }
            quint16 x_g2,y_g2;
            //hacer regla de 3
            x_g2=((quint32)x)*640/w_screen;
            y_g2=((quint32)y)*256/h_screen;
            if(x_g2>640){x_g2=0;}
            if(y_g2>256){y_g2=0;}
            x_g2+=var_offset_x;

            quint16 btn_g2;
            /*
#define BUTTON_TRIGGER	0x2000
#define BUTTON_A		0x0008
#define BUTTON_B		0x0004
#define BUTTON_C		0x0002
#define BUTTON_SELECT	0x4000
#define BUTTON_START	0x8000
#define DPAD_UP      	0x0010
#define DPAD_DOWN		0x0040
#define DPAD_LEFT 		0x0080
#define DPAD_RIGHT      0x0020
*/
            btn_g2=0x1F00;
            if(btn_trigger){btn_g2&=~((quint16)BUTTON_TRIGGER);ui->trigger_r_button->setChecked(1);}else{btn_g2|=((quint16)BUTTON_TRIGGER);ui->trigger_r_button->setChecked(0);}
            //btn_g2|=((quint16)(~btn_trigger))*BUTTON_TRIGGER;

/*a1*/  if(btn_0){btn_g2&=~((quint16)BUTTON_A);}else{btn_g2|=((quint16)BUTTON_A);}

/*a2*/  if(btn_1){btn_g2&=~((quint16)BUTTON_B);}else{btn_g2|=((quint16)BUTTON_B);}

/*b1*/  if(btn_2){btn_g2&=~((quint16)BUTTON_C);}else{btn_g2|=((quint16)BUTTON_C);}

/*b2*/  if(btn_3){btn_g2&=~((quint16)BUTTON_SELECT);}else{btn_g2|=((quint16)BUTTON_SELECT);}

/*c1*/    if(btn_4){btn_g2&=~((quint16)BUTTON_START);}else{btn_g2|=((quint16)BUTTON_START);}
            if(abs_ry<200){btn_g2|=DPAD_DOWN;}
            if(abs_ry>25){btn_g2|=DPAD_UP;}
            if(abs_rx<200){btn_g2|=DPAD_RIGHT;}
            if(abs_rx>25){btn_g2|=DPAD_LEFT;}
            if(btn_8)
            {
                x_g2=0;y_g2=0;
            }
//           if((toongle_trigger==2))
//            {
//                x_g2=y_g2=0;

//            }
            mandar.clear();
            mandar.append(btn_g2&0xFF);
            mandar.append(btn_g2>>8);
            mandar.append(x_g2&0xFF);
            mandar.append(x_g2>>8);
            mandar.append(y_g2&0xFF);
            mandar.append(y_g2>>8);
            quint16 temp_var=0;
            if(btn_8){temp_var=0xFE;}else{temp_var=0;}
            if(btn_5){temp_var|=0x01;}
            mandar.append(temp_var);
            serial.write(mandar);

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
    timer->setInterval(8);
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
    h_screen=height_screen;
    w_screen=width_screen;

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
if(ui->checkbox_mouse->isChecked())
{
    if(events.init_events_mouse(QApplication::screens().at(0)->size().width(),
                              QApplication::screens().at(0)->size().height())<0)
 {
     //error, no se ha abiero uidev
     uidev_opened=0;
     return;
 }
}
else
{
    if(ui->checkBox_guncon2->isChecked()==0)
    {
    if(events.init_events_joy(QApplication::screens().at(0)->size().width(),
                                 QApplication::screens().at(0)->size().height(),ui->checkBox->isChecked())<0)
    {
        //error, no se ha abiero uidev
        uidev_opened=0;
        return;
    }
    }
    else
    {
        uidev_opened=0;
        send_guncon2=1;
        return;
    }
}
    uidev_opened=1;

}

void MainWindow::on_btn_serial_connect_clicked()
{
    serial.setPortName(ui->cb_serial->currentText());
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    //QByteArray nada;
    if (!serial.open(QIODevice::ReadWrite)) {
        //emit error(tr("Can't open %1, error code %2")
        //           .arg(portName).arg(serial.error()),nada);
        QMessageBox::warning(this, tr("Error de conexión"),
                                      tr("No se ha podido conectar al puerto \n"
                                         "%1, err %2").arg(ui->cb_serial->currentText()).arg(serial.errorString()),
                                      QMessageBox::Ok );
        return;
    }
    connect(&serial,SIGNAL(readyRead()),this,SLOT(data_rcv()));
}


void MainWindow::data_rcv()
{

    serial_buffer.append(serial.readAll());
    if(serial_buffer.contains("\n", Qt::CaseInsensitive))
    {
        //esto es un comando completo
        if(serial_buffer.contains("conf:", Qt::CaseInsensitive))
        {
            ui->lbl_conf->setText(serial_buffer);
            //convert hex to int
            QStringList temp_list;
            serial_buffer.remove(0,serial_buffer.indexOf("conf:",Qt::CaseInsensitive));
            temp_list=serial_buffer.split(" ");
            qint16 var;
            QString sep;
           sep=temp_list.at(1);
           bool ok;
           var=sep.toInt(&ok,16);
           var_offset_x=var*(-1)/2;
        ui->txt_log->append(serial_buffer);
        }
        else
        {
            ui->txt_log->append(serial_buffer);
        }
    serial_buffer.clear();
    }

}
