
#include "nr_micro_shell.h"

extern void ui_init(void);
extern void ui_zhenzhen1_value_set(int value);
extern void ui_zhenzhen2_value_set(int value);

void lv_user_gui_init(void)
{
    ui_init();
}

static void aaa(char argc, char **argv)
{
    int value1 = -400;
    int value2 = -300;

    lv_user_gui_init();

    // while (1)
    // {
    //     lv_user_gui_init();
    //     ui_zhenzhen1_value_set(value1);
    //     ui_zhenzhen2_value_set(value2);

    //     value1 += 40;
    //     value2 += 40;

    //     if(value1 > 400) {
    //         value1 = -400;
    //     }

    //     if(value2 > 300) {
    //         value2 = -300;
    //     }

    //     osThreadSleep(200);
    // }
}
NR_SHELL_CMD_EXPORT(ppp, aaa);