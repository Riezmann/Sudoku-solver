#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sudoku_grid.h"
#include "queue.h"
#include "arraylist.h"
#define NUM_THREADS 9

pthread_mutex_t mutex;
ArrayList *explored;

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

void *bfs(void *intputGrid)
{
  Grid *grid = (Grid *)intputGrid;
  Queue *fringe = queue_new();
  queue_push_head(fringe, grid);
  pthread_mutex_lock(&mutex);
  arraylist_append(explored, grid);
  pthread_mutex_unlock(&mutex);
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
      return 0;
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
        pthread_mutex_lock(&mutex);
        arraylist_append(explored, new_grid);
        pthread_mutex_unlock(&mutex);
      }
    }
  }
  printf("No solution found\n");
  return 0;
}

int main(void)
{
  pthread_mutex_init(&mutex, NULL);
  explored = arraylist_new(1000);
  pthread_t *threads[NUM_THREADS];

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
  Cell *min_cell = find_min_cell(&grid);

  Cell *max_cell = find_max_cell(&grid);

  // first 2 possible values of max_cell
  int possible_vals[NUM_THREADS];
  int counter = 0;
  for (int i = 0; i < N; i++)
  {
    if (max_cell->possible_vals[i])
    {
      possible_vals[counter++] = i + 1;
    }
  }

  for (int threadCounter = 0; threadCounter < counter; threadCounter++)
  {
    threads[threadCounter] = (pthread_t *)malloc(sizeof(pthread_t));
    Grid *new_grid = (Grid *)malloc(sizeof(Grid));

    int grid2d[N][N];
    for (int localRow = 0; localRow < N; localRow++)
    {
      for (int localCol = 0; localCol < N; localCol++)
      {
        grid2d[localRow][localCol] = grid.cells[localRow][localCol].val;
      }
    }
    init_grid(new_grid, grid2d);

    new_grid->cells[max_cell->row][max_cell->col].val = possible_vals[threadCounter];
    reduce_possible_vals(new_grid, &new_grid->cells[max_cell->row][max_cell->col]);

    pthread_create(threads[threadCounter], NULL, bfs, new_grid);
  }

  for (int threadCounter = 0; threadCounter < counter; threadCounter++)
  {
    pthread_join(*threads[threadCounter], NULL);
  }

  // bfs(&grid);

  pthread_mutex_destroy(&mutex);

  return 0;
}
