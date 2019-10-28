static void create_tab1(lv_obj_t * parent)
{
    
          /*Create a style for the line meter*/
    static lv_style_t style_lmeter;
    lv_style_copy(&style_lmeter, &lv_style_pretty_color);
    style_lmeter.line.width = 3;
    style_lmeter.line.color = lv_color_hex(0x8d8f94);
    style_lmeter.body.main_color = lv_color_hex(0xff0000);         /*0x59ff00*/
    style_lmeter.body.grad_color = lv_color_hex(0x59ff00);         /*0xff0000*/
    style_lmeter.body.padding.left = 16;                           /*Line length*/

            /*Create a style for the line meter*/
    static lv_style_t style_lmeter2;
    lv_style_copy(&style_lmeter2, &lv_style_pretty_color);
    style_lmeter2.line.width = 2;
    style_lmeter2.line.color = LV_COLOR_SILVER;
    style_lmeter2.body.main_color = lv_color_hex(0xfab6bf);         /*red*/
    style_lmeter2.body.grad_color = lv_color_hex(0xff0000);         /*Dark red*/
    style_lmeter2.body.padding.left = 16;      

            /* Create style for the Arcs */

    static lv_style_t style_arc;
    lv_style_copy(&style_arc, &lv_style_plain);
    style_arc.line.color = lv_color_hex(0x39A4FF);           /*Arc color*/
    style_arc.line.width = 4;                       /*Arc width*/

    static lv_style_t style_arcDrk;
    lv_style_copy(&style_arcDrk, &lv_style_plain);
    style_arcDrk.line.color = lv_color_hex(0x426380);          /*Arc color*/
    style_arcDrk.line.width = 14;

    static lv_style_t style_arcLightDrk;
    lv_style_copy(&style_arcLightDrk, &lv_style_plain);
    style_arcLightDrk.line.color = lv_color_hex(0x1C5180);          /*Arc color*/
    style_arcLightDrk.line.width = 4;

            /* Create Style for Set Label */

    static lv_style_t style_coffee;
    lv_style_copy(&style_coffee, &lv_style_pretty_color);
    style_coffee.text.color = lv_color_hex(0x000000);

    /************************
     *  Create a line meter *
     *************************/
    lmeter = lv_lmeter_create(parent, NULL);
    lv_lmeter_set_range(lmeter, 0, 135);                   /*Set the range*/
    lv_lmeter_set_value(lmeter, 20);                       /*Set the current value*/
    lv_lmeter_set_scale(lmeter, 240, 31);                  /*Set the angle and number of lines*/
    lv_lmeter_set_style(lmeter, LV_LMETER_STYLE_MAIN, &style_lmeter);           /*Apply the new style*/
    lv_obj_set_size(lmeter, 200, 200);
    lv_obj_align(lmeter, parent, LV_ALIGN_CENTER, 0, 15);  
  
    //Event for lmeter value
    lv_obj_set_event_cb(lmeter, event_handler_refresh_input);   /*Assign an event callback*/


 /******************************************
  *       Label for Set temp
  ******************************************/
     
  //#define MY_COFFEE_SYMBOL "\xEF\x83\xB4"
   labelSet = lv_label_create(lmeter, NULL);
   lv_obj_align(labelSet, label, LV_ALIGN_CENTER,-1,13);
   lv_label_set_align(labelSet, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
   lv_label_set_recolor(labelSet, true);
   lv_obj_set_event_cb(labelSet, event_handler_refresh_input);   /*Assign an event callback*/
   /***************************
    * Label for temp          *
    **************************/
    //label lmeter
    label = lv_label_create(lmeter, NULL);
    lv_obj_align(label, lmeter, LV_ALIGN_CENTER,-1,-18);
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_label_set_recolor(label, true); 
    lv_obj_set_event_cb(label, event_handler_refresh_input2);   /*Assign an event callback*/

/*******************************************
 * Create line meter for Heating percentage
 ********************************************/
    lv_page_set_sb_mode(parent,LV_SB_MODE_OFF);  
    lmeter1 = lv_lmeter_create(lmeter, NULL);
    lv_lmeter_set_range(lmeter1, 0, 100);                   /*Set the range*/
    lv_lmeter_set_value(lmeter1, (Output / 10));                       /*Set the current value*/
    lv_lmeter_set_scale(lmeter1, 240, 25);                  /*Set the angle and number of lines*/
    lv_lmeter_set_style(lmeter1, LV_LMETER_STYLE_MAIN, &style_lmeter2);           /*Apply the new style*/
    lv_obj_set_size(lmeter1, 150, 150);
    lv_obj_align(lmeter1, lmeter, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(lmeter1, event_handler_refresh_output);   /*Assign an event callback*/

/**************************************
 *         Create water arc elemet
 ***************************************/
  if (SCALE == 1){
 
  arc3 = lv_arc_create(parent, NULL);
  lv_arc_set_style(arc3, LV_ARC_STYLE_MAIN, &style_arcLightDrk);          /*Use the new style*/
  lv_arc_set_angles(arc3, 0, 360);
  lv_obj_set_size(arc3, 75, 75);
  //lv_obj_set_event_cb(arc, event_handler_refresh_input);
  lv_obj_align(arc3, NULL, LV_ALIGN_CENTER, 0, 0);

  arc2 = lv_arc_create(parent, NULL);
  lv_arc_set_style(arc2, LV_ARC_STYLE_MAIN, &style_arcDrk);          /*Use the new style*/
  lv_arc_set_angles(arc2, 0, 360);
  lv_obj_set_size(arc2, 100, 100);
  //lv_obj_set_event_cb(arc, event_handler_refresh_input);
  lv_obj_align(arc2, NULL, LV_ALIGN_CENTER, 0, 0); 
       
  arc = lv_arc_create(tab1, NULL);
  lv_arc_set_style(arc, LV_ARC_STYLE_MAIN, &style_arc);          /*Use the new style*/
  lv_arc_set_angles(arc, 90, 60);
  lv_obj_set_size(arc, 75, 75);
  lv_obj_set_event_cb(arc, event_handler_IP);
  lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, 0);

 /**************************
  *     Water weight label 
  ***************************/  

}

  
  /*  Buttons + and - to set temp   */     
      
    btnPlus = lv_btn_create(parent,NULL);    
      
    lv_obj_align(btnPlus,lmeter1, LV_ALIGN_IN_TOP_MID,0,-2);    
    lv_btn_set_fit(btnPlus, LV_FIT_TIGHT);   
    lv_btn_set_style(btnPlus, LV_BTN_STYLE_REL, &lv_style_transp_tight);    
    lv_btn_set_style(btnPlus, LV_BTN_STYLE_PR, &lv_style_transp_tight);   
      
      
      
    labelPlus = lv_label_create(btnPlus, NULL);   
    lv_label_set_text(labelPlus, "+");    
      
 /* minus button    */
    btnMinus = lv_btn_create(parent,NULL);   
      
    lv_obj_align(btnMinus,lmeter1, LV_ALIGN_IN_BOTTOM_MID,0,-1);    
    lv_btn_set_fit(btnMinus, LV_FIT_TIGHT);    
    lv_btn_set_style(btnMinus, LV_BTN_STYLE_REL, &lv_style_transp_tight);   
    lv_btn_set_style(btnMinus, LV_BTN_STYLE_PR, &lv_style_transp_tight);    
      
      
    labelMinus = lv_label_create(btnMinus, NULL);   
    lv_label_set_text(labelMinus, "-");
/******************************
 *                            *
 *      Shot Timer            *
 *                            *
 *****************************/


 labelBT = lv_label_create(parent, NULL);
 lv_obj_align(labelBT, btnMinus, LV_ALIGN_IN_BOTTOM_MID,0,15);
 lv_label_set_align(labelBT, LV_LABEL_ALIGN_CENTER);
 lv_obj_set_event_cb(labelBT, event_handler_refresh_input);

    double brtime = bezugsZeit / 1000;
    double tbtime = brewtimersoftware;
    char br[9];
    char tb[4];
    fmtDouble(brtime, 0, br);
    fmtDouble(tbtime, 0, tb);
    strcat(br, "/");
    strcat(br, tb);

    lv_label_set_text(labelBT, br);

     /****
     *
     *  Horizontal deco line
     */
     
    /*Create an array for the points of the line*/

    static lv_point_t line_points[] = { {2, 2}, {66, 2} };

    /*Create new style (thick dark blue)*/
    static lv_style_t style_line;
    lv_style_copy(&style_line, &lv_style_plain);
    style_line.line.color = LV_COLOR_MAKE(0xAA, 0xAA, 0xAA);
    style_line.line.width = 2;
    style_line.line.rounded = 0;

    /*Copy the previous line and apply the new style*/
    lv_obj_t * line1;
    line1 = lv_line_create(parent, NULL);
    lv_line_set_points(line1, line_points, 2);     /*Set the points*/
    lv_line_set_style(line1, LV_LINE_STYLE_MAIN, &style_line);
    lv_obj_align(line1, NULL, LV_ALIGN_CENTER, 0, 0);
}
