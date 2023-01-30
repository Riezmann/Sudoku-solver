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

bool check_val_in_row(struct Grid *grid, struct Cell *cell, int val);
bool check_val_in_col(struct Grid *grid, struct Cell *cell, int val);
bool check_val_in_box(struct Grid *grid, struct Cell *cell, int val);
bool check_grid(struct Grid *grid);
bool are_grids_equal(Grid *grid1, Grid *grid2);
void find_possible_vals(struct Grid *grid, struct Cell *cell);
void reduce_possible_vals(struct Grid *grid, struct Cell *edited_cell);
void init_grid(struct Grid *grid, int grid_values[N][N]);
Cell *find_min_cell(struct Grid *grid);
Cell *find_max_cell(struct Grid *grid);
#endif // SUDOKU_GRID_H
