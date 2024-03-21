NAME = webserv
SRCS = 	src/main.cpp \
		src/Utils/Utils.cpp \
		src/Sockets/Socket.cpp \
        src/Sockets/BindingSocket.cpp \
        src/Sockets/ListeningSocket.cpp \
		src/HttpRequest.cpp \
		src/HttpResponse.cpp \
		src/HttpRequest.cpp \
		src/WebServer.cpp \
		src/Client.cpp \
		src/configFile.cpp \
		src/cgi.cpp \
		src/upload.cpp 

OBJS = $(SRCS:.cpp=.o)
CPPFLAGS = -Wall -Wextra -Werror -std=c++98  #-fsanitize=address
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