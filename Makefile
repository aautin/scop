# ---------------------------------------- #
NAME		:=	scop

INC_PATH	:=	inc
SRC_PATH	:=	src
OBJ_PATH	:=	obj

GLFW		:=	glfw/
GLFW_BUILD	:=	glfw/build/
GLFW_LIB	:=	glfw/build/src/libglfw3.a
GLFW_INC	:=	glfw/include/

GLM_INC		:=	glm/

SRC_FILES	:=	\
				glfwHandling.cpp \
				objFile.cpp \
				glad.cpp \
				vertex.cpp \
				matrix.cpp \
				main.cpp
				
OBJ_FILES	:=	$(SRC_FILES:.cpp=.o)

SRC			:=	$(addprefix $(SRC_PATH)/, $(SRC_FILES))
OBJ			:=	$(addprefix $(OBJ_PATH)/, $(OBJ_FILES))
DEP			:=	$(OBJ:.o=.d)

# ---------------------------------------- #
CC			:=	c++ -std=c++20 -lGL -lX11 -lpthread -ldl -g
RM			:=	rm -rf

# ---------------------------------------- #
$(NAME)			:	$(GLFW_LIB) $(OBJ_PATH) $(OBJ)
					$(CC) $(OBJ) -o $@ $(GLFW_LIB) -I$(GLFW_INC) -I$(GLM_INC)

$(OBJ_PATH)		:
					@mkdir -p $(OBJ_PATH)

-include $(DEP)

$(OBJ_PATH)/%.o	:	$(SRC_PATH)/%.cpp
					$(CC) -MMD -MP -c $< -o $@ -I$(INC_PATH) -I$(GLFW_INC) -I$(GLM_INC)

$(GLFW_LIB)		:
					cmake -S glfw -B $(GLFW_BUILD) -DGLFW_LIBRARY_TYPE=STATIC -DGLFW_BUILD_WAYLAND=OFF -DGLFW_BUILD_X11=ON
					make -C $(GLFW_BUILD) --no-print-directory

# ---------------------------------------- #
.PHONY		:	all clean cleandeps fclean re redeps

all			:	$(NAME)

clean		:	
				$(RM) $(OBJ_PATH)/*.o $(OBJ_PATH)/*.d

cleandeps	:
				$(RM) $(GLFW_BUILD)

fclean		:	clean
				$(RM) $(NAME)

re			:	fclean all
redeps	    :	cleandeps re