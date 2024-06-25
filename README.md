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
- **main:** Drives the simulation, demonstrating the use of the aforementioned classes in a Monte Carlo simulation to estimate prices of Asian options in a synthetic market environment.

## Building and Running the Project
To compile and run the project, ensure you have a C++ compiler that supports C++11 or later and CMake for building. Follow these steps:

1. ** Getting the code and building steps:**
```
    git clone https://github.com/matlogica/Enzyme-example.git
    cd Enzyme-example
    mkdir build && cd build
    cmake ..
    make
```

2. ** Run the base code(no AAD):**
```
    ./base-code
```

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

## Enzyme Integration Plan
The integration of Enzyme into our project aims to facilitate automatic adjoint differentiation (AAD) with minimal impact on the existing C++ code base, ensuring that derivative computation can be seamlessly incorporated without extensive modifications. The plan is structured as follows:

**Assessment of C++ Feature Support**: Initially, we will conduct a thorough analysis to understand which C++ features are supported by Enzyme. This includes assessing compatibility with various object-oriented constructs, template usage, and exception handling mechanisms. Our goal is to determine how well Enzyme can integrate with our existing code structure and workflows. See Enzyme-experiments/

**Minimal Code Changes**: To qualify as an effective AAD tool, Enzyme must integrate with our existing base-code.cpp with minimal changes. This involves identifying the strategic points within the code where Enzyme's automatic differentiation functionalities can be inserted without altering the fundamental logic or architecture of the application. The focus will be on enhancing functions with derivative capabilities using Enzyme's APIs while keeping the original syntax and structure intact as much as possible.

By following this plan, we aim to leverage Enzyme’s capabilities to enhance our project's efficiency in computing derivatives, essential for accurate and fast quantitative analysis, without compromising the simplicity and readability of our base code. This integration not only tests Enzyme's adaptability with complex C++ features but also sets a benchmark for its application in sophisticated financial modeling.
