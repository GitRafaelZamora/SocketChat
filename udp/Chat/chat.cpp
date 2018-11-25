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
       "Hello, Would you like to log in or register?" "\n"
       "[1] Login" "\n"
       "[2] Register" "\n"
       "[3] Exit");
      return choice;
}

int main() {
    // Creating a new client.
    Client client;
    int success = client.connect();
    if (success < 0) {
      return -1;
    }

    int choice = chat_menu();
    Request request;
    switch (choice) {
       case 1:
        request.type = LOGIN;
        request = client.create_login_request();
        client.make_request(request);
        break;
      case 2:
        request.type = SEND;
        client.make_request(request);
        break;
      default:
        request.type = SEND;
        request = client.create_send_request();
        client.make_request(request);
    }

    while (1) {
        // Grabbing input from stdin storing it in send_buffer.
        fgets(client.send_buffer, sizeof(client.send_buffer), stdin);
        // strncat(udp_signature, send_buffer, 15);

        // sending send_buffer to the server.
        if (client.online) {
          // client.send_message("client_a->client_b#SEND<hello client_b>");
        }


        // grabbing the size of our confirmation_buffer.
        socklen_t len;
        len = sizeof(client.confirmation_buffer);

        // grabbing the size of the return msg.
        // storing the return msg in confirmation_buffer.
        client.serv = recvfrom(client.sockfd,
                        client.confirmation_buffer,
                        sizeof(client.confirmation_buffer),
                        0,
                        (struct sockaddr *) &client.servaddr, // Where the message is sent.
                        &len);

        printf("ACK: %s\n", client.confirmation_buffer);
        if (client.ret <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }
    }

    return 0;
}
