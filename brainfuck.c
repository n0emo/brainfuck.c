#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_SIZE 30000
#define MAX_LOOP_DEPTH 512
#define MAX_LOOP_COUNT 2048

typedef unsigned char data_t;

typedef struct pair
{
    char *open;
    char *close;
} pair;

static data_t data[DATA_SIZE];
static pair loops[MAX_LOOP_COUNT];
static size_t loop_count = 0;

static char *program = NULL;
static char *file_name;

void signal_handler(int);
int read_program();
int get_loops();
int execute_program();

int main(int argc, char **argv)
{
    signal(SIGINT, signal_handler);
    if (argc != 2)
    {
        fprintf(stderr, "Provide 1 argument containing a path to the program.\n");
        return 1;
    }

    file_name = argv[1];

    if (read_program())
    {
        fprintf(stderr, "Error opening file\n");
        return 2;
    }

    if (get_loops())
    {
        fprintf(stderr, "Error finding loops\n");
        return 3;
    }

    if (execute_program())
    {
        fprintf(stderr, "Error executing program\n");
        return 4;
    }

    free(program);
    return 0;
}

void signal_handler(int dummy)
{
    if (program)
    {
        free(program);
    }
    exit(0);
}

char *find_close(char *open)
{
    for (int i = 0; i < loop_count; i++)
    {
        if (loops[i].open == open)
        {
            return loops[i].close;
        }
    }
    return NULL;
}

char *find_open(char *close)
{
    for (int i = 0; i < loop_count; i++)
    {
        if (loops[i].close == close)
        {
            return loops[i].open;
        }
    }
    return NULL;
}

int read_program()
{
    FILE *file = fopen(file_name, "r");

    if (file == NULL)
    {
        return 1;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);

    program = malloc(file_size + 1);

    fseek(file, 0, SEEK_SET);

    fread(program, file_size, 1, file);
    program[file_size] = '\0';

    fclose(file);

    return 0;
}

int get_loops()
{
    char *loop_stack[MAX_LOOP_DEPTH];
    size_t loop_top = 0;
    char *program_ptr = program;

    while (*program_ptr != '\0')
    {
        if (*program_ptr == '[')
        {
            loop_stack[loop_top] = program_ptr;
            loop_top++;
        }
        else if (*program_ptr == ']')
        {
            loop_top--;
            loops[loop_count].open = loop_stack[loop_top];
            loops[loop_count].close = program_ptr;
            loop_count++;
        }
        program_ptr++;
    }

    return loop_top > 0;
}

int execute_program()
{
    char *instruction_ptr = program;
    data_t *data_ptr = data;

    size_t loop_top = 0;

    while (1)
    {
        switch (*instruction_ptr)
        {
        case '>':
            data_ptr++;
            if (data_ptr >= data + DATA_SIZE)
            {
                return 1;
            }
            break;
        case '<':
            data_ptr--;
            if (data_ptr < data)
            {
                return 1;
            }
            break;
        case '+':
            (*data_ptr)++;
            break;
        case '-':
            (*data_ptr)--;
            break;
        case '.':
            putchar(*data_ptr);
            break;
        case ',':
            *data_ptr = getchar();
            break;
        case '[':
            if (!*data_ptr)
            {
                instruction_ptr = find_close(instruction_ptr);
                if (instruction_ptr == NULL)
                {
                    return 1;
                }
            }
            break;
        case ']':
            if (*data_ptr)
            {
                instruction_ptr = find_open(instruction_ptr) - 1;
                if (instruction_ptr == NULL)
                {
                    return 1;
                }
            }
            break;
        case '\0':
            goto execution_end;
        }
        instruction_ptr++;
    }

execution_end:

    printf("\n");
    return 0;
}
