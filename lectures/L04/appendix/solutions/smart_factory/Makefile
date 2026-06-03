# Target application.
TARGET := smart_factory

# C++ compiler.
CXX_COMPILER := g++

# C++ compiler flags.
CXX_FLAGS := -Wall -Werror -std=c++17 -Iinclude

# Source files.
SOURCE_FILES := source/main.cpp \

# Build and run the application as default.
default: build run

# Build the application.
build:
	@$(CXX_COMPILER) $(SOURCE_FILES) -o $(TARGET) $(CXX_FLAGS)

# Run the application.
run:
	@./$(TARGET)

# Clean the application.
clean:
	@rm -f $(TARGET)
