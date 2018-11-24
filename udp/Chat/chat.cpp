#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

#include "../Server/Request.cpp"
#include "../Client.cpp"

int main() {

    /////////////////////////////////////////////////////////
    ////////////////// Login ////////////////////////////////
    /////////////////////////////////////////////////////////
    Client client;
    int success = client.connect();
    if (success < 0) {
      return -1;
    }
    client.load_users();
    client.login_menu();

    while (1) {
        // Grabbing input from stdin storing it in send_buffer.
        fgets(client.send_buffer, sizeof(client.send_buffer), stdin);
        // strncat(udp_signature, send_buffer, 15);

        // sending send_buffer to the server.
        client.ret = sendto(client.sockfd,
                     client.send_buffer,
                     sizeof(client.send_buffer),
                     0,
                     (struct sockaddr*) &client.servaddr,
                     sizeof(client.servaddr));

        // grabbing the size of our confirmation_buffer.
        socklen_t len;
        len = sizeof(client.confirmation_buffer);

        // grabbing the size of the return msg.
        // storing the return msg in confirmation_buffer.
        client.serv = recvfrom(client.sockfd,
                        client.confirmation_buffer,
                        sizeof(client.confirmation_buffer),
                        0,
                        (struct sockaddr *) &client.servaddr, // Where the message is sent.
                        &len);

        printf("ACK: %s\n", client.confirmation_buffer);
        if (client.ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}
