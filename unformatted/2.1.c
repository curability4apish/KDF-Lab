
// /
#include <stdio.h>
#include <stdlib.h>


// /
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

	struct chacha20_context
	{
		uint32_t keystream32[16];
		size_t position;

		uint8_t key[32];
		uint8_t nonce[12];
		uint64_t counter;

		uint32_t state[16];
	};

	void chacha20_init_context(struct chacha20_context *ctx, uint8_t key[], uint8_t nounc[],
							   uint64_t counter);

	void chacha20_xor(struct chacha20_context *ctx, uint8_t * bytes, size_t n_bytes);

#ifdef __cplusplus
}
#endif


static uint32_t rotl32(uint32_t x, int n)
{
	return (x << n) | (x >> (32 - n));
}

static uint32_t pack4(const uint8_t * a)
{
	uint32_t res = 0;
	res |= (uint32_t) a[0] << 0 * 8;
	res |= (uint32_t) a[1] << 1 * 8;
	res |= (uint32_t) a[2] << 2 * 8;
	res |= (uint32_t) a[3] << 3 * 8;
	return res;
}

static void unpack4(uint32_t src, uint8_t * dst)
{
	dst[0] = (src >> 0 * 8) & 0xff;
	dst[1] = (src >> 1 * 8) & 0xff;
	dst[2] = (src >> 2 * 8) & 0xff;
	dst[3] = (src >> 3 * 8) & 0xff;
}

static void chacha20_init_block(struct chacha20_context *ctx, uint8_t key[], uint8_t nonce[])
{
	memcpy(ctx->key, key, sizeof(ctx->key));
	memcpy(ctx->nonce, nonce, sizeof(ctx->nonce));

	const uint8_t *magic_constant = (uint8_t *) "expand 32-byte k";
	ctx->state[0] = pack4(magic_constant + 0 * 4);
	ctx->state[1] = pack4(magic_constant + 1 * 4);
	ctx->state[2] = pack4(magic_constant + 2 * 4);
	ctx->state[3] = pack4(magic_constant + 3 * 4);
	ctx->state[4] = pack4(key + 0 * 4);
	ctx->state[5] = pack4(key + 1 * 4);
	ctx->state[6] = pack4(key + 2 * 4);
	ctx->state[7] = pack4(key + 3 * 4);
	ctx->state[8] = pack4(key + 4 * 4);
	ctx->state[9] = pack4(key + 5 * 4);
	ctx->state[10] = pack4(key + 6 * 4);
	ctx->state[11] = pack4(key + 7 * 4);
	// 64 bit counter initialized to zero by default.
	ctx->state[12] = 0;
	ctx->state[13] = pack4(nonce + 0 * 4);
	ctx->state[14] = pack4(nonce + 1 * 4);
	ctx->state[15] = pack4(nonce + 2 * 4);

	memcpy(ctx->nonce, nonce, sizeof(ctx->nonce));
}

static void chacha20_block_set_counter(struct chacha20_context *ctx, uint64_t counter)
{
	ctx->state[12] = (uint32_t) counter;
	ctx->state[13] = pack4(ctx->nonce + 0 * 4) + (uint32_t) (counter >> 32);
}

static void chacha20_block_next(struct chacha20_context *ctx)
{
	// This is where the crazy voodoo magic happens.
	// Mix the bytes a lot and hope that nobody finds out how to undo it.
	for (int i = 0; i < 16; i++)
		ctx->keystream32[i] = ctx->state[i];

#define CHACHA20_QUARTERROUND(x, a, b, c, d) \
    x[a] += x[b]; x[d] = rotl32(x[d] ^ x[a], 16); \
    x[c] += x[d]; x[b] = rotl32(x[b] ^ x[c], 12); \
    x[a] += x[b]; x[d] = rotl32(x[d] ^ x[a], 8); \
    x[c] += x[d]; x[b] = rotl32(x[b] ^ x[c], 7);

	for (int i = 0; i < 10; i++)
	{
	CHACHA20_QUARTERROUND(ctx->keystream32, 0, 4, 8, 12)
			CHACHA20_QUARTERROUND(ctx->keystream32, 1, 5, 9, 13)
			CHACHA20_QUARTERROUND(ctx->keystream32, 2, 6, 10, 14)
			CHACHA20_QUARTERROUND(ctx->keystream32, 3, 7, 11, 15)
			CHACHA20_QUARTERROUND(ctx->keystream32, 0, 5, 10, 15)
			CHACHA20_QUARTERROUND(ctx->keystream32, 1, 6, 11, 12)
			CHACHA20_QUARTERROUND(ctx->keystream32, 2, 7, 8, 13)
			CHACHA20_QUARTERROUND(ctx->keystream32, 3, 4, 9, 14)}

	for (int i = 0; i < 16; i++)
		ctx->keystream32[i] += ctx->state[i];

	uint32_t *counter = ctx->state + 12;
	// increment counter
	counter[0]++;
	if (0 == counter[0])
	{
		// wrap around occured, increment higher 32 bits of counter
		counter[1]++;
		// Limited to 2^64 blocks of 64 bytes each.
		// If you want to process more than 1180591620717411303424 bytes
		// you have other problems.
		// We could keep counting with counter[2] and counter[3] (nonce),
		// but then we risk reusing the nonce which is very bad.
		assert(0 != counter[1]);
	}
}

void chacha20_init_context(struct chacha20_context *ctx, uint8_t key[], uint8_t nonce[],
						   uint64_t counter)
{
	memset(ctx, 0, sizeof(struct chacha20_context));

	chacha20_init_block(ctx, key, nonce);
	chacha20_block_set_counter(ctx, counter);

	ctx->counter = counter;
	ctx->position = 64;
}

void chacha20_xor(struct chacha20_context *ctx, uint8_t * bytes, size_t n_bytes)
{
	uint8_t *keystream8 = (uint8_t *) ctx->keystream32;
	for (size_t i = 0; i < n_bytes; i++)
	{
		if (ctx->position >= 64)
		{
			chacha20_block_next(ctx);
			ctx->position = 0;
		}
		bytes[i] ^= keystream8[ctx->position];
		ctx->position++;
	}
}

// / ISAAC start

/* a ub4 is an unsigned 4-byte quantity */
typedef unsigned long int ub4;

/* external results */
ub4 randrsl[256], randcnt;

/* internal state */
static ub4 mm[256];
static ub4 aa = 0, bb = 0, cc = 0;


void isaac()
{
	register ub4 i, x, y;

	cc = cc + 1;				/* cc just gets incremented once per 256
								   results */
	bb = bb + cc;				/* then combined with bb */

	for (i = 0; i < 256; ++i)
	{
		x = mm[i];
		switch (i % 4)
		{
		case 0:
			aa = aa ^ (aa << 13);
			break;
		case 1:
			aa = aa ^ (aa >> 6);
			break;
		case 2:
			aa = aa ^ (aa << 2);
			break;
		case 3:
			aa = aa ^ (aa >> 16);
			break;
		}
		aa = mm[(i + 128) % 256] + aa;
		mm[i] = y = mm[(x >> 2) % 256] + aa + bb;
		randrsl[i] = bb = mm[(y >> 10) % 256] + x;

		/* Note that bits 2..9 are chosen from x but 10..17 are chosen from y. 
		   The only important thing here is that 2..9 and 10..17 don't
		   overlap.  2..9 and 10..17 were then chosen for speed in the
		   optimized version (rand.c) */
		/* See http://burtleburtle.net/bob/rand/isaac.html for further
		   explanations and analysis. */
	}
}


/* if (flag!=0), then use the contents of randrsl[] to initialize mm[]. */
#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

void randinit(int flag)
{
	int i;
	ub4 a, b, c, d, e, f, g, h;
	aa = bb = cc = 0;
	a = b = c = d = e = f = g = h = 0x9e3779b9;	/* the golden ratio */

	for (i = 0; i < 4; ++i)		/* scramble it */
	{
		mix(a, b, c, d, e, f, g, h);
	}

	for (i = 0; i < 256; i += 8)	/* fill in mm[] with messy stuff */
	{
		if (flag)				/* use all the information in the seed */
		{
			a += randrsl[i];
			b += randrsl[i + 1];
			c += randrsl[i + 2];
			d += randrsl[i + 3];
			e += randrsl[i + 4];
			f += randrsl[i + 5];
			g += randrsl[i + 6];
			h += randrsl[i + 7];
		}
		mix(a, b, c, d, e, f, g, h);
		mm[i] = a;
		mm[i + 1] = b;
		mm[i + 2] = c;
		mm[i + 3] = d;
		mm[i + 4] = e;
		mm[i + 5] = f;
		mm[i + 6] = g;
		mm[i + 7] = h;
	}

	if (flag)
	{							/* do a second pass to make all of the seed
								   affect all of mm */
		for (i = 0; i < 256; i += 8)
		{
			a += mm[i];
			b += mm[i + 1];
			c += mm[i + 2];
			d += mm[i + 3];
			e += mm[i + 4];
			f += mm[i + 5];
			g += mm[i + 6];
			h += mm[i + 7];
			mix(a, b, c, d, e, f, g, h);
			mm[i] = a;
			mm[i + 1] = b;
			mm[i + 2] = c;
			mm[i + 3] = d;
			mm[i + 4] = e;
			mm[i + 5] = f;
			mm[i + 6] = g;
			mm[i + 7] = h;
		}
	}

	isaac();					/* fill in the first set of results */
	randcnt = 256;				/* prepare to use the first set of results */
}

// / ISAAC end

int abs(int x)
{
	if (x < 0)
		return -x;
	else
		return x;
}

int index(char x)
{
	if ((int)x >= 32 && (int)x <= 126)
		return (int)(x - 32);
	else
		return -1;
}

int max(int x, int y)
{
	if (x > y)
		return x;
	else
		return y;
}

int main()
{
	// Seed the random number generator


	// least_bit_size:y, least length:x
	// pow(95,x)>pow(2,y), x>pow(10,y(log2)/95)
	// key requires 256-bit, hence its length requires 9. sizem=9.
	// nonce requires 96-bit, hence its length requires 3. sizen=3.

	char key[100], label[100];

	printf("[key]\n");
	printf("//surpass it\n----------\n");
	scanf("%s", &key);
	printf("\n[label]\n");
	scanf("%s", &label);

	uint8_t _key[256];			// 256-bit key
	uint8_t _nonce[96];			// 96-bit nonce

	ub4 i, j;
	aa = bb = cc = (ub4) 0;

	for (i = 0; i < 256; ++i)
		mm[i] = randrsl[i] = (ub4) 0;
	randinit(1);
	for (i = 0; i < 256; ++i)
		isaac();

	for (int i = 0; i < 100; i++)
	{
		aa += (int)(key[i] - 32);
		bb += (int)(label[i] - 32);
		isaac();
	}


	for (int i = 0; i < 256; i++)
	{
		_key[i] = abs((int)randrsl[i]) % 256;
	}							// Generate random bytes in the range [0, 255]
	isaac();
	for (int i = 0; i < 96; i++)
	{
		_nonce[i] = abs((int)randrsl[i]) % 256;
	}							// Generate random bytes in the range [0, 255]

	// Initialize the ChaCha20 context
	struct chacha20_context ctx;
	uint64_t counter = 0;		// Start counter at 0
	chacha20_init_context(&ctx, _key, _nonce, counter);

	// Define the size of random text to generate
	int sizeb = 64;				// Change this to generate more or fewer bytes
	uint8_t buffer[sizeb];

	// Fill the random bytes array with random data
	chacha20_xor(&ctx, buffer, sizeb);

	// Print the ciphertext
	for (int i = 0; i < sizeb; i++)
	{
		if (i % 16 == 0)
			printf("\n");
		printf("%c", (char)((int)((buffer[i] % 95 + 32))));

	}
	return 0;
}
