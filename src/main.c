/*
 * main.c: entry point for lfsrcrack
 * Creation date: Thu Sep 21 19:57:21 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <lfsrcrack.h>

char *
apply_berlekamp_massey(const char *s, unsigned int size, unsigned int *len)
{
  char *b = NULL;
  char *c = NULL;
  char *t = NULL;
  char *result = NULL;

  int i;
  int d;
  int N;
  int L = 0;
  int m = -1;

  /* Vector initialization */
  if ((b = calloc(1, size)) == NULL)
    goto done;

  if ((c = calloc(1, size)) == NULL)
    goto done;

  if ((t = malloc(size)) == NULL)
    goto done;

  b[0] = 1;
  c[0] = 1;

  for (N = 0; N < size; ++N) {
    d = s[N];
    for (i = 1; i <= L; ++i)
      d ^= c[i] & s[N - i];

    /* Discrepancy is found! */
    if (d) {
      memcpy(t, c, size); /* Save copy of C */

      for (i = N - m; i < size; ++i)
        c[i] ^= b[i - (N - m)];

      if (L <= N / 2) {
        L = N + 1 - L;
        m = N;
        memcpy(b, t, size);
      }
    }
  }

  result = c;
  c = NULL;
  *len = L;

done:
  if (b != NULL)
    free(b);

  if (c != NULL)
    free(c);

  if (t != NULL)
    free(t);

  return result;
}

void
descramble(const char *poly, size_t poly_size, const char *bits, size_t size)
{
  int i, j, iters;
  unsigned int out;

  if (size <= poly_size) {
    printf("Too few input bits (input at least %d)\n", poly_size + 1);
    return;
  } else {
    for (i = 0; i < size - poly_size; ++i) {
      out = 0;

      for (j = 0; j <= poly_size; ++j)
        out ^= poly[poly_size - j] & bits[i + j];

      putchar('0' + out);
    }
  }
}

int
main (int argc, char *argv[], char *envp[])
{
  char *bits, nibble;
  char *direct_poly;
  char *inverse_poly;
  unsigned int direct_len;
  unsigned int inverse_len;
  unsigned int i, j;
  unsigned int size;
  char *buffer = NULL;
  void *tmp;
  unsigned int storage = 0;
  int c, p = 0;

  if (argc != 2) {
    fprintf(stderr, "%s: sequence is missing!\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  bits = argv[1];
  size = strlen(bits);

  if (strncmp(bits, "hex:", 4) == 0) {
    /* Let us process this */
    c = 0;
    for (i = 4; i < size; ++i) {
      if (isspace(bits[i]))
        continue;
      if (!isxdigit(bits[i])) {
        fprintf(stderr, "%s: malformed hex digit %c\n", argv[0], bits[i]);
        exit(EXIT_FAILURE);
      }
      ++c;
    }

    if (c == 0) {
      fprintf(stderr, "%s: no bits\n", argv[0]);
      exit(EXIT_FAILURE);
    }
    
    bits = malloc(c * 4);
    if (bits == NULL)
      abort();

    c = 0;
    for (i = 4; i < size; ++i) {
      if (isspace(argv[1][i]))
        continue;

      nibble  = tolower(argv[1][i]);
      nibble -= isdigit(nibble) ? '0' : 'a' - 0xa;

      for (j = 0; j < 4; ++j) {
        bits[c++] = !!(nibble & (1 << (3 - j)));
      }
    }

    size = c;
  } else {  
    for (i = 0; i < size; ++i) {
      switch (bits[i]) {
      case '0':
        bits[i] = 0;
        break;

      case '1':
        bits[i] = 1;
        break;

      default:
        fprintf(
          stderr,
          "%s: invalid bit in sequence at character %d\n",
          argv[0],
          i + 1);
        exit(EXIT_FAILURE);
      }
    }
  }
  /* Compute direct polynomial */
  fprintf(stderr, "Direct: ");
  if ((direct_poly = apply_berlekamp_massey(bits, size, &direct_len)) == NULL) {
    fprintf(stderr, "%s: failed\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < direct_len; ++i)
    if (direct_poly[i]) {
      if (direct_len - i > 1)
        fprintf(stderr, "x^%d + ", direct_len - i);
      else
        fprintf(stderr, "x + ");
    }

  fprintf(stderr, "1\n");
  fprintf(stderr, "D-form: ");
  for (i = 0; i < direct_len; ++i)
    if (direct_poly[i])
      fprintf(stderr, "D[%2d], ", i);

  fprintf(stderr, "D[%2d]\n\n", direct_len);
  
  for (i = 0; i < size; ++i)
    bits[i] = !bits[i];

  /* Compute inverse polynomial */
  fprintf(stderr, "Inverse: ");
  if ((inverse_poly = apply_berlekamp_massey(bits, size, &inverse_len)) == NULL) {
    fprintf(stderr, "%s: failed\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < inverse_len; ++i)
    if (inverse_poly[i]) {
      if (inverse_len - i > 1)
        fprintf(stderr, "x^%d + ", inverse_len - i);
      else
        fprintf(stderr, "x + ");
    }

  fprintf(stderr, "1\n");

  fprintf(stderr, "D-form: ");
  for (i = 0; i < inverse_len; ++i)
    if (inverse_poly[i])
      fprintf(stderr, "D[%2d], ", i);

  fprintf(stderr, "D[%2d]\n\n", inverse_len);
  fprintf(stderr, "Input some bits to descramble:\n");

  size = 0;

  while ((c = getchar()) != EOF) {
    if (!isspace(c)) {
      if (size >= storage) {
        if (storage == 0)
          storage = 1;
        else
          storage <<= 1;

        if ((tmp = realloc(buffer, storage)) == NULL) {
          fprintf(stderr, "%s: memory exhausted\n", argv[0]);
          exit(EXIT_FAILURE);
        }

        buffer = tmp;
      }

      switch (c) {
        case '0':
          buffer[size++] = 0;
          break;

        case '1':
          buffer[size++] = 1;
          break;

        default:
          fprintf(
              stderr,
              "%s: invalid bit in sequence at character %d\n",
              argv[0],
              size + 1);
          continue;
      }
    }
  }

break_loop:
  fprintf(stderr, "Direct descrambling (%d bits):\n", size);
  descramble(direct_poly, direct_len, buffer, size);
  putchar(10);

  for (i = 0; i < size; ++i)
    buffer[i] = !buffer[i];

  fprintf(stderr, "Inverse descrambling (%d bits):\n", size);
  descramble(inverse_poly, inverse_len, buffer, size);
  putchar(10);

  free(inverse_poly);
  free(direct_poly);

  return 0;
}

