make:
	g++ Server/Request.cpp client.cpp Chat/chat.cpp -std=c++17 -Wall -Wextra -pedantic-errors -o Chat/chat
	g++ Server/Users.cpp Server/Request.cpp Server/server.cpp -o Server/server


server:
	Server/server

chat:
	Chat/chat
