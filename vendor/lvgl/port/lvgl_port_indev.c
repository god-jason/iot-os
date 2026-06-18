/**
 * @file lv_port_indev_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl_port_indev.h"
#include "lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;
lv_indev_t * indev_keypad;
lv_indev_t * indev_button;

/**********************
 *      MACROS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/
 

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    InputDevice_t *pInput = indev_drv->user_data;

    return pInput->read(pInput, data);
    
    #if 0
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /*Save the pressed coordinates and the state*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;
    #endif
}

/*------------------
 * Keypad
 * -----------------*/

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    InputDevice_t *pInput = indev_drv->user_data;

    return pInput->read(pInput, data);

    #if 0    
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_NEXT;
                break;
            case 2:
                act_key = LV_KEY_PREV;
                break;
            case 3:
                act_key = LV_KEY_LEFT;
                break;
            case 4:
                act_key = LV_KEY_RIGHT;
                break;
            case 5:
                act_key = LV_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
    #endif
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    InputDevice_t *pInput = indev_drv->user_data;

    return pInput->read(pInput, data);
        
#if 0
    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
#endif
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void LVGL_IndevInit(InputDevice_t *touch, InputDevice_t *keypad, InputDevice_t *button)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    if(touch)
    {
        /*------------------
         * Touchpad
         * -----------------*/
        static lv_indev_drv_t indevTouchDrv;

        /*Initialize your touchpad if you have*/
        if(touch->init)
        {
            touch->init(touch);
        }

        /*Register a touchpad input device*/
        lv_indev_drv_init(&indevTouchDrv);
        indevTouchDrv.type = LV_INDEV_TYPE_POINTER;
        indevTouchDrv.read_cb = touchpad_read;
        indevTouchDrv.user_data = touch;
        indev_touchpad = lv_indev_drv_register(&indevTouchDrv);
    }
#if 0
    /*------------------
     * Mouse
     * -----------------*/

    /*Initialize your mouse if you have*/
    mouse_init();

    /*Register a mouse input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
    indev_mouse = lv_indev_drv_register(&indev_drv);

    /*Set cursor. For simplicity set a HOME symbol now.*/
    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
    lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
    lv_indev_set_cursor(indev_mouse, mouse_cursor);
#endif
    if(keypad)
    {        
        /*------------------
         * Keypad
         * -----------------*/
        static lv_indev_drv_t indevKeyDrv;
        
        /*Initialize your keypad or keyboard if you have*/
        if(keypad->init)
        {
            keypad->init(touch);
        }
        
        /*Register a keypad input device*/
        lv_indev_drv_init(&indevKeyDrv);
        indevKeyDrv.type = LV_INDEV_TYPE_KEYPAD;
        indevKeyDrv.read_cb = keypad_read;
        indevKeyDrv.user_data = keypad;
        indev_keypad = lv_indev_drv_register(&indevKeyDrv);
        
        /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
         *add objects to the group with `lv_group_add_obj(group, obj)`
         *and assign this input device to group to navigate in it:
         *`lv_indev_set_group(indev_keypad, group);`*/
    }
#if 0
    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/
#endif
    if(button)
    {
        /*------------------
         * Button
         * -----------------*/

        static lv_indev_drv_t indevButtonDrv;
        
        /*Initialize your button if you have*/
        if(button->init)
        {
            button->init(touch);
        }
        
        /*Register a button input device*/
        lv_indev_drv_init(&indevButtonDrv);
        indevButtonDrv.type = LV_INDEV_TYPE_BUTTON;
        indevButtonDrv.read_cb = button_read;
        indevButtonDrv.user_data = button;
        indev_button = lv_indev_drv_register(&indevButtonDrv);
        
        /*Assign buttons to points on the screen*/
        static const lv_point_t btn_points[2] = {
            {10, 10},   /*Button 0 -> x:10; y:10*/
            {40, 100},  /*Button 1 -> x:40; y:100*/
        };
        lv_indev_set_button_points(indev_button, btn_points);        
    }
}

