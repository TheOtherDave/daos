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
 *
 * GOVERNMENT LICENSE RIGHTS-OPEN SOURCE SOFTWARE
 * The Government's rights to use, modify, reproduce, release, perform, display,
 * or disclose this software are subject to the terms of the LGPL License as
 * provided in Contract No. B609815.
 * Any reproduction of computer software, computer software documentation, or
 * portions thereof marked with this legend must also reproduce the markings.
 *
 * (C) Copyright 2016 Intel Corporation.
 */
/*
 * dmgc: Internal Declarations
 */

#ifndef __DMGC_INTERNAL_H__
#define __DMGC_INTERNAL_H__

#include <pthread.h>
#include <uuid/uuid.h>

#include <daos/transport.h>
#include <daos/rpc.h>
#include <daos/common.h>
#include <daos/event.h>
#include "dmg_rpc.h"

extern dtp_context_t dsm_context;

bool
dmg_initialized();

#endif /* __DMGC_INTERNAL_H__ */