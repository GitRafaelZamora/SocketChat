#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <regex>

using namespace std;

void *worker_thread(void *arg) {
  int ret, res;
  int connfd = (long) arg;
  char recv_buffer[1024];

  printf("[%d] worker thread started.\n", connfd);
  while (1) {

    // open a file.
    FILE * pFile;
    pFile = fopen("output.txt","a+");
    if (pFile != NULL) {
      // printf("File opened.\n");
      // Recieving byte stream from the client.
      ret = recv(connfd, recv_buffer, sizeof(recv_buffer), 0);
      // printf("Server Recieved : %c, ", recv_buffer[0]);
      printf("Recieved: %d bytes", ret);
      char *temp = &(recv_buffer[0]);
      fputs(temp, pFile);
      fclose(pFile);
      // printf("Ret : %d\n", ret);
    } else {
      printf("File not opened.\n");
    }


    if (ret <= 0) {
       printf("recv() error: %s.\n", strerror(errno));
       return NULL;
    } else if (ret == 0) {
      // The connection is terminated by the other end.
      printf("[%d] connection lost.\n", connfd);
      break;
    }

    // Send ACK to client if the Recieved character is a #
    // if (strncmp(&recv_buffer[0], "#", 1) == 0 ){
    // memset(&recv_buffer,0, sizeof(recv_buffer));
    // strcpy(recv_buffer,"server->you$file_recieved");
      res = send(connfd, recv_buffer, strlen(recv_buffer), 0);
    // }
    // printf(", Server Sending : %c\n", recv_buffer[0]);
    if(res < 0) {
      printf("send() error: %s.\n", strerror(errno));
      break;
    }
  }
  printf("[%d] worker thread terminated.\n", connfd);
}


int main(int argc, char *argv[]) {
    int ret, res;
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

    ret = ::bind(listenfd, (struct sockaddr*)
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

        if(connfd < 0) {
		      printf("accept() error: %s.\n", strerror(errno));
        	return -1;
        }
	      printf("connection accept from %s.\n", inet_ntoa(client_addr.sin_addr));

        pthread_t tid;
        pthread_create(&tid, NULL, worker_thread, (void *)connfd);
    }
    return 0;
}
