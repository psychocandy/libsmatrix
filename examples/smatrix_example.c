// This file is part of the "libsmatrix" project
//   (c) 2011-2013 Paul Asmuth <paul@paulasmuth.com>
//
// Licensed under the MIT License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of
// the License at: http://opensource.org/licenses/MIT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "smatrix.h"

smatrix_t* db;

void* test(void* fnord) {
  uint64_t i, n, m;

  for (m = 0; m < 100; m++) {
    for (n = 1; n < 30; n++) {
      for (i = 1; i < 50; i++) {
        smatrix_incr(db, n, i, 1);
      }
    }
  }

  return NULL;
}

int main(int argc, char **argv) {
  int i,n,m,l,x=0, num_threads = 4;
  pthread_t threads[num_threads];

  printf("\nloading\n");
  db = smatrix_open("/var/tmp/reco.db");
  //db = smatrix_open(NULL); // in-memory only mode

  if (db == NULL)
    abort();

  printf("\nstarting\n");

  for (n = 0; n < num_threads; n++)
    pthread_create(&threads[n], NULL, test, NULL);

  for (n = 0; n < num_threads; n++)
    pthread_join(threads[n], NULL);

  printf("\ndone\n");

  for (n = 1; n < 30; n++) {
    for (i = 1; i < 50; i++) {
      printf("(%u,%u) => %u, ", n, i, smatrix_get(db, n, i));
      if (x++ % 5 == 0) printf("\n");
    }
  }

  printf("rowlen: %u\n", l = smatrix_rowlen(db, 23));
  size_t bytes = sizeof(uint32_t) * l * 2;
  uint32_t* data = malloc(bytes);

  l = smatrix_getrow(db, 23, data, bytes);

  for (i = 0; i < l; i++) {
    printf("%u => %u, ", data[i * 2], data[i * 2 + 1]);
  }

  printf("\n");

  smatrix_close(db);
  printf("in use at exit: %lu\n", db->mem);

  return 0;
}
