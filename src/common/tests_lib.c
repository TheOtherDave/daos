/**
 * (C) Copyright 2015-2018 Intel Corporation.
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
#define D_LOGFAC	DD_FAC(tests)

/**
 * Test suite helper functions.
 */
#include <daos/common.h>
#include <daos/object.h>
#include <daos/tests_lib.h>
#include <daos.h>

#define DTS_OCLASS_DEF		DAOS_OC_REPL_MAX_RW

static uint32_t obj_id_gen	= 1;
static uint64_t int_key_gen	= 1;

daos_obj_id_t
dts_oid_gen(uint16_t oclass, uint8_t ofeats, unsigned seed)
{
	daos_obj_id_t	oid;
	uint64_t	hdr;

	srand(time(NULL));

	if (oclass == 0)
		oclass = DTS_OCLASS_DEF;

	hdr = seed;
	hdr <<= 32;

	/* generate a unique and not scary long object ID */
	oid.lo	= obj_id_gen++;
	oid.lo	|= hdr;
	oid.hi	= rand() % 100;
	daos_obj_id_generate(&oid, ofeats, oclass);

	return oid;
}

daos_obj_id_t
dts_oid_set_rank(daos_obj_id_t oid, unsigned int rank)
{
	D_ASSERT(daos_obj_id2class(oid) == DAOS_OC_R3S_SPEC_RANK);

	D_ASSERT(rank < 0xff);
	oid.hi |= (uint64_t)rank << 48;
	return oid;
}

daos_unit_oid_t
dts_unit_oid_gen(uint16_t oclass, uint8_t ofeats, uint32_t shard)
{
	daos_unit_oid_t	uoid;

	uoid.id_pub	= dts_oid_gen(oclass, ofeats, time(NULL));
	uoid.id_shard	= shard;
	uoid.id_pad_32	= 0;

	return uoid;
}

void
dts_key_gen(char *key, unsigned int key_len, const char *prefix)
{
	memset(key, 0, key_len);
	if (prefix == NULL)
		memcpy(key, &int_key_gen, sizeof(int_key_gen));
	else
		snprintf(key, key_len, "%s-"DF_U64, prefix, int_key_gen);
	int_key_gen++;
}

void
dts_buf_render(char *buf, unsigned int buf_len)
{
	int	nr = 'z' - 'a' + 1;
	int	i;

	srand(time(NULL));
	for (i = 0; i < buf_len - 1; i++) {
		int randv = rand() % (2 * nr);

		if (randv < nr)
			buf[i] = 'a' + randv;
		else
			buf[i] = 'A' + (randv - nr);
	}
	buf[i] = '\0';
}

#define DTS_LINE_SIZE	1024

void
dts_freeline(char *line)
{
#if HAVE_LIB_READLINE
	free(line);
#else
	D_FREE(line);
#endif
}

/**
 * Read a command line from stdin, save trouble if we don't have libreadline
 */
char *
dts_readline(const char *prompt)
{
#if HAVE_LIB_READLINE
	return readline(prompt);
#else
	char	*line;
	char	*cur;
	bool	 eof;

	D_ALLOC(line, DTS_LINE_SIZE);
	if (!line)
		return NULL;

	if (prompt) {
		fprintf(stdout, "%s", prompt);
		fflush(stdout);
	}

	cur = line;
	eof = false;
	while (1) {
		int	c;

		c = fgetc(stdin);
		if (c == EOF) {
			if (ferror(stdin) || feof(stdin))
				goto out_free;

			eof = true;
			break;
		}

		if (c == '\n')
			break;

		*cur++ = (char)c;
		if (cur - line >= DTS_LINE_SIZE) {
			fprintf(stderr, "line is too long\n");
			goto out_free;
		}
	}
	*cur = '\0';
	if (eof && strlen(line) == 0)
		goto out_free;

	return line;
 out_free:
	dts_freeline(line);
	return NULL;
#endif
}

int
dts_cmd_parser(struct option *opts, const char *prompt,
	       int (*cmd_func)(char opc, char *args))
{
	char	*line = NULL;
	int	 rc;

	for (rc = 0; rc == 0;) {
		char	*args;
		char	*cmd;
		char	 opc;
		int	 i;

		if (line)
			dts_freeline(line);

		line = dts_readline(prompt);
		if (!line)
			break;

		if (strlen(line) == 0)
			continue; /* empty line */

		cmd = daos_str_trimwhite(line);
		dts_add_history(cmd);

		for (i = 0, opc = 0;; i++) {
			struct option *opt;

			opt = &opts[i];
			if (opt->name == NULL) {
				opc = -1;
				break;
			}

			if (strncasecmp(opt->name, cmd, strlen(opt->name)))
				continue;

			/* matched a command */
			opc = (char)opt->val;
			if (opt->has_arg) {
				args = line + strlen(opt->name);
				args = daos_str_trimwhite(args);
			} else {
				args = NULL;
			}
			break;
		}

		if (opc == -1) {
			D_PRINT("Unknown command string %s, try \"help\"\n",
				 cmd);
			continue;
		}

		rc = cmd_func(opc, args);
		if (rc != 0)
			break;
	}

	if (line)
		dts_freeline(line);

	return rc;
}

static void
rand_iarr_swap(void *array, int a, int b)
{
	int	*iarray = (int *)array;
	int	 tmp;

	tmp = iarray[a];
	iarray[a] = iarray[b];
	iarray[b] = tmp;
}

static daos_sort_ops_t rand_iarr_ops = {
	.so_swap	= rand_iarr_swap,
};

int *
dts_rand_iarr_alloc(int nr, int base)
{
	int	*array;
	int	 i;

	array = malloc(nr * sizeof(*array));
	if (!array)
		return NULL;

	for (i = 0; i < nr; i++)
		array[i] = base + i;

	daos_array_shuffle((void *)array, nr, &rand_iarr_ops);
	return array;
}
