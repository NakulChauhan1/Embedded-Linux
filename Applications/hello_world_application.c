#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define HELLOWORLD_DEVICE_PATH   "/dev/helloworld"              // Device file path is created by hello_world kernel module

int main()
{
    int fd;

    fd = open(HELLOWORLD_DEVICE_PATH, O_RDWR);

    if(fd < 0)
    {
        printf("Cannot open device file...\n");
        return 0;
    }

    printf("fd for open file %d \n", fd);

    return 0;
}