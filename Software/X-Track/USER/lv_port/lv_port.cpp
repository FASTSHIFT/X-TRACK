#include "lv_port.h"

void lv_port_init(void)
{
    lv_port_disp_init();
    lv_port_indev_init();
    lv_port_fs_init();
}
