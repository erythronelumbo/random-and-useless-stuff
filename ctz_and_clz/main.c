/**
 * Custom implementations for the CTZ (count trailing zeros) and CLZ (count
 * leading zeros) functions.
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <x86intrin.h>


#define NUM_RUNS 100000


uint_least64_t clz_u64(uint_least64_t x)
{
  uint_least64_t y, c;
  x <<= (c = ((x & 0xffffffff00000000) == 0) << 5); y  = c;
  x <<= (c = ((x & 0xffff000000000000) == 0) << 4); y += c;
  x <<= (c = ((x & 0xff00000000000000) == 0) << 3); y += c;
  x <<= (c = ((x & 0xf000000000000000) == 0) << 2); y += c;
  x <<= (c = ((x & 0xc000000000000000) == 0) << 1); y += c;
  x <<= (c = ((x & 0x8000000000000000) == 0)     ); y += c;
  return y + (x == 0);
}

uint_least64_t clz_u32(uint_least64_t x)
{
  uint_least64_t y, c;
  x  &= 0xffffffff;
  x <<= (c = ((x & 0xffff0000) == 0) << 4); y  = c;
  x <<= (c = ((x & 0xff000000) == 0) << 3); y += c;
  x <<= (c = ((x & 0xf0000000) == 0) << 2); y += c;
  x <<= (c = ((x & 0xc0000000) == 0) << 1); y += c;
  x <<= (c = ((x & 0x80000000) == 0)     ); y += c;
  return y + (x == 0);
}

uint_least64_t clz_u16(uint_least64_t x)
{
  uint_least64_t y, c;
  x  &= 0xffff;
  x <<= (c = ((x & 0xff00) == 0) << 3); y  = c;
  x <<= (c = ((x & 0xf000) == 0) << 2); y += c;
  x <<= (c = ((x & 0xc000) == 0) << 1); y += c;
  x <<= (c = ((x & 0x8000) == 0)     ); y += c;
  return y + (x == 0);
}

uint_least64_t clz_u8(uint_least64_t x)
{
  uint_least64_t y, c;
  x  &= 0xff;
  x <<= (c = ((x & 0xf0) == 0) << 2); y  = c;
  x <<= (c = ((x & 0xc0) == 0) << 1); y += c;
  x <<= (c = ((x & 0x80) == 0)     ); y += c;
  return y + (x == 0);
}


uint_least64_t ctz_u64(uint_least64_t x)
{
  uint_least64_t y, c;
  x >>= (c = ((x & 0x00000000ffffffffull) == 0) << 5); y  = c;
  x >>= (c = ((x & 0x000000000000ffffull) == 0) << 4); y += c;
  x >>= (c = ((x & 0x00000000000000ffull) == 0) << 3); y += c;
  x >>= (c = ((x & 0x000000000000000full) == 0) << 2); y += c;
  x >>= (c = ((x & 0x0000000000000003ull) == 0) << 1); y += c;
  x >>= (c = ((x & 0x0000000000000001ull) == 0)     ); y += c;
  return y + (x == 0);
}

uint_least64_t ctz_u32(uint_least64_t x)
{
  uint_least64_t y, c;
  x  &= 0xffffffff;
  x >>= (c = ((x & 0x0000ffff) == 0) << 4); y  = c;
  x >>= (c = ((x & 0x000000ff) == 0) << 3); y += c;
  x >>= (c = ((x & 0x0000000f) == 0) << 2); y += c;
  x >>= (c = ((x & 0x00000003) == 0) << 1); y += c;
  x >>= (c = ((x & 0x00000001) == 0)     ); y += c;
  return y + (x == 0);
}

uint_least64_t ctz_u16(uint_least64_t x)
{
  uint_least64_t y, c;
  x  &= 0xffff;
  x >>= (c = ((x & 0x00ff) == 0) << 3); y  = c;
  x >>= (c = ((x & 0x000f) == 0) << 2); y += c;
  x >>= (c = ((x & 0x0003) == 0) << 1); y += c;
  x >>= (c = ((x & 0x0001) == 0)     ); y += c;
  return y + (x == 0);
}

uint_least64_t ctz_u8(uint_least64_t x)
{
  uint_least64_t y, c;
  x  &= 0xff;
  x >>= (c = ((x & 0x0f) == 0) << 2); y  = c;
  x >>= (c = ((x & 0x03) == 0) << 1); y += c;
  x >>= (c = ((x & 0x01) == 0)     ); y += c;
  return y + (x == 0);
}


static uint_least64_t (*clzs[4])(uint_least64_t) = {
  &clz_u8, &clz_u16, &clz_u32, &clz_u64
};

static uint_least64_t (*ctzs[4])(uint_least64_t) = {
  &ctz_u8, &ctz_u16, &ctz_u32, &ctz_u64
};


size_t test_clz(void)
{
  size_t res = 1;

  uint_least64_t mask, first_bit, randv;
  uint_least64_t (*cfn)(uint_least64_t);
  size_t         bits;

  for (size_t nb = 0; nb < 4; ++nb)
  {
    cfn  = clzs[nb];
    bits = (1 << nb) << 3;

    for (size_t i = 0; i < bits; ++i)
    {
      mask      = (2ull << (bits - 1 - i)) - 1;
      first_bit = mask ^ (mask >> 1);
      for (size_t j = 0; j < NUM_RUNS; ++j)
      {
        _rdrand64_step(&randv);
        randv = (randv & mask) | first_bit;
        res   = res && (cfn(randv) == i);
      }
    }
  }

  return res;
}

size_t test_ctz(void)
{
  size_t res = 1;

  uint_least64_t mask, last_bit, randv;
  uint_least64_t (*cfn)(uint_least64_t);
  size_t         bits;

  for (size_t nb = 0; nb < 4; ++nb)
  {
    cfn  = ctzs[nb];
    bits = (1 << nb) << 3;

    for (size_t i = 0; i < bits; ++i)
    {
      mask     = ~((1ull << i) - 1);
      last_bit = mask ^ (mask << 1);
      for (size_t j = 0; j < NUM_RUNS; ++j)
      {
        _rdrand64_step(&randv);
        randv = (randv & mask) | last_bit;
        res   = res && (cfn(randv) == i);
      }
    }
  }

  return res;
}


static const char* bools[2] = {"FAILED", "OK"};


int main(void)
{
  size_t res_clz = test_clz();
  size_t res_ctz = test_ctz();

  printf(
    "CLZ test: %s\n"
    "CTZ test: %s\n",
    bools[res_clz], bools[res_ctz]
  );

  return (res_ctz && res_clz) ? EXIT_SUCCESS : EXIT_FAILURE;
}
