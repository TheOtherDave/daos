/**
 * (C) Copyright 2016 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * GOVERNMENT LICENSE RIGHTS-OPEN SOURCE SOFTWARE
 * The Government's rights to use, modify, reproduce, release, perform, display,
 * or disclose this software are subject to the terms of the Apache License as
 * provided in Contract No. B609815.
 * Any reproduction of computer software, computer software documentation, or
 * portions thereof marked with this legend must also reproduce the markings.
 */
#ifndef __DAOS_OBJECT_H__
#define __DAOS_OBJECT_H__

#include <daos/common.h>
#include <daos/tse.h>
#include <daos_types.h>
#include <daos_api.h>

/** object metadata stored in the global OI table of container */
struct daos_obj_md {
	daos_obj_id_t		omd_id;
	uint32_t		omd_ver;
	uint32_t		omd_padding;
	union {
		uint32_t	omd_split;
		uint64_t	omd_loff;
	};
};

/** object shard metadata stored in each contianer shard */
struct daos_obj_shard_md {
	/** ID of the object shard */
	daos_unit_oid_t		smd_id;
	uint64_t		smd_attr;
	uint32_t		smd_po_ver;
	uint32_t		smd_padding;
};

struct pl_obj_layout;

struct daos_oclass_attr *daos_oclass_attr_find(daos_obj_id_t oid);
unsigned int daos_oclass_grp_size(struct daos_oclass_attr *oc_attr);
unsigned int daos_oclass_grp_nr(struct daos_oclass_attr *oc_attr,
				struct daos_obj_md *md);

int dc_obj_init(void);
void dc_obj_fini(void);

int dc_obj_class_register(tse_task_t *task);
int dc_obj_class_query(tse_task_t *task);
int dc_obj_class_list(tse_task_t *task);
int dc_obj_declare(tse_task_t *task);
int dc_obj_open(tse_task_t *task);
int dc_obj_close(tse_task_t *task);
int dc_obj_punch(tse_task_t *task);
int dc_obj_punch_dkeys(tse_task_t *task);
int dc_obj_punch_akeys(tse_task_t *task);
int dc_obj_query(tse_task_t *task);
int dc_obj_fetch(tse_task_t *task);
int dc_obj_update(tse_task_t *task);
int dc_obj_list_dkey(tse_task_t *task);
int dc_obj_list_akey(tse_task_t *task);
int dc_obj_list_rec(tse_task_t *task);
int dc_obj_single_shard_list_dkey(tse_task_t *task);
int dc_obj_fetch_md(daos_obj_id_t oid, struct daos_obj_md *md);
int dc_obj_layout_get(daos_handle_t oh, struct pl_obj_layout **layout,
		      unsigned int *grp_nr, unsigned int *grp_size);
int dc_obj_layout_refresh(daos_handle_t oh);

#define ENUM_ANCHOR_SHARD_OFF		28
#define ENUM_ANCHOR_SHARD_LENGTH	4

/** Decode shard number from enumeration anchor */
static inline uint32_t
dc_obj_anchor2shard(daos_hash_out_t *anchor)
{
	uint32_t tag;

	memcpy(&tag, &anchor->body[ENUM_ANCHOR_SHARD_OFF],
	       ENUM_ANCHOR_SHARD_LENGTH);

	return tag;
}

/** Encode shard into enumeration anchor. */
static inline void
dc_obj_shard2anchor(daos_hash_out_t *anchor, uint32_t shard)
{
	memcpy(&anchor->body[ENUM_ANCHOR_SHARD_OFF], &shard,
	       ENUM_ANCHOR_SHARD_LENGTH);
}

#endif /* __DAOS_OBJECT_H__ */
