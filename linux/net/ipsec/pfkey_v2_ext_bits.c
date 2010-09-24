/*
 * RFC2367 PF_KEYv2 Key management API message parser
 * Copyright (C) 1999, 2000, 2001  Richard Guy Briggs.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 */

/*
 *		Template from klips/net/ipsec/ipsec/ipsec_parse.c.
 */

/*
 * Some ugly stuff to allow consistent debugging code for use in the
 * kernel and in user space
*/

#if defined(__KERNEL__) && (defined(linux) || defined(__linux__))

# include <linux/kernel.h>  /* for printk */

# include "openswan/ipsec_kversion.h" /* for malloc switch */
# ifdef MALLOC_SLAB
#  include <linux/slab.h> /* kmalloc() */
# else /* MALLOC_SLAB */
#  include <linux/malloc.h> /* kmalloc() */
# endif /* MALLOC_SLAB */
# include <linux/errno.h>  /* error codes */
# include <linux/types.h>  /* size_t */
# include <linux/interrupt.h> /* mark_bh */

# include <linux/netdevice.h>   /* struct device, and other headers */
# include <linux/etherdevice.h> /* eth_type_trans */
# include <linux/ip.h>          /* struct iphdr */ 
# if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
#  include <linux/ipv6.h>
# endif /* defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE) */

#else /* __KERNEL__ */

# include <sys/types.h>
# include <sys/errno.h>
# include <stdio.h>
#endif

#include <openswan.h>
#include <openswan/pfkeyv2.h>
#include <openswan/pfkey.h>

#include "openswan/pfkey_debug.h"


pfkey_ext_track extensions_bitmaps[2/*in/out*/][2/*perm/req*/][K_SADB_MAX+1]={

/* INBOUND EXTENSIONS */
{

/* PERMITTED IN */
{
/* K_SADB_RESERVED */
0
,
/* SADB_GETSPI */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_SPIRANGE
,
/* SADB_UPDATE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
,
/* SADB_ADD */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<K_SADB_X_EXT_NAT_T_TYPE
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_OA
| 1ULL<<K_SADB_X_EXT_PLUMBIF
| 1ULL<<K_SADB_X_EXT_SAREF
,
/* SADB_DELETE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_GET */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_ACQUIRE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
,
/* K_SADB_REGISTER */
1ULL<<K_SADB_EXT_RESERVED
,
/* K_SADB_EXPIRE */
0
,
/* K_SADB_FLUSH */
1ULL<<K_SADB_EXT_RESERVED
,
/* K_SADB_DUMP */
1ULL<<K_SADB_EXT_RESERVED
,
/* SADB_X_PROMISC */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_ADDFLOW */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<K_SADB_X_EXT_PROTOCOL
,
/* SADB_X_DELFLOW */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<K_SADB_X_EXT_PROTOCOL
,
/* SADB_X_DEBUG */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_DEBUG
,
/* SADB_X_NAT_T_NEW_MAPPING */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
,
/* SADB_X_PLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
,
/* SADB_X_UNPLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
},

/* REQUIRED IN */
{
/* K_SADB_RESERVED */
0
,
/* K_SADB_GETSPI */
1ULL<<K_SADB_EXT_RESERVED
| 1ULL<<K_SADB_EXT_ADDRESS_SRC
| 1ULL<<K_SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_EXT_SPIRANGE
,
/* K_SADB_UPDATE */
1ULL<<K_SADB_EXT_RESERVED
| 1ULL<<K_SADB_EXT_SA
| 1ULL<<K_SADB_EXT_ADDRESS_SRC
| 1ULL<<K_SADB_EXT_ADDRESS_DST
/*| 1ULL<<K_SADB_EXT_KEY_AUTH*/
/*| 1ULL<<K_SADB_EXT_KEY_ENCRYPT*/
,
/* K_SADB_ADD */
1ULL<<K_SADB_EXT_RESERVED
| 1ULL<<K_SADB_EXT_SA
| 1ULL<<K_SADB_EXT_ADDRESS_SRC
| 1ULL<<K_SADB_EXT_ADDRESS_DST
/*| 1ULL<<K_SADB_EXT_KEY_AUTH*/
/*| 1ULL<<K_SADB_EXT_KEY_ENCRYPT*/
,
/* K_SADB_DELETE */
1ULL<<K_SADB_EXT_RESERVED
| 1ULL<<K_SADB_EXT_SA
| 1ULL<<K_SADB_EXT_ADDRESS_SRC
| 1ULL<<K_SADB_EXT_ADDRESS_DST
,
/* K_SADB_GET */
1ULL<<K_SADB_EXT_RESERVED
| 1ULL<<K_SADB_EXT_SA
| 1ULL<<K_SADB_EXT_ADDRESS_SRC
| 1ULL<<K_SADB_EXT_ADDRESS_DST
,
/* K_SADB_ACQUIRE */
1ULL<<K_SADB_EXT_RESERVED
| 1ULL<<K_SADB_EXT_ADDRESS_SRC
| 1ULL<<K_SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_EXT_PROPOSAL
,
/* K_SADB_REGISTER */
1ULL<<K_SADB_EXT_RESERVED
,
/* K_SADB_EXPIRE */
0
,
/* K_SADB_FLUSH */
1ULL<<K_SADB_EXT_RESERVED
,
/* K_SADB_DUMP */
1ULL<<K_SADB_EXT_RESERVED
,
/* SADB_X_PROMISC */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_DST
/*| 1ULL<<K_SADB_X_EXT_SATYPE2*/
/*| 1ULL<<K_SADB_X_EXT_SA2*/
/*| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2*/
,
/* SADB_X_ADDFLOW */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
,
/* SADB_X_DELFLOW */
1ULL<<SADB_EXT_RESERVED
/*| 1ULL<<SADB_EXT_SA*/
#if 0 /* SADB_X_CLREROUTE doesn't need all these... */
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DEBUG */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_DEBUG
,
/* SADB_X_NAT_T_NEW_MAPPING */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
,
/* SADB_X_PLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
,
/* SADB_X_UNPLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
}

},

/* OUTBOUND EXTENSIONS */
{

/* PERMITTED OUT */
{
/* K_SADB_RESERVED */
0
,
/* SADB_GETSPI */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_UPDATE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
,
/* SADB_ADD */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<K_SADB_X_EXT_NAT_T_TYPE
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_OA
| 1ULL<<K_SADB_X_EXT_PLUMBIF
| 1ULL<<K_SADB_X_EXT_SAREF
,
/* SADB_DELETE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_GET */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<K_SADB_X_EXT_NAT_T_TYPE
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_OA
,
/* SADB_ACQUIRE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
,
/* SADB_REGISTER */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
,
/* SADB_EXPIRE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_FLUSH */
1ULL<<SADB_EXT_RESERVED
,
/* SADB_DUMP */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<K_SADB_X_EXT_NAT_T_TYPE
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_OA
,
/* SADB_X_PROMISC */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_ADDFLOW */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
| 1ULL<<K_SADB_X_EXT_PROTOCOL
,
/* SADB_X_DELFLOW */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
| 1ULL<<K_SADB_X_EXT_PROTOCOL
,
/* SADB_X_DEBUG */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_DEBUG
,
/* SADB_X_NAT_T_NEW_MAPPING */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
,
/* SADB_X_PLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
,
/* SADB_X_UNPLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
},

/* REQUIRED OUT */
{
/* K_SADB_RESERVED */
0
,
/* SADB_GETSPI */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_UPDATE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_ADD */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_DELETE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_GET */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
/* | 1ULL<<SADB_EXT_KEY_AUTH */
/* | 1ULL<<SADB_EXT_KEY_ENCRYPT */
,
/* SADB_ACQUIRE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_PROPOSAL
,
/* SADB_REGISTER */
1ULL<<SADB_EXT_RESERVED
/* | 1ULL<<SADB_EXT_SUPPORTED_AUTH
   | 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT */
,
/* SADB_EXPIRE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
/* | 1ULL<<SADB_EXT_LIFETIME_HARD
   | 1ULL<<SADB_EXT_LIFETIME_SOFT */
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_FLUSH */
1ULL<<SADB_EXT_RESERVED
,
/* SADB_DUMP */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
,
/* SADB_X_PROMISC */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_LIFETIME_CURRENT
| 1ULL<<SADB_EXT_LIFETIME_HARD
| 1ULL<<SADB_EXT_LIFETIME_SOFT
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<SADB_EXT_ADDRESS_PROXY
| 1ULL<<SADB_EXT_KEY_AUTH
| 1ULL<<SADB_EXT_KEY_ENCRYPT
| 1ULL<<SADB_EXT_IDENTITY_SRC
| 1ULL<<SADB_EXT_IDENTITY_DST
| 1ULL<<SADB_EXT_SENSITIVITY
| 1ULL<<SADB_EXT_PROPOSAL
| 1ULL<<SADB_EXT_SUPPORTED_AUTH
| 1ULL<<SADB_EXT_SUPPORTED_ENCRYPT
| 1ULL<<SADB_EXT_SPIRANGE
| 1ULL<<K_SADB_X_EXT_KMPRIVATE
| 1ULL<<K_SADB_X_EXT_SATYPE2
| 1ULL<<K_SADB_X_EXT_SA2
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_DST
,
/* SADB_X_ADDFLOW */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
,
/* SADB_X_DELFLOW */
1ULL<<SADB_EXT_RESERVED
/*| 1ULL<<SADB_EXT_SA*/
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_FLOW
| 1ULL<<K_SADB_X_EXT_ADDRESS_SRC_MASK
| 1ULL<<K_SADB_X_EXT_ADDRESS_DST_MASK
,
/* SADB_X_DEBUG */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_DEBUG
,
/* SADB_X_NAT_T_NEW_MAPPING */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<SADB_EXT_SA
| 1ULL<<SADB_EXT_ADDRESS_SRC
| 1ULL<<SADB_EXT_ADDRESS_DST
| 1ULL<<K_SADB_X_EXT_NAT_T_SPORT
| 1ULL<<K_SADB_X_EXT_NAT_T_DPORT
,
/* SADB_X_PLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
,
/* SADB_X_UNPLUMBIF */
1ULL<<SADB_EXT_RESERVED
| 1ULL<<K_SADB_X_EXT_PLUMBIF
}
}
};

int pfkey_required_extension(enum pfkey_ext_required inout,
			     enum sadb_msg_t sadb_operation,
			     enum sadb_extension_t exttype)
{
	return (extensions_bitmaps[inout][EXT_BITS_REQ][sadb_operation] & (1ULL<<exttype)) != 0;
}

int pfkey_permitted_extension(enum pfkey_ext_required inout,
			      enum sadb_msg_t sadb_operation,
			      enum sadb_extension_t exttype)
{
	return (extensions_bitmaps[inout][EXT_BITS_PERM][sadb_operation] & (1ULL<<exttype)) != 0;
}


int pfkey_extensions_missing(enum pfkey_ext_required inout,
			     enum sadb_msg_t sadb_operation,
			     pfkey_ext_track extensions_seen)

{
	pfkey_ext_track req = extensions_bitmaps[inout][EXT_BITS_REQ][sadb_operation];

	if((extensions_seen & req) != req) {
		ERROR("extensions for op: %d seen: %08llx required %08llx, missing: %08llx\n",
		      sadb_operation,
		      (unsigned long long)extensions_seen,
		      (unsigned long long)req,
		      (unsigned long long)req & ~(req & extensions_seen));
		return 1;
	}
	return 0;
}

/*
 *
 * Local Variables:
 * c-file-style: "linux"
 * End:
 *
 */
