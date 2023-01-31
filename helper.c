#include <stdlib.h>
#include "sudoku_grid.h"

void copy_grid(Grid *origin_grid, Grid *target_grid)
{
  int grid2d[N][N];
  for (int localRow = 0; localRow < N; localRow++)
  {
    for (int localCol = 0; localCol < N; localCol++)
    {
      grid2d[localRow][localCol] = origin_grid->cells[localRow][localCol].val;
    }
  }
  init_grid(target_grid, grid2d);
}
