#include "Server/Request.cpp"
#include <random>

class Client {
  ///////////////////////////// Client Properties //////////////////////////////
  public : std::string username;
  public : std::string password;
  public : std::string token;
  public : int online = 0;

  public : int ret, serv;

  public : int sockfd_tx = 0;
  public : int sockfd_rx = 0;
  public : struct sockaddr_in serv_addr, client_addr;
  public : char send_buffer[1024];
  public : std::string message;
  public : char confirmation_buffer[1024];

  //////////////////////////// Client Constructor //////////////////////////////
  Client() {
  }

  /////////////////////////////// Client Methods ///////////////////////////////
  int connect() {
    // Establishing connection.
    this->sockfd_tx = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->sockfd_tx < 0) {
	     printf("socket() error: %s.\n", strerror(errno));
       return -1;
    }

    // Setting up the server address.
    memset(&this->serv_addr, 0, sizeof(this->serv_addr));
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    this->serv_addr.sin_port = htons(31000);

    sockfd_rx = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_rx < 0) {
    	printf("socket() error: %s.\n", strerror(errno));
      return -1;
    }

    memset(&this->client_addr, 0, sizeof(this->client_addr));
    this->client_addr.sin_family = AF_INET;
    this->client_addr.sin_addr.s_addr = inet_addr("1​27.0.0.1​");
    int random = generate_random_port();
    std::cout << random << std::endl;
    this->client_addr.sin_port = htons(random);

    // struct timeval tv;
    // tv.tv_sec = 1;
    // tv.tv_usec = 0;
    // setsockopt(this->sockfd_rx, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    // setsockopt(this->sockfd_tx, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    bind(this->sockfd_rx, (struct sockaddr *) &client_addr, sizeof(client_addr));

    printf("new client connection: %d\n", this->serv_addr.sin_port);
    return 1;
  } // END CONNECT

  Request create_login_request() {
      Request request;

      request.type = LOGIN_SENT;
      username = get_username();
      password = get_password();
      this->username = username; request.username = username;
      this->password = password; request.password = password;
      request.print();

      return request;
  }

  Request create_show_online_users_request() {
    Request request;

    request.type = SHOW_ALL_ONLINE_USERS;

    return request;
  }

  Request create_send_request() {
    Request request;

    request.type = SEND;
    request.from = this->username;
    request.to = get_input <std::string>("To : ");
    request.body = get_input <std::string>("Message : ");
    request.print();

    return request;
  }

  Request create_user_request() {
    Request request;

    request.type = REGISTER;
    request.username = get_username(); this->username = request.username;
    request.password = get_password(); this->password = request.password;
    request.print();

    return request;
  }

  Request create_logout_request() {
    Request request;

    request.type = LOGOUT_SENT;
    request.username = get_username();
    request.print();

    return request;
  }

  // Send Request to the Server.
  // ret : 0 on success | -1 on Failure
  int make_request(Request request) {
    int ret = sendto(this->sockfd_rx, &request, sizeof(request),
                 0, (struct sockaddr*) &this->serv_addr, sizeof(this->serv_addr));
    return ret;
  }

  // Saving username file with password as the text.
  void save_user(const std::string &username, const std::string &password) {
    std::string filename = "users/" + username + ".txt";
    std::ofstream file(filename);
    file << password << "\n";
    std::cout << "Saved new user.\n";
  }

  void handle(Request request) {
    switch (request.type) {
      case LOGIN_SENT:
        std::cout << "\n============LOGIN RESPONSE============\n" << std::endl;
        break;
      case LOGOUT_SENT:
        std::cout << "\n============LOGOUT_SENT RESPONSE============\n" << std::endl;
        break;
      case OFFLINE:
        std::cout << "\n============OFFLINE RESPONSE============\n" << std::endl;
        this->online = 0;
        break;
      case REGISTER:
        std::cout << "\n============REGISTER RESPONSE============\n" << std::endl;
        break;
      case SEND:
        std::cout << "\n============INCOMING MESSAGE============\n" << std::endl;
        std::cout << "Message From: " << request.from << std::endl;
        std::cout << "Body : " << request.body << std::endl;
        std::cout << "\n============END MESSAGE============\n" << std::endl;
        break;
      case ONLINE:
        std::cout << "\n============ONLINE RESPONSE============\n" << std::endl;
        this->online = 1;
        std::cout << "Welcome back, " << request.username << " you are now logged in." << std::endl;
        break;
      case FAILED:
        std::cout << "\n============FAILED RESPONSE============\n" << std::endl;
        break;
      case SHOW_ALL_ONLINE_USERS:
        std::cout << "\n============SHOW_ALL_ONLINE_USERS RESPONSE============\n" << std::endl;
        break;
      case MESSAGE_SENT:
        std::cout << "\n============MESSAGE_SENT RESPONSE============\n" << std::endl;
        break;
      default:
        std::cout << "DEFAULT\n";
        break;
    }
  }

  ///////////////////////////// Client Helpers /////////////////////////////////
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

  int generate_random_port() {
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(6000,30000); // guaranteed unbiased

    auto random_integer = uni(rng);
    return random_integer;
  }

  void print() {
    std::cout << "==================CLIENT DETAILS==========" << std::endl;
    std::cout << "Username : " << this->username << std::endl;
    std::cout << "Password : " << this->password << std::endl;
    std::cout << "Online : " << this->online << std::endl;
    std::cout << "PORT : " << this->client_addr.sin_port << std::endl;
    std::cout << "Sockfd : " << this->sockfd_rx << std::endl;
    std::cout << "Servaddr : " << (struct sockaddr *) &this->serv_addr << std::endl;
    std::cout << "Clientaddr : " << (struct sockaddr *) &this->client_addr << std::endl;
    std::cout << "Token : " << this->token << std::endl;
  }
};
