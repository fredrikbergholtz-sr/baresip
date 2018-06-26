/**
 * @file l16.c  16-bit linear codec
 *
 * Copyright (C) 2010 - 2015 Creytiv.com
 */
#include <re.h>
#include <rem.h>
#include <baresip.h>
#include <stdlib.h>

/**
 * @defgroup l16 l16
 *
 * Linear 16-bit audio codec
 */


enum {NR_CODECS = 10};


static int encode(struct auenc_state *st, uint8_t *buf, size_t *len,
		  int fmt, const void *sampv, size_t sampc)
{
	int16_t *p = (void *)buf;
	int16_t tmp;
	const int16_t *sampv16 = sampv;
	(void)st;

	if (!buf || !len || !sampv)
		return EINVAL;

	if (*len < sampc * 2)
		return ENOMEM;

	switch (fmt)
	{
	case AUFMT_S16LE:
		*len = sampc * 2;
		while (sampc--)
			*p++ = htons(*sampv16++);
		break;

	case AUFMT_S24_3LE:
		*len = sampc * 2;
		while (sampc--)
		{
			tmp = (int16_t)(((rand() % 256 - 128) + (*((int8_t*)sampv16)++) + 128) / 256);
			*p++ = htons(tmp + *sampv16++);
		}
		break;

	case AUFMT_FLOAT:
		return ENOTSUP;
		break;

	default:
		return ENOTSUP;
		break;
	}


	return 0;
}


static int decode(struct audec_state *st, int fmt, void *sampv, size_t *sampc,
		  const uint8_t *buf, size_t len)
{
	int16_t *p = (void *)buf;
	int8_t *sampv8 = sampv;
	int16_t *sampv16 = sampv;
	(void)st;

	if (!buf || !len || !sampv)
		return EINVAL;

	if (*sampc < len/2)
		return ENOMEM;


	switch (fmt)
	{
	case AUFMT_S16LE:
		*sampc = len / 2;
		len /= 2;
		while (len--)
			*sampv16++ = ntohs(*p++);
		break;

	case AUFMT_S24_3LE:
		*sampc = len / 2;
		len /= 2;
		while (len--)
		{
			*((int8_t*)sampv16)++ = 0;
			*sampv16++ = ntohs(*p++);
		}
		break;

	case AUFMT_FLOAT:
		return ENOTSUP;
		break;

	default:
		return ENOTSUP;
		break;
	}


	return 0;
}


/* See RFC 3551 */
static struct aucodec l16v[NR_CODECS] = {
{LE_INIT,    0, "L16", 48000, 48000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT, "10", "L16", 44100, 44100, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L16", 32000, 32000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L16", 16000, 16000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L16",  8000,  8000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L16", 48000, 48000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT, "11", "L16", 44100, 44100, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L16", 32000, 32000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L16", 16000, 16000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L16",  8000,  8000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
};


static int module_init(void)
{
	struct list *aucodecl = baresip_aucodecl();
	size_t i;

	for (i=0; i<NR_CODECS; i++)
		aucodec_register(aucodecl, &l16v[i]);

	return 0;
}


static int module_close(void)
{
	size_t i;

	for (i=0; i<NR_CODECS; i++)
		aucodec_unregister(&l16v[i]);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(l16) = {
	"l16",
	"codec",
	module_init,
	module_close
};
