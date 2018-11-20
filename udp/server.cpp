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

using namespace std;
// 012345678901234567890
// you->server#something
// server->you#Error: Unrecognized message format

struct client {
  string id;
  string destination;
  string password;

};

#define BUFFSIZE 2048;

string parseMessage(char buffer[1024]) {
  // printf("parseMessage\n", NULL);
  // Need to store source, dest, and msg
  std::string s = buffer;
  std::string src = s.substr(0, s.find("->"));
  std::string dest = s.substr(s.find("->")+2, (s.find("#")) - (s.find("->") + 2));
  std::string msg = s.substr(s.find("#")+1, (s.length() - 3) - s.find("#"));
  // printf("#: %lu\n", s.find("\0"));
  printf("src: |%s| dest: |%s| msg: |%s|\n", src.c_str(), dest.c_str(), msg.c_str());
  string client("client");
  string server("server");
  if (src.compare(client) != 0 && dest.compare(server) != 0) {
    // server->you#Error: Unrecognized message format
    cout << "ERROR PARSER" << endl;
    std::string str = "server->you#Error: Unrecognized message format";
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    buffer = cstr;
    delete [] cstr;
  } else {
    std::string str = "server->you#" + msg;;
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    buffer = cstr;
    delete [] cstr;
  }
  return buffer;
}

int main() {
    int ret;
    int sockfd;
    struct sockaddr_in servaddr, client_address;
    char recv_buffer[1024];
    char send_buffer[1024];
    socklen_t len;

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

        strcpy(recv_buffer, parseMessage(recv_buffer).c_str());
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
