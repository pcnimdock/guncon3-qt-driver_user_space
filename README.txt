Para poder compilar se necesita tener instalado Qt.

En ubuntu 20.04
sudo apt install build-essential qt5-default libqt5serialport5-dev libqt5multimedia5 libqt5multimedia5-dev qtmultimedia5-dev

Dependencias
sudo apt-get install libusb-1.0-0 libusb-1.0-0-dev libevdev2 libevdev-dev

Para compilar
git clone https://github.com/pcnimdock/guncon3-qt-driver_user_space.git
cd guncon3-qt-driver_user_space
make

Dar permisos para que el usuario pueda manejar el uinput. Cambiar pcnimdock por el usuario correspondiente

sudo setfacl -m user:pcnimdock:rw /dev/uinput

