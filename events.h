#ifndef EVENTS_H
#define EVENTS_H
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>


class Events
{
public:
    Events();
    ~Events();
    char init_events_mouse(int width,int height);


    char init_events_joy(int max, int min,bool cb_4_3);
    char set_axis(unsigned int num, short int value);
    char set_button(unsigned int num, int value);
    char syn_report();
private:
    libevdev *dev;
    struct libevdev_uinput *uidev;
};

#endif // EVENTS_H
