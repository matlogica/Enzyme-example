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
   docker build -t enzyme-oop-test .

2. **Run the Docker Container:**

After building the image, start your container with:

    ```docker run -it enzyme-oop-test /bin/bash

3. **Compiling the Code**

The compile.sh script contains all necessary commands to build the program. Execute this script in the Docker container:

    ```./compile.sh

4. **Running the Tests**
Execute the compiled program within the Docker container:

    ```./a.out

5. **Expected Output**
Upon running the program, the expected output should be:

```
square(1.000000)=1.000000, dsquare(1.000000)=3.000000, check(1.000000)=3.000000
square(2.000000)=8.000000, dsquare(2.000000)=12.000000, check(2.000000)=12.000000
square(3.000000)=27.000000, dsquare(3.000000)=27.000000, check(3.000000)=27.000000
square(4.000000)=64.000000, dsquare(4.000000)=48.000000, check(4.000000)=48.000000
```

6. **Actual Output**

    ```Segmentation fault (core dumped)

7. **Files**

virtual-function.cpp: The main C++ source file which includes the function definitions and main test loop.
Dockerfile: Contains all Docker commands necessary to set up the environment.
compile.sh: Script to compile the test program using appropriate compiler flags and settings.