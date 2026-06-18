/*************************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the
 * License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License  for the specific
 *  language governing permissions and limitations under the License.
 *
 ************************************************************************************************************************/

/**
 * @file
 *
 * Header file for Security Policy (SP) Databse (SPD) related Macro, Data Structures and APIs.
 */

#ifndef LWIP_HDR_IPSEC_SPD_H
#define LWIP_HDR_IPSEC_SPD_H

#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Direction of packet from IPsec boundary view.
 */
enum ipsec_spd_direction {
    IPSEC_SPD_INBOUND,
    IPSEC_SPD_OUTBOUND,
    IPSEC_SPD_MAX
};

/**
 * Action of matched traffic.
 */
enum ipsec_spd_action {
    /** Let the packets pass. */
    IPSEC_SPD_BYPASS,
    /** Encrypt or decrypt the packets. */
    IPSEC_SPD_PROTECTED,
    /** Drop the packets. */
    IPSEC_SPD_DISCARD
};

/**
 * Struct for SP record selector. It is 5 tuples.
 */
typedef struct ipsec_selector {
    /** Remote IP address. */
    ip_addr_t daddr;
    /** Local IP address. */
    ip_addr_t saddr;
    /** Remote Port. */
    u16_t dport;
    /** Local Port. */
    u16_t sport;
    /** Protocol. */
    u8_t proto;
} ipsec_selector_t;

#if 0
typedef struct ipsec_lifetime_cfg {
    u64_t soft_byte_limit;
    u64_t hard_byte_limit;
    u64_t soft_packet_limit;
    u64_t hard_packet_limit;
    u64_t soft_add_expires_sec;
    u64_t hard_add_expires_sec;
    u64_t soft_use_expires_sec;
    u64_t hard_use_expires_sec;
} ipsec_lifetime_cfg_t;

typedef struct ipsec_lifetime_cur {
    u64_t bytes;
    u64_t packets;
    u64_t add_time;
    u64_t use_time;
} ipsec_lifetime_cur_t;
#endif

typedef struct ipsec_sa_sel {
    /** Address family. AF_INET or AF_INET6. */
    u16_t family;
    /** Security Parameters Index (SPI) value. a 32-bit
      * value selected by the receiving end of an SA to
      * uniquely identify the SA. Refer RFC4301.
      */
    u32_t spi;
    /** Encrypt or Decrypt protocol. E.g. ESP refer RFC4303
      * or AH refer RFC4302. */
    u8_t proto;
    /** IPsec mode - tunnel or transport. Refer RFC4301. */
    u8_t mode;
    /** Integrity protect algorithm. Bitmap for future use. */
    u32_t aalgos;
    /** Encryption algorithm. Bitmap for future use. */
    u32_t ealgos;
} ipsec_sa_sel_t;

typedef struct ipsec_spd_entry {
    /** Address family. AF_INET or AF_INET6. */
    u16_t family;
    /** SP record selector. Refer @ref ipsec_selector_t. */
    ipsec_selector_t spd_selector;
    #if 0
    ipsec_lifetime_cfg_t lft_cfg;
    ipsec_lifetime_cur_t lft_cur;
    #endif
    /** Action for the traffic. Refer @ref ipsec_spd_action. */
    u8_t action;
    /** Mark the record is invalid. */
    u8_t dead;
    /** SA record selector. It is only for outbound SAD search. Refer @ref ipsec_sa_sel_t. */
    ipsec_sa_sel_t sa_sel;
} ipsec_spd_entry_t;

s32_t ipsec_spd_init(void);
s32_t ipsec_spd_check(const ip_addr_t *src, const ip_addr_t *dest, u8_t proto, u16_t src_port, u16_t dst_port, u8_t direction, ipsec_spd_entry_t *sp);
s32_t ipsec_spd_add(u8_t dir, u16_t family, ip_addr_t *daddr, ip_addr_t *saddr, u16_t dport, u16_t sport, u8_t proto, u8_t encryproto, u32_t spi, u8_t mode);
s32_t ipsec_spd_update(void);
s32_t ipsec_spd_del(u8_t dir, ip_addr_t *daddr, ip_addr_t *saddr, u16_t dport, u16_t sport, u8_t proto);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_IPSEC_SPD_H */
