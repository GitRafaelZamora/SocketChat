#include <iostream>
#include <string>
#include <array>
#include <unordered_set>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>

struct OnlineUsers {
  int numberOfUsersOnline;

  std::map<std::string, struct sockaddr_in> users;

  OnlineUsers() {
    this->numberOfUsersOnline = 0;
  }

  void user_joined(std::string username, struct sockaddr_in forward_addrr) {
    this->numberOfUsersOnline++;
    this->users.insert(std::pair<std::string, struct sockaddr_in>(username, forward_addrr));
  }

  void user_left(std::string username) {
    this->numberOfUsersOnline--;
    this->users.erase ( username );
  }

  struct sockaddr_in find_user_address(std::string username) {
    return this->users.find(username)->second;
  }

  void show_all_users() {
    std::cout << "Showing all online users : \n";
    for ( const auto &user : this->users ) {
        std::cout << "Username : " << user.first << "\n";
        std::cout << "Port : " << user.second.sin_port << "\n";
    }
  }

  void print() {
    std::cout << "Number of Users Online : " << this->numberOfUsersOnline << std::endl;
    show_all_users();
  }
};
