/*
 * Enum to represent the four directions
 * Here is an example of how to use an enum:
 *
 * Direction dir = EAST;
 * if (dir == EAST) {
 *     printf("Heading EAST!");
 * }
 */
typedef enum { NORTH = 0, SOUTH = 1, WEST = 2, EAST = 3 } Direction;

/*
 * Struct to represent a room in the maze.
 * Must store:
 * 	- The location of the room
 * 	- Whether or not this room has been visited
 *      - A pointer to the next room (provided), set this pointer
 *        in the current room's struct to the next room you visit
 *	- The connections to neighboring rooms in all
 *	  four directions.
 */
struct maze_room {
    struct maze_room *next;
    int row;
    int col;
    // 1 if visited, 0 otherwise
    int visited;

    // This array holds each connection at the index corresponding the
    // directional enum value
    int connections[4];
};

int is_in_range(int row, int col, int num_rows, int num_cols);

struct maze_room *get_neighbor(int num_rows, int num_cols,
                               struct maze_room maze[][num_cols],
                               struct maze_room *room, Direction dir);
