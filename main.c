#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define ROWS 20
#define COLS 40
#define MAX_GEN 500
#define SLEEP 10000

typedef unsigned short int usi;

// Do the initial matrix setup
void setup(int m[ROWS][COLS]){
    for(usi i = 0; i < ROWS; i++){
        for(usi j = 0; j < COLS; j++){
            m[i][j] = rand() % 2;
        }
    }
}

// Print the matrix
void print_matrix(int m[ROWS][COLS]){
    system("@cls||clear");
    for(usi i = 0; i < ROWS; i++){
        for(usi j = 0; j < COLS; j++){
            if(m[i][j]){
                printf("\033[0;35m");
                printf("%c ", (char)254u);
            }
            else{
                printf("\033[0m");
                printf("  ");
            }
        }
        printf("\n");
    }
    printf("\033[0m");
}

// Searched the cache for a pattern
int exists_in_cache(int m[ROWS][COLS], int* cache[], int n){
    int exists = 0;

    int i;
    for(i = n - 1; i >= 0 && !exists; i--){
        exists = memcmp(m, cache[i], ROWS * COLS * sizeof(int)) == 0;
    }

    if(exists){
        printf("\033[0;31m");
        printf("Loop detected in %d. generation. Started in %d. generation. Lasted for %d generations.\n", n+1, i, (n - i + 1));
    }
    printf("\033[0m");

    return exists;
}

// Adds a record in cache
void add_to_cache(int* cache[], int m[ROWS][COLS], int i){
    int* matrix = malloc(ROWS * COLS * sizeof(int));
    memcpy(matrix, m, ROWS * COLS * sizeof(int));
    cache[i] = matrix;
}

// Gets the count of alive members for each cell
usi get_count_of_alive_neighbours(int m[ROWS][COLS], usi i, usi j){
    usi count = 0;

    // Top
    if((i - 1 >= 0 && m[i - 1][j]) || (i - 1 < 0 && m[ROWS-1][j])) count++;
    // Right
    if((j + 1 < COLS && m[i][j + 1]) || (j + 1 == COLS && m[i][0])) count++;
    // Bottom
    if((i + 1 < ROWS && m[i + 1][j]) || (i + 1 == ROWS && m[0][j])) count++;
    // Left
    if((j - 1 >= 0 && m[i][j - 1]) || (j - 1 < 0 && m[i][COLS-1])) count++;

    // Top-left
    if((j - 1 >= 0 && i - 1 >= 0 && m[i - 1][j - 1]) ||
       (j - 1 < 0 && i - 1 < 0 && m[ROWS-1][COLS-1])) count++;
    // Top-right
    if((j + 1 < COLS && i - 1 >= 0 && m[i - 1][j + 1]) ||
       j + 1 == COLS && i - 1 < 0 && m[ROWS-1][0]) count++;
    // Bottom-right
    if((j + 1 < COLS && i + 1 < ROWS && m[i + 1][j + 1]) ||
       (j + 1 == COLS && i + 1 == ROWS && m[0][0])) count++;
    // Bottom-left
    if((j - 1 >= 0 && i + 1 < ROWS && m[i + 1][j - 1]) ||
       (j - 1 < 0 && i + 1 == ROWS && m[0][COLS - 1])) count++;

    return count;
}

// Determines if the cell should live
int should_live(int m[ROWS][COLS], usi i, usi j){
    usi alive = get_count_of_alive_neighbours(m, i, j);

    if(m[i][j] && (alive < 2 || alive > 3)) return 0;
    if((m[i][j] && (alive == 2 || alive == 3)) || (!m[i][j] && alive == 3)) return 1;
}

// Every tick this function will be called
int tick(int current_gen[ROWS][COLS], int next_gen[ROWS][COLS], int* cache[], int* generations){
    // Go through the current generation, but save changes to the next generation
    for(usi i = 0; i < ROWS; i++){
        for(usi j = 0; j < COLS; j++){
            next_gen[i][j] = should_live(current_gen, i, j);
        }
    }

    // Cache the next gen
    add_to_cache(cache, current_gen, *generations);

    // If the current and next state are the same, do not tick again (loop or all dead)
    int continue_evolution = !exists_in_cache(next_gen, cache, *generations);

    // Copy from next to current
    memcpy(current_gen, next_gen, ROWS * COLS * sizeof(int));

    // Increase generation count by 1
    (*generations)++;

    // Return if loop should continue or not
    return continue_evolution;
}

int main()
{
    // Seed for random
    srand(time(NULL));

    // Set up vars
    int generations = 0;
    int* cache[MAX_GEN];
    int current_gen[ROWS][COLS];
    int next_gen[ROWS][COLS];

    // Generate matrix
    setup(current_gen);

    // Do the evolution
    int continue_evolution = 1;
    while(generations < MAX_GEN && continue_evolution){
        print_matrix(current_gen);
        continue_evolution = tick(current_gen, next_gen, cache, &generations);
        usleep(SLEEP);
    }

    // Print number of generations
    printf("Generations: %d\n", generations);
    return 0;
}
