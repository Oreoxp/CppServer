#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_nodelay(int fd)
{
    int one = 1;
    int err = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(int));
    if (err != 0)
    {
        printf("set TCP_NODELAY failed.\n");
    }
    assert(err == 0);
}


int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    int res, sfd;
    char buf[128];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;

    res = getaddrinfo("192.168.1.159", "8080", &hint, &result);
    if (res == -1) 
    {
        perror("error : cannot get socket address!\n");
        exit(1);
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) 
    {
        perror("error : cannot get socket file descriptor!\n");
        exit(1);
    }

    res = connect(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) 
    {
        perror("error : cannot connect the server!\n");
        exit(1);
    }

    set_nodelay(sfd);

    while(1)
    {
        memset(buf, 0, sizeof(buf));
        strcpy(buf, "Hello Server!");
        write(sfd, buf, sizeof(buf));
        printf("write < %s > to server\n", buf);

        memset(buf, 0, sizeof(buf));
        read(sfd, buf, sizeof(buf));
        printf("read < %s > from server\n", buf);
    }

    return 0;
}

