#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>

#define FIELD_ROW 20
#define FIELD_COL 15

char Table[FIELD_ROW][FIELD_COL] = {0};
int final = 0;
char GameOn = true;
suseconds_t timer = 400000;
int decrease = 1000;

typedef struct {
    char **array;
    int width, row, col;
} Struct;

Struct current;

const Struct StructsArray[7] = {
    // Z-shaped block
    {
        (char *[]){
            (char []){0, 1, 1},
            (char []){1, 1, 0},
            (char []){0, 0, 0}
        }, 3
    },
    // Reverse Z-shaped block
    {
        (char *[]){
            (char []){1, 1, 0},
            (char []){0, 1, 1},
            (char []){0, 0, 0}
        }, 3
    },
    // T-shaped block
    {
        (char *[]){
            (char []){0, 1, 0},
            (char []){1, 1, 1},
            (char []){0, 0, 0}
        }, 3
    },
    // Reverse L-shaped block
    {
        (char *[]){
            (char []){0, 0, 1},
            (char []){1, 1, 1},
            (char []){0, 0, 0}
        }, 3
    },
    // L-shaped block
    {
        (char *[]){
            (char []){1, 0, 0},
            (char []){1, 1, 1},
            (char []){0, 0, 0}
        }, 3
    },
    // Square block
    {
        (char *[]){
            (char []){1, 1},
            (char []){1, 1}
        }, 2
    },
    // Line block
    {
        (char *[]){
            (char []){0, 0, 0, 0},
            (char []){1, 1, 1, 1},
            (char []){0, 0, 0, 0},
            (char []){0, 0, 0, 0}
        }, 4
    }
};
