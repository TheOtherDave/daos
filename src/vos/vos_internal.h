/**
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * GOVERNMENT LICENSE RIGHTS-OPEN SOURCE SOFTWARE
 * The Government's rights to use, modify, reproduce, release, perform, display,
 * or disclose this software are subject to the terms of the LGPL License as
 * provided in Contract No. B609815.
 * Any reproduction of computer software, computer software documentation, or
 * portions thereof marked with this legend must also reproduce the markings.
 *
 * (C) Copyright 2016 Intel Corporation.
 */
/**
 * Layout definition for VOS root object
 * vos/include/vos_internal.h
 *
 * Author: Vishwanath Venkatesan <vishwanath.venkatesan@intel.com>
 */
#ifndef _VOS_INTERNAL_H
#define _VOS_INTERNAL_H

#include <daos/daos_list.h>
#include <daos/daos_hash.h>
#include <daos/daos_btree.h>

#include "vos_layout.h"

/**
 * VOS object, assume all objects are KV store...
 *
 * NB: PMEM data structure.
 */
struct vos_obj {
	daos_unit_oid_t			vo_oid;
	/** btree root */
	struct btr_root			vo_kv_btr;
	/**
	 * TODO: link it to the container object table
	 */
};

/**
 * Reference of a cached object.
 *
 * NB: DRAM data structure.
 */
struct vos_obj_ref {
	/** TODO: link it to object cache lru and hash table */

	/** Key for searching, container uuid */
	uuid_t				 or_co_uuid;
	/** Key for searching, object ID within a container */
	daos_unit_oid_t			 or_oid;
	/** btree open handle of the object */
	daos_handle_t			 or_toh;
	/** Persistent memory ID for the object */
	struct vos_obj			*or_obj;
};

/**
 * percpu object cache. It can include a hash table and a LRU for
 * cached objects.
 *
 * This structure is not exported (move to TLS).
 */
struct vos_obj_cache;

/**
 * Find an object in the cache \a occ and take its reference. If the object is
 * not in cache, this function will load it from PMEM pool or create it, then
 * add it to the cache.
 *
 * \param occ	[IN]	Object cache, it could be a percpu data structure.
 * \param coh	[IN]	Container open handle.
 * \param oid	[IN]	VOS object ID.
 * \param oref_p [OUT]	Returned object cache reference.
 */
int  vos_obj_ref_hold(struct vos_obj_cache *occ, daos_handle_t coh,
		      daos_unit_oid_t oid, struct vos_obj_ref **oref_p);

/**
 * Release the object cache reference.
 *
 * \param oref	[IN]	Reference to be released.
 */
void vos_obj_ref_release(struct vos_obj_cache *occ, struct vos_obj_ref *oref);

/**
 * Create an object cache.
 *
 * \param occ_p	[OUT]	Newly created cache.
 */
int  vos_obj_cache_create(struct vos_obj_cache **occ_p);

/**
 * Destroy an object cache, and release all cached object references.
 *
 * \param occ	[IN]	Cache to be destroyed.
 */
void vos_obj_cache_destroy(struct vos_obj_cache *occ);

/**
 * VOS pool handle
 */
struct vos_pool {
	struct daos_hlink	vpool_hlink;
	PMEMobjpool		*ph;
	char			*path;
};

struct vos_co_hdl {
	struct daos_hlink        co_hdl_hlink;
	PMEMobjpool		*ph;
	uuid_t			 vc_co_id;
	struct vos_object_table	*obj_table;
	struct vos_epoch_table	*epoch_table;
};

/*
 * Handle hash globals
 **/
struct daos_hhash	*daos_vos_hhash;

int
vos_create_hhash(void);

struct vos_pool*
vos_pool_lookup_handle(daos_handle_t poh);

/* Not yet implemented */
struct vos_pool*
vos_co_lookup_handle(daos_handle_t poh);


#endif