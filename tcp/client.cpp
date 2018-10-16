#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>

using namespace std;

string parseMessage(char buffer[1024]) {
  // Need to store source, dest, and msg.
  string s(buffer);
  string src = s.substr(0, s.find("->"));
  int length = (s.find("$")) - (s.find("->") + 2);
  string dest = s.substr(s.find("->") + 2, length);
  string path = s.substr(s.find("$") + 1, s.find('\0'));
  printf("-----File Details-----\n");
  printf("src: %s dest: %s path: %s\n", src.c_str(), dest.c_str(), path.c_str());
  printf("----------------------\n");
  return path;
}

void openFile(string path) {
    ifstream myfile(path, fstream::in);

    if (myfile.is_open()) {
      string line;
      printf("Opening : %s\n", path.c_str());
      while ( getline(myfile,line) ) {
        printf("%s\n", line.c_str());
      }
      myfile.close();
    } else {
      cout << "Unable to open file";
    }
}

int main(int argc, char *argv[]) {
    int ret;
    int sockfd = 0;
    char send_buffer[1024];
    struct sockaddr_in serv_addr;

    // Create socket socket stream.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Make sure at least one connection exist.
    if (sockfd < 0) {
        printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }

    // Allocating size for our socket protocol and port
    memset(&serv_addr, 0, sizeof(serv_addr));
    // settin fam to AF_NET
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr
        = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(31000);

    // Connecting
    ret = connect(sockfd,
                  (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr));
    if (ret < 0) {
	printf("connect() error: %s.\n", strerror(errno));
        return -1;
    }

    while (1) {
        fgets(send_buffer,
              sizeof(send_buffer),
              stdin);
        // Test: you->server$file.txt
        string path = parseMessage(send_buffer).c_str();
        openFile(path);

        ret = send(sockfd,
                   send_buffer,
                   strlen(send_buffer) + 1,
                   0);
        if(ret < 0) {
	    printf("send() error: %s.\n", strerror(errno));
            break;
        }
    }

    return 0;
}
