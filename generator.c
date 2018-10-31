#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generator.h"

/*
 * Given a direction, get its opposite
 *
 * Parameters:
 *	- dir: direction
 *
 * Returns:
 *	- the opposite direction to dir
 */
Direction get_opposite_dir(Direction dir) {
    if (dir == NORTH) {
        return SOUTH;
    } else if (dir == SOUTH) {
        return NORTH;
    } else if (dir == EAST) {
        return WEST;
    } else {
        return EAST;
    }
}

/*
 * Given an array of the four directions, randomizes the order of the directions
 *
 * Parameters:
 *	- directions: an array of Direction enums of size 4
 *
 * Returns:
 *	- nothing - the array should be shuffled in place
 */
void shuffle_array(Direction directions[]) {
    int i;
    // For each index
    for (i = 0; i < 4; i++) {
        // Get random number from 0 to 3 inclusive
        int randNum = rand() % (4);
        // Swap item at index i with item at random index
        Direction temp = directions[i];
        directions[i] = directions[randNum];
        directions[randNum] = temp;
    }
}

/*
 * Recursive function for stepping through a maze and setting its
 *connections/walls using the drunken_walk algorithm
 *
 * Parameters:
 * 	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: a 2D array of maze_room structs representing your maze
 * Returns:
 *	- nothing - the state of your maze should be saved in the maze array
 *being passed in (make sure to use pointers correctly!).
 */
void drunken_walk(int row, int col, int num_rows, int num_cols,
                  struct maze_room maze[num_rows][num_cols]) {
    struct maze_room *room = &maze[row][col];
    room->visited = 1;

    // Create an array of all 4 directions and randomize order
    Direction directions[4] = {NORTH, SOUTH, WEST, EAST};
    shuffle_array(directions);

    int i;
    int newRow;
    int newCol;

    // For each direction in random order
    for (i = 0; i < 4; i++) {
        // Reset newRow/newCol to be current room indices
        newRow = row;
        newCol = col;
        // Offset row/col based on given direction
        if (directions[i] == NORTH) {
            newRow--;
        } else if (directions[i] == EAST) {
            newCol++;
        } else if (directions[i] == SOUTH) {
            newRow++;
        } else if (directions[i] == WEST) {
            newCol--;
        }
        // Convenience variable to hold int value of specified direction
        int dir = directions[i];
        // If the neighbor would be out of range, create wall
        if (is_in_range(newRow, newCol, num_rows, num_cols) == 0) {
            room->connections[dir] = 1;

        } else {
            struct maze_room *neighbor =
                get_neighbor(num_rows, num_cols, maze, room, dir);
            // If neighbor is not visited, create connection and recurse
            if (neighbor->visited == 0) {
                room->connections[dir] = 0;
                drunken_walk(newRow, newCol, num_rows, num_cols, maze);

            } else {
                // Retrieve opposite direction and store in variable
                Direction opposite_conn =
                    neighbor->connections[get_opposite_dir(dir)];

                if (opposite_conn == 0) {
                    room->connections[dir] = 0;

                } else {
                    room->connections[dir] = 1;
                }
            }
        }
    }
}

/*
 * Represents a maze_room struct as a hexadecimal number based on its
 *connections
 *
 * Parameters:
 *	- room: a struct maze_room to be converted to a hexadecimal number
 *
 * Returns:
 *	- the hexadecimal representation of room
 */
int encode_room(struct maze_room room) {
    int decimal;
    decimal = (room.connections[0] * 1) + (room.connections[1] * 2) +
              (room.connections[2] * 4) + (room.connections[3] * 8);

    return decimal;
}

/*
 * Represents a maze_room array as a hexadecimal array based on its connections
 *
 * Parameters:
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: a 2D array of maze_room structs representing the maze
 *	- result: a 2D array of hexadecimal numbers representing the maze
 *
 * Returns:
 *	- nothing - each maze_room in the maze should be represented
 *	  as a hexadecimal number  and put in nums at the corresponding index
 */
void encode_maze(int num_rows, int num_cols,
                 struct maze_room maze[num_rows][num_cols],
                 int result[num_rows][num_cols]) {
    int i;
    int j;

    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_cols; j++) {
            // Encode each room and store returned int in new result array
            result[i][j] = encode_room(maze[i][j]);
        }
    }
}

/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 4
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["generator", <output file>, <number of rows>, <number of
 *columns>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */
int main(int argc, char **argv) {
    srand(time(NULL));

    char *file_name;
    int num_rows;
    int num_cols;

    if (argc != 4) {
        printf("Incorrect number of arguments.\n");
        printf(
            "./generator <output file> <number of rows> <number of columns>\n");
        return 1;
    } else {
        file_name = argv[1];
        num_rows = atoi(argv[2]);
        num_cols = atoi(argv[3]);
    }
    // Error checking to see if rows/cols are in range
    if ((num_rows <= 0) || (num_cols <= 0)) {
        fprintf(stderr, "error - row/col are either 0 or negative\n");
        return 1;
    }

    FILE *file = fopen(file_name, "w");
    // Error check to see if output file is null
    if (file == NULL) {
        fprintf(stderr, "error, output maze file is null!\n");
        return 1;
    }

    int i;
    int j;
    // Initialize maze array
    struct maze_room maze[num_rows][num_cols];
    // Fill maze array with maze rooms
    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_cols; j++) {
            // Initialize room with row, col, and not visited
            struct maze_room room;
            room.row = i;
            room.col = j;
            room.visited = 0;
            // Initialize the room to have undefined connection status for each
            // direction (neither wall-1 nor connection-0) represented as a 2
            room.connections[0] = 2;
            room.connections[1] = 2;
            room.connections[2] = 2;
            room.connections[3] = 2;

            maze[i][j] = room;
        }
    }

    // Recursively construct the maze via drunken_walk
    drunken_walk(0, 0, num_rows, num_cols, maze);

    // Initialize array representing hexidecimal version of the maze
    int hexmaze[num_rows][num_cols];
    encode_maze(num_rows, num_cols, maze, hexmaze);

    // For each room, print hex character to file
    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_cols; j++) {
            fprintf(file, "%x", hexmaze[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);

    return 0;
}
