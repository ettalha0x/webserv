NAME = webserv
SRCS = 	src/main.cpp \
		src/Utils/Utils.cpp \
		src/Sockets/Socket.cpp \
        src/Sockets/BindingSocket.cpp \
        src/Sockets/ListeningSocket.cpp \
		src/HttpRequest.cpp \
		src/HttpResponse.cpp \
		src/WebServer.cpp \
		src/Client.cpp \
		src/configFile.cpp \
		src/cgi.cpp \
		src/upload.cpp 

INCLUDS = src/Utils/Utils.hpp \
		src/Sockets/Socket.hpp \
        src/Sockets/BindingSocket.hpp \
        src/Sockets/ListeningSocket.hpp \
		src/HttpRequest.hpp \
		src/HttpResponse.hpp \
		src/WebServer.hpp \
		src/Client.hpp \
		src/configFile.hpp \
		src/cgi.hpp \
		src/upload.hpp 

OBJS = $(SRCS:.cpp=.o)
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

%.o:%.cpp $(INCLUDS)
		c++ $(CPPFLAGS) -c $< -o $@

$(NAME): $(OBJS) 
		c++ ${CPPFLAGS} $^ -o $@
		mkdir session || true

all : $(NAME)

clean:
		$(RM) $(OBJS)

fclean: clean
		$(RM) $(NAME)

re: fclean all