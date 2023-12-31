/*
 ****************************************************************************************
 * icom_timer_api.h
 *
 * Author    : Biju Nair
 * Ver       : 1.0
 * Date      : 5-Feb-2020
 *
 * Copyright Infinicomm Solutions Pvt Ltd, 2019
 *
 ****************************************************************************************
 */

#ifndef _ICOM_TIMER_API_H_
#define _ICOM_TIMER_API_H_

typedef int    (* ICOM_TIMER_CALLBACK)(void *p);

typedef struct _icon_timer_hndl_
{
    unsigned short int    timer_id;
    esp_timer_handle_t    poll_timer;
    unsigned int          timeout;
    unsigned char         repeat_flag;
    ICOM_TIMER_CALLBACK   p_callback; 
    void                  *p_callback_context;
} ICOM_TIMER_HNDL;

#define MODBUS_TIMER_ID                      0x01
#define HEALTH_PING_TIMER_ID                 0x02

extern ICOM_TIMER_HNDL *icom_create_timer(unsigned short int timer_id, 
                                          ICOM_TIMER_CALLBACK *p,
                                          void *p_callback_context);
extern int icom_start_timer(ICOM_TIMER_HNDL *p_timer, unsigned int timeout, unsigned char repeat);

#endif

