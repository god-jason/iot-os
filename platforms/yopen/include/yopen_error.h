/*==========================================================================
 |
 |--------------------------------------------------------------------------
 |
 | File Description
 | ----------------
 |      Error Code Definition
 |
 \=========================================================================*/

#ifndef __YOPEN_ERROR_H__
#define __YOPEN_ERROR_H__
/****************************************************************************
 * Error Code Definition
 ***************************************************************************/
enum {
    YOPEN_RET_OK           = 0,
    YOPEN_RET_ERR_PARAM = -1,
    YOPEN_RET_ERR_PORT_NOT_OPEN = -2,
    YOPEN_RET_ERR_INVALID_TIMER = -3,
    YOPEN_RET_ERR_FATAL = -4,
    YOPEN_RET_ERR_NOSUPPORT      = -5,
    YOPEN_RET_ERR_NOSUPPORT_INISR      = -6,
    YOPEN_RET_ERR_INVALID_PARAMETER    = -7,
    YOPEN_RET_ERR_MEM_FULL             = -8,
    YOPEN_RET_ERR_BUSY                 = -9,

};

#endif
// End-of YOPEN_ERROR_H
