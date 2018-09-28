/*
 * IKE modular algorithm handling interface, for libreswan
 *
 * Copyright (C) 2016 Andrew Cagney <cagney@gnu.org>
 * Copyright (C) 2018 Paul Woutets <pwouters@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <https://www.gnu.org/licenses/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#ifdef USE_DH2
extern const struct oakley_group_desc oakley_group_modp1024;
#endif
extern const struct oakley_group_desc oakley_group_modp1536;
extern const struct oakley_group_desc oakley_group_modp2048;
extern const struct oakley_group_desc oakley_group_modp3072;
extern const struct oakley_group_desc oakley_group_modp4096;
extern const struct oakley_group_desc oakley_group_modp6144;
extern const struct oakley_group_desc oakley_group_modp8192;
extern const struct oakley_group_desc oakley_group_dh19;
extern const struct oakley_group_desc oakley_group_dh20;
extern const struct oakley_group_desc oakley_group_dh21;
#ifdef USE_DH22
extern const struct oakley_group_desc oakley_group_dh22;
#endif
#ifdef USE_DH23
extern const struct oakley_group_desc oakley_group_dh23;
#endif
#ifdef USE_DH24
extern const struct oakley_group_desc oakley_group_dh24;
#endif
#ifdef USE_DH31
extern const struct oakley_group_desc oakley_group_dh31;
#endif

/*
 * IKEv2 RFC 7296 uses the term "NONE" when referring to no DH
 * algorithm.
 */
extern const struct oakley_group_desc ike_alg_dh_none;
