# Appendix A
## Compiling and Executing Code in Linux
If you are using a Linux environment, you can use *make* to compile and execute your code.  
Below is a description of how to install **WSL** in order to build and run code in a Linux environment on a Windows computer.

---

## Installation
WSL *(Windows Subsystem for Linux)* allows running a Linux distribution in a terminal environment directly inside Windows, without using a virtual machine.   
In this course we will use the Linux distribution **Ubuntu**.

WSL is used in this course to:
* Build, store, and run code examples.
* Compile and execute C and C++ programs.
* Work in a Linux environment using standard development tools such as `gcc`, `g++`, `make`, and `cmake`.

---

### Installing WSL and Ubuntu
1.  Open **Windows PowerShell as Administrator**.
2.  Run the following commands:

``` bash
wsl --install
wsl --set-default-version 2
```

Alternatively, Ubuntu can be installed directly using:

``` bash
wsl --install -d Ubuntu
```

**Note:** If the system asks for a restart, restart the computer before continuing.

Verify the installation with:

``` bash
wsl --status
```

Ensure that **Default Version: 2** is displayed.

---

### If Ubuntu was not installed automatically
1.  Open **Microsoft Store**.
2.  Search for `Ubuntu`.
3.  Select the latest LTS version (currently `Ubuntu 24.04 LTS`).
4.  Click **Install**.

---

### Starting Ubuntu
1.  Type `Ubuntu` in the Windows search bar.
2.  Launch the distribution.

At the first startup:
* Choose a short and clear username (for example your first name).
* Choose a password.
* Store the password safely.

**Note:** The password is used only inside the Linux environment.

---

### Installing Basic Development Tools
After Ubuntu has started, update the system and install the necessary tools:

``` bash
sudo apt -y update
sudo apt -y upgrade
sudo apt -y install build-essential cmake
```

This installs:
* `gcc` and `g++`
* `make`
* various development libraries
* `cmake`

Verify the installation:

``` bash
gcc --version
g++ --version
```

---

## Creating a New Program
For each program, create a new directory, for example *example-dir*:

``` bash
mkdir example-dir
cd example-dir
```

Inside this directory, create a file called `Makefile`:

``` bash
touch Makefile
```

---

## Simple Makefile
In this makefile, add the following content:

```makefile
# Build and run the application as default.
default: build run

# Build the application.
build:
	@g++ main.cpp -o main -Wall -Werror -std=c++17

# Run the application.
run:
	@./main

# Clean the application.
clean:
	@rm -f main
```

**Note:** 
* The indentation under the targets `build`, `run`, and `clean` must consist of **tabs**, not spaces.
* This makefile builds an executable named `main`.
* Regarding the compiler flags:
    * `-Wall` enables most compiler warnings.
    * `-Werror` converts warnings into errors, which helps prevent subtle bugs.
    * `-std=c++17` sets the C++ version to C++17.
* To see the compilation commands executed by make, remove the `@` prefix.

---

## Simple Makefile with Parameters
It is also possible to use parameters to make the makefile easier to maintain, especially when the number of source files increases:

```makefile
# Target application.
TARGET := main

# C++ compiler.
CXX_COMPILER := g++

# C++ compiler flags.
CXX_FLAGS := -Wall -Werror -std=c++17

# Source files.
SRC_FILES := main.cpp

# Build and run the application as default.
default: build run

# Build the application.
build:
	@$(CXX_COMPILER) $(SRC_FILES) -o $(TARGET) $(CXX_FLAGS)

# Run the application.
run:
	@./$(TARGET)

# Clean the application.
clean:
	@rm -f $(TARGET)
```

---

## Tips
For compilation to work correctly:
* Place all header and source files (`.hpp` and `.cpp` files) directly in this directory.
* Make sure to include all source files (`.cpp` files) between `g++` and `-o` in the build target.
* For simple examples, all source files can be placed in the same directory.

### Embedded project structure
In larger embedded projects, source files are typically organized in separate directories such as:

```text
include/
    driver/
        gpio.hpp
source/
    driver/
        gpio.cpp
    main.cpp
Makefile
```

The Makefile can then be extended to support these directories:
* The `include` directory is included via the compiler flags.
* The source files are listed one per line using line continuation (`\`).

```makefile
# Target application.
TARGET := main

# C++ compiler.
CXX_COMPILER := g++

# C++ compiler flags.
CXX_FLAGS := -Wall -Werror -std=c++17 -Iinclude

# Source files.
SRC_FILES := source/driver/gpio.cpp \
                source/main.cpp \

# Build and run the application as default.
default: build run

# Build the application.
build:
	@$(CXX_COMPILER) $(SRC_FILES) -o $(TARGET) $(CXX_FLAGS)

# Run the application.
run:
	@./$(TARGET)

# Clean the application.
clean:
	@rm -f $(TARGET)
```

---

## Compiling a Test Program
You can compile and run your program using the following command:

``` bash
make
```

You can also build the program without running it:

``` bash
make build
```

You can run the program without compiling first:

``` bash
make run
```

You can remove compiled files:

``` bash
make clean
```

---

Create and run the following test program in a file named `main.cpp`:

``` cpp
#include <cstdio>

int main()
{
    std::printf("Hello to C++!\n");
    return 0;
}
```

Run the program using:

```bash
make
```

Expected output:

```text
Hello to C++!
```

---

## Notes
* We begin by including the standard header `<cstdio>`, which is the C++ version of the C header `<stdio.h>` and contains functionality for input and output operations.
* Most functionality in the C++ standard library exists inside the namespace `std` (*standard*).
* A namespace works somewhat like a directory in a filesystem and helps prevent naming conflicts.
* When using something from the standard library we therefore prefix it with `std::`, for example:
    * `std::uint8_t`
    * `std::size_t`
    * `std::vector`
* `std::printf` is the C++ version of the C function `printf` and is used to print text to the terminal.

---
