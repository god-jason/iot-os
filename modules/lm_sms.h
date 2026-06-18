/*
@module  sms
@summary 短信模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     SMS
*/

#ifndef __IOT_SMS_H__
#define __IOT_SMS_H__

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

LUAMOD_API int luaopen_sms(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif /* __IOT_SMS_H__ */