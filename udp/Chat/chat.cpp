#include "../client.cpp"

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

void clear_stdin_buffer() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int chat_menu() {
  int choice;
  choice = get_input <int>("\n=============CHAT MENU============\n"
                            "[1] Login\n"
                            "[2] Send Message\n"
                            "[3] Register\n"
                            "[4] Logout\n"
                            "[5] Print Client\n"
                            "[6] Whose Online\n"
                            "[7] Exit\n");
  return choice;
}

int main() {
    // Creating a new client.
    Client client;
    bool skip;
    int maxfd;
    fd_set read_set;
    FD_ZERO(&read_set);

    int success = client.connect();
    if (success < 0) {
      return -1;
    }

    maxfd = client.sockfd_rx + 1;

    while (1) {
      Request request;
      int choice;

      std::cout << "\nClient listening for event...\n";

      // Use select to wait on keyboard input or socket receiving.
      FD_SET(fileno(stdin), &read_set);
      FD_SET(client.sockfd_rx, &read_set);
      select(maxfd, &read_set, NULL, NULL, NULL);

      if (FD_ISSET(fileno(stdin), &read_set)) {
        std::cout << "Keyboard Event!";
        choice = chat_menu( );
        skip = true;
        switch (choice) {
           case 1:
            request.type = LOGIN_SENT;
            request = client.create_login_request();
            client.make_request(request);
            break;
          case 2:
            std::cout << "case 2\n";
            request.type = SEND;
            if (client.online) {
              request = client.create_send_request();
              client.make_request(request);
            } else {
              std::cout << "Please login to send message.\n";
              skip = false;
            }
            break;
          case 3:
            request.type = REGISTER;
            request = client.create_user_request();
            client.make_request(request);
            break;
          case 4:
            request.type = LOGOUT_SENT;
            request = client.create_logout_request(client.username);
            client.make_request(request);
            break;
          case 5:
            client.print();
            break;
          case 6:
            request.type = SHOW_ALL_ONLINE_USERS;
            request = client.create_show_online_users_request();
            client.make_request(request);
            break;
          default:
            std::cout << "DEFAULT\n";
            request.type = SEND;
            request = client.create_send_request();
            client.make_request(request);
        } // END SWITCH STATEMENT

      } // END KEYBOARD EVENT IF STATEMENT

      if (FD_ISSET(client.sockfd_rx, &read_set)) {
        std::cout << "\nSERVER EVENT!";

        // client.print(); // For DEBUGGING purposes
        client.ret = recvfrom(client.sockfd_rx, &request,
                              sizeof(request), 0, NULL, NULL);
        if (client.ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }
        // Client will handle the Server RESPONSE;
        client.handle(request);
      }

    } // END WHILE LOOP

    return 0;
}
