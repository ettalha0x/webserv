NAME = webserv
SRCS = main.cpp Networking/Sockets/Socket.cpp \
        Networking/Sockets/BindingSocket.cpp \
        Networking/Sockets/ConnectingSocket.cpp \
        Networking/Sockets/ListeningSocket.cpp \
		Networking/Servers/Server.cpp \
		Networking/Servers/TestServer.cpp
OBJS = $(SRCS:.cpp=.o)
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

%.o:%.cpp Networking/Networking.hpp
		c++ $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
		c++ $^ -o $@

all : $(NAME)

clean:
		$(RM) $(OBJS)

fclean: clean
		$(RM) $(NAME)

re: fclean all