#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sudoku_grid.h"
#include "queue.h"
#include "arraylist.h"
#include "helper.h"
#define NUM_THREADS 9

pthread_mutex_t fringe_mutex;
pthread_mutex_t explore_mutex;
pthread_cond_t work_available;
ArrayList *explored;
Queue *dfs_fringe;
int thread_waiting = 0;

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

  // initialize the queue with the input grid
  queue_push_head(fringe, grid);
  // Critical Section
  pthread_mutex_lock(&explore_mutex);
  arraylist_append(explored, grid);
  pthread_mutex_unlock(&explore_mutex);
  // End of Critical Section
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

        copy_grid(current_grid, new_grid);

        new_grid->cells[min_cell->row][min_cell->col].val = i + 1;

        if (check_grid_explored(explored, new_grid))
        {
          continue;
        }
        // Update possible values for the new grid
        reduce_possible_vals(new_grid, &new_grid->cells[min_cell->row][min_cell->col]);
        queue_push_tail(fringe, new_grid);
        // Critical Section
        pthread_mutex_lock(&explore_mutex);
        arraylist_append(explored, new_grid);
        pthread_mutex_unlock(&explore_mutex);
        // End of Critical Section
      }
    }
  }
  printf("No solution found :(\n");
  return 0;
}

void *dfs(void *arg)
{
  while (thread_waiting < NUM_THREADS)
  {
    pthread_mutex_lock(&fringe_mutex);

    thread_waiting++;

    if (thread_waiting == NUM_THREADS)
    {
      printf("No solution found :(\n");
      exit(1);
    }

    while (queue_is_empty(dfs_fringe))
    {
      pthread_cond_wait(&work_available, &fringe_mutex);
    }
    thread_waiting--;

    Grid *current_grid = (Grid *)queue_pop_head(dfs_fringe);
    if (check_grid(current_grid))
    {
      // print out solution
      printf("Solution found:\n");
      for (int i = 0; i < N; i++)
      {
        for (int j = 0; j < N; j++)
        {
          printf("%d ", current_grid->cells[i][j].val);
        }
        printf("\n");
      }
      exit(1);
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

        copy_grid(current_grid, new_grid);

        new_grid->cells[min_cell->row][min_cell->col].val = i + 1;

        if (check_grid_explored(explored, new_grid))
        {
          continue;
        }
        // Update possible values for the new grid
        reduce_possible_vals(new_grid, &new_grid->cells[min_cell->row][min_cell->col]);
        // Add grid to stack, then signal that work is available
        queue_push_head(dfs_fringe, new_grid);
        pthread_cond_broadcast(&work_available);
        // Critical Section
        pthread_mutex_lock(&explore_mutex);
        arraylist_append(explored, new_grid);
        pthread_mutex_unlock(&explore_mutex);
        // End of Critical Section
      }
    }
    pthread_mutex_unlock(&fringe_mutex);
  }
  return 0;
}

int main(void)
{
  pthread_mutex_init(&fringe_mutex, NULL);
  pthread_mutex_init(&explore_mutex, NULL);
  pthread_cond_init(&work_available, NULL);
  pthread_t *threads[NUM_THREADS];

  explored = arraylist_new(1000);
  dfs_fringe = queue_new();
  int algorithm;

  int input_grid[N][N];
  printf("Enter the sudoku grid:\n");
  for (int i = 0; i < 9; i++)
  {
    for (int j = 0; j < 9; j++)
    {
      scanf("%d", &input_grid[i][j]);
    }
  }
  printf("Choose the algorithm to solve this puzzle (0 for bfs, 1 for dfs): ");
  scanf("%d", &algorithm);
  printf("\n");

  Grid grid;
  init_grid(&grid, input_grid);

  if (check_grid(&grid))
  {
    printf("The grid is already solved!\n");
    return 0;
  }

  if (algorithm == 0)
  {
    // Find the cell that has the most possible values
    Cell *max_cell = find_max_cell(&grid);

    // find the number of possible values of max_cell
    int possible_vals[NUM_THREADS];
    int counter = 0;
    for (int i = 0; i < N; i++)
    {
      if (max_cell->possible_vals[i])
      {
        possible_vals[counter++] = i + 1;
      }
    }

    // Spawn a # of threads equal to the number of possible values of max_cell
    for (int threadCounter = 0; threadCounter < counter; threadCounter++)
    {
      threads[threadCounter] = (pthread_t *)malloc(sizeof(pthread_t));
      Grid *new_grid = (Grid *)malloc(sizeof(Grid));

      copy_grid(&grid, new_grid);

      new_grid->cells[max_cell->row][max_cell->col].val = possible_vals[threadCounter];
      reduce_possible_vals(new_grid, &new_grid->cells[max_cell->row][max_cell->col]);

      pthread_create(threads[threadCounter], NULL, bfs, new_grid);
    }

    // Wait for all threads to finish
    for (int threadCounter = 0; threadCounter < counter; threadCounter++)
    {
      pthread_join(*threads[threadCounter], NULL);
    }
  }
  else
  {
    // initialize the queue with the input grid
    queue_push_head(dfs_fringe, &grid);
    arraylist_append(explored, &grid);

    // Spawn threads to perform DFS
    for (int threadCounter = 0; threadCounter < NUM_THREADS; threadCounter++)
    {
      threads[threadCounter] = (pthread_t *)malloc(sizeof(pthread_t));

      pthread_create(threads[threadCounter], NULL, dfs, NULL);
    }

    for (int threadCounter = 0; threadCounter < NUM_THREADS; threadCounter++)
    {
      pthread_join(*threads[threadCounter], NULL);
    }
  }
  pthread_mutex_destroy(&fringe_mutex);
  pthread_mutex_destroy(&explore_mutex);

  return 0;
}
