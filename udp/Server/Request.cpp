////////////////////////////////////////////////////////////////////////////////
/////////////////////////// REQUEST LIBRARY ////////////////////////////////////
//// Library will be the mid layer for all the messages                 ////////
//// sent between the clients and the server.                           ////////
//// Example: source_address->destination_address#request_type          ////////
//////// request_type can be of type LOGIN, SUCCESS, SEND, FAILED       ////////
//////// SEND requires a client_token, message_id, and the message_body ////////
////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <string>

enum RequestType { LOGIN, SEND };

struct Request {
  // Required fields
  RequestType type;

  // Dependant on Type: SEND
  std::string from;
  std::string to;
  std::string body;

  // Dependant on Type: LOGIN
  std::string username;
  std::string password;

  std::string host;

  Request() {
  }

  Request(RequestType type, std::string host, std::string body) {
    this->type = type;
    this->host = host;
    this->body = body;
  }

  std::string parse(std::string buffer) {
    // printf("parseMessage\n", NULL);
    // Need to store source, dest, and msg
    std::string s = buffer;
    std::string src = s.substr(0, s.find("->"));
    std::string dest = s.substr(s.find("->")+2, (s.find("#")) - (s.find("->") + 2));
    std::string msg = s.substr(s.find("#")+1, (s.length() - 3) - s.find("#"));
    // printf("#: %lu\n", s.find("\0"));
    printf("src: |%s| dest: |%s| msg: |%s|\n", src.c_str(), dest.c_str(), msg.c_str());
    std::string client("client");
    std::string server("server");
    if (src.compare(client) != 0 && dest.compare(server) != 0) {
      // server->you#Error: Unrecognized message format
      std::cout << "ERROR PARSER\n";
      std::string str = "server->you#Error: Unrecognized message format";
      char *cstr = new char[str.length() + 1];
      strcpy(cstr, str.c_str());
      buffer = cstr;
      delete [] cstr;
    } else {
      std::string str = "server->you#" + msg;;
      char *cstr = new char[str.length() + 1];
      strcpy(cstr, str.c_str());
      buffer = cstr;
      delete [] cstr;
    }
    return buffer;
  }

  void print() {
    std::cout << "\n============REQUEST DETAILS============\n" << std::endl;
    std::cout << "username : " << this->username << std::endl;
    std::cout << "password : " << this->password << std::endl;
    std::cout << "to : " << this->to << std::endl;
    std::cout << "host : " << this->host << std::endl;
    std::cout << "body : " << this->body << std::endl;
    std::cout << "type : " << this->type << std::endl;
    std::cout << "FOUND NIL ERROR : FALSE" << std::endl;
  }
};
