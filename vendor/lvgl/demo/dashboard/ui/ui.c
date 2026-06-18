
#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Main;
lv_obj_t * ui_ImgRightBak;
lv_obj_t * ui_ImgLeftBak;
lv_obj_t * ui_ImgWaterTemp;
lv_obj_t * ui_ImgOilMeter;
lv_obj_t * ui_ImgOut;
lv_obj_t * ui_ImgPtr2;
lv_obj_t * ui_ImgIn;
lv_obj_t * ui_ImgUnder;
lv_obj_t * ui_ImgPtr1;
lv_obj_t * ui_BtnPlay;
lv_obj_t * ui_LabelPlay;
lv_obj_t * ui_ImgOil;
lv_obj_t * ui_ImgWater;
lv_obj_t * ui_ImgRight;
lv_obj_t * ui_ImgLeft;
lv_obj_t * ui_ImgBat;
lv_obj_t * ui_ImgP;
lv_obj_t * ui_ImgOil2;
lv_obj_t * ui_ImgLight1;
lv_obj_t * ui_BtnChgScreen2;
lv_obj_t * ui_ImgLight2;
lv_obj_t * ui_Comm;
lv_obj_t * ui_Panel1;
lv_obj_t * ui_Slider1;
lv_obj_t * ui_Label1;
lv_obj_t * ui_ArcTemp;
lv_obj_t * ui_Panel3;
lv_obj_t * ui_LblTemp;
lv_obj_t * ui_LblTemp2;
lv_obj_t * ui_Label4;
lv_obj_t * ui_Switch1;
lv_obj_t * ui_Switch2;
lv_obj_t * ui_LblL1;
lv_obj_t * ui_LblL2;
lv_obj_t * ui_BtnSend1;
lv_obj_t * ui_LblSend;
lv_obj_t * ui_Panel2;
lv_obj_t * ui_Label2;
lv_obj_t * ui_BtnSend2;
lv_obj_t * ui_LblSend1;
lv_obj_t * ui_TextArea1;
lv_obj_t * ui_TextInput;
lv_obj_t * ui_Keyboard2;
lv_obj_t * ui_BtnChgScreen1;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "#error LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

// ui_EloAnimation0
// FUNCTION HEADER
void AniPtr1_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniPtr1_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 4000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_image_angle);
    lv_anim_set_values(&PropertyAnimation_0, 0, 2600);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_image_angle);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 4000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_image_angle);
    lv_anim_set_values(&PropertyAnimation_1, 0, -2600);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 4500);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_image_angle);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniPtr2_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniPtr2_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 3000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_image_angle);
    lv_anim_set_values(&PropertyAnimation_0, 0, 2400);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_image_angle);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 3000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_image_angle);
    lv_anim_set_values(&PropertyAnimation_1, 0, -2400);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 3500);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_image_angle);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniOil1_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniOil1_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 2000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_in);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 2000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 0, -255);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_ease_out);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 2500);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniWater_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniWater_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 2000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 2000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 3000);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniOil2_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniOil2_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 1000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 1000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 2000);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniBat_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniBat_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 1000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 1000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 1000);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniLeft_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniLeft_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 500);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 500);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniRight_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniRight_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 500);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 500);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 500);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniP_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniP_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 2000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 2000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 2500);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniLight1_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniLight1_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 1000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 1000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 1000);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

// ui_EloAnimation0
// FUNCTION HEADER
void AniLight2_Animation(lv_obj_t * TargetObject, int delay);

// FUNCTION
void AniLight2_Animation(lv_obj_t * TargetObject, int delay)
{

    //
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 2000);
    lv_anim_set_user_data(&PropertyAnimation_0, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_0);

    //
    lv_anim_t PropertyAnimation_1;
    lv_anim_init(&PropertyAnimation_1);
    lv_anim_set_time(&PropertyAnimation_1, 2000);
    lv_anim_set_user_data(&PropertyAnimation_1, TargetObject);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_1, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_1, 255, 0);
    lv_anim_set_path_cb(&PropertyAnimation_1, lv_anim_path_linear);
    lv_anim_set_delay(&PropertyAnimation_1, delay + 2000);
    lv_anim_set_early_apply(&PropertyAnimation_1, false);
    lv_anim_set_get_value_cb(&PropertyAnimation_1, &_ui_anim_callback_get_opacity);
    lv_anim_start(&PropertyAnimation_1);

}

///////////////////// FUNCTIONS ////////////////////
static void ui_event_BtnPlay(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        AniPtr2_Animation(ui_ImgPtr2, 0);
        AniPtr1_Animation(ui_ImgPtr1, 0);
        AniOil1_Animation(ui_ImgOil, 0);
        AniWater_Animation(ui_ImgWater, 0);
        AniOil2_Animation(ui_ImgOil2, 1000);
        AniBat_Animation(ui_ImgBat, 1500);
        AniLeft_Animation(ui_ImgLeft, 2000);
        AniRight_Animation(ui_ImgRight, 3000);
        AniP_Animation(ui_ImgP, 3000);
        AniLight1_Animation(ui_ImgLight1, 4000);
        AniLight2_Animation(ui_ImgLight2, 5000);
    }
}
static void ui_event_BtnChgScreen2(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Comm, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}
static void ui_event_ArcTemp(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_VALUE_CHANGED) {
        _ui_arc_set_text_value(ui_LblTemp, ta, "", "°");
    }
}
// static void ui_event_BtnSend1(lv_event_t * e)
// {
//     lv_event_code_t event = lv_event_get_code(e);
//     lv_obj_t * ta = lv_event_get_target(e);
//     if(event == LV_EVENT_CLICKED) {
//         event_btn_send_1(e);
//     }
// }
// static void ui_event_BtnSend2(lv_event_t * e)
// {
//     lv_event_code_t event = lv_event_get_code(e);
//     lv_obj_t * ta = lv_event_get_target(e);
//     if(event == LV_EVENT_CLICKED) {
//         event_btn_send_2(e);
//     }
// }
static void ui_event_TextInput(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_flag_modify(ui_Keyboard2, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_TOGGLE);
    }
}
static void ui_event_BtnChgScreen1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Main, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0);
    }
}

///////////////////// SCREENS ////////////////////
void ui_Main_screen_init(void)
{

    // ui_Main

    ui_Main = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Main, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgRightBak

    ui_ImgRightBak = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgRightBak, &ui_img_rightleft2_png);

    lv_obj_set_width(ui_ImgRightBak, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgRightBak, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgRightBak, 113);
    lv_obj_set_y(ui_ImgRightBak, 13);

    lv_obj_set_align(ui_ImgRightBak, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgRightBak, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgRightBak, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgLeftBak

    ui_ImgLeftBak = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgLeftBak, &ui_img_rightleft1_png);

    lv_obj_set_width(ui_ImgLeftBak, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgLeftBak, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgLeftBak, -122);
    lv_obj_set_y(ui_ImgLeftBak, 15);

    lv_obj_set_align(ui_ImgLeftBak, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgLeftBak, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgLeftBak, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgWaterTemp

    ui_ImgWaterTemp = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgWaterTemp, &ui_img_right1_png);

    lv_obj_set_width(ui_ImgWaterTemp, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgWaterTemp, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgWaterTemp, 117);
    lv_obj_set_y(ui_ImgWaterTemp, 2);

    lv_obj_set_align(ui_ImgWaterTemp, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgWaterTemp, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgWaterTemp, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgOilMeter

    ui_ImgOilMeter = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgOilMeter, &ui_img_left1_png);

    lv_obj_set_width(ui_ImgOilMeter, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgOilMeter, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgOilMeter, -125);
    lv_obj_set_y(ui_ImgOilMeter, 3);

    lv_obj_set_align(ui_ImgOilMeter, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgOilMeter, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgOilMeter, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgOut

    ui_ImgOut = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgOut, &ui_img_biao2_png);

    lv_obj_set_width(ui_ImgOut, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgOut, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgOut, -1);
    lv_obj_set_y(ui_ImgOut, 10);

    lv_obj_set_align(ui_ImgOut, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgOut, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgOut, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgPtr2

    ui_ImgPtr2 = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgPtr2, &ui_img_zhizhen2_png);

    lv_obj_set_width(ui_ImgPtr2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgPtr2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgPtr2, -61);
    lv_obj_set_y(ui_ImgPtr2, 8);

    lv_obj_set_align(ui_ImgPtr2, LV_ALIGN_CENTER);
    lv_img_set_zoom(ui_ImgPtr2, 200);
    lv_img_set_pivot(ui_ImgPtr2, 114, 3);
    lv_img_set_angle(ui_ImgPtr2, -300);
    lv_obj_add_flag(ui_ImgPtr2, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgPtr2, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgIn

    ui_ImgIn = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgIn, &ui_img_biao1_png);

    lv_obj_set_width(ui_ImgIn, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgIn, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgIn, -1);
    lv_obj_set_y(ui_ImgIn, 7);

    lv_obj_set_align(ui_ImgIn, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgIn, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgIn, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgUnder

    ui_ImgUnder = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgUnder, &ui_img_underside_png);

    lv_obj_set_width(ui_ImgUnder, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgUnder, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgUnder, -6);
    lv_obj_set_y(ui_ImgUnder, 112);

    lv_obj_set_align(ui_ImgUnder, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgUnder, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgUnder, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgPtr1

    ui_ImgPtr1 = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgPtr1, &ui_img_zhizhen1_png);

    lv_obj_set_width(ui_ImgPtr1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgPtr1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgPtr1, -37);
    lv_obj_set_y(ui_ImgPtr1, 8);

    lv_obj_set_align(ui_ImgPtr1, LV_ALIGN_CENTER);
    lv_img_set_zoom(ui_ImgPtr1, 220);
    lv_img_set_pivot(ui_ImgPtr1, 63, 2);
    lv_img_set_angle(ui_ImgPtr1, -400);
    lv_obj_add_flag(ui_ImgPtr1, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgPtr1, LV_OBJ_FLAG_SCROLLABLE);

    // ui_BtnPlay

    ui_BtnPlay = lv_btn_create(ui_Main);

    lv_obj_set_width(ui_BtnPlay, 50);
    lv_obj_set_height(ui_BtnPlay, 30);

    lv_obj_set_x(ui_BtnPlay, -201);
    lv_obj_set_y(ui_BtnPlay, -113);

    lv_obj_set_align(ui_BtnPlay, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_BtnPlay, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BtnPlay, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_BtnPlay, ui_event_BtnPlay, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_BtnPlay, lv_color_hex(0x8A173F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BtnPlay, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LabelPlay

    ui_LabelPlay = lv_label_create(ui_Main);

    lv_obj_set_width(ui_LabelPlay, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelPlay, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LabelPlay, -202);
    lv_obj_set_y(ui_LabelPlay, -113);

    lv_obj_set_align(ui_LabelPlay, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LabelPlay, "Play");

    // ui_ImgOil

    ui_ImgOil = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgOil, &ui_img_jiayou_png);

    lv_obj_set_width(ui_ImgOil, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgOil, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgOil, -136);
    lv_obj_set_y(ui_ImgOil, 31);

    lv_obj_set_align(ui_ImgOil, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgOil, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgOil, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgWater

    ui_ImgWater = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgWater, &ui_img_wendu_png);

    lv_obj_set_width(ui_ImgWater, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgWater, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgWater, 130);
    lv_obj_set_y(ui_ImgWater, 29);

    lv_obj_set_align(ui_ImgWater, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgWater, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgWater, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgRight

    ui_ImgRight = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgRight, &ui_img_jiantou_png);

    lv_obj_set_width(ui_ImgRight, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgRight, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgRight, 53);
    lv_obj_set_y(ui_ImgRight, -114);

    lv_obj_set_align(ui_ImgRight, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgRight, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgRight, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgLeft

    ui_ImgLeft = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgLeft, &ui_img_jiantou_png);

    lv_obj_set_width(ui_ImgLeft, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgLeft, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgLeft, -37);
    lv_obj_set_y(ui_ImgLeft, -96);

    lv_obj_set_align(ui_ImgLeft, LV_ALIGN_CENTER);
    lv_img_set_pivot(ui_ImgLeft, 0, 0);
    lv_img_set_angle(ui_ImgLeft, 1800);
    lv_obj_add_flag(ui_ImgLeft, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgLeft, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgBat

    ui_ImgBat = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgBat, &ui_img_dianchi_png);

    lv_obj_set_width(ui_ImgBat, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgBat, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgBat, -108);
    lv_obj_set_y(ui_ImgBat, -98);

    lv_obj_set_align(ui_ImgBat, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgBat, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgBat, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgP

    ui_ImgP = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgP, &ui_img_p_png);

    lv_obj_set_width(ui_ImgP, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgP, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgP, 105);
    lv_obj_set_y(ui_ImgP, -96);

    lv_obj_set_align(ui_ImgP, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgP, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgP, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgOil2

    ui_ImgOil2 = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgOil2, &ui_img_jiyouyali_png);

    lv_obj_set_width(ui_ImgOil2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgOil2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgOil2, -169);
    lv_obj_set_y(ui_ImgOil2, -71);

    lv_obj_set_align(ui_ImgOil2, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgOil2, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgOil2, LV_OBJ_FLAG_SCROLLABLE);

    // ui_ImgLight1

    ui_ImgLight1 = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgLight1, &ui_img_yuanguang_png);

    lv_obj_set_width(ui_ImgLight1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgLight1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgLight1, 160);
    lv_obj_set_y(ui_ImgLight1, -75);

    lv_obj_set_align(ui_ImgLight1, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgLight1, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgLight1, LV_OBJ_FLAG_SCROLLABLE);

    // ui_BtnChgScreen2

    ui_BtnChgScreen2 = lv_btn_create(ui_Main);

    lv_obj_set_width(ui_BtnChgScreen2, 72);
    lv_obj_set_height(ui_BtnChgScreen2, 37);

    lv_obj_set_x(ui_BtnChgScreen2, 201);
    lv_obj_set_y(ui_BtnChgScreen2, -116);

    lv_obj_set_align(ui_BtnChgScreen2, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_BtnChgScreen2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BtnChgScreen2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_BtnChgScreen2, ui_event_BtnChgScreen2, LV_EVENT_ALL, NULL);
    lv_obj_set_style_opa(ui_BtnChgScreen2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_ImgLight2

    ui_ImgLight2 = lv_img_create(ui_Main);
    lv_img_set_src(ui_ImgLight2, &ui_img_jinguang_png);

    lv_obj_set_width(ui_ImgLight2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_ImgLight2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_ImgLight2, 207);
    lv_obj_set_y(ui_ImgLight2, -53);

    lv_obj_set_align(ui_ImgLight2, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_ImgLight2, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_ImgLight2, LV_OBJ_FLAG_SCROLLABLE);

}
void ui_Comm_screen_init(void)
{

    // ui_Comm

    ui_Comm = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Comm, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Panel1

    ui_Panel1 = lv_obj_create(ui_Comm);

    lv_obj_set_width(ui_Panel1, 255);
    lv_obj_set_height(ui_Panel1, 240);

    lv_obj_set_x(ui_Panel1, -94);
    lv_obj_set_y(ui_Panel1, -4);

    lv_obj_set_align(ui_Panel1, LV_ALIGN_CENTER);

    lv_obj_clear_flag(ui_Panel1, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Slider1

    ui_Slider1 = lv_slider_create(ui_Panel1);
    lv_slider_set_range(ui_Slider1, 0, 100);
    lv_slider_set_value(ui_Slider1, 40, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_Slider1) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Slider1, 0, LV_ANIM_OFF);

    lv_obj_set_width(ui_Slider1, 15);
    lv_obj_set_height(ui_Slider1, 160);

    lv_obj_set_x(ui_Slider1, -91);
    lv_obj_set_y(ui_Slider1, -6);

    lv_obj_set_align(ui_Slider1, LV_ALIGN_CENTER);

    lv_obj_set_style_radius(ui_Slider1, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Slider1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(ui_Slider1, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(ui_Slider1, &ui_img_indicator_ver_png, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_opa(ui_Slider1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    // ui_Label1

    ui_Label1 = lv_label_create(ui_Panel1);

    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label1, -90);
    lv_obj_set_y(ui_Label1, 87);

    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label1, "Bright");

    // ui_ArcTemp

    ui_ArcTemp = lv_arc_create(ui_Panel1);

    lv_obj_set_width(ui_ArcTemp, 100);
    lv_obj_set_height(ui_ArcTemp, 100);

    lv_obj_set_x(ui_ArcTemp, 33);
    lv_obj_set_y(ui_ArcTemp, -35);

    lv_obj_set_align(ui_ArcTemp, LV_ALIGN_CENTER);

    lv_arc_set_range(ui_ArcTemp, 10, 35);
    lv_arc_set_value(ui_ArcTemp, 26);
    lv_arc_set_bg_angles(ui_ArcTemp, 120, 60);

    lv_obj_add_event_cb(ui_ArcTemp, ui_event_ArcTemp, LV_EVENT_ALL, NULL);

    lv_obj_set_style_arc_color(ui_ArcTemp, lv_color_hex(0x4040FF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_ArcTemp, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_ArcTemp, lv_color_hex(0xF51111), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ArcTemp, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_ArcTemp, 1, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_ArcTemp, 1, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_ArcTemp, 1, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_ArcTemp, 1, LV_PART_KNOB | LV_STATE_DEFAULT);

    // ui_Panel3

    ui_Panel3 = lv_obj_create(ui_Panel1);

    lv_obj_set_width(ui_Panel3, 60);
    lv_obj_set_height(ui_Panel3, 60);

    lv_obj_set_x(ui_Panel3, 33);
    lv_obj_set_y(ui_Panel3, -35);

    lv_obj_set_align(ui_Panel3, LV_ALIGN_CENTER);

    lv_obj_clear_flag(ui_Panel3, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_radius(ui_Panel3, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Panel3, lv_color_hex(0x462F2F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Panel3, lv_color_hex(0x513D3D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_Panel3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_Panel3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Panel3, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LblTemp

    ui_LblTemp = lv_label_create(ui_Panel3);

    lv_obj_set_width(ui_LblTemp, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblTemp, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LblTemp, 0);
    lv_obj_set_y(ui_LblTemp, 0);

    lv_obj_set_align(ui_LblTemp, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LblTemp, "26°");

    // ui_LblTemp2

    ui_LblTemp2 = lv_label_create(ui_Panel1);

    lv_obj_set_width(ui_LblTemp2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblTemp2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LblTemp2, 32);
    lv_obj_set_y(ui_LblTemp2, 20);

    lv_obj_set_align(ui_LblTemp2, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LblTemp2, "Temperature");

    lv_obj_set_style_text_font(ui_LblTemp2, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Label4

    ui_Label4 = lv_label_create(ui_Panel1);

    lv_obj_set_width(ui_Label4, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label4, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label4, 0);
    lv_obj_set_y(ui_Label4, -108);

    lv_obj_set_align(ui_Label4, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label4, "IoT Remote Controller");

    // ui_Switch1

    ui_Switch1 = lv_switch_create(ui_Panel1);

    lv_obj_set_width(ui_Switch1, 40);
    lv_obj_set_height(ui_Switch1, 18);

    lv_obj_set_x(ui_Switch1, -26);
    lv_obj_set_y(ui_Switch1, 52);

    lv_obj_set_align(ui_Switch1, LV_ALIGN_CENTER);

    // ui_Switch2

    ui_Switch2 = lv_switch_create(ui_Panel1);

    lv_obj_set_width(ui_Switch2, 40);
    lv_obj_set_height(ui_Switch2, 18);

    lv_obj_set_x(ui_Switch2, -26);
    lv_obj_set_y(ui_Switch2, 83);

    lv_obj_set_align(ui_Switch2, LV_ALIGN_CENTER);

    // ui_LblL1

    ui_LblL1 = lv_label_create(ui_Panel1);

    lv_obj_set_width(ui_LblL1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblL1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LblL1, 20);
    lv_obj_set_y(ui_LblL1, 52);

    lv_obj_set_align(ui_LblL1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LblL1, "Light 1");

    lv_obj_set_style_text_font(ui_LblL1, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LblL2

    ui_LblL2 = lv_label_create(ui_Panel1);

    lv_obj_set_width(ui_LblL2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LblL2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LblL2, 20);
    lv_obj_set_y(ui_LblL2, 83);

    lv_obj_set_align(ui_LblL2, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LblL2, "Light 2");

    lv_obj_set_style_text_font(ui_LblL2, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // // ui_BtnSend1

    // ui_BtnSend1 = lv_btn_create(ui_Panel1);

    // lv_obj_set_width(ui_BtnSend1, 60);
    // lv_obj_set_height(ui_BtnSend1, 45);

    // lv_obj_set_x(ui_BtnSend1, 85);
    // lv_obj_set_y(ui_BtnSend1, 66);

    // lv_obj_set_align(ui_BtnSend1, LV_ALIGN_CENTER);

    // lv_obj_add_flag(ui_BtnSend1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    // lv_obj_clear_flag(ui_BtnSend1, LV_OBJ_FLAG_SCROLLABLE);

    // lv_obj_add_event_cb(ui_BtnSend1, ui_event_BtnSend1, LV_EVENT_ALL, NULL);
    // lv_obj_set_style_radius(ui_BtnSend1, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui_BtnSend1, lv_color_hex(0x750729), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(ui_BtnSend1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // // ui_LblSend

    // ui_LblSend = lv_label_create(ui_BtnSend1);

    // lv_obj_set_width(ui_LblSend, LV_SIZE_CONTENT);
    // lv_obj_set_height(ui_LblSend, LV_SIZE_CONTENT);

    // lv_obj_set_x(ui_LblSend, 0);
    // lv_obj_set_y(ui_LblSend, 0);

    // lv_obj_set_align(ui_LblSend, LV_ALIGN_CENTER);

    // lv_label_set_text(ui_LblSend, "Send");

    // lv_obj_set_style_text_font(ui_LblSend, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Panel2

    ui_Panel2 = lv_obj_create(ui_Comm);

    lv_obj_set_width(ui_Panel2, 166);
    lv_obj_set_height(ui_Panel2, 240);

    lv_obj_set_x(ui_Panel2, 137);
    lv_obj_set_y(ui_Panel2, -4);

    lv_obj_set_align(ui_Panel2, LV_ALIGN_CENTER);

    lv_obj_clear_flag(ui_Panel2, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Label2

    ui_Label2 = lv_label_create(ui_Panel2);

    lv_obj_set_width(ui_Label2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label2, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label2, 0);
    lv_obj_set_y(ui_Label2, -108);

    lv_obj_set_align(ui_Label2, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label2, "Chat");

    // ui_BtnSend2

    // ui_BtnSend2 = lv_btn_create(ui_Panel2);

    // lv_obj_set_width(ui_BtnSend2, 55);
    // lv_obj_set_height(ui_BtnSend2, 25);

    // lv_obj_set_x(ui_BtnSend2, 51);
    // lv_obj_set_y(ui_BtnSend2, 78);

    // lv_obj_set_align(ui_BtnSend2, LV_ALIGN_CENTER);

    // lv_obj_add_flag(ui_BtnSend2, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    // lv_obj_clear_flag(ui_BtnSend2, LV_OBJ_FLAG_SCROLLABLE);

    // lv_obj_add_event_cb(ui_BtnSend2, ui_event_BtnSend2, LV_EVENT_ALL, NULL);
    // lv_obj_set_style_radius(ui_BtnSend2, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(ui_BtnSend2, lv_color_hex(0x750729), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(ui_BtnSend2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LblSend1

    // ui_LblSend1 = lv_label_create(ui_BtnSend2);

    // lv_obj_set_width(ui_LblSend1, LV_SIZE_CONTENT);
    // lv_obj_set_height(ui_LblSend1, LV_SIZE_CONTENT);

    // lv_obj_set_x(ui_LblSend1, 0);
    // lv_obj_set_y(ui_LblSend1, 0);

    // lv_obj_set_align(ui_LblSend1, LV_ALIGN_CENTER);

    // lv_label_set_text(ui_LblSend1, "Send");

    // lv_obj_set_style_text_font(ui_LblSend1, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_TextArea1

    ui_TextArea1 = lv_textarea_create(ui_Panel2);

    lv_obj_set_width(ui_TextArea1, 150);
    lv_obj_set_height(ui_TextArea1, 147);

    lv_obj_set_x(ui_TextArea1, 0);
    lv_obj_set_y(ui_TextArea1, -20);

    lv_obj_set_align(ui_TextArea1, LV_ALIGN_CENTER);

    if("" == "") lv_textarea_set_accepted_chars(ui_TextArea1, NULL);
    else lv_textarea_set_accepted_chars(ui_TextArea1, "");

    lv_textarea_set_text(ui_TextArea1, "Me: Hi there!");

    lv_obj_clear_flag(ui_TextArea1, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_text_font(ui_TextArea1, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_TextArea1, lv_color_hex(0x91A821), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_TextArea1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_TextInput

    ui_TextInput = lv_textarea_create(ui_Panel2);

    lv_obj_set_width(ui_TextInput, 86);
    lv_obj_set_height(ui_TextInput, 27);

    lv_obj_set_x(ui_TextInput, -29);
    lv_obj_set_y(ui_TextInput, 78);

    lv_obj_set_align(ui_TextInput, LV_ALIGN_CENTER);

    if("" == "") lv_textarea_set_accepted_chars(ui_TextInput, NULL);
    else lv_textarea_set_accepted_chars(ui_TextInput, "");

    lv_textarea_set_text(ui_TextInput, "");
    lv_textarea_set_placeholder_text(ui_TextInput, "I say ...");

    lv_obj_add_event_cb(ui_TextInput, ui_event_TextInput, LV_EVENT_ALL, NULL);
    lv_obj_set_style_text_font(ui_TextInput, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_TextInput, lv_color_hex(0x91A821), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_TextInput, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_TextInput, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_TextInput, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_TextInput, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_TextInput, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Keyboard2

    ui_Keyboard2 = lv_keyboard_create(ui_Comm);

    lv_obj_set_width(ui_Keyboard2, 266);
    lv_obj_set_height(ui_Keyboard2, 118);

    lv_obj_set_x(ui_Keyboard2, -95);
    lv_obj_set_y(ui_Keyboard2, 61);

    lv_obj_set_align(ui_Keyboard2, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_Keyboard2, LV_OBJ_FLAG_HIDDEN);

    // ui_BtnChgScreen1

    ui_BtnChgScreen1 = lv_btn_create(ui_Comm);

    lv_obj_set_width(ui_BtnChgScreen1, 72);
    lv_obj_set_height(ui_BtnChgScreen1, 37);

    lv_obj_set_x(ui_BtnChgScreen1, 201);
    lv_obj_set_y(ui_BtnChgScreen1, -116);

    lv_obj_set_align(ui_BtnChgScreen1, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_BtnChgScreen1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_BtnChgScreen1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_BtnChgScreen1, ui_event_BtnChgScreen1, LV_EVENT_ALL, NULL);
    lv_obj_set_style_opa(ui_BtnChgScreen1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // POST CALLS
    lv_keyboard_set_textarea(ui_Keyboard2, ui_TextInput);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Main_screen_init();
    // ui_Comm_screen_init();
    lv_disp_load_scr(ui_Main);
}

void ui_zhenzhen1_value_set(int value)
{
    lv_img_set_angle(ui_ImgPtr1, value);
}

void ui_zhenzhen2_value_set(int value)
{
    lv_img_set_angle(ui_ImgPtr2, value);
}