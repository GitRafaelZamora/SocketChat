#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Server/Request.cpp"

class Client {
  public : std::string username;
  public : std::string password;
  public : std::string token;
  public : int online = 0;
  public : long ip;
  public : long port;

  public : int ret, serv;

  public : int sockfd;
  public : struct sockaddr_in servaddr;
  public : char send_buffer[1024];
  public : std::string message;
  public : char confirmation_buffer[1024];

  Client() {
  }

  int connect() {
    // Establishing connection.
    this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->sockfd < 0) {
	     printf("socket() error: %s.\n", strerror(errno));
       return -1;
    }

    // Setting up the server address.
    memset(&this->servaddr, 0, sizeof(this->servaddr));
    this->servaddr.sin_family = AF_INET;
    this->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->servaddr.sin_port = htons(31000);

    printf("new client connection: %d\n", this->servaddr.sin_port);
    this->ip = this->sockfd;
    this->port = this->servaddr.sin_port;
    return 1;
  }

  int make_request(Request req) {
    int ret = sendto(this->sockfd,
                 &req,
                 sizeof(req),
                 0,
                 (struct sockaddr*) &this->servaddr,
                 sizeof(this->servaddr));
    return ret;
  }

  int make_request(Request req, std::string username, std::string password) {
    int ret = sendto(this->sockfd,
                 &req,
                 sizeof(req),
                 0,
                 (struct sockaddr*) &this->servaddr,
                 sizeof(this->servaddr));
    return ret;
  }

  int send_message(std::string) {
    ret = sendto(sockfd,
                 send_buffer,
                 sizeof(send_buffer),
                 0,
                 (struct sockaddr*) &servaddr,
                 sizeof(servaddr));
    return ret;
  }

  template <typename T> T get_input(const std::string &strQuery) {
      std::cout << strQuery << "\n> ";
      T out = T();
      while (!(std::cin >> out)) {
          std::cin.clear();
          std::cin.ignore(std::numeric_limits <std::streamsize>::max(), '\n');
          std::cout << "Error!" "\n";
          std::cout << strQuery << "\n> ";
      }
      std::cout << out << " got this\n";
      return out;
  }

  std::string get_username() {
      std::string username = get_input <std::string>("Please enter a username.");
      std::cout << "Username: \"" << username << "\"\n";
      while (get_input <int>("Confirm? [ 0 | 1 ]") != 1) {
          username = get_input <std::string>("Please enter a username.");
          std::cout << "Username: \"" << username << "\"\n";
      }
      return username;
  }

  std::string get_password() {
      std::string password1 = get_input <std::string> ("Please enter your password.");
      std::string password2 = get_input <std::string> ("Please re-enter your password.");

      while (password1 != password2) {
          std::cout << "Error! Passwords do not match." "\n";
          password1 = get_input <std::string>("Please enter your password.");
          password2 = get_input <std::string>("Please re-enter your password.");
      }

      return password1;
  }

  // Saving username file with password as the text.
  void save_user(const std::string &username, const std::string &password) {
    std::string filename = "users/" + username + ".txt";
    std::ofstream file(filename);
    file << password << "\n";
    std::cout << "Saved new user.\n";
  }

  void login() {
    Request request;
    request.type = LOGIN;
    username = get_username();
    password = get_password();
    this->username = username;
    this->password = password;
    request.msg_body += username;
    request.msg_body += ",";
    request.msg_body += password;
    request.print();
    make_request(request);
    online = 1;
  }

  void print() {
    std::cout << "Username : " << this->username << std::endl;
    std::cout << "Password : " << this->password << std::endl;
    std::cout << "IP : " << this->ip << std::endl;
    std::cout << "PORT : " << this->port << std::endl;
    std::cout << "Token : " << this->token << std::endl;
  }
};
