#include "adept_source.h"
#include "adept.h"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <numeric>
#include <random>

using namespace adept;

class Curve1D {
public:
    virtual ~Curve1D() {}  // Virtual destructor to ensure proper cleanup of derived classes

    // Virtual method that must be implemented by derived classes
    virtual adouble operator()(adouble x) const = 0;  
};

class LinearInterpolation : public Curve1D {
private:
    std::vector<adouble> x_vals;
    std::vector<adouble> y_vals;

public:
    LinearInterpolation(const std::vector<adouble>& x, const std::vector<adouble>& y) {
        if (x.size() != y.size()) {
            throw std::invalid_argument("X and Y vectors must be of the same size.");
        }
        x_vals = x;
        y_vals = y;
    }

    virtual adouble operator()(adouble x) const {
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
            adouble t = (x - x_vals[idx]) / (x_vals[idx_next] - x_vals[idx]);
            return y_vals[idx] + t * (y_vals[idx_next] - y_vals[idx]);
        }
        return y_vals.front();
    }
};

// write base class Model with method evolve(dt, vector<adouble>& normals and getState() returning vector&, write implementation of this base class LogNormalProcess where r(t) and vol(t) defined as Curve1D

class Model {
public:
    virtual ~Model() {} // Virtual destructor for safe polymorphic use

    // Pure virtual method to evolve the state of the model
    virtual void evolve(adouble dt, const std::vector<adouble>& normals) = 0;

    // Pure virtual method to get the current state of the model
    virtual const std::vector<adouble>& getState() const = 0;

    virtual int dims() const = 0; // Pure virtual method to get the dimension of the model

    virtual void reset() {} // Virtual method to reset the model to its initial state
};

class LogNormalProcess : public Model {
private:
    std::vector<std::shared_ptr<Curve1D>> r_curves;  // Vector of shared pointers for interest rate curves
    std::vector<std::shared_ptr<Curve1D>> vol_curves;  // Vector of shared pointers for volatility curves
    std::vector<adouble> state;  // Current state of the model, one for each dimension
    const std::vector<adouble> initial_values;  // Initial values for each dimension
    adouble current_time;  // Current time of the process

public:
    // Constructor takes vectors of curves and initial values for multi-dimensional support
    LogNormalProcess(const std::vector<std::shared_ptr<Curve1D>>& r, const std::vector<std::shared_ptr<Curve1D>>& vol, const std::vector<adouble>& _initial_values)
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

    void evolve(adouble dt, const std::vector<adouble>& normals) override {
        if (normals.size() != initial_values.size()) {
            throw std::invalid_argument("Normal vector size must match the number of dimensions.");
        }

        current_time += dt;

        for (size_t i = 0; i < state.size(); ++i) {
            adouble r_t = (*r_curves[i])(current_time);  // Interest rate for the current dimension
            adouble vol_t = (*vol_curves[i])(current_time);  // Volatility for the current dimension

            adouble drift = (r_t - 0.5 * vol_t * vol_t) * dt;
            adouble diffusion = vol_t * sqrt(dt) * normals[i];
            adouble S_t_plus_dt = state[i] * exp(drift + diffusion);

            state[i] = S_t_plus_dt;  // Update state for this dimension
        }
    }

    const std::vector<adouble>& getState() const override {
        return state;
    }
};

// write trade base class with evolve(t, state) and payoff() methods, write implementation of this base class AsianOption with start time and end time


class Trade {
public:
    virtual ~Trade() {}  // Virtual destructor for safe polymorphic use

    // Pure virtual method to evolve the state of the trade
    virtual void evolve(adouble t, const std::vector<adouble>& state) = 0;

    // Pure virtual method to calculate the payoff of the trade
    virtual adouble payoff() const = 0;

    virtual void reset() {}  // Virtual method to reset the trade to its initial state
};

class AsianOption : public Trade {
private:
    const int asset_id;                    // ID of the underlying asset
    const adouble strike;                   // Strike price of the option
    const adouble start_time, end_time;     // Start and end times for averaging
    adouble sum_prices;               // Sum of prices for averaging
    int count;                       // Count of prices added

public:
    AsianOption(int asset_id, adouble strike, adouble start, adouble end)
        : asset_id(asset_id), strike(strike), start_time(start), end_time(end)
    {
        reset();
    }

    void reset() override {
        sum_prices = 0.0;
        count = 0;
    }

    // Record the price only if it's within the averaging period
    void evolve(adouble t, const std::vector<adouble>& state) override {
        if (t >= start_time && t <= end_time) {
            adouble price = state[asset_id];
            sum_prices += price;
            count++;
        }
    }

    // Calculate the payoff based on the average price
    adouble payoff() const override {
        if (count == 0) return 0;  // Avoid division by zero
        adouble average_price = sum_prices / count;
        return max(average_price - strike, 0.0);  // Payoff for a call option
    }
};

double price(
    const std::vector<double>& initial_values,
    const std::vector<double>& time_points,
    const std::vector<double>& rates1,
    const std::vector<double>& rates2,
    const std::vector<double>& vols1,
    const std::vector<double>& vols2,
    std::vector<double>& d_initial_values,
    std::vector<double>& d_rates1,
    std::vector<double>& d_rates2,
    std::vector<double>& d_vols1,
    std::vector<double>& d_vols2
) {

    // Define constants for the simulation
    const int num_paths = 10000;
    const int num_days = 252;  // Assume 252 trading days in a year
    const double dt = 1.0 / num_days;  // Time step for each day
    // Random number generator setup
    std::mt19937 rng(17);
    std::normal_distribution<double> dist(0.0, 1.0);

    // Initialize path-wise derivatives to zero
    d_initial_values.resize(initial_values.size()); std::fill(d_initial_values.begin(), d_initial_values.end(), 0.0);
    d_rates1.resize(rates1.size()); std::fill(d_rates1.begin(), d_rates1.end(), 0.0);
    d_rates2.resize(rates2.size()); std::fill(d_rates2.begin(), d_rates2.end(), 0.0);
    d_vols1.resize(vols1.size()); std::fill(d_vols1.begin(), d_vols1.end(), 0.0);
    d_vols2.resize(vols2.size()); std::fill(d_vols2.begin(), d_vols2.end(), 0.0);

    // Running the Monte Carlo simulation
    double total_payoff = 0.0;

    for (int i = 0; i < num_paths; ++i) {
        adept::Stack stack;
        stack.new_recording(); // Start recording

        std::vector<adouble> a_initial_values(initial_values.begin(), initial_values.end());
        std::vector<adouble> a_time_points(time_points.begin(), time_points.end());
        std::vector<adouble> a_rates1(rates1.begin(), rates1.end());
        std::vector<adouble> a_rates2(rates2.begin(), rates2.end());
        std::vector<adouble> a_vols1(vols1.begin(), vols1.end());
        std::vector<adouble> a_vols2(vols2.begin(), vols2.end());


        // Create curves using shared pointers
        std::shared_ptr<Curve1D> r_curve1 = std::make_shared<LinearInterpolation>(a_time_points, a_rates1);
        std::shared_ptr<Curve1D> r_curve2 = std::make_shared<LinearInterpolation>(a_time_points, a_rates2);
        std::shared_ptr<Curve1D> vol_curve1 = std::make_shared<LinearInterpolation>(a_time_points, a_vols1);
        std::shared_ptr<Curve1D> vol_curve2 = std::make_shared<LinearInterpolation>(a_time_points, a_vols2);

        // Create the LogNormalProcess model for two assets
        std::vector<std::shared_ptr<Curve1D>> r_curves = {r_curve1, r_curve2};
        std::vector<std::shared_ptr<Curve1D>> vol_curves = {vol_curve1, vol_curve2};
        LogNormalProcess model(r_curves, vol_curves, a_initial_values);

        // Define two Asian options
        AsianOption option1(0, 100.0, 0.0, 1.0);  // Asian option on the first asset
        AsianOption option2(1, 100.0, 0.25, 0.75);  // Asian option on the second asset

        if (i != 0) {
            model.reset();  // Reset the model to initial values
            option1.reset();
            option2.reset();
        }

        for (int day = 0; day < num_days; ++day) {
            adouble current_time = day * dt;
            std::vector<adouble> normals = {dist(rng), dist(rng)};
            model.evolve(dt, normals);
            const std::vector<adouble>& state = model.getState();

            option1.evolve(current_time, state);
            option2.evolve(current_time, state);
        }
        adouble total_payoff_path = option1.payoff() + option2.payoff();

        total_payoff += total_payoff_path.value();
        total_payoff_path.set_gradient(1.0);  // Set the payoff as the objective function
        stack.compute_adjoint();  // Run the adjoint algorithm

        // Accumulate the derivatives
        for (size_t j = 0; j < initial_values.size(); ++j) {
            d_initial_values[j] += a_initial_values[j].get_gradient();
        }
        for (size_t j = 0; j < rates1.size(); ++j) {
            d_rates1[j] += a_rates1[j].get_gradient();
        }
        for (size_t j = 0; j < rates2.size(); ++j) {
            d_rates2[j] += a_rates2[j].get_gradient();
        }
        for (size_t j = 0; j < vols1.size(); ++j) {
            d_vols1[j] += a_vols1[j].get_gradient();
        }
        for (size_t j = 0; j < vols2.size(); ++j) {
            d_vols2[j] += a_vols2[j].get_gradient();
        }
    }

    // Calculate the average payoff for each option
    double price = total_payoff / num_paths;

    return price;
}

int main() {

    std::vector<double> initial_values = {100.0, 100.0};  // Starting prices for each asset

    // Define time points and corresponding rates and volatilities (weekly for a year)

    std::vector<double> time_points;
    for (int week = 0; week <= 52; week++) {
        time_points.push_back(static_cast<double>(week) / 52.0);
    }

    // Define oscillating interest rates and peaking volatilities
    std::vector<double> rates1, rates2, vols1, vols2;
    std::vector<double> d_initial_values, d_rates1, d_rates2, d_vols1, d_vols2;
    for (size_t i = 0; i < time_points.size(); ++i) {
        double t = time_points[i];
        // Simple sinusoidal oscillations for interest rates
        rates1.push_back(0.01 + 0.005 * sin(2 * M_PI * t));
        rates2.push_back(0.02 + 0.005 * sin(2 * M_PI * t));

        // Volatilities peak in the middle of the year and are lower at the start/end
        vols1.push_back(0.15 + 0.10 * (1 - cos(2 * M_PI * t)));
        vols2.push_back(0.20 + 0.10 * (1 - cos(2 * M_PI * t)));
    }

    // Calculate the price of two Asian options using the Monte Carlo simulation
    double option_price = price(
        initial_values, time_points, rates1, rates2, vols1, vols2
        , d_initial_values, d_rates1, d_rates2, d_vols1, d_vols2
    );

    std::cout << "Asian option price: " << option_price << std::endl;

    for (size_t i = 0; i < initial_values.size(); ++i) {
        std::cout << "Gradient of price with respect to S" << i << ": " << d_initial_values[i] << std::endl;
    }
    for (size_t i = 0; i < rates1.size(); ++i) {
        std::cout << "Gradient of price with respect to r1[" << i << "]: " << d_rates1[i] << std::endl;
    }
    for (size_t i = 0; i < rates2.size(); ++i) {
        std::cout << "Gradient of price with respect to r2[" << i << "]: " << d_rates2[i] << std::endl;
    }
    for (size_t i = 0; i < vols1.size(); ++i) {
        std::cout << "Gradient of price with respect to vol1[" << i << "]: " << d_vols1[i] << std::endl;
    }
    for (size_t i = 0; i < vols2.size(); ++i) {
        std::cout << "Gradient of price with respect to vol2[" << i << "]: " << d_vols2[i] << std::endl;
    }


    return 0;
}
