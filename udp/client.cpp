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

using namespace std;

struct Client {
  string username;
  string password;
};

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
    while (get_input <int>("Confirm? [0|1]") != 1) {
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
  string username = get_username();
  string password = get_password();
  
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

int main() {
    int ret, serv;
    socklen_t len;
    int sockfd;
    struct sockaddr_in servaddr;
    char send_buffer[1024];
    char confirmation_buffer[1024];

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
    servaddr.sin_port = htons(31000);

    printf("new client connection: %d\n", servaddr.sin_port);

    /////////////////////////////////////////////////////////
    ////////////////// Login ////////////////////////////////
    /////////////////////////////////////////////////////////
    load_users();
    login_menu();

    while (1) {
        // Grabbing input from stdin storing it in send_buffer.
        fgets(send_buffer, sizeof(send_buffer), stdin);
        // strncat(udp_signature, send_buffer, 15);

        // sending send_buffer to the server.
        ret = sendto(sockfd,
                     send_buffer,
                     sizeof(send_buffer),
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

        printf("ACK: %s\n", confirmation_buffer);
        if (ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}
