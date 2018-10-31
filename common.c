#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

/*
 * Determines whether or not the room at [row][col] is a valid room within the
 *maze with dimensions num_rows x num_cols
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *
 * Returns:
 *	- 0 if room is not in the maze, 1 if room is in the maze
 */
int is_in_range(int row, int col, int num_rows, int num_cols) {
    if ((row >= num_rows) || (row < 0) || (col >= num_cols) || (col < 0)) {
        return 0;
    }
    return 1;
}

/*
 * Given a pointer to the room and a Direction to travel in, return a pointer to
 *the room that neighbors the current room on the given Direction. For example:
 *get_neighbor(&maze[3][4], EAST) should return &maze[3][5]
 *
 * Parameters:
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- room: pointer to the current room
 *	- dir: Direction to get the neighbor from
 *	- maze: a 2D array of maze_room structs representing your maze
 * Returns:
 *	- pointer to the neighboring room
 */
struct maze_room *get_neighbor(int num_rows, int num_cols,
                               struct maze_room maze[num_rows][num_cols],
                               struct maze_room *room, Direction dir) {
    // Needed? Place elsewhere?
    assert(is_in_range(room->row, room->col, num_rows, num_cols));

    int newRow = room->row;
    int newCol = room->col;
    // WHAT IF NEIGHBORING ROOM OUT OF RANGE? // WHAT IF NO CONNECTION?
    // INITIALIZE MAZE ARRAY TO ALL ZEROS in generator?
    // I could, in drunken, make neighbor and get row/col using pointers, but..
    if (dir == NORTH) {
        newRow--;
    } else if (dir == EAST) {
        newCol++;
    } else if (dir == SOUTH) {
        newRow++;
    } else if (dir == WEST) {
        newCol--;
    }

    return &maze[newRow][newCol];
    // Fill in this function
}
