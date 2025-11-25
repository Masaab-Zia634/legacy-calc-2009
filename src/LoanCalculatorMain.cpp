/**
 * @file main.cpp
 * @brief Command-line and GUI entry point for the Loan Calculator application.
 *
 * This file loads command-line arguments, configures a LoanCalculator instance,
 * and either launches the Qt GUI or performs a one-off calculation in CLI mode.
 *
 * Supported calculations:
 *  - Loan balance after elapsed payments
 *  - Payment amount
 *  - Number of payments
 *  - Loan amount
 *  - Interest rate
 */

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include <QApplication>

#include <LoanCalcQtMainWindow.h>
#include <CmdLineParser.h>
#include <LoanCalculator.h>

using namespace std;

/**
 * @enum CALC_TYPE
 * @brief Represents which type of calculation the user requested.
 */
enum CALC_TYPE
{
    CALC_UNKNOWN = 0,   ///< No calculation specified
    CALC_BALANCE = 100, ///< Calculate loan balance after n payments
    CALC_PAYMENT,       ///< Calculate monthly payment
    CALC_NUMPAYMENTS,   ///< Calculate number of months required
    CALC_AMOUNT,        ///< Calculate original loan amount
    CALC_INTEREST       ///< Calculate interest rate
};

//
// ─────────────────────────────────────────────────────────────
// Command-line argument constants
// ─────────────────────────────────────────────────────────────
//
/** @brief CLI flag: calculate balance */
const string ARG_CALC_BALANCE     = "-cb";
/** @brief CLI flag: calculate payment */
const string ARG_CALC_PAYMENT     = "-cp";
/** @brief CLI flag: calculate number of payments */
const string ARG_CALC_NUMPAYMENTS = "-cn";
/** @brief CLI flag: calculate loan amount */
const string ARG_CALC_AMOUNT      = "-ca";
/** @brief CLI flag: calculate interest rate */
const string ARG_CALC_INTEREST    = "-ci";

/// Payment amount flag
const string ARG_PAYMENT         = "-p";
/// Total period flag
const string ARG_PERIOD_TOTAL    = "-N";
/// Elapsed period flag
const string ARG_PERIOD_ELAPSED  = "-n";
/// Loan amount flag
const string ARG_AMOUNT          = "-a";
/// Initial payment flag
const string ARG_INITIAL_PAYMENT = "-ai";
/// Interest rate flag
const string ARG_INTEREST        = "-i";
/// Opening fee flag
const string ARG_OPENFEE         = "-of";
/// Opening fee percentage flag
const string ARG_OPENPERCENT     = "-op";


// ─────────────────────────────────────────────────────────────
// loadCmdLine()
// ─────────────────────────────────────────────────────────────

/**
 * @brief Registers all supported command-line options with the parser.
 *
 * @param clp The command-line parser to populate.
 *
 * This function defines:
 *  - Mutually exclusive calculation flags (e.g., -cb, -cp)
 *  - Value-based options (loan amount, interest rate, etc.)
 *  - Minimum argument count
 */
void loadCmdLine(CmdLineParser &clp)
{
    clp.setMainHelpText("A simple loan calculator");
    clp.setMainHelpTextEnd("With no options set, a GUI will be launched");

    // Mutually exclusive calculation type options
    clp.addMutExclCmdLineOption(new CmdLineOptionFlag(
        ARG_CALC_BALANCE,
        "Calculate loan balance after a number of payments",
        false, CALC_BALANCE));

    clp.addMutExclCmdLineOption(new CmdLineOptionFlag(
        ARG_CALC_PAYMENT,
        "Calculate monthly payment",
        false, CALC_PAYMENT));

    clp.addMutExclCmdLineOption(new CmdLineOptionFlag(
        ARG_CALC_NUMPAYMENTS,
        "Calculate number of payments needed",
        false, CALC_NUMPAYMENTS));

    clp.addMutExclCmdLineOption(new CmdLineOptionFlag(
        ARG_CALC_AMOUNT,
        "Calculate loan amount",
        false, CALC_AMOUNT));

    clp.addMutExclCmdLineOption(new CmdLineOptionFlag(
        ARG_CALC_INTEREST,
        "Calculate interest rate",
        false, CALC_INTEREST));

    clp.setMutExclUsageText("Calculations");

    // Numeric input options
    clp.addCmdLineOption(new CmdLineOptionFloat(ARG_PAYMENT, "Monthly payment, e.g., 325.67"));
    clp.addCmdLineOption(new CmdLineOptionInt(ARG_PERIOD_TOTAL, "Total loan period in months, e.g., 60"));
    clp.addCmdLineOption(new CmdLineOptionInt(ARG_PERIOD_ELAPSED, "Elapsed months, e.g., 32"));
    clp.addCmdLineOption(new CmdLineOptionInt(ARG_AMOUNT, "Initial amount, e.g., 19300"));
    clp.addCmdLineOption(new CmdLineOptionFloat(ARG_INITIAL_PAYMENT, "Initial payment, default 0.0"));
    clp.addCmdLineOption(new CmdLineOptionFloat(ARG_INTEREST, "Yearly interest rate, e.g., 6.75"));
    clp.addCmdLineOption(new CmdLineOptionFloat(ARG_OPENFEE, "Opening fee, default 0.0"));
    clp.addCmdLineOption(new CmdLineOptionFloat(ARG_OPENPERCENT, "Opening fee % value, default 0.0"));

    clp.setMinNumberArgs(3);
}


// ─────────────────────────────────────────────────────────────
// parseCommandLine()
// ─────────────────────────────────────────────────────────────

/**
 * @brief Parses the command-line arguments and loads them into a LoanCalculator.
 *
 * @param argc Argument count (from main)
 * @param argv Argument vector (from main)
 * @param clp  Initialized command-line parser
 * @param calculator Loan calculator instance to configure
 *
 * @return CALC_TYPE The calculation mode requested by the user.
 *
 * @throws std::invalid_argument if parsing fails or if incompatible options are provided.
 */
CALC_TYPE parseCommandLine(int argc, char **argv, CmdLineParser &clp, LoanCalculator &calculator)
{
    if (!clp.parseCmdLine(argc, argv))
    {
        clp.printUsage();
        return CALC_UNKNOWN;
    }

    // Helpers to extract typed values
    auto getInt = [&](const string &arg) {
        auto *opt = dynamic_cast<CmdLineOptionInt *>(clp.getCmdLineOption(arg));
        return opt ? opt->getValue() : 0;
    };

    auto getFloat = [&](const string &arg) {
        auto *opt = dynamic_cast<CmdLineOptionFloat *>(clp.getCmdLineOption(arg));
        return opt ? opt->getValue() : 0.0f;
    };

    // Load calculator values
    calculator.setAmount(getInt(ARG_AMOUNT));
    calculator.setInitialPayment(getFloat(ARG_INITIAL_PAYMENT));
    calculator.setInterest(getFloat(ARG_INTEREST));
    calculator.setPayment(getFloat(ARG_PAYMENT));
    calculator.setPeriodTotal(getInt(ARG_PERIOD_TOTAL));
    calculator.setPeriodElapsed(getInt(ARG_PERIOD_ELAPSED));
    calculator.setOpeningFee(getFloat(ARG_OPENFEE));
    calculator.setOpeningPercent(getFloat(ARG_OPENPERCENT));

    // Determine which calculation was selected
    auto *m = clp.getMutExclOption();
    if (!m) return CALC_UNKNOWN;

    auto *flag = dynamic_cast<CmdLineOptionFlag *>(m);
    return flag ? static_cast<CALC_TYPE>(flag->getValueKey()) : CALC_UNKNOWN;
}


// ─────────────────────────────────────────────────────────────
// main()
// ─────────────────────────────────────────────────────────────

/**
 * @brief Program entry point.
 *
 * If run without arguments, the Qt GUI is launched.
 * If run with arguments, executes a command-line loan calculation.
 *
 * @param argc Number of command-line arguments
 * @param argv Command-line argument values
 *
 * @return int Exit code (0 = success)
 */
int main(int argc, char **argv)
{
    LoanCalculator calculator;

    // Launch GUI if no CLI args
    if (argc == 1)
    {
        QApplication app(argc, argv);
        LoanCalcQtMainWindow mainWindow(&calculator);
        mainWindow.show();
        return app.exec();
    }

    // Command-line mode
    CmdLineParser clp;
    loadCmdLine(clp);
    CALC_TYPE ct = parseCommandLine(argc, argv, clp, calculator);

    try
    {
        cout << endl;

        switch (ct)
        {
            case CALC_BALANCE:
                cout << "Loan Balance = "
                     << static_cast<float>(calculator.calculateLoanBalance()) << endl;
                break;

            case CALC_PAYMENT:
            {
                float payment = calculator.calculatePayment();
                cout << "Monthly Payment    = " << payment << "\n"
                     << "Total amt paid     = " << payment * calculator.getPeriodTotal() << endl;

                if (calculator.getOpeningPercent() != 0.0 ||
                    calculator.getOpeningFee()     != 0.0)
                {
                    cout << "Interest with fees = "
                         << calculator.calculateEffectiveInterestRate() << "%" << endl;
                }
                break;
            }

            case CALC_NUMPAYMENTS:
                cout << "Number of payments = "
                     << calculator.calculateNumberPayments() << endl;
                break;

            case CALC_AMOUNT:
                cout << "Initial Loan amount = "
                     << calculator.calculateLoanAmount() << endl;
                break;

            case CALC_INTEREST:
                cout << "Yearly Interest Rate = "
                     << calculator.calculateInterestRate() << "%" << endl;
                break;

            case CALC_UNKNOWN:
                return 1;

            default:
                cerr << "Unrecognized calculation type, exiting" << endl;
                return 0;
        }

        // Summary dump
        cout << calculator.toString() << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error executing loan calculator: " << e.what() << endl;
    }

    cout << endl;
    return 0;
}
