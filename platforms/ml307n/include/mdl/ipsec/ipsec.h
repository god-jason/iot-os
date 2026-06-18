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

#ifndef LWIP_HDR_IPSEC_H
#define LWIP_HDR_IPSEC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lwip/ip.h"
#include "ipsec_sad.h"

#define IP_PROTO_ESP 50

#define XFRM_TYPE_REPLAY_PROTECT 2

enum ipsec_esp_mode{
    IPSEC_MODE_TRANSPORT,
    IPSEC_MODE_TUNNEL,
    IPSEC_MODE_UNKNOWN
};

struct xfrm_type{
    u8_t proto;
    u8_t flags;

    s32_t (*init_sa)(ipsec_sad_entry_t *sa);
    void (*destructor)(ipsec_sad_entry_t *sa);
    struct pbuf *(*input)(ipsec_sad_entry_t *sa, struct pbuf *p, u8_t *proto);
    struct pbuf *(*output)(ipsec_sad_entry_t *sa, struct pbuf *p, u8_t *proto);
    s32_t (*reject)(ipsec_sad_entry_t *sa, struct pbuf *p);
};

struct ipsec_afinfo
{
    u16_t family;

    const struct xfrm_type *type_esp;
#if 0
	const struct xfrm_type		*type_ipip;
	const struct xfrm_type		*type_ipip6;
	const struct xfrm_type		*type_comp;
	const struct xfrm_type		*type_ah;
	const struct xfrm_type		*type_routing;
	const struct xfrm_type		*type_dstopts;
  int			(*output)(struct pbuf* p);
#endif
};

void ipsec_init(void);

struct ipsec_afinfo *ipsec_get_afinfo(u16_t family);
s32_t ipsec_register_type(const struct xfrm_type *type, u16_t family);
s32_t ipsec_xfrm_init_sa(ipsec_sad_entry_t *sa, u8_t init_replay);

struct pbuf *ipsec_input(struct pbuf *q, struct ip_hdr *iphdr);
struct pbuf *ipsec6_input(struct pbuf *q, struct ip6_hdr *iphdr);
struct pbuf *ipsec_output(ipsec_sad_entry_t *ipsec_sa, struct pbuf *q, const ip_addr_t *src_ip, const ip_addr_t *dst_ip,
                          u8_t *ip_proto, u16_t src_port, u16_t dst_port);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_IPSEC_H */
