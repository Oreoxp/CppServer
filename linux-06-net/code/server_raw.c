



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <errno.h>
#include <fcntl.h>

#include <sys/poll.h>
#include <sys/epoll.h>


#define BUFFER_LENGTH	1024

#define POLL_SIZE		1024
#define EPOLL_SIZE		1024

void *client_route(void *arg) {
	int clientfd = *(int*)arg;

	while (1) {
		char buffer[BUFFER_LENGTH] = {0};
		int ret = recv(clientfd, buffer, BUFFER_LENGTH, 0);
		if (ret < 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				printf("read all data\n");
			}
			close(clientfd);
			break;
		} else if (ret == 0) {
			printf("disconnect \n");
			close(clientfd);
			break;
		} else {
			printf("Recv:%s, %d Bytes\n", buffer, ret);
			
		}
	}
}



int main(int argc, char *argv[]) {

	if (argc < 2) {
		printf("Paramter Error\n");
		return -1;
	}
	int port = atoi(argv[1]);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		return 2;
	}

	if (listen(sockfd, 5) < 0) {
		perror("listen");
		return 3;
	}
#if 1
	while (1) {
		struct sockaddr_in client_addr;
		memset(&client_addr, 0, sizeof(struct sockaddr_in));
		socklen_t client_len = sizeof(client_addr);

		int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
		if (clientfd <= 0) return -1;
#if 0
		client_route(&clientfd);
#else
		pthread_t thread_id;
		int ret = pthread_create(&thread_id, NULL, client_route, &clientfd);
		if (ret < 0) {
			perror("pthread_create");
			exit(1);
		}
		
#endif
	}
#elif 0

	fd_set rfds, rset;

	FD_ZERO(&rfds);
	FD_SET(sockfd, &rfds);

	int max_fd = sockfd;

	while (1) {
		rset = rfds;

		int nready = select(max_fd+1, &rset, NULL, NULL, NULL);
		if (nready < 0) {
			printf("select error: %d\n", errno);
			continue;
		}

		if (FD_ISSET(sockfd, &rset)) {
			struct sockaddr_in client_addr;
			memset(&client_addr, 0, sizeof(struct sockaddr_in));
			socklen_t client_len = sizeof(client_addr);

			int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
			if (clientfd <= 0) return -1;

			FD_SET(clientfd, &rfds);

			if (clientfd > max_fd) max_fd = clientfd;

			if (--nready == 0) continue;
		}

		int i = 0;
		for (i = sockfd+1;i <= max_fd;i ++) {
			if (FD_ISSET(i, &rset)) {
				char buffer[BUFFER_LENGTH] = {0};
				int ret = recv(i, buffer, BUFFER_LENGTH, 0);
				if (ret < 0) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						printf("read all data\n");
					}

					FD_CLR(i, &rfds);
					close(i);
					
					break;
				} else if (ret == 0) {
					printf("disconnect \n");
					FD_CLR(i, &rfds);
					close(i);
					break;
				} else {
					printf("Recv:%s, %d Bytes\n", buffer, ret);
					
				}
			}
		}
		
	}
#elif 0

	struct pollfd fds[POLL_SIZE] = {0};

	fds[0].fd = sockfd;
	fds[0].events = POLLIN;

	int max_fd = 0, i = 0;
	for (i = 1;i < POLL_SIZE;i ++) {
		fds[i].fd = -1;
	}

	while (1) {

		int nready = poll(fds, max_fd+1, 5);
		if (nready <= 0) continue;

		if ((fds[0].revents & POLLIN) == POLLIN) {
			struct sockaddr_in client_addr;
			memset(&client_addr, 0, sizeof(struct sockaddr_in));
			socklen_t client_len = sizeof(client_addr);

			int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
			if (clientfd <= 0) return -1;

			fds[clientfd].fd = clientfd;
			fds[clientfd].events = POLLIN;

			if (clientfd > max_fd) max_fd = clientfd;

			if (--nready == 0) continue;
			
		}

		int i = 0;
		for (i = sockfd+1;i <= max_fd;i ++) {
			if (fds[i].revents & (POLLIN|POLLERR)) {
				char buffer[BUFFER_LENGTH] = {0};
				int ret = recv(i, buffer, BUFFER_LENGTH, 0);
				if (ret < 0) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						printf("read all data\n");
					}

					fds[i].fd = -1;
					close(i);
					
					break;
				} else if (ret == 0) {
					printf("disconnect \n");
					fds[i].fd = -1;
					close(i);
					
					break;
				} else {
					printf("Recv:%s, %d Bytes\n", buffer, ret);
					
				}
			}
		}
	}
#else

	int epoll_fd = epoll_create(EPOLL_SIZE);
	struct epoll_event events[EPOLL_SIZE] = {0};

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev);

	while (1) {

		int nready = epoll_wait(epoll_fd, events, EPOLL_SIZE, -1);
		if (nready == -1) {
			printf("epoll_wait\n");
			continue;
		}

		int i = 0;
		for (i = 0;i < nready;i ++) {
			if (events[i].data.fd == sockfd) {
				struct sockaddr_in client_addr;
				memset(&client_addr, 0, sizeof(struct sockaddr_in));
				socklen_t client_len = sizeof(client_addr);

				int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
				if (clientfd <= 0) return -1;

				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = clientfd;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientfd, &ev);
				
			} else {

				int clientfd = events[i].data.fd;
				
				char buffer[BUFFER_LENGTH] = {0};
				int ret = recv(clientfd, buffer, BUFFER_LENGTH, 0);
				if (ret < 0) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						printf("read all data\n");
					}

					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = clientfd;
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientfd, &ev);
				
					close(i);
					
					break;
				} else if (ret == 0) {
					printf("disconnect \n");

					ev.events = EPOLLIN | EPOLLET;
					ev.data.fd = clientfd;
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientfd, &ev);
					
					close(i);
					
					break;
				} else {
					printf("Recv:%s, %d Bytes\n", buffer, ret);
					
				}
			}
		}
		
	}

#endif
	
	return 0;
}





