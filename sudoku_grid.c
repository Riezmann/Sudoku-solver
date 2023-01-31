#include "sudoku_grid.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool are_grids_equal(Grid *grid1, Grid *grid2)
{
  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      if (grid1->cells[i][j].val != grid2->cells[i][j].val)
      {
        return false;
      }
    }
  }

  return true;
}

bool check_grid(struct Grid *grid)
{
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      if (grid->cells[i][j].val == 0)
      {
        return false;
      }
    }
  }
  return true;
}

bool check_val_in_row(struct Grid *grid, struct Cell *cell, int val)
{
  int i;
  for (i = 0; i < N; i++)
  {
    if (grid->cells[cell->row][i].col != cell->col && grid->cells[cell->row][i].row == cell->row && grid->cells[cell->row][i].val == val)
    {
      return false;
    }
  }
  return true;
}

bool check_val_in_col(struct Grid *grid, struct Cell *cell, int val)
{
  int i;
  for (i = 0; i < N; i++)
  {
    if (grid->cells[i][cell->col].row != cell->row && grid->cells[i][cell->col].col == cell->col && grid->cells[i][cell->col].val == val)
    {
      return false;
    }
  }
  return true;
}

bool check_val_in_box(struct Grid *grid, struct Cell *cell, int val)
{
  int box_row = (cell->row / 3) * 3;
  int box_col = (cell->col / 3) * 3;
  int i;
  for (i = 0; i < N; i++)
  {
    if (grid->cells[box_row + i / 3][box_col + i % 3].row != cell->row && grid->cells[box_row + i / 3][box_col + i % 3].col != cell->col && grid->cells[box_row + i / 3][box_col + i % 3].val == val)
    {
      return false;
    }
  }
  return true;
}

void find_possible_vals(struct Grid *grid, struct Cell *cell)
{
  int i, j, val;
  memset(cell->possible_vals, 0, sizeof(cell->possible_vals));
  cell->num_possible_vals = 0;
  for (val = 1; val <= N; val++)
  {
    bool val_not_in_row = check_val_in_row(grid, cell, val);
    bool val_not_in_col = check_val_in_col(grid, cell, val);
    bool val_not_in_box = check_val_in_box(grid, cell, val);

    if (val_not_in_row && val_not_in_col && val_not_in_box)
    {
      cell->possible_vals[val - 1] = true;
      cell->num_possible_vals++;
    }
  }
}

void reduce_possible_vals(struct Grid *grid, struct Cell *edited_cell)
{
  int row = edited_cell->row;
  int col = edited_cell->col;
  int val = edited_cell->val;

  for (int i = 0; i < 9; i++)
  {
    if (grid->cells[row][i].possible_vals[val - 1])
    {
      grid->cells[row][i].possible_vals[val - 1] = false;
      grid->cells[row][i].num_possible_vals--;
    }
    if (grid->cells[i][col].possible_vals[val - 1])
    {
      grid->cells[i][col].possible_vals[val - 1] = false;
      grid->cells[i][col].num_possible_vals--;
    }
  }

  int box_start_row = (row / 3) * 3;
  int box_start_col = (col / 3) * 3;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      int curr_row = box_start_row + i;
      int curr_col = box_start_col + j;
      if (grid->cells[curr_row][curr_col].possible_vals[val - 1])
      {
        grid->cells[curr_row][curr_col].possible_vals[val - 1] = false;
        grid->cells[curr_row][curr_col].num_possible_vals--;
      }
    }
  }
}

void init_grid(struct Grid *grid, int grid_values[N][N])
{
  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      grid->cells[i][j].row = i;
      grid->cells[i][j].col = j;
      grid->cells[i][j].val = grid_values[i][j];
    }
  }

  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      if (grid_values[i][j] == 0)
      {
        grid->cells[i][j].num_possible_vals = 9;
        for (int k = 0; k < 9; k++)
        {
          grid->cells[i][j].possible_vals[k] = true;
        }
      }
      else
      {
        grid->cells[i][j].num_possible_vals = 0;
        for (int k = 0; k < 9; k++)
        {
          grid->cells[i][j].possible_vals[k] = false;
        }
      }
    }
  }

  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      if (grid_values[i][j] != 0)
      {
        reduce_possible_vals(grid, &grid->cells[i][j]);
      }
    }
  }
}

Cell *find_min_cell(struct Grid *grid)
{
  int min_possible_vals = 10;
  struct Cell *min_cell = NULL;

  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      if (grid->cells[i][j].val == 0 && grid->cells[i][j].num_possible_vals < min_possible_vals)
      {
        min_possible_vals = grid->cells[i][j].num_possible_vals;
        min_cell = &grid->cells[i][j];
      }
    }
  }

  return min_cell;
}

Cell *find_max_cell(struct Grid *grid)
{
  int max_possible_vals = -1;
  Cell *max_cell = NULL;

  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      if (grid->cells[i][j].val == 0 && grid->cells[i][j].num_possible_vals > max_possible_vals)
      {
        max_possible_vals = grid->cells[i][j].num_possible_vals;
        max_cell = &grid->cells[i][j];
      }
    }
  }

  return max_cell;
}


