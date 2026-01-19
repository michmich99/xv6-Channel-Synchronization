#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// returns 1 if n is prime, 0 otherwise
int isPrime(int n)
{
    for(int i=2; i*i <= n; i++)
    {
        if(n%i == 0)
        {
            return 0;
        }
    }
    return 1;
}


int main(int argc, char *argv[])
{
    int cd1;
    int cd2;
    int cpid;
    int numOfCheckers = 3;
    int checkerIndex;

    if(argc==2)
    {
        numOfCheckers = atoi(argv[1]);
    }

    while(1)
    {
        cd1 = channel_create();
        cd2 = channel_create();
        cpid = -1;
        if (cd1 < 0 || cd2 < 0) 
        {
            printf("Failed to create at least one of the channels\n");
            exit(1);
        }

        for(checkerIndex=0; checkerIndex<numOfCheckers; checkerIndex++){
            cpid = fork();
            if (cpid == 0)
            {
                break;
            }
        }

        if(cpid < 0){
            printf("Checker fork failed\n");
            exit(0);
        }
        else if(cpid == 0)
        {
            //checker
            int data;
            for(;;)
            {
                if (channel_take(cd1, &data) < 0)
                {
                    printf("Failed to take data from channel %d\n", cd1);
                }
                else if(isPrime(data))
                {
                    if(channel_put(cd2, data) < 0)
                    {
                        printf("Shutdown:: pid: %d, role: checker no. %d\n", getpid(), checkerIndex);
                        channel_destroy(cd1);
                        exit(0);
                    }
                }
            }

        }
        else
        {
            int ppid = fork();
            if(ppid < 0)
            {
                printf("Printer fork failed\n");
                exit(0);
            }
            else if(ppid == 0)
            {
                int n = 0;
                int data;
                //printer
                for(;;)
                {
                    if (channel_take(cd2, &data) < 0)
                    {
                        printf("Failed to take data from channel %d\n", cd2);
                    }
                    else
                    {
                        if(n < 100)
                        {
                            printf("Prime number: %d\n", data);
                            n++;
                        }
                        else
                        {
                            printf("Shutdown:: pid: %d, role: printer\n", getpid());
                            channel_destroy(cd2);
                            exit(0);
                        }

                    }
                }
            }
            else
            {
                //generator
                int number = 2;
                for(;;)
                {
                    if(channel_put(cd1, number) < 0)
                    {
                        printf("Shutdown:: pid: %d, role: generator\n", getpid());
                        wait(0);
                        for(int i=0; i<numOfCheckers; i++)
                        {
                            wait(0);
                        }

                        char buf[3];
                        printf("Would you like to start over?\n");
                        gets(buf, 3);
                        if(buf[0] == 'Y')
                        {
                            break;
                        }
                        else
                        {
                            exit(0);
                        }
                    }
                    else
                    {
                        number++;
                    }
                }
            }
        }
    }
}


