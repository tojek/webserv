EXEC        = webserv
CC          = c++
CFLAGS      = -Wall -Werror -Wextra -std=c++98 -I include
RM          = rm -f

# Colors

DEF_COLOR   = \033[0;39m
YELLOW      = \033[0;93m
GREEN       = \033[0;92m
BLUE        = \033[0;94m
CYAN        = \033[0;96m

# Sources
SRC_DIR     = ./src/
OBJ_DIR     = ./obj/
SRC_FILES   = Utils.cpp LocationTokens.cpp Definitions/Location.cpp Definitions/Request.cpp Definitions/ConfigParser.cpp Definitions/Server.cpp Definitions/Client.cpp Definitions/Signal.cpp ConfigTokens.cpp main.cpp 
SRC         = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ         = $(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))

###

OBJF        = .cache_exists

all: $(EXEC)

$(EXEC): $(OBJ)
	@echo "$(YELLOW)Linking executable: $(EXEC)$(DEF_COLOR)"
	@$(CC) $(OBJ) -o $(EXEC)
	@echo "$(GREEN)Executable $(EXEC) created!$(DEF_COLOR)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp | $(OBJF)
	@echo "$(YELLOW)Compiling: $< $(DEF_COLOR)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJF):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p obj/Definitions

clean:
	@$(RM) -rf $(OBJ_DIR)
	@echo "$(BLUE)Object files cleaned!$(DEF_COLOR)"

fclean: clean
	@$(RM) -f $(EXEC)
	@echo "$(CYAN)Executable file cleaned!$(DEF_COLOR)"

re: fclean all
	@echo "$(GREEN)Cleaned and rebuilt everything!$(DEF_COLOR)"

.PHONY: all clean