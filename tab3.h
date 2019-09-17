static void create_tab3(lv_obj_t * parent)
{


static const char * btnm_map[] = {LV_SYMBOL_EDIT, LV_SYMBOL_LOOP, ""};

    btnm1 = lv_btnm_create(parent, NULL);
    lv_btnm_set_map(btnm1, btnm_map);
    lv_obj_align(btnm1, tab3, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, event_handler_btn_matrix);

    /*Create a window*/
    win = lv_win_create(parent, NULL);
    lv_win_set_title(win, "Scale Calibration");                        /*Set the title*/
    lv_obj_align(win, tab3, LV_ALIGN_IN_TOP_MID,50,0);
    lv_win_set_layout(win,LV_LAYOUT_PRETTY);
    lv_obj_set_size(win, 230, 200);
    lv_obj_set_hidden(win,true);

    /*Add control button to the header*/
    lv_obj_t * close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);           /*Add close button and use built-in close action*/
    lv_obj_set_event_cb(close_btn, lv_win_close_event_cb);
    lv_win_add_btn(win, LV_SYMBOL_OK);        /*Add a setup button*/

    /*Add some dummy content*/
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "Please remove the water,\nthen add the known calibration\nweight!\nSpecify the calibration\nweight below and press ok.");

    
    }
