static void event_handler_btn_matrix(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
    	if(lv_btnm_get_active_btn_text(btnm1) == MY_CAL_SYMBOL){
    	    		tpcal_create();
    	    	}
        const char * txt = lv_btnm_get_active_btn_text(obj);
        lv_obj_set_hidden(win,false);

        printf("%s was pressed\n", txt);
    }
}
static void event_handler_hide(lv_obj_t * obj, lv_event_t event)
{ 
        lv_obj_set_hidden(win,true);    
}
static void event_handler_minus(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
      static char p_id[4];
      P = bPID.GetKp();
      P--;
      sprintf(p_id, "%d", P);
      lv_label_set_text(labelP, p_id);
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
       
    }
}

static void event_handler_plus(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {

       P++;
      
      sprintf(p_id, "%d", P);
       lv_label_set_text(labelP, p_id);
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
       
    }
}

static void event_handler_IM(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
      static char pi_d[4];
       I--;
      sprintf(pi_d, "%d", I);
      lv_label_set_text(labelI, pi_d);
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
       
    }
}
static void event_handler_IP(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {

       I++;
      
      sprintf(pi_d, "%d", I);
      lv_label_set_text(labelI, pi_d);
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
       
    }
}

static void event_handler_refresh_input(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_REFRESH) {
     lv_lmeter_set_value(lmeter, Input);                       /*Set the current value*/
     lv_chart_set_next(chart, s1, Input);
     lv_chart_set_next(chart, s2, Output / 10);
     if (SCALE == 1){
     int w = (getWeight() - tareweight);
     lv_arc_set_angles(arc,0,(w / 5.5));
     char weight[10];
     itoa(w, weight, 10);
     strcat(weight, "ml");
    lv_label_set_text(labelW, weight);
    if(w > 0 && w < 601 && PUSHNOTIFACTIONS == 1 && NOTIFY == 0){
      Blynk.notify("Warning: Water level low!");
      NOTIFY = 1;
    }else{
      if(w > 600 && PUSHNOTIFACTIONS == 1){
           NOTIFY = 0;
      }
    }
     }
    double brtime = bezugsZeit / 1000;
    double tbtime = brewtimersoftware;
    char br[9];
    char tb[4];
    fmtDouble(brtime, 0, br);
    fmtDouble(tbtime, 0, tb);
   // itoa(brtime, br,4);
    //itoa(tbtime2, tb,4);
    strcat(br, "/");
    strcat(br, tb);
 
    lv_label_set_text(labelBT, br);
     
  }
}
static void event_handler_refresh_input2(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_REFRESH) {
    int num = lv_lmeter_get_value(lmeter);
    char snum[4];
    itoa(num, snum, 10);
    //strcat(snum, "\xB0");
    strcat(snum, "°");


    
    
    lv_label_set_text(label, snum); 
  }
}
static void event_handler_refresh_output(lv_obj_t * obj, lv_event_t event)
{
  if(event == LV_EVENT_REFRESH) {
    //line meter animation

    a.var = lmeter1;
    a.exec_cb = (lv_anim_exec_xcb_t)lv_lmeter_set_value;    /*Set the animator function and variable to animate*/ 
    a.start = lv_lmeter_get_value(lmeter1);
    a.end = Output / 10;

    a.path_cb = lv_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = 0;
    a.time = 200;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    lv_anim_create(&a);
    //lv_lmeter_set_value(lmeter1, (Output/10)); 
  }
}
