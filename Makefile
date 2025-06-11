# Webserv Makefile
NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Source files
SRCS = main.cpp config_parser.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
INCS = webserv_config.hpp

# Main rule
all: $(NAME)

# Compiling the executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(NAME) has been compiled!"

# Compiling object files
%.o: %.cpp $(INCS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cleaning object files
clean:
	rm -f $(OBJS)
	@echo "Object files have been cleaned!"

# Cleaning everything
fclean: clean
	rm -f $(NAME)
	@echo "$(NAME) has been cleaned!"

# Rebuilding everything
re: fclean all

# Additional rules
run: $(NAME)
	./$(NAME) default.conf

# Phony targets
.PHONY: all clean fclean re run
