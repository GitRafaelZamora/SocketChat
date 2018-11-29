#include <cstring>

#include "Request.cpp"
#include "Users.cpp"

#define BUFFSIZE 2048;

bool checkCredentials(Request request) {
  bool valid = false;

  std::string username = request.username;
  std::string password = request.password;
  std::cout << "username : " << username << std::endl;
  std::cout << "password : " << password << std::endl;
  std::string filename = "Users/";
  filename += request.username;
  filename += ".txt";

  std::ifstream f(filename);
  std::string pass;
  std::getline(f, pass);
  // std::cout << "Filename : |" << filename << "|" << std::endl;
  // std::cout << "pass : |" << pass << "|" << std::endl;
  if ( f.good() ) {
    if (pass == request.password) {
      valid = true;
    } else {
      std::cout << "Error : Invalid Password" << std::endl;
    }
  } else {
    std::cout << "Error : Username \"" << request.username << "\" Not Found." << std::endl;
    valid = false;
  }

  return valid;
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

void send(int sockfd, Request request, struct sockaddr_in client_address, socklen_t len) {
  sendto(sockfd, &request, sizeof(request), 0,
        (struct sockaddr *) &client_address, len);
}

void printEndLog() {
  std::cout << "\n============END LOG============\n" << std::endl;
}

int main() {
    int ret;
    int sockfd;
    struct sockaddr_in servaddr, client_address, forward_address;
    char recv_buffer[1024];
    char send_buffer[1024];
    socklen_t len;
    Request request;
    OnlineUsers online_users;

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

    while (1) {
        len = sizeof(client_address);
        // printf("server.cpp len: %d\n", len);

        // The address of the client.
        // ret = listen(sockfd, request, client_address, len);
        ret = recvfrom(sockfd,
                      &request,
                      sizeof(request), 0,
                      (struct sockaddr *) &client_address,
                      &len
                    );

        switch (request.type) {
          case LOGIN_SENT:
            std::cout << "\n============LOGIN EVENT============\n" << std::endl;
            if (checkCredentials(request)) {
              request.type = ONLINE;
              online_users.user_joined(request.username, client_address); // TODO: Store Port for the user
            } else {
              request.type = FAILED;
            }
            printEndLog();
            break;
          case SHOW_ALL_ONLINE_USERS:
            std::cout << "\n============SHOW ONLINE USERS EVENT============\n" << std::endl;
            request.body = online_users.show_all_users();
            printEndLog();
            break;
          case LOGOUT_SENT:
            std::cout << "\n============LOGOUT SENT EVENT============\n" << std::endl;
            std::cout << request.type << std::endl;
            online_users.user_left(request.username);
            request.type = OFFLINE;
            printEndLog();
            break;
          case REGISTER:
            std::cout << "\n============REGISTER EVENT============\n" << std::endl;
            std::cout << "Registered new user : " << request.username << std::endl;
            printEndLog();
            break;
          case SEND:
            std::cout << "\n============SEND EVENT============\n" << std::endl;
            forward_address = online_users.find_user_address(request.to);
            std::cout << "From : " << request.from << std::endl;
            std::cout << "To : " << request.to << std::endl;
            std::cout << "Port : " << forward_address.sin_port << std::endl;
            std::cout << "sin_family : " << forward_address.sin_family << std::endl;
            std::cout << "Body : " << request.body << std::endl;
            // send the message back to the correct reciever.
            send(sockfd, request, forward_address, len);
            printEndLog();
            break;
          case ONLINE:
            std::cout << "\n============ONLINE EVENT============\n" << std::endl;
            std::cout << request.type << std::endl;
            printEndLog();
            break;
          case OFFLINE:
            std::cout << "\n============OFFLINE EVENT============\n" << std::endl;
            std::cout << request.type << std::endl;
            printEndLog();
            break;
          case FAILED:
            std::cout << "\n============FAILED EVENT============\n" << std::endl;
            std::cout << request.type << std::endl;
            printEndLog();
            break;
        }
        if (request.type != SEND) {
          // Return the request status.
          send(sockfd, request, client_address, len);
        }

        if (ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}
