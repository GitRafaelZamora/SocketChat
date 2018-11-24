#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>

#include "Request.cpp"

using namespace std;
// 012345678901234567890
// you->server#something
// server->you#Error: Unrecognized message format

#define BUFFSIZE 2048;

int main() {
    int ret;
    int sockfd;
    struct sockaddr_in servaddr, client_address;
    char recv_buffer[1024];
    char send_buffer[1024];
    socklen_t len;
    Request request;
    // login
    request.parse("​duolu->server#login<123456>");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
	     printf("socket() error: %s.\n", strerror(errno));
       return -1;
    }
    // Fill in the server's address given its name.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(31000);

    bind(sockfd,
         (struct sockaddr *) &servaddr,
         sizeof(servaddr));

    printf("%s", "listening on port ");
    printf("%d\n", servaddr.sin_addr.s_addr);

    while (1) {
        len = sizeof(client_address);
        // printf("server.cpp len: %d\n", len);

        // The address of the client.
        ret = recvfrom(sockfd,
                      recv_buffer,
                      sizeof(recv_buffer), 0,
                      (struct sockaddr *) &client_address,
                      &len
                    );

        // Parse the recv_buffer source->destination#msg

        strcpy(recv_buffer, request.parse(recv_buffer).c_str());
        printf("%s\n", recv_buffer);

        sendto(sockfd,
              recv_buffer,
              sizeof(recv_buffer),
              0,
              (struct sockaddr *) &client_address,
              len
            );

        if (ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }


    }

    return 0;
}
