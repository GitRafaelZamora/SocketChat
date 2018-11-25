#include <cstring>

#include "Request.cpp"

#define BUFFSIZE 2048;

bool checkCredentials(Request request) {
    std::string username = request.username;
    std::string password = request.password;
    std::cout << "username : " << username << std::endl;
    std::cout << "password : " << password << std::endl;

    std::string filename = "Users/";
    filename += request.username;
    filename += ".txt";
    std::cout << "Filename : |" << filename << "|" << std::endl;
    if (FILE *file = fopen(filename.c_str(), "r")) {
      std::cout << "Sweet?" << std::endl;
      fclose(file);
      return true;
    } else {
      std::cout << "No Sweet?" << std::endl;
      return false;
    }
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
  strcpy(recv_buffer, request.body.c_str());
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
            std::cout << "\n============LOGIN EVENT============\n" << std::endl;
            if (checkCredentials(request)) {
              std::cout << "Welcome back, " << request.username << std::endl;
            } else {
              std::cout << "User " << request.username << " Not Found." << std::endl;
              return -1;
            }
            std::cout << "\n============Login Complete============\n" << std::endl;
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
