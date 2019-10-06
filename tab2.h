static void create_tab2(lv_obj_t * parent)
{
    lv_coord_t w = 265;

    chart = lv_chart_create(parent, NULL);
    lv_page_set_sb_mode(parent,LV_SB_MODE_OFF);
    lv_chart_set_type(chart, LV_CHART_TYPE_POINT | LV_CHART_TYPE_LINE);
    lv_obj_set_size(chart, w, 180);
    lv_chart_set_range(chart, 0, 100);
    lv_obj_align(chart, parent, LV_ALIGN_IN_RIGHT_MID,-10,0);
    //lv_obj_set_pos(chart, LV_DPI / 10, LV_DPI / 10);
    lv_chart_set_series_darking(chart, 50);
    lv_chart_set_point_count(chart, 20);
    
    lv_chart_set_y_tick_texts(chart, "100\n75\n50\n25\n0", 2, LV_CHART_AXIS_DRAW_LAST_TICK);
    lv_chart_set_y_tick_length(chart, 10, 5);
    lv_chart_set_div_line_count(chart, 3, 3);
    lv_chart_set_margin(chart, 50);
    lv_chart_set_series_opa(chart, 80);
    
    s1 = lv_chart_add_series(chart, LV_COLOR_BLUE);
    s2 = lv_chart_add_series(chart, LV_COLOR_RED);

    
}
