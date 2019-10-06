#include "lvgl.h"
#define MY_WATER_SYMBOL "\xEF\x81\x83"

void statusbar(void)
{


    /*
     * Status symbols
     */

    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_align(label1, LV_LABEL_ALIGN_RIGHT);       /*Center aligned lines*/
    lv_obj_align(label1, NULL, LV_ALIGN_IN_TOP_RIGHT,0,1);
    lv_label_set_text(label1,LV_SYMBOL_WIFI);


    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
       lv_label_set_align(label2, LV_LABEL_ALIGN_RIGHT);       /*Center aligned lines*/
       lv_obj_align(label2, label1, LV_ALIGN_OUT_LEFT_MID,0,0);
       lv_label_set_text(label2, MY_WATER_SYMBOL);
   }

