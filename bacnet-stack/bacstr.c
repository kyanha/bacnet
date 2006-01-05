/*####COPYRIGHTBEGIN####
 -------------------------------------------
 Copyright (C) 2004 Steve Karg

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to:
 The Free Software Foundation, Inc.
 59 Temple Place - Suite 330
 Boston, MA  02111-1307, USA.

 As a special exception, if other files instantiate templates or
 use macros or inline functions from this file, or you compile
 this file and link it with other works to produce a work based
 on this file, this file does not by itself cause the resulting
 work to be covered by the GNU General Public License. However
 the source code for this file must still be made available in
 accordance with section (3) of the GNU General Public License.

 This exception does not invalidate any other reasons why a work
 based on this file might be covered by the GNU General Public
 License.
 -------------------------------------------
####COPYRIGHTEND####*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h> /* for strlen */
#include "bacstr.h"
#include "bits.h"

void bitstring_init(BACNET_BIT_STRING *bit_string)
{
  int i;
  
  bit_string->bits_used = 0;
  for (i = 0; i < MAX_BITSTRING_BYTES; i++)
  {
    bit_string->value[i] = 0;
  }
}

void bitstring_set_bit(BACNET_BIT_STRING *bit_string, uint8_t bit, bool value)
{
  uint8_t byte_number = bit/8;
  uint8_t bit_mask = 1;

  if (byte_number < MAX_BITSTRING_BYTES)
  {
    // set max bits used
    if (bit_string->bits_used < (bit + 1))
      bit_string->bits_used = bit + 1;
    bit_mask = bit_mask << (bit - (byte_number * 8));
    if (value)
      bit_string->value[byte_number] |= bit_mask;
    else
      bit_string->value[byte_number] &= (~(bit_mask));
  }
}

bool bitstring_bit(BACNET_BIT_STRING *bit_string, uint8_t bit)
{
  bool value = false;
  uint8_t byte_number = bit/8;
  uint8_t bit_mask = 1;
  
  if (bit < (MAX_BITSTRING_BYTES * 8))
  {
    bit_mask = bit_mask << (bit - (byte_number * 8));
    if (bit_string->value[byte_number] & bit_mask)
      value = true;
  }

  return value;
}

uint8_t bitstring_bits_used(BACNET_BIT_STRING *bit_string)
{
  return bit_string->bits_used;
}

// returns the number of bytes that a bit string is using
int bitstring_bytes_used(BACNET_BIT_STRING *bit_string)
{
  int len = 0; // return value
  uint8_t used_bytes = 0;
  uint8_t last_bit = 0;

  if (bit_string->bits_used)
  {
    last_bit = bit_string->bits_used - 1;
    used_bytes = last_bit / 8;
    // add one for the first byte
    used_bytes++;
    len = used_bytes;
  }

  return len;
}

uint8_t bitstring_octet(BACNET_BIT_STRING *bit_string, uint8_t index)
{
  uint8_t octet = 0;

  if (bit_string)
  {
    if (index < MAX_BITSTRING_BYTES)
    {
      octet = bit_string->value[index];
    }
  }

  return octet;
}

bool bitstring_set_octet(
    BACNET_BIT_STRING *bit_string,
    uint8_t index,
    uint8_t octet)
{
  bool status = false;

  if (bit_string)
  {
    if (index < MAX_BITSTRING_BYTES)
    {
      bit_string->value[index] = octet;
      status = true;
    }
  }

  return status;
}

bool bitstring_set_bits_used(
  BACNET_BIT_STRING *bit_string,
  uint8_t bytes_used,
  uint8_t unused_bits)
{
  bool status = false;

  if (bit_string)
  {
    /* FIXME: check that bytes_used is at least one? */
    bit_string->bits_used = bytes_used * 8;
    bit_string->bits_used -= unused_bits;
    status = true;
  }

  return status;
}


uint8_t bitstring_bits_capacity(BACNET_BIT_STRING *bit_string)
{
  if (bit_string)
    return (sizeof(bit_string->value) * 8);
  else
    return 0;
}

/* returns false if the string exceeds capacity
   initialize by using length=0 */
bool characterstring_init(
  BACNET_CHARACTER_STRING *char_string,
  uint8_t encoding,
  char *value,
  size_t length)
{
  bool status = false; /* return value */
  size_t i; /* counter */

  if (char_string)
  {
    char_string->length = 0;
    char_string->encoding = encoding;
    if (length <= sizeof(char_string->value))
    {
      if (value)
      {
        for (i = 0; i < length; i++)
        {
          char_string->value[char_string->length] = value[i];
          char_string->length++;
        }
      }
      else
      {
        for (i = 0; i < sizeof(char_string->value); i++)
        {
          char_string->value[i] = 0;
        }
      }
      status = true;
    }
  }

  return status;
}

bool characterstring_init_ansi(
    BACNET_CHARACTER_STRING *char_string,
    char *value)
{
  return characterstring_init(
      char_string,
      CHARACTER_ANSI,
      value, value?strlen(value):0);
}

/* returns false if the string exceeds capacity */
bool characterstring_append(
  BACNET_CHARACTER_STRING *char_string,
  char *value,
  size_t length)
{
  size_t i; /* counter */
  bool status = false; /* return value */

  if (char_string)
  {
    if ((length + char_string->length) <= sizeof(char_string->value))
    {
      for (i = 0; i < length; i++)
      {
        char_string->value[char_string->length] = value[i];
        char_string->length++;
      }
      status = true;
    }
  }

  return status;
}

/* This function sets a new length without changing the value.
   If length exceeds capacity, no modification happens and
   function returns false.  */
bool characterstring_truncate(
  BACNET_CHARACTER_STRING *char_string,
  size_t length)
{
  bool status = false; /* return value */

  if (char_string)
  {
    if (length <= sizeof(char_string->value))
    {
      char_string->length = length;
      status = true;
    }
  }

  return status;
}

/* Returns the value. */
char *characterstring_value(BACNET_CHARACTER_STRING *char_string)
{
  char *value = NULL;
  
  if (char_string)
  {
    value = char_string->value;
  }

  return value;
}

/* returns the length. */
size_t characterstring_length(BACNET_CHARACTER_STRING *char_string)
{
  size_t length = 0;

  if (char_string)
  {
    /* FIXME: validate length is within bounds? */
    length = char_string->length;
  }

  return length;
}

size_t characterstring_capacity(BACNET_CHARACTER_STRING *char_string)
{
  size_t length = 0;

  if (char_string)
  {
    length = sizeof(char_string->value);
  }

  return length;
}

/* returns the encoding. */
uint8_t characterstring_encoding(BACNET_CHARACTER_STRING *char_string)
{
  uint8_t encoding = 0;

  if (char_string)
  {
    encoding = char_string->encoding;
  }

  return encoding;
}

/* returns false if the string exceeds capacity
   initialize by using length=0 */
bool octetstring_init(
    BACNET_OCTET_STRING *octet_string,
  uint8_t *value,
  size_t length)
{
  bool status = false; /* return value */
  size_t i; /* counter */

  if (octet_string)
  {
    octet_string->length = 0;
    if (length <= sizeof(octet_string->value))
    {
      if (value)
      {
        for (i = 0; i < length; i++)
        {
          octet_string->value[octet_string->length] = value[i];
          octet_string->length++;
        }
      }
      else
      {
        for (i = 0; i < sizeof(octet_string->value); i++)
        {
          octet_string->value[i] = 0;
        }
      }
      status = true;
    }
  }

  return status;
}

/* returns false if the string exceeds capacity */
bool octetstring_append(
    BACNET_OCTET_STRING *octet_string,
  uint8_t *value,
  size_t length)
{
  size_t i; /* counter */
  bool status = false; /* return value */

  if (octet_string)
  {
    if ((length + octet_string->length) <= sizeof(octet_string->value))
    {
      for (i = 0; i < length; i++)
      {
        octet_string->value[octet_string->length] = value[i];
        octet_string->length++;
      }
      status = true;
    }
  }

  return status;
}

/* This function sets a new length without changing the value.
   If length exceeds capacity, no modification happens and
   function returns false.  */
bool octetstring_truncate(
    BACNET_OCTET_STRING *octet_string,
  size_t length)
{
  bool status = false; /* return value */

  if (octet_string)
  {
    if (length <= sizeof(octet_string->value))
    {
      octet_string->length = length;
      status = true;
    }
  }

  return status;
}

/* returns the length.  Returns the value in parameter. */
uint8_t *octetstring_value(BACNET_OCTET_STRING *octet_string)
{
  uint8_t *value = NULL;
  
  if (octet_string)
  {
    value = octet_string->value;
  }

  return value;
}

/* returns the length. */
size_t octetstring_length(BACNET_OCTET_STRING *octet_string)
{
  size_t length = 0;

  if (octet_string)
  {
    /* FIXME: validate length is within bounds? */
    length = octet_string->length;
  }

  return length;
}

/* returns the length. */
size_t octetstring_capacity(BACNET_OCTET_STRING *octet_string)
{
  size_t length = 0;

  if (octet_string)
  {
    /* FIXME: validate length is within bounds? */
    length = sizeof(octet_string->value);
  }

  return length;
}

#ifdef TEST
#include <assert.h>
#include <string.h>
#include "ctest.h"

void testBitString(Test * pTest)
{
  uint8_t bit = 0;
  BACNET_BIT_STRING bit_string;

  bitstring_init(&bit_string);
  // verify initialization 
  ct_test(pTest, bitstring_bits_used(&bit_string) == 0);
  for (bit = 0; bit < (MAX_BITSTRING_BYTES*8); bit++)
  {
    ct_test(pTest, bitstring_bit(&bit_string, bit) == false);
  }
  
  // test for true
  for (bit = 0; bit < (MAX_BITSTRING_BYTES*8); bit++)
  {
    bitstring_set_bit(&bit_string, bit, true);
    ct_test(pTest, bitstring_bits_used(&bit_string) == (bit + 1));
    ct_test(pTest, bitstring_bit(&bit_string, bit) == true);
  }
  // test for false
  bitstring_init(&bit_string);
  for (bit = 0; bit < (MAX_BITSTRING_BYTES*8); bit++)
  {
    bitstring_set_bit(&bit_string, bit, false);
    ct_test(pTest, bitstring_bits_used(&bit_string) == (bit + 1));
    ct_test(pTest, bitstring_bit(&bit_string, bit) == false);
  }
}

void testCharacterString(Test * pTest)
{
  BACNET_CHARACTER_STRING bacnet_string;
  char *value = "Joshua,Mary,Anna,Christopher";
  char test_value[MAX_APDU] = "Patricia";
  char test_append_value[MAX_APDU] = " and the Kids";
  char test_append_string[MAX_APDU] = "";
  bool status = false;
  size_t length = 0;
  size_t test_length = 0;
  size_t i = 0;

  // verify initialization
  status = characterstring_init(&bacnet_string,CHARACTER_ANSI,NULL,0);
  ct_test(pTest, status == true);
  ct_test(pTest,characterstring_length(&bacnet_string) == 0);
  ct_test(pTest,characterstring_encoding(&bacnet_string) == CHARACTER_ANSI);
  /* bounds check */
  status = characterstring_init(
      &bacnet_string,
      CHARACTER_ANSI,
      NULL,
      characterstring_capacity(&bacnet_string)+1);
  ct_test(pTest, status == false);
  status = characterstring_truncate(
      &bacnet_string,characterstring_capacity(&bacnet_string)+1);
  ct_test(pTest, status == false);
  status = characterstring_truncate(
      &bacnet_string,characterstring_capacity(&bacnet_string));
  ct_test(pTest, status == true);

  test_length = strlen(test_value); 
  status = characterstring_init(
    &bacnet_string,
    CHARACTER_ANSI,
    &test_value[0],
    test_length);
  ct_test(pTest, status == true);
  value = characterstring_value(&bacnet_string);
  length = characterstring_length(&bacnet_string);
  ct_test(pTest, length == test_length);
  for (i = 0; i < test_length; i++)
  {
    ct_test(pTest, value[i] == test_value[i]);
  }
  test_length = strlen(test_append_value);
  status = characterstring_append(
    &bacnet_string,
    &test_append_value[0],
    test_length);
  strcat(test_append_string,test_value);
  strcat(test_append_string,test_append_value);
  test_length = strlen(test_append_string);
  ct_test(pTest, status == true);
  length = characterstring_length(&bacnet_string);
  value = characterstring_value(&bacnet_string);
  ct_test(pTest, length == test_length);
  for (i = 0; i < test_length; i++)
  {
    ct_test(pTest, value[i] == test_append_string[i]);
  }
}

void testOctetString(Test * pTest)
{
  BACNET_OCTET_STRING bacnet_string;
  uint8_t *value = NULL;
  uint8_t test_value[MAX_APDU] = "Patricia";
  uint8_t test_append_value[MAX_APDU] = " and the Kids";
  uint8_t test_append_string[MAX_APDU] = "";
  bool status = false;
  size_t length = 0;
  size_t test_length = 0;
  size_t i = 0;

  // verify initialization
  status = octetstring_init(&bacnet_string,NULL,0);
  ct_test(pTest, status == true);
  ct_test(pTest,octetstring_length(&bacnet_string) == 0);
  value = octetstring_value(&bacnet_string);
  for (i = 0; i < octetstring_capacity(&bacnet_string); i++)
  {
    ct_test(pTest, value[i] == 0);
  }
  /* bounds check */
  status = octetstring_init(&bacnet_string,NULL,
                             octetstring_capacity(&bacnet_string)+1);
  ct_test(pTest, status == false);
  status = octetstring_init(&bacnet_string,NULL,
                             octetstring_capacity(&bacnet_string));
  ct_test(pTest, status == true);
  status = octetstring_truncate(&bacnet_string,octetstring_capacity(&bacnet_string)+1);
  ct_test(pTest, status == false);
  status = octetstring_truncate(&bacnet_string,octetstring_capacity(&bacnet_string));
  ct_test(pTest, status == true);

  test_length = strlen((char *)test_value);
  status = octetstring_init(
      &bacnet_string,
  &test_value[0],
  test_length);
  ct_test(pTest, status == true);
  length = octetstring_length(&bacnet_string);
  value = octetstring_value(&bacnet_string);
  ct_test(pTest, length == test_length);
  for (i = 0; i < test_length; i++)
  {
    ct_test(pTest, value[i] == test_value[i]);
  }

  test_length = strlen((char *)test_append_value);
  status = octetstring_append(
      &bacnet_string,
  &test_append_value[0],
  test_length);
  strcat((char *)test_append_string,(char *)test_value);
  strcat((char *)test_append_string,(char *)test_append_value);
  test_length = strlen((char *)test_append_string);
  ct_test(pTest, status == true);
  length = octetstring_length(&bacnet_string);
  value = octetstring_value(&bacnet_string);
  ct_test(pTest, length == test_length);
  for (i = 0; i < test_length; i++)
  {
    ct_test(pTest, value[i] == test_append_string[i]);
  }
}

#ifdef TEST_BACSTR
int main(void)
{
  Test *pTest;
  bool rc;

  pTest = ct_create("BACnet Strings", NULL);
  /* individual tests */
  rc = ct_addTestFunction(pTest, testBitString);
  assert(rc);
  rc = ct_addTestFunction(pTest, testCharacterString);
  assert(rc);
  rc = ct_addTestFunction(pTest, testOctetString);
  assert(rc);
    // configure output
  ct_setStream(pTest, stdout);
  ct_run(pTest);
  (void) ct_report(pTest);
  ct_destroy(pTest);

  return 0;
}
#endif                          /* TEST_BACSTR */
#endif                          /* TEST */
