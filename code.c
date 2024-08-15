#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_PROCESSES 20
#define MAX_RESOURCES 10

typedef struct
{
    int number_processes;
    int number_resources;
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int maximum[MAX_PROCESSES][MAX_RESOURCES];
    int need[MAX_PROCESSES][MAX_RESOURCES];
    int available[MAX_RESOURCES];
} system_state;

bool initialization(system_state *s, FILE *data);
void print_state(system_state *s);
int *is_safe(system_state *s);
bool request(system_state *s, int ix, int *res);
bool release(system_state *s, int ix, int *res);

int main(int argc, char const *argv[])
{
    FILE *datafile = fopen(argv[1], "r");

    system_state *s = (system_state *)malloc(sizeof(system_state));
    if (!initialization(s, datafile))
    {
        printf("initialization failed\n");
        return 1;
    }

    int opt = 0, index = 0, helper[MAX_RESOURCES] = {0}, *seq;
    do
    {
        printf("\n--menu--\n");
        printf("0. print current state\n");
        printf("1. find system state\n");
        printf("2. request resource\n");
        printf("3. release\n");
        printf("4. exit\n");
        printf("\nenter option no: ");
        scanf("%d", &opt);

        switch (opt)
        {
        case 0:
            print_state(s);
            break;
        case 1:
            if (seq = is_safe(s))
            {
                printf("system is in safe state\n");
                printf("safe state sequence:\n");
                for (int i = 0; i < s->number_processes; i++)
                {
                    printf("P%d ", seq[i]);
                }
                printf("\n");
                free(seq);
            }
            else
            {
                printf("system is not in safe state\n");
            }
            break;
        case 2:
            printf("enter the process index: ");
            scanf("%d", &index);

            printf("enter resources: ");
            for (int i = 0; i < s->number_resources; i++)
            {
                scanf("%d", &helper[i]);
            }

            if (request(s, index, helper))
            {
                printf("request granted\n");
            }
            else
            {
                printf("request not granted\n");
            }
            break;
        case 3:
            printf("enter the process index: ");
            scanf("%d", &index);

            printf("enter resources: ");
            for (int i = 0; i < s->number_resources; i++)
            {
                scanf("%d", &helper[i]);
            }

            if (release(s, index, helper))
            {
                printf("resource released\n");
            }
            else
            {
                printf("resource not released\n");
            }
            break;
        case 4:
            printf("exiting...\n");
            break;
        default:
            printf("(enter correct option number)\n");
        }

    } while (opt != 4);

    free(s);
    return 0;
}

bool initialization(system_state *s, FILE *data)
{
    printf("enter no of processes: ");
    scanf("%d", &s->number_processes);

    printf("enter no of resources: ");
    scanf("%d", &s->number_resources);

    for (int i = 0; i < s->number_processes; i++)
    {
        for (int j = 0; j < s->number_resources; j++)
        {
            if (!fscanf(data, "%d", &(s->allocation[i][j])))
                return false;
        }
    }

    for (int i = 0; i < s->number_processes; i++)
    {
        for (int j = 0; j < s->number_resources; j++)
        {
            if (!fscanf(data, "%d", &(s->maximum[i][j])))
                return false;
        }
    }

    for (int i = 0; i < s->number_processes; i++)
    {
        for (int j = 0; j < s->number_resources; j++)
        {
            if ((s->need[i][j] = s->maximum[i][j] - s->allocation[i][j]) < 0)
                return false;
        }
    }

    for (int j = 0; j < s->number_resources; j++)
    {
        if (!fscanf(data, "%d", &(s->available[j])))
            return false;
    }

    return true;
}

void print_state(system_state *s)
{
    printf("\ncurrent system state:\n");
    printf("allocation:\n");
    for (int i = 0; i < s->number_processes; i++)
    {
        for (int j = 0; j < s->number_resources; j++)
        {
            printf("%d ", s->allocation[i][j]);
        }
        printf("\n");
    }
    printf("maximum:\n");
    for (int i = 0; i < s->number_processes; i++)
    {
        for (int j = 0; j < s->number_resources; j++)
        {
            printf("%d ", s->maximum[i][j]);
        }
        printf("\n");
    }
    printf("need:\n");
    for (int i = 0; i < s->number_processes; i++)
    {
        for (int j = 0; j < s->number_resources; j++)
        {
            printf("%d ", s->need[i][j]);
        }
        printf("\n");
    }
    printf("available:\n");
    for (int j = 0; j < s->number_resources; j++)
    {
        printf("%d ", s->available[j]);
    }
    printf("\n");
}

int *is_safe(system_state *s)
{
    bool accepted_need_flag;
    int *seq = (int *)malloc(MAX_PROCESSES * sizeof(int));
    int idx = 0;

    int available2[MAX_RESOURCES];
    for (int i = 0; i < s->number_resources; i++)
    {
        available2[i] = s->available[i];
    }

    bool completed[MAX_PROCESSES] = {false};

    for (int i = 0; i < s->number_processes; i++)
    {
        for (int j = 0; j < s->number_processes; j++)
        {
            if (completed[j] == false)
            {
                accepted_need_flag = true;

                for (int k = 0; k < s->number_resources; k++)
                {
                    if (s->need[j][k] > available2[k])
                    {
                        accepted_need_flag = false;
                        break;
                    }
                }

                if (accepted_need_flag)
                {
                    completed[j] = true;
                    seq[idx++] = j;

                    for (int k = 0; k < s->number_resources; k++)
                    {
                        available2[k] += s->allocation[j][k];
                    }
                }
            }
        }
    }

    for (int i = 0; i < s->number_processes; i++)
    {
        if (!completed[i])
        {
            free(seq);
            return seq = NULL;
        }
    }

    return seq;
}

bool request(system_state *s, int ix, int *res)
{
    for (int i = 0; i < s->number_resources; i++)
    {
        if (res[i] > s->need[ix][i])
        {
            return false;
        }
    }

    for (int i = 0; i < s->number_resources; i++)
    {
        if (res[i] > s->available[i])
        {
            return false;
        }
    }

    system_state s2 = *s;

    for (int i = 0; i < s2.number_resources; i++)
    {
        s2.available[i] -= res[i];
        s2.allocation[ix][i] += res[i];
        s2.need[ix][i] -= res[i];
    }

    if (is_safe(&s2))
    {
        *s = s2;
        return true;
    }
    else
    {
        return false;
    }
}

bool release(system_state *s, int ix, int *res)
{
    for (int i = 0; i < s->number_resources; i++)
    {
        if (res[i] > s->allocation[ix][i])
        {
            return false;
        }
    }

    for (int i = 0; i < s->number_resources; i++)
    {
        s->available[i] += res[i];
        s->allocation[ix][i] -= res[i];
        s->need[ix][i] += res[i];
    }

    return true;
}