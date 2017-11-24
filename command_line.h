#ifndef COMMAND_LINE_H_INCLUDED
#define COMMAND_LINE_H_INCLUDED

#define COUNT 255
#define INIT 281520
#define CREATE 562020
#define RM 84660
#define COPY 290955
#define MV 87975
#define WRITE 415905
#define READ 256785
#define LS 86190
#define HELP 274890
#define SET 169575
#define EXIT 285600

#define WRONG_PARAMETERS -2
#define MAX_WRITE_READ_VALUE 24


int command_line();
int read_string(char* name);

#endif // COMMAND_LINE_H_INCLUDED
