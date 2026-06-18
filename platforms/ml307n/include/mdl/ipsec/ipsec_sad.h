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
 * Header file for Security Associations (SA) Databse (SAD) related Macro, Data Structures and APIs.
 */

#ifndef LWIP_HDR_IPSEC_SAD_H
#define LWIP_HDR_IPSEC_SAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ipsec_spd.h"

/**
  * Algorithms IDs base on RFC4301 Appendix C.
  * @defgroup algid
*/
/** Invalid integrity algorithm. @ingroup algid */
#define SAD_AALG_NONE            0
/** HMAC MD5 integrity algorithm. Refer RFC2403. @ingroup algid */
#define SAD_AALG_MD5HMAC         2
/** HMAC SHA1 integrity algorithm. Refer RFC2403. @ingroup algid */
#define SAD_AALG_SHA1HMAC        3
#define SAD_X_AALG_SHA2_256HMAC  5
#define SAD_X_AALG_SHA2_384HMAC  6
#define SAD_X_AALG_SHA2_512HMAC  7
#define SAD_X_AALG_RIPEMD160HMAC 8
#define SAD_X_AALG_AES_XCBC_MAC  9
#define SAD_X_AALG_SM3_256HMAC   10
#define SAD_X_AALG_NULL          251
#define SAD_AALG_MAX             251

/** Invalid encryption algorithm. @ingroup algid */
#define SAD_EALG_NONE 0
#define SAD_EALG_DESCBC 2
/** DES-EDE3-CBC encryption algorithm. Refer RFC2451. @ingroup algid */
#define SAD_EALG_3DESCBC 3
#define SAD_X_EALG_CASTCBC 6
#define SAD_X_EALG_BLOWFISHCBC 7
/** NULL encryption algorithm. Refer RFC2401. @ingroup algid */
#define SAD_EALG_NULL 11
/** AES-CBC encryption algorithm. Refer RFC3602. @ingroup algid */
#define SAD_X_EALG_AESCBC 12
#define SAD_X_EALG_AESCTR 13
#define SAD_X_EALG_AES_CCM_ICV8 14
#define SAD_X_EALG_AES_CCM_ICV12 15
#define SAD_X_EALG_AES_CCM_ICV16 16
#define SAD_X_EALG_AES_GCM_ICV8 18
#define SAD_X_EALG_AES_GCM_ICV12 19
#define SAD_X_EALG_AES_GCM_ICV16 20
#define SAD_X_EALG_CAMELLIACBC 22
#define SAD_X_EALG_NULL_AES_GMAC 23
#define SAD_X_EALG_SM4CBC 24
#define SAD_EALG_MAX 253 /* last EALG */

/**
  * SAD operation error code
  * @defgroup saderrcode
*/
/** Can't find the record. @ingroup saderrcode */
#define IPSEC_SAD_NO_RECORD                -1
/** Operation success. @ingroup saderrcode */
#define IPSEC_SAD_NO_ERROR                  0
/** Invalid sa entry. e.g sa pointer is NULL. @ingroup saderrcode */
#define IPSEC_SAD_SA_STRUCT_ERROR           1
/** Unsupport integrity protect algorithm. @ingroup saderrcode */
#define IPSEC_SAD_AALG_ID_ERROR             2
/** Unsupport encryption algorithm. @ingroup saderrcode */
#define IPSEC_SAD_EALG_ID_ERROR             3
#if IPSEC_OUTBOUND_SOCKET_SAD_SUPPORT
#define IPSEC_SAD_SOCKET_STRUCT_ERROR       4
#define IPSEC_SAD_SOCKET_TYPE_ERROR         5
#define IPSEC_SAD_SOCKET_TCP_ERROR          6
#define IPSEC_SAD_SOCKET_UDP_ERROR          7
#endif

/**
 * Struct for replay window record.
 */
struct ipsec_replay_state{
    /** Outbound sequence number. */
    u32_t oseq;
    /** Inbound sequence number. */
    u32_t seq;
    /** Bitmap for tracking inbound sequence number. */
    u32_t bitmap;
};

/**
 * Struct for integrity protect algorithm.
 */
struct ipsec_algo_auth{
    /** Integrity protect algorithm name. */
    u8_t alg_name[64];
    /** Integrity protect algorithm key length in bit. */
    u32_t alg_key_len;
    /** Integrity protect algorithm truncted key length in bit. */
    u32_t alg_trunc_len;
    /** Integrity protect algorithm key value. */
    u8_t alg_key[32];
};

/**
 * Struct for encryption algorithm.
 */
struct ipsec_algo{
    /** Encryption algorithm name. */
    u8_t alg_name[64];
    /** Encryption algorithm key length in bit. */
    u32_t alg_key_len;
    /** Encryption algorithm key value. */
    u8_t alg_key[32];
};

/**
 * Struct for Security Associations (SA) Databse (SAD) entry.
 */
typedef struct ipsec_sad_entry{
    /** Security Parameters Index (SPI) value. a 32-bit
      * value selected by the receiving end of an SA to
      * uniquely identify the SA. Refer RFC4301.
      */
    u32_t spi;
    /** Encrypt or Decrypt protocol. E.g. ESP refer RFC4303
      * or AH refer RFC4302. */
    u8_t proto;
    /** IPsec mode - tunnel or transport. Refer RFC4301.*/
    u8_t mode;
    /** Address family. AF_INET or AF_INET6 */
    u16_t family;
    #if 0
    /** Selector to match the traffic. It is 5 tuples. */
    ipsec_selector_t spd_selector;
    /** Lifetime of this SA - a time interval after which an
      * SA must be replaced with a new SA (and new SPI) or
      * terminated, plus an indication of which of these actions
      * should occur.
      */
    ipsec_lifetime_cfg_t lft_cfg;
    /** Current lifetime. */
    ipsec_lifetime_cur_t lft_cur;
    #endif
    /** Integrity protect algorithm id. Refer @ref algid. */
    u8_t aalg_id;
    /** Encryption algorithm id. Refer @ref algid. */
    u8_t ealg_id;
    /** Detailed integrity protect algorithm info. Refer @ref struct ipsec_algo_auth. */
    struct ipsec_algo_auth aalg;
    /** Detailed encryption algorithm info. Refer @ref struct ipsec_algo. */
    struct ipsec_algo ealg;
    /** Encryption and decryption operation entry. */
    const struct xfrm_type *xfrm_type;
    /* Replay detection mode. For future extend */
    /* enum ipsec_replay_mode    repl_mode; */
    /** Replay window record to determine whether an
      * inbound AH or ESP packet is a replay.
      */
    struct ipsec_replay_state replay;
    /** Replay window size. default is 32. */
    u8_t replay_window;
} ipsec_sad_entry_t;

s32_t ipsec_sad_init(void);
ipsec_sad_entry_t *ipsec_sad_search_out(ipsec_sa_sel_t sa_sel);
ipsec_sad_entry_t *ipsec_sad_search_in(u32_t spi, u8_t proto, u16_t af);
s32_t ipsec_sad_add(u32_t spi, u8_t proto, u8_t mode, u16_t af, u8_t aalg_id,
                    const unsigned char ik[64], u8_t ealg_id, const unsigned char ck[64]);
s32_t ipsec_sa_socket_add(int socket, u32_t spi, u8_t proto, u8_t mode, u16_t af, u8_t aalg_id,
                    const unsigned char ik[64], u8_t ealg_id, const unsigned char ck[64]);
s32_t ipsec_sa_database_add(u32_t spi, u8_t proto, u8_t mode, u16_t af, u8_t aalg_id,
                    const unsigned char ik[64], u8_t ealg_id, const unsigned char ck[64]);
s32_t ipsec_sad_update(void);
s32_t ipsec_sad_del(u32_t spi, u8_t proto, u8_t mode, u16_t af);
s32_t ipsec_sa_socket_del(int socket);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_IPSEC_SAD_H */
