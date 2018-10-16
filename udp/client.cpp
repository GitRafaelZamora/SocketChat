#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct udp_header {
  sockaddr_in source_address;
  sockaddr_in destination_address;
  int udp_length;
  int check_sum;
};


int main() {


    int ret, serv;
    socklen_t len;
    int sockfd;
    struct sockaddr_in servaddr;
    char send_buffer[1024];
    char confirmation_buffer[1024];
    char udp_signature[1024] = "client->server#";

    // Establishing connection.
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
	     printf("socket() error: %s.\n", strerror(errno));
       return -1;
    }

    // Setting up the server address.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(32000);

    printf("new client connection: %d\n", servaddr.sin_port);

    while (1) {
        // Grabbing input from stdin storing it in send_buffer.
        fgets(send_buffer, sizeof(send_buffer), stdin);
        strncat(udp_signature, send_buffer, 15);

        // sending send_buffer to the server.
        ret = sendto(sockfd,
                     udp_signature,
                     sizeof(udp_signature),
                     0,
                     (struct sockaddr *) &servaddr,
                     sizeof(servaddr));

        // grabbing the size of our confirmation_buffer.
        len = sizeof(confirmation_buffer);

        // grabbing the size of the return msg.
        // storing the return msg in confirmation_buffer.
        serv = recvfrom(sockfd,
                        confirmation_buffer,
                        sizeof(confirmation_buffer),
                        0,
                        (struct sockaddr *) &servaddr, // Where the message is sent.
                        &len);

        printf("server->you#[echo] %s\n", confirmation_buffer);

        if (ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}
