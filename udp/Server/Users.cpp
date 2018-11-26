#include <iostream>
#include <string>
#include <array>
#include <unordered_set>
#include <netinet/in.h>

struct User {
  std::string username;
  struct sockaddr_in address;
};

struct OnlineUsers {
  int numberOfUsersOnline;

  std::unordered_set<std::string> users;

  OnlineUsers() {
    this->numberOfUsersOnline = 0;
  }

  void user_joined(std::string username) {
    this->numberOfUsersOnline++;
    this->users.insert(username);
  }

  void user_left(std::string username) {
    this->users.erase ( username );
  }

  void find_user(std::string username) {
    std::unordered_set<std::string>::const_iterator finder = this->users.find(username);
    if ( finder == this->users.end() ) {
      std::cout << "not found in myset";
    } else {
      std::cout << *finder << " is in myset";
    }
    std::cout << std::endl;
  }

  void show_all_users() {
    std::cout << "Showing all online users : \n";
    for (const std::string& x: this->users) std::cout << " " << x;
    std::cout <<  std::endl;
  }

  void print() {
    std::cout << "Number of Users Online : " << this->numberOfUsersOnline << std::endl;
    show_all_users();
  }
};
