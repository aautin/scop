# ---------------------------------------- #
NAME		:=	scop

INC_PATH	:=	inc
SRC_PATH	:=	src
OBJ_PATH	:=	obj

GLFW		:=	glfw/
GLFW_BUILD	:=	glfw/build/
GLFW_LIB	:=	glfw/build/src/libglfw3.a
GLFW_INC	:=	glfw/include/

SRC_FILES	:=	\
				main.cpp

OBJ_FILES	:=	$(SRC_FILES:.cpp=.o)

SRC			:=	$(addprefix $(SRC_PATH)/, $(SRC_FILES))
OBJ			:=	$(addprefix $(OBJ_PATH)/, $(OBJ_FILES))
DEP			:=	$(OBJ:.o=.d)

# ---------------------------------------- #
CC			:=	c++
RM			:=	rm -rf

# ---------------------------------------- #
$(NAME)			:	$(GLFW_LIB) $(OBJ_PATH) $(OBJ)
					$(CC) $(OBJ) -o $@ $(GLFW_LIB) -I$(GLFW_INC)

$(OBJ_PATH)		:
					@mkdir -p $(OBJ_PATH)

-include $(DEP)

$(OBJ_PATH)/%.o	:	$(SRC_PATH)/%.cpp
					$(CC) -c $< -o $@ -I$(INC_PATH) -I$(GLFW_INC)

$(GLFW_LIB)		:
					cmake -S glfw -B $(GLFW_BUILD)
					make -C $(GLFW_BUILD)

# ---------------------------------------- #
.PHONY		:	all clean cleanlibs fclean re

all			:	$(NAME)

clean		:	
				$(RM) $(OBJ_PATH)

cleandeps	:
				$(RM) $(GLFW_BUILD)

fclean		:	clean cleandeps
				$(RM) $(NAME)

re			:	fclean all