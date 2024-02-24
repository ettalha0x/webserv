NAME = webserv
SRCS = main.cpp Servers/Utils/Utils.cpp \
		Sockets/Socket.cpp \
        Sockets/BindingSocket.cpp \
        Sockets/ListeningSocket.cpp \
		Servers/Server.cpp \
		Servers/HttpRequest.cpp \
		Servers/HttpResponse.cpp \
		Servers/HttpRequest.cpp \
		Servers/WebServer.cpp \
		Servers/Client.cpp \
		config/configFile.cpp \
		cgi.cpp
OBJS = $(SRCS:.cpp=.o)
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address
RM = rm -rf

%.o:%.cpp Webserv.hpp
		c++ $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS)
		c++ ${CPPFLAGS} $^ -o $@

all : $(NAME)

clean:
		$(RM) $(OBJS)

fclean: clean
		$(RM) $(NAME)

re: fclean all