#ifndef GUNCON_USB_H
#define GUNCON_USB_H

#include <QObject>
#include <QUsb>

const quint8 USB_PIPE_IN = 0x2;   /* Bulk output endpoint for responses */
const quint8 USB_PIPE_OUT = 0x82;	   /* Bulk input endpoint for commands */
const quint16 USB_TIMEOUT_MSEC = 30;

class guncon_usb : public QObject
{
    Q_OBJECT
public:
    explicit guncon_usb(QObject *parent = 0);
    ~guncon_usb();

    void setupDevice(void);
    bool openDevice(void);
    bool closeDevice(void);
    qint16 read(QByteArray *buf);
    void write(QByteArray *buf);
    QByteArray decode(QByteArray data2);
    bool isOpen();

signals:
    void new_readed_data(QByteArray &);
public slots:
    void new_read();

private:
    QUsbManager mUsbManager;
    QUsbDevice* mUsbDev;

    QtUsb::DeviceFilter mFilter;
    QtUsb::DeviceConfig mConfig;
    bool opened;
};

#endif // GUNCON_USB_H
