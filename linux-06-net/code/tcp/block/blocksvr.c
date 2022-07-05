#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#define __DEBUG__

#define MAX_BUFFER_LEN 1024
#ifdef __DEBUG__
    #define log(format, ...) printf("File: "__FILE__", Line: %05d: "format"/n", __LINE__, ##__VA_ARGS__)  
#else
    #define log(format, ...)
#endif

#define handle_error_en(en, msg) \
           do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


void * client_routine(void * argv)
{
    int fd = *(int*)argv;
    char buf[MAX_BUFFER_LEN] = {0};
    while (1)
    {
        memset(buf, 0, MAX_BUFFER_LEN);
        int res = read(fd, buf, sizeof(buf));
        if (res <0 )
        {
            log("remote client socket is closed.\n");
            break;
        }
        log("recved user data : %s\n", buf);

        memset(buf, 0, MAX_BUFFER_LEN);

        strcpy(buf, "Hello Client");
        res = write(fd, buf, sizeof(buf));
        log("send data : %s\n", buf);
    }
    close(fd);
    return NULL;
}

int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    struct sockaddr remote;
    int res, sfd, fd;

    if (argc < 2) 
    {
	log("please input with :./%s port\n", argv[0]);
	return -1;
    }
    int port = atoi(argv[1]);

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) 
    {
 	perror("socket");
	return -1;
    }

    int opt = 1;  
    // sockfd为需要端口复用的套接字  
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) 
    {
	perror("bind");
	exit(1);
    }

    res = listen(sfd, SOMAXCONN);
    if (res == -1) 
    {
        perror("error : cannot listen at the given socket!\n");
        exit(1);
    }

   
    while (1) 
    {
        int len = sizeof(struct sockaddr);
        fd = accept(sfd, &remote, &len);
        if (fd < 0)
        {
            perror("accept failed");
        }
        else
        {
            log("client socket=%d connect success\n", fd);
            pthread_t tid;
            int ret = pthread_create(&tid, NULL, client_routine, &fd);
            if (ret != 0)
            {
                handle_error_en(ret, "create thread failed.");
            }
        }
    }
    
    return 0;
}
