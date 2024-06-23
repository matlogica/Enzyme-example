#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <numeric>
#include <random>

class Curve1D {
public:
    virtual ~Curve1D() {}  // Virtual destructor to ensure proper cleanup of derived classes

    // Virtual method that must be implemented by derived classes
    virtual double operator()(double x) const = 0;  
};

class LinearInterpolation : public Curve1D {
private:
    std::vector<double> x_vals;
    std::vector<double> y_vals;

public:
    LinearInterpolation(const std::vector<double>& x, const std::vector<double>& y) {
        if (x.size() != y.size()) {
            throw std::invalid_argument("X and Y vectors must be of the same size.");
        }
        x_vals = x;
        y_vals = y;
    }

    virtual double operator()(double x) const {
        if (x_vals.empty()) {
            throw std::runtime_error("Interpolation vectors are empty.");
        }
        if (x < x_vals.front() || x > x_vals.back()) {
            throw std::out_of_range("X value out of interpolation range.");
        }

        // Lower bound finds the first element which does not compare less than x
        auto low = std::lower_bound(x_vals.begin(), x_vals.end(), x);
        if (low != x_vals.begin()) {
            // Find indices of the two points forming the interpolation interval
            size_t idx = std::distance(x_vals.begin(), low) - 1;
            size_t idx_next = idx + 1;

            // Linear interpolation formula
            double t = (x - x_vals[idx]) / (x_vals[idx_next] - x_vals[idx]);
            return y_vals[idx] + t * (y_vals[idx_next] - y_vals[idx]);
        }
        return y_vals.front();
    }
};

// write base class Model with method evolve(dt, vector<double>& normals and getState() returning vector&, write implementation of this base class LogNormalProcess where r(t) and vol(t) defined as Curve1D

class Model {
public:
    virtual ~Model() {} // Virtual destructor for safe polymorphic use

    // Pure virtual method to evolve the state of the model
    virtual void evolve(double dt, const std::vector<double>& normals) = 0;

    // Pure virtual method to get the current state of the model
    virtual const std::vector<double>& getState() const = 0;

    virtual int dims() const = 0; // Pure virtual method to get the dimension of the model

    virtual void reset() {} // Virtual method to reset the model to its initial state
};

class LogNormalProcess : public Model {
private:
    std::vector<std::shared_ptr<Curve1D>> r_curves;  // Vector of shared pointers for interest rate curves
    std::vector<std::shared_ptr<Curve1D>> vol_curves;  // Vector of shared pointers for volatility curves
    std::vector<double> state;  // Current state of the model, one for each dimension
    const std::vector<double> initial_values;  // Initial values for each dimension
    double current_time;  // Current time of the process

public:
    // Constructor takes vectors of curves and initial values for multi-dimensional support
    LogNormalProcess(const std::vector<std::shared_ptr<Curve1D>>& r, const std::vector<std::shared_ptr<Curve1D>>& vol, const std::vector<double>& _initial_values)
        : r_curves(r), vol_curves(vol), initial_values(_initial_values), state(_initial_values), current_time(0.0) {
        if (r_curves.size() != vol_curves.size() || r_curves.size() != initial_values.size()) {
            throw std::invalid_argument("All vectors must have the same size.");
        }
        for (auto& curve : r_curves) {
            if (!curve) throw std::invalid_argument("Interest rate curves cannot be null.");
        }
        for (auto& curve : vol_curves) {
            if (!curve) throw std::invalid_argument("Volatility curves cannot be null.");
        }
    }

    int dims() const override {
        return static_cast<int>(initial_values.size());
    }

    void reset() override {
        state = initial_values;  // Reset state to initial values
        current_time = 0.0;  // Reset time
    }

    void evolve(double dt, const std::vector<double>& normals) override {
        if (normals.size() != initial_values.size()) {
            throw std::invalid_argument("Normal vector size must match the number of dimensions.");
        }

        current_time += dt;

        for (size_t i = 0; i < state.size(); ++i) {
            double r_t = (*r_curves[i])(current_time);  // Interest rate for the current dimension
            double vol_t = (*vol_curves[i])(current_time);  // Volatility for the current dimension

            double drift = (r_t - 0.5 * vol_t * vol_t) * dt;
            double diffusion = vol_t * std::sqrt(dt) * normals[i];
            double S_t_plus_dt = state[i] * std::exp(drift + diffusion);

            state[i] = S_t_plus_dt;  // Update state for this dimension
        }
    }

    const std::vector<double>& getState() const override {
        return state;
    }
};

// write trade base class with evolve(t, state) and payoff() methods, write implementation of this base class AsianOption with start time and end time


class Trade {
public:
    virtual ~Trade() {}  // Virtual destructor for safe polymorphic use

    // Pure virtual method to evolve the state of the trade
    virtual void evolve(double t, const std::vector<double>& state) = 0;

    // Pure virtual method to calculate the payoff of the trade
    virtual double payoff() const = 0;

    virtual void reset() {}  // Virtual method to reset the trade to its initial state
};

class AsianOption : public Trade {
private:
    const int asset_id;                    // ID of the underlying asset
    const double strike;                   // Strike price of the option
    const double start_time, end_time;     // Start and end times for averaging
    double sum_prices;               // Sum of prices for averaging
    int count;                       // Count of prices added

public:
    AsianOption(int asset_id, double strike, double start, double end)
        : asset_id(asset_id), strike(strike), start_time(start), end_time(end)
    {
        reset();
    }

    void reset() override {
        sum_prices = 0.0;
        count = 0;
    }

    // Record the price only if it's within the averaging period
    void evolve(double t, const std::vector<double>& state) override {
        if (t >= start_time && t <= end_time) {
            double price = state[asset_id];
            sum_prices += price;
            count++;
        }
    }

    // Calculate the payoff based on the average price
    double payoff() const override {
        if (count == 0) return 0;  // Avoid division by zero
        double average_price = sum_prices / count;
        return std::max(average_price - strike, 0.0);  // Payoff for a call option
    }
};

int main() {
    // Define constants for the simulation
    const int num_paths = 10000;
    const int num_days = 252;  // Assume 252 trading days in a year
    const double dt = 1.0 / num_days;  // Time step for each day
    // Random number generator setup
    std::mt19937 rng(std::random_device{}());
    std::normal_distribution<double> dist(0.0, 1.0);

    // Define time points and corresponding rates and volatilities (weekly for a year)
    std::vector<double> time_points;
    for (int week = 0; week <= 52; week++) {
        time_points.push_back(static_cast<double>(week) / 52.0);
    }

    // Define oscillating interest rates and peaking volatilities
    std::vector<double> rates1, rates2, vols1, vols2;
    for (size_t i = 0; i < time_points.size(); ++i) {
        double t = time_points[i];
        // Simple sinusoidal oscillations for interest rates
        rates1.push_back(0.01 + 0.005 * sin(2 * M_PI * t));
        rates2.push_back(0.02 + 0.005 * sin(2 * M_PI * t));

        // Volatilities peak in the middle of the year and are lower at the start/end
        vols1.push_back(0.15 + 0.10 * (1 - cos(2 * M_PI * t)));
        vols2.push_back(0.20 + 0.10 * (1 - cos(2 * M_PI * t)));
    }

    // Create curves using shared pointers
    std::shared_ptr<Curve1D> r_curve1 = std::make_shared<LinearInterpolation>(time_points, rates1);
    std::shared_ptr<Curve1D> r_curve2 = std::make_shared<LinearInterpolation>(time_points, rates2);
    std::shared_ptr<Curve1D> vol_curve1 = std::make_shared<LinearInterpolation>(time_points, vols1);
    std::shared_ptr<Curve1D> vol_curve2 = std::make_shared<LinearInterpolation>(time_points, vols2);

    // Create the LogNormalProcess model for two assets
    std::vector<std::shared_ptr<Curve1D>> r_curves = {r_curve1, r_curve2};
    std::vector<std::shared_ptr<Curve1D>> vol_curves = {vol_curve1, vol_curve2};
    std::vector<double> initial_values = {100.0, 100.0};  // Starting prices for each asset
    LogNormalProcess model(r_curves, vol_curves, initial_values);

    // Define two Asian options
    AsianOption option1(0, 100.0, 0.0, 1.0);  // Asian option on the first asset
    AsianOption option2(1, 100.0, 0.25, 0.75);  // Asian option on the second asset

    // Running the Monte Carlo simulation
    double total_payoff1 = 0.0;
    double total_payoff2 = 0.0;

    for (int i = 0; i < num_paths; ++i) {
        model.reset();  // Reset the model to initial values
        option1.reset();
        option2.reset();

        for (int day = 0; day < num_days; ++day) {
            double current_time = day * dt;
            std::vector<double> normals = {dist(rng), dist(rng)};
            model.evolve(dt, normals);
            const std::vector<double>& state = model.getState();

            option1.evolve(current_time, state);
            option2.evolve(current_time, state);
        }

        total_payoff1 += option1.payoff();
        total_payoff2 += option2.payoff();
    }

    // Calculate the average payoff for each option
    double price1 = total_payoff1 / num_paths;
    double price2 = total_payoff2 / num_paths;

    // Output the results
    std::cout << "Price of Asian Option 1: " << price1 << std::endl;
    std::cout << "Price of Asian Option 2: " << price2 << std::endl;


    // We want to see dprice[i]/drates[j] and dprice[i]/dvols[j]
    // 
    // TODO:
    // Add implementations using adept, AADC, Enzyme
    // adept-code.cpp
    // aadc-code.cpp
    // enzyme-code.cpp
    

    return 0;
}

