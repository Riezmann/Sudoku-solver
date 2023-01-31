#ifndef SUDOKU_GRID_H
#define SUDOKU_GRID_H
#include <stdbool.h>
#define N 9

typedef struct Cell
{
  int row, col;
  int val;
  int num_possible_vals;
  bool possible_vals[N];
} Cell;

typedef struct Grid
{
  struct Cell cells[N][N];
} Grid;

/**
 * Check if a value is already in a row
 *
 * @param grid      The grid pointer.
 * @param cell      The cell pointer.
 * @param val       The value to check.
 */
bool check_val_in_row(struct Grid *grid, struct Cell *cell, int val);

/**
 * Check if a value is already in a column
 *
 * @param grid      The grid pointer.
 * @param cell      The cell pointer.
 * @param val       The value to check.
 */
bool check_val_in_col(struct Grid *grid, struct Cell *cell, int val);

/**
 * Check if a value is already in a box
 *
 * @param grid      The grid pointer.
 * @param cell      The cell pointer.
 * @param val       The value to check.
 */
bool check_val_in_box(struct Grid *grid, struct Cell *cell, int val);

/**
 * Check if the puzzle is solved
 *
 * @param grid      The grid pointer.
 */
bool check_grid(struct Grid *grid);

/**
 * Compare if two grids are equal
 *
 * @param grid1     The first grid pointer.
 * @param grid2     The second grid pointer.
 */
bool are_grids_equal(Grid *grid1, Grid *grid2);

/**
 * Find the possible values for a cell
 *
 * @param grid      The grid pointer.
 * @param cell      The cell pointer.
 */
void find_possible_vals(struct Grid *grid, struct Cell *cell);

/**
 * Reduce the possible values for the cells in the same row, column and box with the edited cell
 *
 * @param grid      The grid pointer.
 * @param cell      The cell pointer.
 */
void reduce_possible_vals(struct Grid *grid, struct Cell *edited_cell);

/**
 * Initialize the grid object from the 2D input.
 *
 * @param grid          The grid pointer.
 * @param grid_values   The 2D array of values.
 */
void init_grid(struct Grid *grid, int grid_values[N][N]);

/**
 * Find cell with minimum number of possible values
 *
 * @param grid      The grid pointer.
 */
Cell *find_min_cell(struct Grid *grid);

/**
 * Find cell with maximum number of possible values
 *
 * @param grid      The grid pointer.
 */
Cell *find_max_cell(struct Grid *grid);
#endif // SUDOKU_GRID_H
