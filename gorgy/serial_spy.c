#include <stdio.h>
#include <stdlib.h>

#include "tty.h"

#define BUFFER_SIZE 1000



extern int fd;
static  unsigned char buffer[BUFFER_SIZE];
static  unsigned int index;

static void store (unsigned char oct) {
  if (index == BUFFER_SIZE - 1) {
    index = 0;
  }
  buffer[index] = oct;
  index ++;
}

static void print (char oct) {
    if (oct < 32) printf ("'%02X' ", (int)oct);
    else printf ("%c ", oct);
}

static void flush (void) {
  unsigned int i;

  for (i = 0; i < index; i++) {
    print (buffer[i]);
  }
  printf ("\n");
  fflush(stdout);
  index = 0;
}


int main(int argc, char *argv[]) {
  unsigned char oct;
#if defined(DACOTA)
  unsigned char prev_oct, prev_prev_oct;
#endif

  int started;

  if ( argc != 2) {
    printf ("SYNTAX ERROR. Usage : serial_spy <tty_no>:<stopb>:<datab>:<parity>:<bauds>\n");
    exit (1);
  }

  init_tty(argv[1], 1);

  started = 0;
#if defined(DACOTA)
  prev_oct = '0';
  prev_prev_oct = '0';
#endif
  index = 0;

  for(;;) {
    read_tty (&oct, 1);
#if defined(STANDARD) || defined(TAAATS)
    if (oct == 0x02) started = 1;
    if (started) {
      store (oct);
      if (oct == 0x0d) flush();
    }
#elif defined(PALLAS)
    if (oct == 'T') started = 1;
    if (started) {
      store (oct);
      if (oct == 0x0a) flush ();
    }
#elif defined(DACOTA)
    if (oct == 0x01) started = 1;
    if (started) {
      if ( (oct == 0x01) && (prev_prev_oct == 0x04) ) flush ();
      store (oct);
    }
    prev_prev_oct = prev_oct;
    prev_oct = oct;
#else
define STANDARD or TAAATS or PALLAS or DACOTA
#endif
  }
}

