#include "tpcal.c"

static void event_handler_btn_matrix(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED) {
    if (lv_btnm_get_active_btn_text(btnm1) == MY_CAL_SYMBOL) {
      tpcal_create();
    }
    if (lv_btnm_get_active_btn_text(btnm1) == MY_PID_SYMBOL) {
      list_settings();
    }
    if (lv_btnm_get_active_btn_text(btnm1) == MY_SCALE_SYMBOL) {
      /*Add some content to win*/
      txt = lv_label_create(win, NULL);
      lv_label_set_text(txt, "Please remove the water,\nthen add the known calibration\nweight!\nSpecify the calibration\nweight below and press ok.");
    }
    //const char * txt = lv_btnm_get_active_btn_text(obj);
    lv_obj_set_hidden(win, false);

    //printf("%s was pressed\n", txt);
  }
}

static void event_handler_minus(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    static char p_id[4];
    P = bPID.GetKp();
    P--;
    sprintf(p_id, "%d", P);
    lv_label_set_text(labelP, p_id);
  }
  else if (event == LV_EVENT_VALUE_CHANGED) {

  }
}

static void event_handler_plus(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {

    P++;

    sprintf(p_id, "%d", P);
    lv_label_set_text(labelP, p_id);
  }
  else if (event == LV_EVENT_VALUE_CHANGED) {

  }
}

static void event_handler_IM(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    static char pi_d[4];
    I--;
    sprintf(pi_d, "%d", I);
    lv_label_set_text(labelI, pi_d);
  }
  else if (event == LV_EVENT_VALUE_CHANGED) {

  }
}
static void event_handler_IP(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_REFRESH) {
    if (SCALE == 1) {
      int w = (getWeight() - tareweight);
      lv_arc_set_angles(arc, 0, (w / 5.5));

      if (w > 0 && w < 601 && PUSHNOTIFACTIONS == 1 && NOTIFY == 0) {
        Blynk.notify("Warning: Water level low!");
        NOTIFY = 1;
      } else {
        if (w > 600 && PUSHNOTIFACTIONS == 1) {
          NOTIFY = 0;
        }
      }
    }


  }
}

static void event_handler_refresh_input(lv_obj_t * obj, lv_event_t event)
{
  static int16_t prevVal = 0;

  if (event == LV_EVENT_REFRESH) {
    prevVal = setPoint;
    if (prevVal != setPoint) {
      lv_chart_set_next(chart, s3, setPoint);
    }



    prevVal = setPoint;
    lv_lmeter_set_value(lmeter, Input);
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
static void event_handler_refresh_tab2(lv_obj_t * obj, lv_event_t event)
{
  lv_chart_set_next(chart, s1, Input);
  lv_chart_set_next(chart, s2, Output / 10);
}

static void event_handler_refresh_input2(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_REFRESH) {
    uint32_t value;
    value = setPointTemp;
    uint32_t num = lv_lmeter_get_value(lmeter);
    char snum[32];
    char buff[32];
    snprintf(buff, 32, "#ff0000 %d °C", value);
    lv_label_set_text(labelSet, buff);
    //strcat(snum, "#ff0000 ");
    //itoa(num, snum, 10);
    //strcat(snum, "°");
    //strcat(snum, "C");
    if (num > (value + TRESHOLDMAX) || num < (value - TRESHOLDMIN)) {
      snprintf(snum, 32, "#ff0000 %d °C", num);
    } else {
      snprintf(snum, 32, "#07f747 %d °C", num);
    }
    lv_label_set_text(label, snum);
  }
}
static void event_handler_refresh_setTemp(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_REFRESH) {
    uint32_t value2;
    value2 = setPointTemp;
    char buff[32];
    snprintf(buff, 32, "#ff0000 %d °C", value2);
    lv_label_set_text(labelSet, buff);
  }
}
static void event_handler_refresh_output(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_REFRESH) {
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

static void arc_loader(lv_task_t * t)
{
  static int16_t a = 0;

  a += 100;
  if (a >= 359) a = 359;

  if (a < 180) lv_arc_set_angles(arc4, 180 - a , 180);
  else lv_arc_set_angles(arc4, 540 - a , 180);

  if (a >= 359) {
    lv_task_del(t);
    return;
  }
}
