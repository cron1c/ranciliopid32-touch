static void create_tab1(lv_obj_t * parent)
{
    
          /*Create a style for the line meter*/
    static lv_style_t style_lmeter;
    lv_style_copy(&style_lmeter, &lv_style_pretty_color);
    style_lmeter.line.width = 3;
    style_lmeter.line.color = LV_COLOR_SILVER;
    style_lmeter.body.main_color = lv_color_hex(0x59ff00);         /*Light blue*/
    style_lmeter.body.grad_color = lv_color_hex(0xff0000);         /*Dark blue*/
    style_lmeter.body.padding.left = 16;                           /*Line length*/

            /*Create a style for the line meter*/
    static lv_style_t style_lmeter2;
    lv_style_copy(&style_lmeter2, &lv_style_pretty_color);
    style_lmeter2.line.width = 2;
    style_lmeter2.line.color = LV_COLOR_SILVER;
    style_lmeter2.body.main_color = lv_color_hex(0xfab6bf);         /*Light blue*/
    style_lmeter2.body.grad_color = lv_color_hex(0xff0000);         /*Dark blue*/
    style_lmeter2.body.padding.left = 16;      


            /*Create style for the Arcs*/
    static lv_style_t style_arc;
    lv_style_copy(&style_arc, &lv_style_plain);
    style_arc.line.color = LV_COLOR_BLUE;           /*Arc color*/
    style_arc.line.width = 4;                       /*Arc width*/

            /* Create Style for Set Label */

    static lv_style_t style_coffee;
    lv_style_copy(&style_coffee, &lv_style_pretty_color);
    style_coffee.text.font = &icon_coffee_40;
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
    lv_obj_set_event_cb(lmeter, event_handler_refresh_input2);   /*Assign an event callback*/


 /******************************************
  *       Label for Set temp
  ******************************************/
     
  #define MY_COFFEE_SYMBOL "\xEF\x83\xB4"
   labelSet = lv_label_create(lmeter, NULL);
   lv_obj_align(labelSet, label, LV_ALIGN_CENTER,-3,-5);
   lv_label_set_align(labelSet, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
   lv_label_set_style(labelSet, LV_LABEL_STYLE_MAIN, &style_coffee);
   lv_label_set_text(labelSet, MY_COFFEE_SYMBOL);

   /***************************
    * Label for temp          *
    **************************/
    //label lmeter
    label = lv_label_create(lmeter, NULL);
    lv_obj_align(label, lmeter, LV_ALIGN_CENTER,0,0);
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_obj_set_event_cb(label, event_handler_refresh_input);   /*Assign an event callback*/
    
    int num = lv_lmeter_get_value(lmeter);
    char snum[7];
    itoa(num, snum, 10);
    
    strcat(snum, "\xB0");
    strcat(snum, "°");

    lv_label_set_text(label, snum);       
    
    


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
       
  arc = lv_arc_create(lmeter1, NULL);
  lv_arc_set_style(arc, LV_ARC_STYLE_MAIN, &style_arc);          /*Use the new style*/
  lv_arc_set_angles(arc, 90, 60);
  lv_obj_set_size(arc, 75, 75);
  lv_obj_set_event_cb(arc, event_handler_refresh_input);
  lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, 0);

 /**************************
  *     Water weight label 
  ***************************/
 
    
  
    labelW = lv_label_create(arc, NULL);
    lv_obj_align(labelW, arc, LV_ALIGN_IN_BOTTOM_MID,0,0);
    lv_label_set_align(labelW, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_obj_set_event_cb(labelW, event_handler_refresh_input);   /*Assign an event callback*/
    
    int weight = getWeight();
    char wnum[10];
    itoa(weight, wnum, 10);
    
    strcat(wnum, "ml");

    lv_label_set_text(labelW, wnum);       /* convert Temp int  to string [buf] */
}


/******************************
 *                            *
 *      Shot Timer            *
 *                            *
 *****************************/


 labelBT = lv_label_create(parent, NULL);
 lv_obj_align(labelBT, labelW, LV_ALIGN_OUT_BOTTOM_MID,0,0);
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
}
