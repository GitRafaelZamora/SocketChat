#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


int main(int argc, char *argv[]) {
    int ret;
    socklen_t len;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    char recv_buffer[1024];
    //
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
	     printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(31000);

    ret = bind(listenfd, (struct sockaddr*)
               &serv_addr, sizeof(serv_addr));
    if (ret < 0) {
	     printf("bind() error: %s.\n", strerror(errno));
        return -1;
    }

    // 10 is the max amount of clients -1 on error
    if (listen(listenfd, 10) < 0) {
	       printf("listen() error: %s.\n", strerror(errno));
        return -1;
    }

    while (1) {
	      printf("waiting for connection...\n");
        // New Socket listening
        connfd = accept(listenfd,
                 (struct sockaddr*) &client_addr,
                 &len);

        // Start New Thread:
    //     pthread_t client1;
    //     int ret1;
    //     ret1 = pthread_create(&client1, NULL, start_client_thread,(void*) "client 1");
    //     printf("ret1 %d", ret1);
    //
    //     pthread_join(client1, NULL);
    //     pthread_join(client2, NULL);

        if(connfd < 0) {
		      printf("accept() error: %s.\n", strerror(errno));
        	return -1;
        }
	      printf("connection accept from %s.\n", inet_ntoa(client_addr.sin_addr));

        while (1) {
             ret = recv(connfd,
                        recv_buffer,
                        sizeof(recv_buffer),
                        0);

             if (ret <= 0) {
		             printf("recv() error: %s.\n", strerror(errno));
        	        return -1;
             }
             printf("%s", recv_buffer);
        }
    }
    return 0;
}
