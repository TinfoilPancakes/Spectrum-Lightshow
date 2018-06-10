#
#	C / C++ Project Template.
#	Version 0.0.0 r2
#	Prateek R. Patil
#

NAME		:=	lightshow

CC			:=	g++
C_FLAGS		:=	-Wall -O3 -Wextra
CXX_FLAGS 	:=	-Wall -O3 -Wextra -std=c++14 -ffast-math
LNK_FLAGS	:=	-Wall -O3 -Wextra -std=c++14 -L. -ffast-math
LNK_LIBS	:=	pthread fftw3 pulse-simple pulse m

SRC_DIR 	:=	./Sources
INC_DIR		:=	./Includes
OBJ_DIR		:=	./Objects

#	Including source files.
#	Variable should be named C_FILES or CXX_FILES

-include sources.mk

SOURCES		:=	$(addprefix $(SRC_DIR)/, $(C_FILES)) \
				$(addprefix $(SRC_DIR)/, $(CXX_FILES))

INCUDES		:=	$(addprefix -I , $(INC_DIR))

OBJECTS		:=	$(addprefix $(OBJ_DIR)/, $(patsubst %.c, %.c.o, $(C_FILES))) \
				$(addprefix $(OBJ_DIR)/, $(patsubst %.cpp, %.cpp.o, $(CXX_FILES)))

LIBRARIES	:=	$(addprefix -l , $(LNK_LIBS))

all : $(NAME)

#	Implicit rule for object files

$(OBJ_DIR)/%.c.o : $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@printf "Compiling [ %s ]: " $<
	@$(CC) $(C_FLAGS) -c $< -o $@ $(INCUDES)
	@printf "Done.\n"

$(OBJ_DIR)/%.cpp.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@printf "Compiling [ %s ]: " $<
	@$(CC) $(CXX_FLAGS) -c $< -o $@ $(INCUDES)
	@printf "Done.\n"

$(NAME) : $(OBJECTS)
	@printf "Linking [ %s ]: " $(NAME)
	@$(CC) $(LNK_FLAGS) -o $(NAME) $(OBJECTS) $(INCUDES) $(LIBRARIES)
	@printf "Done.\n"

fclean : clean
	@printf "Cleaning [ %s ]: " $(NAME)
	@rm -rf $(NAME)
	@printf "Done.\n"

clean :
	@printf "Cleaning [ %s ]: " $(OBJ_DIR)
	@rm -rf $(OBJ_DIR)
	@printf "Done.\n"

re : fclean all

.PHONY : re fclean clean