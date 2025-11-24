#include <iostream>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <string>

#include "LoanCalculator.h"

LoanCalculator::LoanCalculator()
    : amountSet_(false),
      initialPayment_(0.0),
      interestSet_(false),
      paymentSet_(false),
      periodTotalSet_(false),
      periodElapsedSet_(false),
      openingFee_(0.0),
      openingPercent_(0.0)
{
}

/*
 * Loan balance after n payments:
 * B_n = A*(1+i)^n - (P/i)*((1+i)^n - 1)
 */
double LoanCalculator::calculateLoanBalance()
{
    if(!amountSet_ || !interestSet_ || !periodElapsedSet_ || !paymentSet_)
        throw std::invalid_argument("Must set loan amount, interest, and elapsed period for this calculation");

    if(interestPeriodic_ == 0)
        throw std::invalid_argument("Interest rate cannot be zero for this calculation");

    double factor = std::pow(1 + interestPeriodic_, periodElapsed_);

    return (amount_ * factor) -
           ((payment_ / interestPeriodic_) * (factor - 1));
}

/*
 * Payment amount:
 * P = i*A / (1 - (1+i)^-N)
 */
double LoanCalculator::calculatePayment()
{
    if(!amountSet_ || !interestSet_ || !periodTotalSet_)
        throw std::invalid_argument("Must set loan amount, interest, and total period");

    if(interestPeriodic_ == 0)
        throw std::invalid_argument("Interest rate cannot be zero for payment calculation");

    double totalAmount = amount_ - initialPayment_;
    totalAmount += openingFee_ + (totalAmount * (openingPercent_ / 100.0));

    return (interestPeriodic_ * totalAmount) /
           (1 - std::pow(1 + interestPeriodic_, -periodTotal_));
}

/*
 * Number of payments:
 * N = - ln(1 - i*A/P) / ln(1+i)
 */
double LoanCalculator::calculateNumberPayments()
{
    if(!amountSet_ || !interestSet_ || !paymentSet_)
        throw std::invalid_argument("Must set loan amount, interest, and payment");

    if(interestPeriodic_ == 0)
        throw std::invalid_argument("Interest cannot be zero");

    double ratio = 1.0 - (interestPeriodic_ * amount_ / payment_);

    if(ratio <= 0)
        throw std::invalid_argument("Invalid values: loan or payment too small");

    return -std::log(ratio) / std::log(1.0 + interestPeriodic_);
}

/*
 * Original loan amount:
 * A = (P/i)*(1 - (1+i)^-N)
 */
double LoanCalculator::calculateLoanAmount()
{
    if(!paymentSet_ || !interestSet_ || !periodTotalSet_)
        throw std::invalid_argument("Must set payment, interest, and total period");

    if(interestPeriodic_ == 0)
        throw std::invalid_argument("Interest cannot be zero");

    return (payment_ / interestPeriodic_) *
           (1 - std::pow(1 + interestPeriodic_, -periodTotal_));
}

/*
 * Approximate interest rate:
 * i = (((1 + P/A)^(1/q) - 1 )^q - 1)
 * where q = ln(1+1/N) / ln(2)
 */
double LoanCalculator::calculateInterestRate()
{
    if(!amountSet_ || !paymentSet_ || !periodTotalSet_)
        throw std::invalid_argument("Must set amount, payment, and total period");

    double q = std::log(1.0 + 1.0 / periodTotal_) / std::log(2.0);
    double base = std::pow((1.0 + payment_ / amount_), 1.0 / q) - 1.0;
    double monthlyInterest = std::pow(base, q) - 1.0;

    return monthlyInterest * 12 * 100;  // annual percentage
}

double LoanCalculator::calculateEffectiveInterestRate()
{
    if(!amountSet_ || !periodTotalSet_)
        throw std::invalid_argument("Must set amount and total period");

    double payment = calculatePayment();
    double totalAmount = amount_ - initialPayment_;

    double q = std::log(1.0 + 1.0 / periodTotal_) / std::log(2.0);
    double base = std::pow((1.0 + payment / totalAmount), 1.0 / q) - 1.0;
    double monthlyInterest = std::pow(base, q) - 1.0;

    return monthlyInterest * 12 * 100;
}

std::string LoanCalculator::toString()
{
    std::stringstream ss;

    if(amountSet_)
        ss << "Initial Amount:      " << amount_ << "\n";

    if(initialPayment_ != 0.0)
    {
        ss << "Initial Payment:     " << initialPayment_ << "\n";
        ss << "Actual Loan Amount:  " << (amount_ - initialPayment_) << "\n";
    }

    if(interestSet_)
        ss << "Yearly Interest:     " << interest_ << "%\n";

    if(paymentSet_)
        ss << "Monthly payment:     " << payment_ << "\n";

    if(periodTotalSet_)
        ss << "Loan Period:         " << periodTotal_ << " months\n";

    if(periodElapsedSet_)
        ss << "Elapsed Period:      " << periodElapsed_ << " months\n";

    if(openingFee_ != 0.0)
        ss << "Opening Fee:         " << openingFee_ << "\n";

    if(openingPercent_ != 0.0)
        ss << "Opening Fee %:       " << openingPercent_
           << "% = " << (openingPercent_ / 100 * (amount_ - initialPayment_)) << "\n";

    return ss.str();
}
