# Enzyme-example

## Introduction
This project explores the integration of object-oriented C++ code with Enzyme, a tool for automatic differentiation (AD) at the LLVM level. We compare this integration with Automatic Adjoint Differentiation (AAD) tools that utilize operator overloading. The primary focus lies in applications within quantitative finance, where complex mathematical models and simulations are commonplace.

The code base demonstrates typical quantitative finance workflows starting with the definition of model parameters and market objects (e.g., scalars, 1D and 2D curves). It utilizes interpolation methods abstracted behind a virtual interface and simulates market dynamics which then interact with trade state objects. The project includes an example of simulating Asian options, which rely on the average value of underlying assets over time.

## Note on Complexity:
This project serves as a foundational example intended to demonstrate the integration of object-oriented C++ with tools like Enzyme for automatic differentiation and its comparison with AAD tools using operator overloading. It is important to note that this code is highly simplified and intended primarily for educational and demonstration purposes. Real-world quantitative finance libraries are significantly more complex. They are designed to handle the nuances and intricacies of financial markets. Users seeking to apply these concepts professionally should consider leveraging or studying established quantitative libraries such as QuantLib(https://github.com/lballabio/QuantLib) that offer the depth and breadth required for live financial applications.

## Project Structure
The project is structured as follows:
- **Curve1D and LinearInterpolation:** Defines a base class and a derived class for handling 1D interpolation of curves, essential for modeling interest rates and volatilities in financial instruments.
- **Model and LogNormalProcess:** Abstract and concrete classes for stochastic processes, with LogNormalProcess demonstrating a multi-dimensional model for asset prices influenced by dynamic rates and volatilities.
- **Trade and AsianOption:** Abstract and concrete classes where `Trade` defines a base for trading instruments and `AsianOption` implements an option dependent on the average price of an underlying asset.
- **main.cpp:** Drives the simulation, demonstrating the use of the aforementioned classes in a Monte Carlo simulation to estimate prices of Asian options in a synthetic market environment.

## High-Level Code Description
- **Curve1D:** An abstract base class that defines a virtual function for curve interpolation.
- **LinearInterpolation:** Implements the `Curve1D` interface, providing linear interpolation between given data points.
- **Model:** An abstract base class that outlines methods for evolving the state of financial models.
- **LogNormalProcess:** A multi-dimensional stochastic model implementing the `Model` interface. It simulates asset price evolution based on log-normal dynamics, with each dimension having its own interest rate and volatility curves.
- **Trade:** An abstract base class defining the essential functionalities of a financial trade instrument.
- **AsianOption:** Extends `Trade`, specifically to handle the calculations for options that depend on the average price of an asset over a specified time frame.
- **main.cpp:** Contains the main function which sets up and runs a Monte Carlo simulation to estimate option prices, showcasing the entire system's functionality.

## Building and Running the Project
To compile and run the project, ensure you have a C++ compiler that supports C++11 or later and CMake for building. Follow these steps:

1. ** Getting the code and building steps:**
    git clone https://github.com/matlogica/Enzyme-example.git
    cd Enzyme-example
    mkdir build && cd build
    cmake ..
    make

2. ** Run the base code(no AAD):**
    ./base-code

## TODO:

This project is currently at a foundational stage, with many potential enhancements and expansions to consider. Key areas of future development include:

- **Integration with Automatic Differentiation (AD) Tools:**
  - **Adept Integration:** Implement a version of the code using the Adept automatic differentiation library. This will enable performance comparisons between manual differentiation and AD techniques, providing insights into the efficiency gains possible with Adept.
  - **AADC (Automatic Adjoint Differentiation Compiler) Integration:** Adapt the existing codebase to leverage AADC, which would bring a different approach to automatic differentiation, focusing on improving performance in pricing and adjoint derivatives computation.
  - **Enzyme Integration:** While the current project discusses potential integration with Enzyme, actual code implementation is pending. Future updates should include an Enzyme-enabled version of the code to harness LLVM-based differentiation directly from C++ for potentially greater performance and accuracy in derivatives computation.

- **Benchmarking and Performance Analysis:**
  - Develop benchmarks to compare the performance (both speed and accuracy) of the simulations across different versions of the code with various AD tools. This would help quantify the benefits of each AD integration and guide best practices for similar quantitative finance applications.

- **Enhance Model Complexity:**
  - The current models are simplified versions intended for educational purposes. A critical next step would be to introduce more complex models that better reflect the sophisticated dynamics of financial markets. This includes solvers, regression based models, and models incorporating stochastic volatility.

- **Community Engagement and Feedback:**
  - We actively encourage feedback from quantitative analysts and other users on the types of features and functionalities they would like to see in automatic differentiation tools. This engagement will help tailor the development to meet real-world needs and make the tools more effective for specific quantitative finance applications. We invite the community to share their experiences, suggest improvements, and discuss potential features that could enhance their workflow and analysis capabilities.


