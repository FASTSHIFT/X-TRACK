#ifndef __STATUS_BAR_H
#define __STATUS_BAR_H

#include "../Page.h"

typedef enum {
    STATUS_BAR_STYLE_TRANSP,
    STATUS_BAR_STYLE_BLACK,
    STATUS_BAR_STYLE_MAX
}StatusBar_Style_t;

void StatusBar_Init(lv_obj_t* par);
void StatusBar_SetStyle(StatusBar_Style_t style);
void StatusBar_AppearAnimStart(bool playback = false);

#endif
