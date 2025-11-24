#ifndef LOANCALCULATOR_H_INCLUDED
#define LOANCALCULATOR_H_INCLUDED

/*
  Loan and interest formulas based on:
  http://oakroadsystems.com/math/loan.htm

  Variables:
    A = loan amount (principal)
    Bn = balance after n payments
    i = periodic interest rate
    n = elapsed payment periods
    N = total payment periods
    P = payment amount
*/

#include <string>

class LoanCalculator
{
public:
    LoanCalculator();
    ~LoanCalculator() noexcept {}

    //
    // Setters and Getters
    //

    // Principal A
    inline void setAmount(double A) { amount_ = A; amountSet_ = true; }
    inline double getAmount() const { return amount_; }

    // Initial down payment
    inline void setInitialPayment(double initialA) { initialPayment_ = initialA; }
    inline double getInitialPayment() const        { return initialPayment_; }

    /*
      Yearly interest rate (e.g. 6.75)
      Internally stored as periodic (monthly) rate: i/100/12
    */
    inline void setInterest(double i)
    {
        interest_ = i;
        interestPeriodic_ = i / 100.0 / 12.0;
        interestSet_ = true;
    }

    inline double getInterest() const         { return interest_; }
    inline double getPeriodicInterest() const { return interestPeriodic_; }

    // Monthly payment
    inline void setPayment(double P) { payment_ = P; paymentSet_ = true; }
    inline double getPayment() const { return payment_; }

    // Total loan period (N)
    inline void setPeriodTotal(int N)      { periodTotal_ = N; periodTotalSet_ = true; }
    inline int  getPeriodTotal() const     { return periodTotal_; }

    // Elapsed payment periods (n)
    inline void setPeriodElapsed(int n)   { periodElapsed_ = n; periodElapsedSet_ = true; }
    inline int  getPeriodElapsed() const  { return periodElapsed_; }

    // Optional opening fees
    inline void setOpeningFee(double fee)        { openingFee_ = fee; }
    inline double getOpeningFee() const          { return openingFee_; }

    inline void setOpeningPercent(double percent) { openingPercent_ = percent; }
    inline double getOpeningPercent() const       { return openingPercent_; }

    // Reset all variables and flags
    inline void reset()
    {
        amount_ = initialPayment_ = interest_ = interestPeriodic_ =
        payment_ = openingFee_ = openingPercent_ = 0.0;

        periodTotal_ = periodElapsed_ = 0;

        amountSet_ = interestSet_ = paymentSet_ =
        periodTotalSet_ = periodElapsedSet_ = false;
    }

    //
    // Calculation Methods
    //

    double calculateLoanBalance();
    double calculatePayment();
    double calculateNumberPayments();
    double calculateLoanAmount();
    double calculateInterestRate();
    double calculateEffectiveInterestRate();

    std::string toString();

private:
    double amount_;           // principal A
    bool amountSet_;

    double initialPayment_;   // initial down payment

    double interest_;         // yearly interest %
    double interestPeriodic_; // monthly interest rate (decimal form)
    bool interestSet_;

    double payment_;          // payment P
    bool paymentSet_;

    int periodTotal_;         // N
    bool periodTotalSet_;

    int periodElapsed_;       // n
    bool periodElapsedSet_;

    double openingFee_;       // flat fee
    double openingPercent_;   // % fee
};

#endif // LOANCALCULATOR_H_INCLUDED
