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
/**
 * dsms: Internal Declarations
 *
 * This file contains all declarations that are only used by dsms but do not
 * belong to the more specific headers like dsms_layout.h. All external
 * variables and functions must have a "dsms_" prefix, however, even if they
 * are only used by dsms.
 */

#ifndef __DSMS_INTERNAL_H__
#define __DSMS_INTERNAL_H__

#include <libpmemobj.h>
#include <pthread.h>
#include <uuid/uuid.h>

#include <daos/btree.h>
#include <daos/list.h>
#include <daos/lru.h>
#include <daos/transport.h>
#include <daos/daos_m.h>
#include <daos_srv/daos_mgmt_srv.h>
#include <daos_srv/daos_server.h>

/**
 * DSM server thread local storage structure
 */
struct dsm_tls {
	/* in-memory structures TLS instance */
	struct daos_lru_cache  *dt_cont_cache;
	struct dhash_table	dt_cont_hdl_hash;
};

extern struct dss_module_key cont_module_key;

static inline struct dsm_tls *
dsm_tls_get()
{
	struct dsm_tls			*tls;
	struct dss_thread_local_storage	*dtc;

	dtc = dss_tls_get();
	tls = (struct dsm_tls *)dss_module_key_get(dtc, &cont_module_key);
	return tls;
}

/*
 * dsms_module.c
 */
int dcont_corpc_create(dtp_context_t ctx, dtp_group_t *group,
		      dtp_opcode_t opcode, dtp_rpc_t **rpc);
int dsms_rpc_send(dtp_rpc_t *rpc);

/*
 * dsms_pool.c
 */
int dsms_module_pool_init(void);
void dsms_module_pool_fini(void);
int dsms_hdlr_pool_connect(dtp_rpc_t *rpc);
int dsms_hdlr_pool_disconnect(dtp_rpc_t *rpc);

/*
 * dsms_container.c
 */
int dsms_hdlr_cont_create(dtp_rpc_t *rpc);
int dsms_hdlr_cont_destroy(dtp_rpc_t *rpc);
int dsms_hdlr_cont_open(dtp_rpc_t *rpc);
int dsms_hdlr_cont_close(dtp_rpc_t *rpc);
int dsms_hdlr_cont_op(dtp_rpc_t *rpc);

/*
 * dsms_target.c
 */
int dsms_module_target_init(void);
void dsms_module_target_fini(void);
int dsms_hdlr_tgt_pool_connect(dtp_rpc_t *rpc);
int dsms_hdlr_tgt_pool_connect_aggregate(dtp_rpc_t *source, dtp_rpc_t *result,
					 void *priv);
int dsms_hdlr_tgt_pool_disconnect(dtp_rpc_t *rpc);
int dsms_hdlr_tgt_pool_disconnect_aggregate(dtp_rpc_t *source,
					    dtp_rpc_t *result, void *priv);
int dsms_hdlr_tgt_cont_destroy(dtp_rpc_t *rpc);
int dsms_hdlr_tgt_cont_destroy_aggregate(dtp_rpc_t *source, dtp_rpc_t *result,
					 void *priv);
int dsms_hdlr_tgt_cont_open(dtp_rpc_t *rpc);
int dsms_hdlr_tgt_cont_open_aggregate(dtp_rpc_t *source, dtp_rpc_t *result,
				      void *priv);
int dsms_hdlr_tgt_cont_close(dtp_rpc_t *rpc);
int dsms_hdlr_tgt_cont_close_aggregate(dtp_rpc_t *source, dtp_rpc_t *result,
				       void *priv);
int dsms_vcont_cache_create(struct daos_lru_cache **cache);
void dsms_vcont_cache_destroy(struct daos_lru_cache *cache);
int dsms_tgt_cont_hdl_hash_create(struct dhash_table *hash);
void dsms_tgt_cont_hdl_hash_destroy(struct dhash_table *hash);

void dsms_conts_close(void);

#endif /* __DSMS_INTERNAL_H__ */