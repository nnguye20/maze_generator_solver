#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "solver.h"

/*
 * Given a pointer to a maze_room, set its connections in all four directions
 *based on the hex value passed in.
 *
 * For example:
 *
 * 		create_room_connections(&maze[3][4], 0xb)
 *
 * 0xb is 1011 in binary, which means we have walls on all sides except the
 *WEST. This means that every connection in maze[3][4] should be set to 1,
 *except the WEST connection, which should be set to 0.
 *
 * Parameters:
 *	- room: pointer to the current room
 *	- hex: hexadecimal integer (between 0-15 inclusive) that represents the
 *connections in all four directions from the given room.
 *
 * Returns:
 *	- nothing. The connections should be saved in the maze_room struct
 *pointed to by the parameter (make sure to use pointers correctly!).
 */
void create_room_connections(struct maze_room *room, unsigned int hex) {
    // Initiliaze array of bit masks for each direction, 0001 0010 0100 1000
    int mask[4] = {0x1, 0x2, 0x4, 0x8};
    int i;
    // For each direction
    for (i = 0; i < 4; i++) {
        // If there is overlap between specified bitmask and hex
        if ((mask[i] & hex) != 0) {
            // There exists a wall in the given direction
            room->connections[i] = 1;
        } else {
            // There exists a connection in the given direction
            room->connections[i] = 0;
        }
    }
}

/*
 * Recursive depth-first search algorithm for solving your maze.
 * This function should also print out either every visited room as it goes, or
 * the final pruned solution, depending on whether the FULL macro is set.
 *
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- goal_row: row of the goal room
 *	- goal_col: col of the goal room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: a 2d array of maze_room structs representing your maze
 *	- file: the file to write the solution to
 *
 * Returns:
 *	- 1 if the current branch finds a valid solution, 0 if no branches are
 *valid.
 */
int dfs(int row, int col, int goal_row, int goal_col, int num_rows,
        int num_cols, struct maze_room maze[num_rows][num_cols], FILE *file) {
// For FULL, print row/col of each visited room
#ifdef FULL
    fprintf(file, "%d, ", row);
    fprintf(file, "%d\n", col);
#endif

    // If the goal room has been reached, return 1
    if ((row == goal_row) && (col == goal_col)) {
        return 1;
    }

    // Set current room as visited
    maze[row][col].visited = 1;

    // Initialize arrays holding row/col offset for North, South, West, East
    int newRowArray[4] = {-1, 1, 0, 0};
    int newColArray[4] = {0, 0, -1, 1};
    int i;
    // For each direction
    for (i = 0; i < 4; i++) {
        // Retrieve pointer to neighbor in direction
        struct maze_room *neighbor =
            &maze[row + newRowArray[i]][col + newColArray[i]];
        // If there is a connection and neighbor is not visited
        if ((maze[row][col].connections[i] == 0) && (neighbor->visited == 0)) {
            // Set next of current room to point to neighbor
            maze[row][col].next = neighbor;

            // Recurse
            if (dfs(neighbor->row, neighbor->col, goal_row, goal_col, num_rows,
                    num_cols, maze, file)) {
                return 1;
            }
        }
    }

    return 0;
}

/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 9
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["solver", <input maze file>, <number of rows>, <number of
 *columns> <output path file>, <starting row>, <starting column>, <ending row>,
 *<ending column>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */

int main(int argc, char **argv) {
    int num_rows, num_cols, start_row, start_col, goal_row, goal_col;
    char *maze_file;
    char *path_file;
    if (argc != 9) {
        printf("Incorrect number of arguments.\n");
        printf(
            "./solver <input maze file> <number of rows> <number of columns>");
        printf(" <output path file> <starting row> <starting column>");
        printf(" <ending row> <ending column>\n");
        return 1;
    } else {
        maze_file = argv[1];
        num_rows = atoi(argv[2]);
        num_cols = atoi(argv[3]);
        path_file = argv[4];
        start_row = atoi(argv[5]);
        start_col = atoi(argv[6]);
        goal_row = atoi(argv[7]);
        goal_col = atoi(argv[8]);
    }
    // Error checking to see if rows/cols are in range
    if ((num_rows <= 0) || (num_cols <= 0)) {
        fprintf(stderr, "error - row/col are either 0 or negative\n");
        return 1;
    }
    // Error checking to see if start rows/cols are in range
    if (is_in_range(start_row, start_col, num_rows, num_cols) == 0) {
        fprintf(stderr, "error, either row or col is out of range\n");
        return 1;
    }
    // Error checking to see if goal row/cols are in range
    if (is_in_range(goal_row, goal_col, num_rows, num_cols) == 0) {
        fprintf(stderr, "error, either row or col is out of range\n");
        return 1;
    }

    // Open the file to read from
    FILE *readfile = fopen(maze_file, "r");
    // Error check to see if file to read is null
    if (readfile == NULL) {
        fprintf(stderr, "error, read maze file is null!\n");
        return 1;
    }

    // Initialize array to store scanned hexidecimal at first index
    unsigned int hex[1];

    struct maze_room maze[num_rows][num_cols];
    int i;
    int j;
    // For each index in maze
    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_cols; j++) {
            // Create and initialize room
            struct maze_room room;
            room.row = i;
            room.col = j;
            room.visited = 0;
            room.next = NULL;
            // Retrieve corresponding hex value and create connections
            if (fscanf(readfile, "%1x", hex) < 0) {
                fprintf(stderr,
                        "error- fscanf read negative hex value from file");
                return 1;
            }

            create_room_connections(&room, hex[0]);

            maze[i][j] = room;
        }
    }

    fclose(readfile);

    // Open the file to write to
    FILE *writefile = fopen(path_file, "w");
    // Error check to see if path_file is null
    if (writefile == NULL) {
        fprintf(stderr, "error, output maze file is null!\n");
        return 1;
    }

    // Print FULL or PRUNED based on macro
#ifdef FULL
    fprintf(writefile, "FULL\n");
#else
    fprintf(writefile, "PRUNED\n");
#endif

    // Recursively find maze solution through dfs, passing in start indices
    if (dfs(start_row, start_col, goal_row, goal_col, num_rows, num_cols, maze,
            writefile) == 0) {
        fprintf(stderr, "dfs could not reach goal index!\n");
        return 1;
    }

#ifndef FULL

    // Create variable to hold maze_room for while loop
    struct maze_room *currRoom = &maze[start_row][start_col];
    // Until the currentRoom is the goal room, print row/col and iterate through
    // next pointers
    // while ((currRoom->row != goal_row) && (currRoom->col != goal_col)) {
    while (currRoom->next != NULL) {
        fprintf(writefile, "%d, ", currRoom->row);
        fprintf(writefile, "%d\n", currRoom->col);
        currRoom = currRoom->next;
    }
    fprintf(writefile, "%d, ", goal_row);
    fprintf(writefile, "%d\n", goal_col);
#endif

    fclose(writefile);

    return 0;
}
