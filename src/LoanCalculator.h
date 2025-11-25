#ifndef LOANCALCULATOR_H_INCLUDED
#define LOANCALCULATOR_H_INCLUDED

/**
 * @file LoanCalculator.h
 * @brief Declares the LoanCalculator class used for computing loan-related values.
 *
 * This calculator implements formulas described at:
 * http://oakroadsystems.com/math/loan.htm
 *
 * Key Variables:
 * - A : Loan principal
 * - Bn: Remaining balance after n payments
 * - i : Interest rate per period (monthly)
 * - n : Number of elapsed payment periods
 * - N : Total number of payment periods
 * - P : Fixed periodic payment amount
 */

#include <string>
using namespace std;

/**
 * @class LoanCalculator
 * @brief Provides functionality to compute loan balances, payments, interest rates,
 *        effective rates including fees, and other related financial metrics.
 *
 * The class stores loan parameters (principal, interest, payment, etc.) and exposes
 * several functions that compute values based on standard amortization formulas.
 */
class LoanCalculator
{
public:
    /** @brief Constructs an empty LoanCalculator instance. */
    LoanCalculator();

    /** @brief Default destructor. */
    ~LoanCalculator() noexcept {}

    // -------------------------------------------------------------------------
    // Setters and Getters
    // -------------------------------------------------------------------------

    /**
     * @brief Sets the principal loan amount (A).
     * @param A The original loan amount.
     */
    inline void setAmount(double A) { amount_ = A; amountSet_ = true; }

    /**
     * @brief Gets the loan amount.
     * @return The current principal value.
     */
    inline double getAmount() const { return amount_; }

    /**
     * @brief Sets the initial down payment applied before the loan begins.
     * @param initialA The amount paid upfront.
     */
    inline void setInitialPayment(double initialA) { initialPayment_ = initialA; }

    /**
     * @brief Gets the initial down payment.
     * @return The stored initial payment.
     */
    inline double getInitialPayment() const { return initialPayment_; }

    /**
     * @brief Sets the yearly nominal interest rate.
     *
     * Example: If the yearly rate is 6.75, pass `6.75`.
     * Internally, the equivalent monthly rate is stored as:
     * @f[
     *     i_{periodic} = \frac{rate}{100 \times 12}
     * @f]
     *
     * @param i Annual interest rate expressed as a percentage.
     */
    inline void setInterest(double i)
    {
        interest_ = i;
        interestPeriodic_ = i / 100.0 / 12.0;
        interestSet_ = true;
    }

    /**
     * @brief Gets the stored annual interest rate (in percentage units).
     * @return Yearly interest rate.
     */
    inline double getInterest() const { return interest_; }

    /**
     * @brief Gets the monthly periodic interest rate (decimal form).
     * @return Monthly interest (e.g., 0.005).
     */
    inline double getPeriodicInterest() const { return interestPeriodic_; }

    /**
     * @brief Sets the periodic payment amount (P).
     * @param P The monthly payment.
     */
    inline void setPayment(double P) { payment_ = P; paymentSet_ = true; }

    /**
     * @brief Gets the monthly payment.
     * @return Stored payment amount.
     */
    inline double getPayment() const { return payment_; }

    /**
     * @brief Sets the total number of payment periods (N).
     * @param N Total loan duration in months.
     */
    inline void setPeriodTotal(int N) { periodTotal_ = N; periodTotalSet_ = true; }

    /**
     * @brief Gets the total number of payment periods.
     * @return Total loan duration.
     */
    inline int getPeriodTotal() const { return periodTotal_; }

    /**
     * @brief Sets the number of elapsed payments (n).
     * @param n Number of payments already made.
     */
    inline void setPeriodElapsed(int n) { periodElapsed_ = n; periodElapsedSet_ = true; }

    /**
     * @brief Gets the number of payments already made.
     * @return Elapsed payment count.
     */
    inline int getPeriodElapsed() const { return periodElapsed_; }

    /**
     * @brief Sets a one-time opening fee charged at loan creation.
     * @param fee The fee amount.
     */
    inline void setOpeningFee(double fee) { openingFee_ = fee; }

    /**
     * @brief Gets the flat opening fee.
     * @return Opening fee value.
     */
    inline double getOpeningFee() const { return openingFee_; }

    /**
     * @brief Sets the opening fee percentage applied to the loan amount.
     * @param percent Fee percentage.
     */
    inline void setOpeningPercent(double percent) { openingPercent_ = percent; }

    /**
     * @brief Gets the opening fee percentage.
     * @return Percentage fee value.
     */
    inline double getOpeningPercent() const { return openingPercent_; }

    /**
     * @brief Resets all values and internal state flags.
     */
    inline void reset()
    {
        amount_ = initialPayment_ = interest_ = interestPeriodic_ =
        payment_ = openingFee_ = openingPercent_ = 0.0;

        periodTotal_ = periodElapsed_ = 0;

        amountSet_ = interestSet_ = paymentSet_ =
        periodTotalSet_ = periodElapsedSet_ = false;
    }

    // -------------------------------------------------------------------------
    // Calculation Methods
    // -------------------------------------------------------------------------

    /**
     * @brief Computes the outstanding loan balance after n payments.
     * @return Remaining balance.
     *
     * Uses:
     * @f[
     * B_n = A(1+i)^n - \frac{P}{i}((1+i)^n - 1)
     * @f]
     */
    double calculateLoanBalance();

    /**
     * @brief Computes the required monthly payment for the loan.
     * @return Monthly payment amount.
     *
     * Formula:
     * @f[
     * P = \frac{iA}{1 - (1+i)^{-N}}
     * @f]
     */
    double calculatePayment();

    /**
     * @brief Computes the number of payments needed to pay off the loan.
     * @return Number of required payments.
     *
     * Formula:
     * @f[
     * N = -\frac{\log (1 - iA/P)}{\log (1+i)}
     * @f]
     */
    double calculateNumberPayments();

    /**
     * @brief Computes the original loan amount given payments and rate.
     * @return Principal loan amount.
     *
     * Formula:
     * @f[
     * A = \frac{P}{i}(1 - (1+i)^{-N})
     * @f]
     */
    double calculateLoanAmount();

    /**
     * @brief Approximates the yearly interest rate based on payment schedule.
     * @return Approximated yearly interest rate (percentage).
     */
    double calculateInterestRate();

    /**
     * @brief Computes the effective interest rate including opening fees.
     * @return Effective annual interest rate (percentage).
     */
    double calculateEffectiveInterestRate();

    /**
     * @brief Generates a descriptive string containing the calculator's parameters.
     * @return Summary string.
     */
    string toString();

private:
    double amount_;          ///< Principal loan amount (A)
    bool amountSet_;         ///< True if principal has been set

    double initialPayment_;  ///< Up-front down payment

    double interest_;        ///< Yearly nominal interest rate (%)
    double interestPeriodic_;///< Monthly interest rate (decimal)
    bool interestSet_;       ///< True if interest rate is set

    double payment_;         ///< Monthly payment amount
    bool paymentSet_;        ///< True if payment is set

    int periodTotal_;        ///< Total number of payments (N)
    bool periodTotalSet_;    ///< True if total periods are set

    int periodElapsed_;      ///< Payments completed (n)
    bool periodElapsedSet_;  ///< True if elapsed periods are set

    double openingFee_;      ///< Fixed opening fee
    double openingPercent_;  ///< Opening fee as % of amount
};

#endif // LOANCALCULATOR_H_INCLUDED
