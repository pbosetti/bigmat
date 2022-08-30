#include "libbmp.h"
#include <stdio.h>
#include "bigmat.h"
#include <string.h>

typedef struct bigmat {
  value_t *data;
  value_t *head;
  size_t rows, cols;
  size_t srow, scol;
  size_t erow, ecol;
  int submat;
} bm_t;

bm_t *bm_new(size_t rows, size_t cols) {
  bm_t *bm = malloc(sizeof(bm_t));
  size_t len = rows * cols;
  bm->data = calloc(len, sizeof(value_t));
  if (!bm->data) {
    perror("Cannot allocate mamory");
    return NULL;
  }
  memset(bm->data, 0, len * sizeof(value_t));
  bm->head = bm->data;
  bm->rows = rows;
  bm->cols = cols;
  bm->erow = rows;
  bm->ecol = cols;
  bm->srow = 0;
  bm->scol = 0;
  bm->submat = 0;
  return bm;
}

size_t bm_capa(const bm_t *bm) { return bm->cols * bm->rows; }

void bm_free(bm_t *bm) {
  if (bm->submat) {
    free(bm->data);
  }
  free(bm);
  bm = NULL;
}

size_t bm_append(bm_t *bm, const value_t *values, size_t len) {
  size_t capa = bm_capa(bm);
  size_t pos = bm->head - bm->data;
  if (pos + len > capa) {
    int newlen = ((capa + len) / bm->cols + 1) * bm->cols;
    bm->data = realloc(bm->data, newlen * sizeof(value_t));
    if (!bm->data) {
      perror("Cannot reallocate memory");
      return 0;
    }
    bm->erow = bm->rows = newlen / bm->cols;
    capa = newlen;
  }
  memcpy(bm->head, values, len * sizeof(value_t));
  bm->head += len;
  return capa;
}

size_t bm_rows(const bm_t *bm) {
  return bm->submat ? bm->erow - bm->srow : bm->rows;
}

size_t bm_cols(const bm_t *bm) {
  return bm->submat ? bm->ecol - bm->scol : bm->cols;
}

void bm_reset(bm_t *bm) {
  bm->head = bm->data;
  memset(bm->data, 0, bm_capa(bm) * sizeof(value_t));
}

static inline size_t idx(const bm_t *bm, size_t r, size_t c) {
  return (r + bm->srow) * bm->cols + c + bm->scol;
}

value_t bm_xy_get(const bm_t *bm, size_t row, size_t col) {
  if (row > bm_rows(bm) || col > bm_cols(bm)) {
    perror("Out of bound");
    return 0;
  } else {
    return bm->data[idx(bm, row, col)];
  }
}

void bm_xy_set(bm_t *bm, size_t row, size_t col, value_t v) {
  if (row > bm_rows(bm) || col > bm_cols(bm)) {
    perror("Out of bound");
  } else {
    bm->data[idx(bm, row, col)] = v;
  }
}

void bm_print(const bm_t *bm) {
  size_t i, j;
  for (i = 0; i < bm_rows(bm); i++) {
    printf("[%05lu] ", bm->cols * i);
    for (j = 0; j < bm_cols(bm); j++) {
      printf("%6d ", bm_xy_get(bm, i, j));
    }
    printf("\n");
  }
}

bm_t *bm_sub_new(const bm_t *bm, size_t srow, size_t nrow, size_t scol,
                         size_t ncol) {
  bm_t *n = malloc(sizeof(bm_t));
  memcpy(n, bm, sizeof(bm_t));
  n->submat = 1;
  n->srow = srow;
  n->scol = scol;
  n->erow = srow + nrow;
  n->ecol = scol + ncol;
  return n;
}

void bm_map(bm_t *bm, mapf_t f, void *ud) {
  size_t i = 0;
  if (bm->submat) {
    size_t j = 0;
    for (i = 0; i < bm_rows(bm); i++) {
      for (j = 0; j < bm_cols(bm); j++) {
        bm_xy_set(bm, i, j, f(bm_xy_get(bm, i, j), ud));
      }
    }
  }
  else {
    for (i = 0; i < bm_capa(bm); i++) {
      bm->data[i] = f(bm->data[i], ud);
    }
  }
}

value_t bm_reduce(const bm_t *bm, reducef_t f, void *ud) {
  size_t i = 0;
  value_t store = 0;
  if (bm->submat) {
    size_t j;
    for (i = 0; i < bm_rows(bm); i++) {
      for (j = 0; j < bm_cols(bm); j++) {
        store = f(bm_xy_get(bm, i, j), store, ud);
      }
    }
  }
  else {
    for (i = 0; i < bm_capa(bm); i++) {
      store = f(bm->data[i], store, ud);
    }
  }
  return store;
}

void bm_to_bmp(bm_t *bm, const char *filename, value_t min, value_t max) {
  size_t i, j;
  bmp_img img;
  value_t v = 0, rng = max - min;
  bmp_img_init_df(&img, bm_cols(bm), bm_rows(bm));
  for (i = 0; i < bm_cols(bm); i++) {
    for (j = 0; j < bm_rows(bm); j++) {
      v = bm_xy_get(bm, j, i);
      v = (float)(v - min) / rng * 255;
      v = v < 0 ? 0 : v;
      v = v > 255 ? 255 : v;
      bmp_pixel_init(&img.img_pixels[j][i], v, v, v);
    }
  }
  bmp_img_write(&img, filename);
  bmp_img_free(&img);
}


#ifdef TEST_MAIN

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
#endif