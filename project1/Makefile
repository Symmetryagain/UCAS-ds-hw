CC = gcc
CXX = g++
SRC_DIR = src
OBJ_DIR = build
HEAD_DIR = include
CFLAGS = -Wall -Wextra -O2 -I $(HEAD_DIR)
CXXFLAGS = -Wall -Wextra -O2 -I $(HEAD_DIR)
SRC_C_FILES = $(wildcard $(SRC_DIR)/*.c)
SRC_CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_C_FILES = $(SRC_C_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_CPP_FILES = $(SRC_CPP_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJ_FILES = $(OBJ_C_FILES) $(OBJ_CPP_FILES)
TARGET = Project.out

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@echo "Linking with: $(OBJ_FILES)..."
	$(CXX) $(OBJ_FILES) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
