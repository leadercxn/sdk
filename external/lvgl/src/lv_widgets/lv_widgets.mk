CSRCS += lv_arc.c
CSRCS += lv_bar.c
CSRCS += lv_checkbox.c
CSRCS += lv_cpicker.c
CSRCS += lv_dropdown.c
CSRCS += lv_keyboard.c
CSRCS += lv_line.c
CSRCS += lv_msgbox.c
CSRCS += lv_spinner.c
CSRCS += lv_roller.c
CSRCS += lv_table.c
CSRCS += lv_tabview.c
CSRCS += lv_tileview.c
CSRCS += lv_btn.c
CSRCS += lv_calendar.c
CSRCS += lv_chart.c
CSRCS += lv_canvas.c
CSRCS += lv_gauge.c
CSRCS += lv_label.c
CSRCS += lv_list.c
CSRCS += lv_slider.c
CSRCS += lv_textarea.c
CSRCS += lv_spinbox.c
CSRCS += lv_btnmatrix.c
CSRCS += lv_cont.c
CSRCS += lv_img.c
CSRCS += lv_imgbtn.c
CSRCS += lv_led.c
CSRCS += lv_linemeter.c
CSRCS += lv_page.c
CSRCS += lv_switch.c
CSRCS += lv_win.c
CSRCS += lv_objmask.c

DEPPATH += --dep-path $(SDK_DIR)/external/lvgl/src/lv_widgets
VPATH += :$(SDK_DIR)/external/lvgl/src/lv_widgets

IFLAGS += "-I$(SDK_DIR)/external/lvgl/src/lv_widgets"
