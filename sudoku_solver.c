#include <stdio.h>
#include <stdlib.h>
#include "sudoku_grid.h"
#include "queue.h"
#include "arraylist.h"

bool check_grid_explored(ArrayList *explored, Grid *grid)
{
  for (int i = 0; i < explored->length; i++)
  {
    if (are_grids_equal((Grid *)(explored->data[i]), grid))
    {
      return true;
    }
  }
  return false;
}

void bfs(Queue *fringe, ArrayList *explored, Grid *grid)
{
  queue_push_head(fringe, grid);
  arraylist_append(explored, grid);
  while (!queue_is_empty(fringe))
  {
    Grid *current_grid = (Grid *)queue_pop_head(fringe);
    if (check_grid(current_grid))
    {
      grid = current_grid;
      // print out solution
      printf("Solution found:\n");
      for (int i = 0; i < N; i++)
      {
        for (int j = 0; j < N; j++)
        {
          printf("%d ", grid->cells[i][j].val);
        }
        printf("\n");
      }
      return;
    }
    else
    {
      Cell *min_cell = find_min_cell(current_grid);
      for (int i = 0; i < N; i++)
      {
        if (!min_cell->possible_vals[i])
        {
          continue;
        }
        Grid *new_grid = (Grid *)malloc(sizeof(Grid));
        
        int grid2d[N][N];
        for (int localRow = 0; localRow < N; localRow++)
        {
          for (int localCol = 0; localCol < N; localCol++)
          {
            grid2d[localRow][localCol] = current_grid->cells[localRow][localCol].val;
          }
        }
        init_grid(new_grid, grid2d);
        new_grid->cells[min_cell->row][min_cell->col].val = i + 1;

        if (check_grid_explored(explored, new_grid))
        {
          continue;
        }
        reduce_possible_vals(new_grid, &new_grid->cells[min_cell->row][min_cell->col]);
        queue_push_tail(fringe, new_grid);
        arraylist_append(explored, new_grid);
      }
    }
  }
  printf("No solution found\n");
}

int main(void)
{
  int input_grid[N][N];

  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      scanf("%d", &input_grid[i][j]);
    }
  }

  printf("\n");

  Grid grid;
  init_grid(&grid, input_grid);

  // code to solve the sudoku grid using BFS algorithm can be written here
  Queue *fringe = queue_new();
  ArrayList *explored = arraylist_new(1000);

  Cell *min_cell = find_min_cell(&grid);

  bfs(fringe, explored, &grid);

  return 0;
}
