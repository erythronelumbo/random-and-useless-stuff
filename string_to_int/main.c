#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


/**
 * Parses a string for obtaining an integral value. This function assumes that
 * the input string has the folllowing form:
 *
 * ```
 * "[+/-][0/0b/0B/0x/0X]...digits..."
 * ```
 *
 * @param result
 *   A pointer to the variable that stores the result. If the parsing failed,
 *   the *pointed* variable will be left unchanged.
 * @param str
 *   The input string. Assumed to have the form described above.
 * @param base
 *   The numeric base, which must be between 2 and 36 (inclusive) or 0 for
 *   detecting it automatically (decimal, binary, octal or hexadecimal).
 *
 * @returns
 *   1 if the conversion is sucessful and the function arguments are valid, 0
 *   otherwise.
 */
uint_least32_t string_to_int(
  int_least32_t* result, const char* str, uint_least32_t base
)
{
  if ((base == 1 || base > 36) || result == NULL || str == NULL)
    return 0;

  int_least32_t  res_aux = 0;
  int_least32_t  is_uc, is_lc;
  uint_least32_t has_valid_digits = 1;
  char           cc = *str;

  // -1 if cc == '-', 1 otherwise
  int_least32_t res_sign = (int_least32_t)(cc == '-');
  res_sign = (res_sign + res_sign) - 1;

  // Advances if a sign is found
  str += (cc == '+') | (cc == '-');

  if (base == 0)
  {
    char cc_aux = *(str + 1);
    base = (
      (
        (8 + ((*str != '0') << 1)) <<
        ((cc_aux == 'X') | (cc_aux == 'x'))
      ) >>
      (((cc_aux == 'B') | (cc_aux == 'b')) << 1)
    );
  }

  // Discards the "0x"/"0X"/"0b"/"0B" prefix
  cc   = *str;
  str += ((base == 2) | (base == 16)) & (cc == '0');
  cc   = *str;
  str += (
    ((base == 16) & ((cc == 'X') | (cc == 'x'))) |
    ((base ==  2) & ((cc == 'B') | (cc == 'b')))
  );

  size_t sl = strlen(str);

  if (sl == 0)
    return 0;

  uint_least32_t curr_digit;

  if (base <= 10)
  {
    while (sl-- && has_valid_digits)
    {
      cc                = *str++;
      curr_digit        = cc - '0';
      has_valid_digits &= (curr_digit < base);
      res_aux           = base*res_aux - (int_least32_t)curr_digit;
    }
  }
  else
  {
    while (sl-- && has_valid_digits)
    {
      cc         = *str++;
      is_uc      = (cc >= 'A') & (cc <= 'Z');
      is_lc      = (cc >= 'a') & (cc <= 'z');
      curr_digit = (
        (cc + 10*(is_uc | is_lc)) -
        ('0' + (('A' - '0')*is_uc + ('a' - '0')*is_lc))
      );
      has_valid_digits &= (curr_digit < base);
      res_aux           = base*res_aux - (int_least32_t)curr_digit;
    }
  }

  if (has_valid_digits)
    *result = res_sign*res_aux;

  return has_valid_digits;
}


int main(void)
{
  int_least32_t  result = 0;
  uint_least32_t ok     = 1;

  ok &= string_to_int(&result, "0", 0);
  ok &= (result == 0);
  ok &= string_to_int(&result, "00", 0);
  ok &= (result == 0);
  ok &= string_to_int(&result, "000", 0);
  ok &= (result == 0);
  ok &= string_to_int(&result, "0000", 0);
  ok &= (result == 0);

  ok &= string_to_int(&result, "0b00001001", 0);
  ok &= (result == 9);
  ok &= string_to_int(&result, "0B1110", 0);
  ok &= (result == 14);
  ok &= string_to_int(&result, "+0B1110", 0);
  ok &= (result == 14);
  ok &= string_to_int(&result, "-0b1110", 0);
  ok &= (result == -14);

  ok &= string_to_int(&result, "01", 0);
  ok &= (result == 01);
  ok &= string_to_int(&result, "-0175", 0);
  ok &= (result == -0175);
  ok &= string_to_int(&result, "+040", 0);
  ok &= (result == +040);

  ok &= string_to_int(&result, "98501234", 0);
  ok &= (result == 98501234);
  ok &= string_to_int(&result, "+8432810", 0);
  ok &= (result == +8432810);
  ok &= string_to_int(&result, "-5067891", 0);
  ok &= (result == -5067891);

  ok &= string_to_int(&result, "0xbada55", 0);
  ok &= (result == 0xbada55);
  ok &= string_to_int(&result, "-0X600dBeef", 0);
  ok &= (result == -0X600dBeef);

  // These are supposed to fail
  ok &= !string_to_int(&result, "-0b", 0);
  ok &= !string_to_int(&result, "0b", 0);
  ok &= !string_to_int(&result, "0x", 0);
  ok &= !string_to_int(&result, "0B", 0);
  ok &= !string_to_int(&result, "0X", 0);
  ok &= !string_to_int(&result, "0bB", 0);
  ok &= !string_to_int(&result, "0xX", 0);
  ok &= !string_to_int(&result, "*_-_*", 0);
  ok &= !string_to_int(NULL, "-567890", 0);
  ok &= !string_to_int(&result, "-567890", 8);
  ok &= !string_to_int(&result, NULL, 0);
  ok &= !string_to_int(NULL, NULL, 100);

  puts(ok ? "OK." : "Failed.");

  return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
