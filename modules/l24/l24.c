/**
 * @file l24.c  24-bit linear codec
 *
 * 
 */
#include <re.h>
#include <rem.h>
#include <baresip.h>
#include <stdlib.h>

/**
 * @defgroup l24 l24
 *
 * Linear 24-bit audio codec
 */


enum {NR_CODECS = 10};


static int encode(struct auenc_state *st,
				  uint8_t *out8,            // Output buffer
	              size_t *len,              // Number of bytes in output buffer
		          int fmt,
	              const void *sampv,        // Input buffer
	              size_t sampc)             // Number of samples in input buffer
{
	int16_t *out16 = (void *)out8;
	const int16_t *in16 = sampv;
	const uint8_t *in8 = sampv;
	(void)st;

	if (!out8 || !len || !sampv)
		return EINVAL;

	if (*len < sampc * 3)
		return ENOMEM;

	switch (fmt)
	{
	case AUFMT_S16LE:
		*len = sampc * 3;
		while (sampc--)
		{
			*out16++ = htons(*in16++);
			*((int8_t*)out16)++ = 0;
		}
		break;

	case AUFMT_S24_3LE:
		*len = sampc * 3;
		while (sampc--)
		{
			*out8++ = in8[2];
			*out8++ = in8[1];
			*out8++ = in8[0];
			in8 += 3;
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


static int decode(struct audec_state *st,
	              int fmt,
	              void *sampv,               // Output buffer
	              size_t *sampc,             // Number of samples we can receive
		          const uint8_t *in8,        // Input buffer
	              size_t len)                // Number of bytes in the input
{
	int16_t *in16 = (void *)in8;
	int8_t *out8 = sampv;
	int16_t *out16 = sampv;
	int16_t tmp;
	(void)st;

	if (!in8 || !len || !sampv)
		return EINVAL;

	if (*sampc < len / 3)
		return ENOMEM;


	switch (fmt)
	{
	case AUFMT_S16LE:
		*sampc = len / 3;
		len /= 3;
		while (len--)
		{
			tmp = (int16_t)(((rand() % 256 - 128) + (*((int8_t*)in16)++) + 128) / 256);
			*out16++ = ntohs(tmp + *in16++);
//			tmp = *in16++;
//			*out16++ = ntohs(tmp + (int16_t)(((rand() % 256 - 128) + (*((int8_t*)in16)++) + 128) / 256));
		}
		break;

	case AUFMT_S24_3LE:
		*sampc = len / 3;
		len /= 3;
		while (len--)
		{
			*out8++ = in8[2];
			*out8++ = in8[1];
			*out8++ = in8[0];
			in8 += 3;
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
static struct aucodec l24v[NR_CODECS] = {
{LE_INIT,    0, "L24", 48000, 48000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24", 44100, 44100, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24", 32000, 32000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24", 16000, 16000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24",  8000,  8000, 2, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24", 48000, 48000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24", 44100, 44100, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24", 32000, 32000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24", 16000, 16000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
{LE_INIT,    0, "L24",  8000,  8000, 1, 0, 0, encode, 0, decode, 0, 0, 0},
};


static int module_init(void)
{
	struct list *aucodecl = baresip_aucodecl();
	size_t i;

	for (i=0; i<NR_CODECS; i++)
		aucodec_register(aucodecl, &l24v[i]);

	return 0;
}


static int module_close(void)
{
	size_t i;

	for (i=0; i<NR_CODECS; i++)
		aucodec_unregister(&l24v[i]);

	return 0;
}


EXPORT_SYM const struct mod_export DECL_EXPORTS(l24) = {
	"l24",
	"codec",
	module_init,
	module_close
};
