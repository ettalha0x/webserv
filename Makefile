NAME = webserv
SRCS = main.cpp Servers/Utils/Utils.cpp \
		Sockets/Socket.cpp \
        Sockets/BindingSocket.cpp \
        Sockets/ListeningSocket.cpp \
		Servers/Server.cpp \
		Servers/HttpResponse.cpp \
		Servers/WebServer.cpp
OBJS = $(SRCS:.cpp=.o)
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

%.o:%.cpp Webserv.hpp
		c++ $(FLAGS) -c $< -o $@

$(NAME): $(OBJS)
		c++ $^ -o $@

all : $(NAME)

clean:
		$(RM) $(OBJS)

fclean: clean
		$(RM) $(NAME)

re: fclean all