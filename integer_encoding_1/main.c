/**
 * Another experimental (?) scheme for encoding integers up to 64 bits
 * (8 bytes), although it can be adapted to encode bigger integers.
 *
 * The encoded integers are in big-endian format.
 *
 * ```
 * [ 0|x6|x5|x4|x3|x2|x1|x0]
 *
 * [ 1| 0| 0| 0|a3|a2|a1|a0]
 * [a7|a6|a5|a4| 0| 0| 0| 0]
 *
 * [ 1| 0| 0| 1|a3|a2|a1|a0]
 * [a7|a6|a5|a4|b3|b2|b1|b0]
 * [b7|b6|b5|b4| 0| 0| 0| 0]
 *
 * [ 1| 0| 1| 0|a3|a2|a1|a0]
 * [a7|a6|a5|a4|b3|b2|b1|b0]
 * [b7|b6|b5|b4|c3|c2|c1|c0]
 * [c7|c6|c5|c4| 0| 0| 0| 0]
 *
 * [ 1| 0| 1| 1|a3|a2|a1|a0]
 * [a7|a6|a5|a4|b3|b2|b1|b0]
 * [b7|b6|b5|b4|c3|c2|c1|c0]
 * [c7|c6|c5|c4|d3|d2|d1|d0]
 * [d7|d6|d5|d4| 0| 0| 0| 0]
 *
 * [ 1| 1| 0| 0|a3|a2|a1|a0]
 * [a7|a6|a5|a4|b3|b2|b1|b0]
 * [b7|b6|b5|b4|c3|c2|c1|c0]
 * [c7|c6|c5|c4|d3|d2|d1|d0]
 * [d7|d6|d5|d4|e3|e2|e1|e0]
 * [e7|e6|e5|e4| 0| 0| 0| 0]
 *
 * [ 1| 1| 0| 1|a3|a2|a1|a0]
 * [a7|a6|a5|a4|b3|b2|b1|b0]
 * [b7|b6|b5|b4|c3|c2|c1|c0]
 * [c7|c6|c5|c4|d3|d2|d1|d0]
 * [d7|d6|d5|d4|e3|e2|e1|e0]
 * [e7|e6|e5|e4|f3|f2|f1|f0]
 * [f7|f6|f5|f4| 0| 0| 0| 0]
 *
 * [ 1| 1| 1| 0|a3|a2|a1|a0]
 * [a7|a6|a5|a4|b3|b2|b1|b0]
 * [b7|b6|b5|b4|c3|c2|c1|c0]
 * [c7|c6|c5|c4|d3|d2|d1|d0]
 * [d7|d6|d5|d4|e3|e2|e1|e0]
 * [e7|e6|e5|e4|f3|f2|f1|f0]
 * [f7|f6|f5|f4|g3|g2|g1|g0]
 * [g7|g6|g5|g4| 0| 0| 0| 0]
 *
 * [ 1| 1| 1| 1|a3|a2|a1|a0]
 * [a7|a6|a5|a4|b3|b2|b1|b0]
 * [b7|b6|b5|b4|c3|c2|c1|c0]
 * [c7|c6|c5|c4|d3|d2|d1|d0]
 * [d7|d6|d5|d4|e3|e2|e1|e0]
 * [e7|e6|e5|e4|f3|f2|f1|f0]
 * [f7|f6|f5|f4|g3|g2|g1|g0]
 * [g7|g6|g5|g4|h3|h2|h1|h0]
 * [h7|h6|h5|h4| 0| 0| 0| 0]
 * ```
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <x86intrin.h> // for _rdrand64_step


uint_least64_t decode(const uint_least8_t* data)
{
  uint_least64_t res = 0;
  uint_least64_t nb  = data[0] >> 5;

  for (size_t i = 0; i <= nb; ++i)
  {
    res <<= 8;
    res  |= (data[i] & 0x1f) | (data[i + 1] & 0xe0);
  }

  return res;
}

void encode(uint_least8_t* data, const uint_least64_t in, const size_t nb)
{
  if (nb == 0)
    return;

  data[0] = ((nb - 1) & 0x07) << 5;

  for (size_t i = 0; i < nb; ++i)
  {
    uint_least8_t cb = in >> ((nb - 1 - i) << 3);

    data[i    ] |= cb & 0x1f;
    data[i + 1]  = cb & 0xe0;
  }
}



static const uint_least64_t masks[9] = {
  0x0ull, 0xffull, 0xffffull, 0xffffffull, 0xffffffffull, 0xffffffffffull,
  0xffffffffffffull, 0xffffffffffffffull, 0xffffffffffffffffull
};

int main(void)
{
  uint_least8_t data[9] = {0};

  unsigned is_ok = 1;
  for (size_t num_bytes = 1; num_bytes <= 8; ++num_bytes)
  {
    for (size_t i = 0; i < 1000000; ++i)
    {
      uint_least64_t rv;
      _rdrand64_step(&rv);
      rv &= masks[num_bytes];

      encode(data, rv, num_bytes);

      uint_least64_t res = decode(data);

      is_ok = is_ok && (rv == res);

      for (size_t j = 0; j < 9; ++j)
        data[j] = 0;
    }
  }

  puts(is_ok ? "OK" : "FAILED");

  return EXIT_SUCCESS;
}

