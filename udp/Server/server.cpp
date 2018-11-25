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
#include <cstring>
#include <string>

#include "Request.cpp"

using namespace std;
// 012345678901234567890
// you->server#something
// server->you#Error: Unrecognized message format

#define BUFFSIZE 2048;

bool checkCredentials(const std::string& msg) {
    std::string s = msg;
    std::string username = s.substr(0, s.find(","));
    std::string password = s.substr(s.find(","), s.find("\0"));
    std::cout << username << " " << password << endl;
    ifstream f(msg.c_str());
    return f.good();
}

void save_user(const std::string &username, const std::string &password) {
  std::string filename = "users/" + username + ".txt";
  std::ofstream file(filename);
  file << password << "\n";
  std::cout << "Saved new user.\n";
}

int listen(int sockfd, Request &request, struct sockaddr_in client_address, socklen_t len) {
  int ret = recvfrom(sockfd,
                &request,
                sizeof(request), 0,
                (struct sockaddr *) &client_address,
                &len
              );
  return ret;
}

void send(int sockfd, char recv_buffer[1024], Request request, struct sockaddr_in client_address, socklen_t len) {
  strcpy(recv_buffer, request.msg_body.c_str());
  printf("%s\n", recv_buffer);

  sendto(sockfd,
        recv_buffer,
        sizeof(*recv_buffer),
        0,
        (struct sockaddr *) &client_address,
        len
      );
}

int main() {
    int ret;
    int sockfd;
    struct sockaddr_in servaddr, client_address;
    char recv_buffer[1024];
    char send_buffer[1024];
    socklen_t len;
    Request request;

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

    // request.parse("​duolu->server#login<123456>");

    while (1) {
        len = sizeof(client_address);
        printf("server.cpp len: %d\n", len);

        // The address of the client.
        ret = listen(sockfd, request, client_address, len);

        switch (request.type) {
          case LOGIN:
            std::cout << "Client Login Event" << std::endl;
            std::cout << request.type << std::endl;
            checkCredentials(request.msg_body);
          break;
          case SEND:
            std::cout << "Client SEND Event" << std::endl;
            std::cout << request.type << std::endl;
          break;
        }
        // Parse the recv_buffer source->destination#msg
        send(sockfd, recv_buffer, request, client_address, len);


        if (ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}
