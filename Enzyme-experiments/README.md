# Enzyme Feature Tests: C++ Object-Oriented Support

## Overview

This repository contains a C++ program designed to test the automatic differentiation capabilities of [Enzyme](https://enzyme.mit.edu/), focusing on its support for object-oriented features in C++. The test involves a simple modification of the square function example from Enzyme's official "Getting Started" page, augmented with a virtual function to demonstrate the interaction between Enzyme and C++ OOP constructs.

## Prerequisites

- Enzyme installation (see dockerfile for details)

## Getting Started

### Setting up the Environment

To ensure consistency across different environments, a Docker container is used. Here's how to build and start the Docker container:

1. **Build the Docker Image:**

   Navigate to the directory containing the `dockerfile` and run the following command:

   ```bash
   docker build -t enzyme-oop-test .```

2. **Run the Docker Container:**

After building the image, start your container with:

    ```docker run -it enzyme-oop-test /bin/bash```

3. **Compiling the Code**

The compile.sh script contains all necessary commands to build the program. Execute this script in the Docker container:

    ```./compile.sh```

4. **Running the Tests**
Execute the compiled program within the Docker container:

    ```./a.out```

5. **Expected Output**
Upon running the program, the expected output should be:

```
square(1.000000)=1.000000, dsquare(1.000000)=3.000000, check(1.000000)=3.000000
square(2.000000)=8.000000, dsquare(2.000000)=12.000000, check(2.000000)=12.000000
square(3.000000)=27.000000, dsquare(3.000000)=27.000000, check(3.000000)=27.000000
square(4.000000)=64.000000, dsquare(4.000000)=48.000000, check(4.000000)=48.000000
```

6. **Actual Output**

    **virtual-function.cpp** - crashes

    ```Segmentation fault (core dumped)```

    **virtual-function-fixed.cpp** - return expected results

7. **Required modifications**

Looks like Enzyme requires creating shadow version of all objects that participate in differentiation. Also virtual function tables of such objects should be patched with Enzyme magic. **virtual-function-fixed.cpp** includes example of this.

8. **TODO:**

Integrating and modifying virtual tables is a complex endeavor, particularly in environments with multiple inheritance and virtual bases. More comprehensive testing is necessary to explore the potential and limitations of this approach. The ABI (Application Binary Interface) handling for virtual tables in the presence of multiple inheritance is non-trivial, and understanding it requires a deep dive into the specifics outlined in the Itanium C++ ABI https://itanium-cxx-abi.github.io/cxx-abi/abi.html#vtable. Further exploration and experiments are needed to determine how effectively these complex features can be managed and what best practices may emerge for implementing these modifications in production code.

9. **Files**

virtual-function.cpp: The main C++ source file which includes the function definitions and main test loop.
virtual-function-fixed.cpp: The main C++ source file which includes the function definitions and main test loop.
Dockerfile: Contains all Docker commands necessary to set up the environment.
compile.sh: Script to compile the test program using appropriate compiler flags and settings.