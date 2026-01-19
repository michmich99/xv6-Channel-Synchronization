#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int cd = channel_create();
    if (cd < 0) 
    {
        printf("Failed to create channel\n");
        exit(1);
    }
    if (fork() == 0)
    {
        if (channel_put(cd, 42) < 0) 
        {
            printf("Failed to put data in channel\n");
            exit(1);
        }
        channel_put(cd, 43); // Sleeps until cleared
        // Handle error
        channel_destroy(cd);
        // Handle error
    } 
    else 
    {
        int data;
        if (channel_take(cd, &data) < 0) 
        { // 42
            printf("Failed to take data from channel\n");
            exit(1);
        }
        printf("data: %d\n", data);
        data = 0;
        int returnCode = channel_take(cd, &data); // 43
        printf("data: %d, returnCode: %d\n", data, returnCode);
        data = 0;
        // Handle error
        int returnCode2 = channel_take(cd, &data); // Sleep until child destroys channel
        printf("data: %d, returnCode: %d\n", data, returnCode2);
        // Handle error
    }
    return 1;
}
