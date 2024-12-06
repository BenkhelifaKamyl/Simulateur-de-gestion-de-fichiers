#include <stdio.h>
#include <stdlib.h>
#include "TextColor.h"

//ANSI Colors 0-7, 0:Black, 1:Red, 2:Green, 3:Yellow, 4:Blue, 5:Magenta, 6:Cyan, 7:White
void color(int i){
    switch (i)
    {
    case 0:
    printf("\033[0;30m");
        break;

    case 1:
    printf("\033[0;31m");
        break;
    case 2:
    printf("\033[0;32m");
        break;
    case 3:
    printf("\033[0;33m");
        break;
    case 4:
    printf("\033[0;34m");
        break;
    case 5:
    printf("\033[0;35m");
        break;
    case 6:
    printf("\033[0;36m");
        break;
    case 7:
    printf("\033[0;37m");
        break;
    default: printf("No available color!");
    }
}
//ANSI Bold colors 0-7, 0:Black, 1:Red, 2:Green, 3:Yellow, 4:Blue, 5:Magenta, 6:Cyan, 7:White
void boldColor(int i){
    switch (i)
    {
    case 0:
    printf("\033[1;30m");
        break;

    case 1:
    printf("\033[1;31m");
        break;
    case 2:
    printf("\033[1;32m");
        break;
    case 3:
    printf("\033[1;33m");
        break;
    case 4:
    printf("\033[1;34m");
        break;
    case 5:
    printf("\033[1;35m");
        break;
    case 6:
    printf("\033[1;36m");
        break;
    case 7:
    printf("\033[1;37m");
        break;
    default: printf("This color is not available!\n");
    }
}
//Reset ANSI Color to default
void resetColor(){
    printf("\033[0m");
}
