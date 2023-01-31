#ifndef HELPER_H
#define HELPER_H
#include "sudoku_grid.h"

/**
 * Deep copy the original grid to a new grid address
 *
 * @param origin_grid      The original grid pointer.
 * @param target_grid      The target grid pointer.
 */
void copy_grid(Grid *origin_grid, Grid *target_grid);

#endif
