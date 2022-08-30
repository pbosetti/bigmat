/*
  bigmat.h - Simple Big Matrix storage
  Paolo Bosetti 2022
 */
#ifndef BIGMAT_H
#define BIGMAT_H

#include "defines.h"

// Opaque object
typedef struct bigmat bm_t;

// create new matrix
// Matrix storage is row-first: elements are stored as a single vector
// filling rows before columns
bm_t *bm_new(size_t rows, size_t cols);

// matrix storage capacity
size_t bm_capa(const bm_t *bm);

// free memory and delete object
// if it is a submatrix, the storage is left untouched
void bm_free(bm_t *bm);

// append a vector to the matrix object, BY ROW
// if the storage capacity is not sufficient, the matrix grows by keeping
// the same number of columns but increasing the number of rows.
// The insertion point is internally stored and automatically incremented
size_t bm_append(bm_t *bm, const value_t *values, size_t len);

// get matrix dimensions
size_t bm_rows(const bm_t *bm);
size_t bm_cols(const bm_t *bm);

// reset the matrix
// set all values to 0 and reset the insertion point to the first position
void bm_reset(bm_t *bm);

// get value at row, col
value_t bm_xy_get(const bm_t *bm, size_t row, size_t col);

// set value at row, col
void bm_xy_set(bm_t *bm, size_t row, size_t col, value_t v);

// print a matrix description
void bm_print(const bm_t *bm);

// create a submatrix
// it shares the storage with the parent one
bm_t *bm_sub_new(const bm_t *bm, size_t srow, size_t nrow, size_t scol, size_t ncol);

// prototypes for map&reduce user functions
typedef value_t (* mapf_t)(value_t val, void *ud);
typedef value_t (* reducef_t)(value_t val, value_t store, void *ud);

// map a function over all elements
void bm_map(bm_t *bm, mapf_t f, void *ud);

// reduce to a single value by recursively apply a function to all elements
value_t bm_reduce(const bm_t *bm, reducef_t f, void *ud);

// save a BMP image of the matrix, rescaling values 
// between min (black) and max (white)
void bm_to_bmp(bm_t *bm, const char *filename, value_t min, value_t max);

#endif