#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct Client {
  std::string username;
  std::string password;
  std::string token;
  int online = 0;
  long ip;
  long port;

  int ret, serv;

  int sockfd;
  struct sockaddr_in servaddr;
  char send_buffer[1024];
  char confirmation_buffer[1024];

  Client() {
  }

  int connect() {
    // Establishing connection.
    this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
	     printf("socket() error: %s.\n", strerror(errno));
       return -1;
    }

    // Setting up the server address.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(31000);

    printf("new client connection: %d\n", servaddr.sin_port);
  }

  void login(std::string username, std::string password) {
    std::cout << "Logging in " << username << " with " << password << "\n";
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
    std::string username = get_username();
    std::string password = get_password();

  }

  void login_menu() {
    int choice = get_input <int>(
         "Hello, Would you like to log in or register?" "\n"
         "[1] Login" "\n"
         "[2] Register" "\n"
         "[3] Exit");

         switch (choice) {
           case 1:
            login();
            break;
          case 2:
            // register_user();
            break;
        }
  }

  void load_users() {

  }
};
