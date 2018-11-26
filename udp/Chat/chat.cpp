#include "../Client.cpp"

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

int chat_menu() {
  int choice = get_input <int>(
       "=============CHAT MENU============" "\n"
       "[1] Login" "\n"
       "[2] Send Message\n"
       "[3] Register" "\n"
       "[4] Logout" "\n"
       "[5] Print Client" "\n"
       "[6] Whose Online" "\n"
       "[7] Exit");
      return choice;
}

int main() {
    // Creating a new client.
    Client client;
    int success = client.connect();
    if (success < 0) {
      return -1;
    }
    bool skip;
    while (1) {
        int choice = chat_menu();
        skip = true;
        Request request;
        switch (choice) {
           case 1:
            request.type = LOGIN_SENT;
            request = client.create_login_request();
            client.make_request(request);
            break;
          case 2:
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
            request = client.create_logout_request();
            client.make_request(request);
            std::cout << "GOODBYE!" << std::endl;
            client.print();
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
            request.type = SEND;
            request = client.create_send_request();
            client.make_request(request);
        }
        // Grabbing input from stdin storing it in send_buffer.
        // fgets(client.send_buffer, sizeof(client.send_buffer), stdin);

        if (choice != 5) {
          if (skip) {
            // grabbing the size of our confirmation_buffer.
            socklen_t len;
            len = sizeof(request);

            // grabbing the size of the return msg.
            // storing the return msg in confirmation_buffer.
            // client.print();
            client.ret = recvfrom(client.sockfd, &request, sizeof(request), 0,
                            NULL, NULL);

            client.handle(request);
            printf("ACK: %u\n", request.type);
            if (client.ret <= 0) {
                printf("recvfrom() error: %s.\n", strerror(errno));
                return -1;
            }
          }
        }
    }

    return 0;
}
