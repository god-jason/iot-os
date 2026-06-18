
#ifndef _ADAS_UI_H
#define _ADAS_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

extern lv_obj_t * ui_Main;
extern lv_obj_t * ui_ImgRightBak;
extern lv_obj_t * ui_ImgLeftBak;
extern lv_obj_t * ui_ImgWaterTemp;
extern lv_obj_t * ui_ImgOilMeter;
extern lv_obj_t * ui_ImgOut;
extern lv_obj_t * ui_ImgPtr2;
extern lv_obj_t * ui_ImgIn;
extern lv_obj_t * ui_ImgUnder;
extern lv_obj_t * ui_ImgPtr1;
extern lv_obj_t * ui_BtnPlay;
extern lv_obj_t * ui_LabelPlay;
extern lv_obj_t * ui_ImgOil;
extern lv_obj_t * ui_ImgWater;
extern lv_obj_t * ui_ImgRight;
extern lv_obj_t * ui_ImgLeft;
extern lv_obj_t * ui_ImgBat;
extern lv_obj_t * ui_ImgP;
extern lv_obj_t * ui_ImgOil2;
extern lv_obj_t * ui_ImgLight1;
extern lv_obj_t * ui_BtnChgScreen2;
extern lv_obj_t * ui_ImgLight2;
extern lv_obj_t * ui_Comm;
extern lv_obj_t * ui_Panel1;
extern lv_obj_t * ui_Slider1;
extern lv_obj_t * ui_Label1;
extern lv_obj_t * ui_ArcTemp;
extern lv_obj_t * ui_Panel3;
extern lv_obj_t * ui_LblTemp;
extern lv_obj_t * ui_LblTemp2;
extern lv_obj_t * ui_Label4;
extern lv_obj_t * ui_Switch1;
extern lv_obj_t * ui_Switch2;
extern lv_obj_t * ui_LblL1;
extern lv_obj_t * ui_LblL2;
extern lv_obj_t * ui_BtnSend1;
extern lv_obj_t * ui_LblSend;
extern lv_obj_t * ui_Panel2;
extern lv_obj_t * ui_Label2;
extern lv_obj_t * ui_BtnSend2;
extern lv_obj_t * ui_LblSend1;
extern lv_obj_t * ui_TextArea1;
extern lv_obj_t * ui_TextInput;
extern lv_obj_t * ui_Keyboard2;
extern lv_obj_t * ui_BtnChgScreen1;


LV_IMG_DECLARE(ui_img_rightleft2_png);    // assets\rightleft2.png
LV_IMG_DECLARE(ui_img_rightleft1_png);    // assets\rightleft1.png
LV_IMG_DECLARE(ui_img_right1_png);    // assets\right1.png
LV_IMG_DECLARE(ui_img_left1_png);    // assets\left1.png
LV_IMG_DECLARE(ui_img_biao2_png);    // assets\biao2.png
LV_IMG_DECLARE(ui_img_zhizhen2_png);    // assets\zhizhen2.png
LV_IMG_DECLARE(ui_img_biao1_png);    // assets\biao1.png
LV_IMG_DECLARE(ui_img_underside_png);    // assets\underside.png
LV_IMG_DECLARE(ui_img_zhizhen1_png);    // assets\zhizhen1.png
LV_IMG_DECLARE(ui_img_jiayou_png);    // assets\jiayou.png
LV_IMG_DECLARE(ui_img_wendu_png);    // assets\wendu.png
LV_IMG_DECLARE(ui_img_jiantou_png);    // assets\jiantou.png
LV_IMG_DECLARE(ui_img_dianchi_png);    // assets\dianchi.png
LV_IMG_DECLARE(ui_img_p_png);    // assets\P.png
LV_IMG_DECLARE(ui_img_jiyouyali_png);    // assets\jiyouyali.png
LV_IMG_DECLARE(ui_img_yuanguang_png);    // assets\yuanguang.png
LV_IMG_DECLARE(ui_img_jinguang_png);    // assets\jinguang.png
LV_IMG_DECLARE(ui_img_indicator_ver_png);    // assets\indicator_ver.png




void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
