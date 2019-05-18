#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Using A-star to solve simple path finding case


typedef struct Node Node;
typedef struct Queue Queue;

struct Node { // double linked list
    int cost;
    int expectedCostLeft;
    int y, x;
    Node* prev; // for store result of visited path
    Node* next; // for A-star travel
};

struct Queue {
    Node* head;
};

// return width and height
char** getWidHgt(char* str) { // used for split width and height from single line string
    char** token = malloc(16 * sizeof(char*)); // considering only 2 int need to be split
    char* ptr;
    char delim[] = " \t\r";
    int counter = 0;
    ptr = strtok(str, delim);

    while (ptr != NULL) {
        token[counter] = malloc(16);
        strcpy(token[counter], ptr);
        ptr = strtok(NULL, delim);
        counter++;
    }
    return token;
}

// return row
int** getMatrixRow(char* str, int width) {
    int** token = malloc(sizeof(int*) * width +1); // considering only 2 int need to be split
    char* ptr;
    char delim[] = " \t\r";
    int counter = 0;
    ptr = strtok(str, delim);

    while (ptr != NULL) {
        token[counter] = malloc(sizeof(int));
        *token[counter] = atoi(ptr);
        ptr = strtok(NULL, delim);
        counter++;
    }
    return token;
}

//readFile from file and assign width, height and matrix to argument variable pointed var
int*** loadFile(FILE* file, int *width, int *height, int ***matrix) {
    char* line = malloc(1024); // up to 1024 char
    char** token;
    int counter_y = -1; // count height (y), set -1 for considering first line is matrix size info
    while (!feof(file)) {
        fgets(line, 1024, file);
        if (counter_y == -1) {
            token = getWidHgt(line);
            *width = atoi((token[0]));
            *height = atoi(token[1]);
            matrix = malloc(sizeof(int*) * *height);
            counter_y = 0;
            continue;
        }
        matrix[counter_y] = getMatrixRow(line, *width); // assign to each row
        counter_y++;
    }
    return matrix;
}

Node* newNode(int cost, int y, int x, int expectedCostLeft, Node* prev) {
    Node* node = malloc(sizeof(Node));
    node->cost = cost;
    node->y = y;
    node->x = x;
    node->expectedCostLeft = expectedCostLeft;
    node->next = NULL;
    node->prev = prev;
    return node;
}

void push_sortedQueue(Queue* queue, Node* node) {
    Node* temp = queue->head;
    int flag = 0; // used to indicate whether prevprev is not valid

    if (queue->head == NULL)
        queue->head = node;
    else {
        while (temp->next != NULL &&
               temp->next->cost + temp->next->expectedCostLeft < node->cost + node->expectedCostLeft) { // check next node's next node
            // get suitable for place node
            temp = temp->next;
        }

        if (temp == queue->head) {
            if (temp->cost + temp->expectedCostLeft >= node->cost + node->expectedCostLeft) {
                queue->head = node;
                node->next = temp;
                return;
            }
        }
        node->next = temp->next;
        temp->next = node;
    }
}

// pop head
Node* pop_queue(Queue* queue) {
    Node* temp = queue->head;
    if (queue->head == NULL) {
        queue->head = NULL;
        return NULL;
    }
    queue->head = queue->head->next;
    temp->next = NULL;
    return temp;
}

// test use
void printSize(Queue* queue) {
    Node* node = queue->head;
    int counter = 0;
    while (node != NULL) {
        counter++;
        node= node->next;
    }
    printf("size of queue is %d", counter);
}

// given queue, push next node can be moved next
void pushNearbyCell(Queue* queue, int*** matrix, int y, int x, int goal_y, int goal_x, int width, int height, int cost, Node* prev, int* node_counter) {
    if (abs(goal_y - y) + abs(goal_x - x) == 0) // reach dest
        push_sortedQueue(queue, newNode(cost, y, x, 0, prev));
    else {
        if (x != 0 && *matrix[x - 1][y] == 0) { //cell above
            push_sortedQueue(queue, newNode(cost, y, x - 1, abs(goal_y - y) + abs(goal_x - (x - 1)), prev));
            *node_counter += 1; // visited node ++
        } if (y != width - 1&& *matrix[x][y + 1] == 0) { // cell right
            push_sortedQueue(queue, newNode(cost, y + 1, x, abs(goal_y - (y + 1)) + abs(goal_x - x), prev));
            *node_counter += 1; // visited node ++
        } if (x != height - 1 && *matrix[x + 1][y] == 0) {// cell below
            push_sortedQueue(queue, newNode(cost, y, x + 1, abs(goal_y - y) + abs(goal_x - (x + 1)), prev));
            *node_counter += 1; // visited node ++
        } if (y != 0 && *matrix[x][y - 1] == 0) { // cell left
            push_sortedQueue(queue, newNode(cost, y - 1, x, abs(goal_y - (y - 1)) + abs(goal_x - x), prev));
            *node_counter += 1; // visited node ++
        }
    }
}

//return 1 if loop detected
// prevent looping
int checkLoop(Node* current) {
    Node* temp = current;
    while ((temp = temp->prev) != NULL) {
        if (temp->y == current->y
                && temp->x == current->x) {
                return 1;
        }
    }
    return 0;
}

// 0 means top edge
// 1 means top right
// 2 means right edge
// 3 means bottom right
// 4 means bottom edge
// 5 means bottom left
// 6 means left edge
// 7 means top left
// 8 means not in edge
int isCornor(int width, int height, int y, int x) {
    if (width - 1 == y) {
        if (height - 1 == x)
            return 3;
        else if (x == 0)
            return 1;
        else
            return 2;
    } else if (y == 0) {
        if (height - 1 == x)
            return 5;
        else if (x == 0)
            return 7;
        else
            return 6;
    } else if (x == 0) {
        return 0;
    } else if (x == height - 1) {
        return 4;
    } else
        return 8;
}

// return 1 means valid, return 0 means invalid
int isValidGoal(int*** matrix, int goal_y, int goal_x, int width, int height, int start_y, int start_x) {
    int v = isCornor(width, height, goal_y, goal_x); // check whether cornor info

    if (*matrix[goal_x][goal_y] == 1)
        return 0;
    if (*matrix[start_x][start_y] == 1)
        return 0;
    if (v == 8) {
        if (*matrix[goal_x - 1][goal_y] == 1
            && *matrix[goal_x][goal_y - 1] == 1
            && *matrix[goal_x][goal_y + 1] == 1
            && *matrix[goal_x + 1][goal_y] == 1)
            return 0;
    } else if (v % 2 == 1) {
        if (v == 1) {
            if (*matrix[goal_x][goal_y - 1] == 1
                && *matrix[goal_x + 1][goal_y] == 1)
                return 0;
        } else if (v == 3) {
            if (*matrix[goal_x][goal_y - 1] == 1
                && *matrix[goal_x - 1][goal_y] == 1)
                return 0;
        } else if (v == 5) {
            if (*matrix[goal_x - 1][goal_y] == 1
                && *matrix[goal_x][goal_y + 1] == 1)
                return 0;
        } else if (v == 7) {
            if (*matrix[goal_x + 1][goal_y] == 1
                && *matrix[goal_x][goal_y + 1] == 1)
                return 0;
        }
    } else if (v % 2 == 0) {
        if (v == 0) {
            if (*matrix[goal_x][goal_y - 1] == 1
                && *matrix[goal_x][goal_y + 1] == 1
                && *matrix[goal_x + 1][goal_y] == 1)
                return 0;
        } else if (v == 2) {
            if (*matrix[goal_x][goal_y - 1] == 1
                && *matrix[goal_x][goal_y + 1] == 1
                && *matrix[goal_x - 1][goal_y] == 1)
                return 0;
        } else if (v == 4) {
            if (*matrix[goal_x - 1][goal_y] == 1
                && *matrix[goal_x][goal_y + 1] == 1
                && *matrix[goal_x][goal_y - 1] == 1)
                return 0;
        } else if (v == 6) {
            if (*matrix[goal_x + 1][goal_y] == 1
                && *matrix[goal_x - 1][goal_y] == 1
                && *matrix[goal_x][goal_y + 1] == 1)
                return 0;
        }
    }
    return 1;

}

// by using A_star
Node* getSequence(int* node_counter, int width, int height, int start_y, int start_x, int goal_y, int goal_x, int*** matrix) {
    int cost = 0;
    Queue* queue = malloc(sizeof(Queue)); // store unvisited node
    queue->head = NULL;
    Node* current = newNode(0, start_y, start_x, abs(goal_y - start_y) + abs(goal_x - start_x), NULL);
    *node_counter = 0;
    Node* result = NULL;

    if (!isValidGoal(matrix, goal_y, goal_x, width, height, start_y, start_x)) // return immediately if goal not valid
        return NULL;

    do {
        if (checkLoop(current)) // check whether running in a loop
            continue;

        start_y = current->y;
        start_x = current->x;

        if (current->y == goal_y && current->x == goal_x) {
            if (result == NULL) // result has not been assigned before
                result = current;
            if (queue->head != NULL) {
                // check whether best result
                if (queue->head->expectedCostLeft + queue->head->cost >= result->cost)
                    break;
                else
                    result = current;
            } else
                break;
        }

        cost = current->cost+1;

        pushNearbyCell(queue, matrix, start_y, start_x, goal_y, goal_x, width, height, cost, current, node_counter);
    } while (NULL != (current = pop_queue(queue)));

    return result;
}

// get direction for prev node to get current node
// for printing result
char getDir(Node* current, Node* prev) {
    if (prev->y == current->y) {
        if (prev->x+1 == current->x) // D
            return 'D';
        else // U
            return 'U';
    } else {
        if (prev->y+1 == current->y) // R
            return 'R';
        else // L
            return 'L';
    }
}

void printResultPath(Node* result, int* node_counter) {
    Node *current = result;
    Node *prev;
    char *dir;
    int counter = 0;
    int i = 0;

    if (result != NULL) { // valid path
        prev = current->prev;
        dir = malloc(1024);

        while (prev != NULL) { // visit through node->prev
            dir[counter] = getDir(current, prev);
            current = prev;
            prev = prev->prev;
            counter++;
        }

        printf("%d %d\n", *node_counter, counter);
        for (i = counter - 1; i >= 0; i--) {
            printf("%c ", dir[i]);
        }
        printf("\n");
    } else { // cannot be reached
        printf("%d %d\n%s\n", *node_counter, 0, "X");
    }
}


int main(int argc, char* argv[]) {
    int* node_counter = malloc(sizeof(int));
    int *width, *height;
    int ***matrix;
    int start_x, start_y, goal_x, goal_y;
    FILE* matrix_file;
    char* matrix_fileName = malloc(64);
    int i = 0, j = 0; // for free matrix mem

    width = malloc(sizeof(int));
    height = malloc(sizeof(int));

    matrix_file = fopen(argv[1], "r");
    start_y = atoi(argv[2]);
    start_x = atoi(argv[3]);
    goal_y = atoi(argv[4]);
    goal_x = atoi(argv[5]);

    matrix = loadFile(matrix_file, width, height, matrix);

    printResultPath(
            getSequence(node_counter, *width, *height, start_y, start_x, goal_y, goal_x, matrix), node_counter);
    fclose(matrix_file);

    // freeing mem
    free(matrix_fileName);
    for (; i < *height; i++) {
        for (j = 0; j < *width; j++)
            free(matrix[i][j]);
    }
    free(width);
    free(height);

    return 0;
}