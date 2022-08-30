#include "../bigmat.h"
#include <stdio.h>

value_t scale(value_t v, void *fac) {
  return v * *(value_t *)fac;
}

value_t sum(value_t v, value_t store, void *ud) {
  return store + v;
}

value_t max(value_t v, value_t store, void *ud) {
  return v > store ? v : store;
}

int main() {
  bm_t *bm = bm_new(5, 10);
  bm_t *sm;
  value_t fac = 3, max_v = 0;
  size_t i;
  value_t ary[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  for (i = 0; i < 55; i++) {
    bm_append(bm, (value_t *)&i, 1);
  }
  // bm_append(bm, ary, 11);
  bm_print(bm);
  sm = bm_sub_new(bm, 3, 3, 4, 4);
  printf("\nSubmatrix:\n");
  bm_xy_set(sm, 0, 1, 100);
  bm_print(sm);
  printf("\n");

  bm_map(sm, scale, &fac);
  bm_print(bm);
  printf("\n");

  printf("Mat Sum: %d\n", bm_reduce(bm, sum, NULL));
  printf("SubMat Sum: %d\n", bm_reduce(sm, sum, NULL));

  max_v = bm_reduce(bm, max, NULL);

  bm_to_bmp(bm, "bigmat.bmp", 0, max_v);

  bm_free(sm);
  bm_free(bm);
  return 0;
}
