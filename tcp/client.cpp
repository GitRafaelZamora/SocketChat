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
#include <pthread.h>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <regex>

using namespace std;

struct tcp_header {
  string src;
  string dest;
  string msg;
  string file_path;
  int file_size;
};
/*
012345678901234567890
you->server$file.txt
you->sever$file.txt
yo->server$file.txt
you->serverfile.txt
*/
tcp_header parseMessage(char buffer[1024]) {
  // Create a tcp_header struct to store src, dest, msg
  tcp_header header;
  // Convert buffer into a string.
  string s(buffer);
  // Grab the src from the string
  header.src       =  s.substr(0, s.find("->"));
  header.dest      =  s.substr(s.find("->") + 2, (s.find("$") - (s.find("->") + 2)));
  header.file_path =  s.substr((s.find("$") + 1), (s.length() - 1) - (s.find("$") + 1) );
  //&& s.find("$") > s.find("->")
  if (header.src == "you" && header.dest == "server") {
    cout << "Valid Message Format." << endl;
    printf("---------Message Details---------\n");
    printf("src: %s dest: %s path: %s \n", header.src.c_str(), header.dest.c_str(), header.file_path.c_str());
    printf("---------------------------------\n");
  } else {
    cout << "Invalid Message Format." << endl;
    header.src = "";
    header.dest = "";
    header.file_path = "";
  }


  return header;
}

long getFileSize(string filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

tcp_header openFile(tcp_header client_header) {
    // TODO: You need to parse the string you read from the keyboard.
	  // If it follows the format "you->server$file_name", extract the
	  // file name and open the file, read each chunk and send the
	  // chunk. You may need to write an inner loop to read and send
	  // each chunk.

    // Open file.
    fstream myfile;
    myfile.open(client_header.file_path, fstream::in);
    // store the file size in client_header
    client_header.file_size = getFileSize(client_header.file_path);
    printf("%s size: %d bytes\n", client_header.file_path.c_str(), client_header.file_size);

    if (myfile.is_open()) {
      string line;
      printf("Opening : %s\n", client_header.file_path.c_str());
      while ( getline(myfile, line) ) {
        // build msg from file
        printf("%s\n", line.c_str() );
        client_header.msg += line;
      }
      printf("%s\n", client_header.msg.c_str());
      myfile.close();
    } else {
      cout << "Unable to open file\n";
    }
    return client_header;
}

int main(int argc, char *argv[]) {
    int ret, res;
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
        // Get src->dest#msg from console input.
        fgets(send_buffer, sizeof(send_buffer), stdin);

        // Parse the console string into a tcp_header struct.
        tcp_header client_header = parseMessage(send_buffer);
        // Open a file client's specified filepath.
        // Store the tcp message in the client_header.
        client_header = openFile(client_header);


        char char_buffer[1];
        char recv_buffer[1];
        // Send client_header.msg to server byte by byte
        for (int i = 0; i < client_header.msg.length(); i++) {
          char_buffer[0] = client_header.msg[i];

          // Sending one byte to the server.
          ret = send(sockfd, char_buffer, strlen(char_buffer), 0);
          printf("Client Sending : %c, ", char_buffer[0]);
           if(ret < 0) {
             printf("send() error: %s.\n", strerror(errno));
             break;
           }

           // Listen for ACK from server.
           res = recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);
           printf("Server ACK : %c\n", recv_buffer[0]);
           if (res <= 0) {
              printf("recv() error: %s.\n", strerror(errno));
              return -1;
           }
        }
    }
    return 0;
}
