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

enum CALC_TYPE
{
    CALC_UNKNOWN = 0,
    CALC_BALANCE = 100,
    CALC_PAYMENT,
    CALC_NUMPAYMENTS,
    CALC_AMOUNT,
    CALC_INTEREST
};

// Argument constants
const string ARG_CALC_BALANCE     = "-cb";
const string ARG_CALC_PAYMENT     = "-cp";
const string ARG_CALC_NUMPAYMENTS = "-cn";
const string ARG_CALC_AMOUNT      = "-ca";
const string ARG_CALC_INTEREST    = "-ci";

const string ARG_PAYMENT         = "-p";
const string ARG_PERIOD_TOTAL    = "-N";
const string ARG_PERIOD_ELAPSED  = "-n";
const string ARG_AMOUNT          = "-a";
const string ARG_INITIAL_PAYMENT = "-ai";
const string ARG_INTEREST        = "-i";
const string ARG_OPENFEE         = "-of";
const string ARG_OPENPERCENT     = "-op";


// ------------------------------------------------------------
// Load command-line options
// ------------------------------------------------------------
void loadCmdLine(CmdLineParser &clp)
{
    clp.setMainHelpText("A simple loan calculator");
    clp.setMainHelpTextEnd("With no options set, a GUI will be launched");

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

    // Value options
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


// ------------------------------------------------------------
// Parse arguments into calculator + detect calculation type
// ------------------------------------------------------------
CALC_TYPE parseCommandLine(int argc, char **argv, CmdLineParser &clp, LoanCalculator &calculator)
{
    if (!clp.parseCmdLine(argc, argv))
    {
        clp.printUsage();
        return CALC_UNKNOWN;
    }

    auto getInt = [&](const string &arg) {
        auto *opt = dynamic_cast<CmdLineOptionInt *>(clp.getCmdLineOption(arg));
        return (opt ? opt->getValue() : 0);
    };

    auto getFloat = [&](const string &arg) {
        auto *opt = dynamic_cast<CmdLineOptionFloat *>(clp.getCmdLineOption(arg));
        return (opt ? opt->getValue() : 0.0f);
    };

    calculator.setAmount(getInt(ARG_AMOUNT));
    calculator.setInitialPayment(getFloat(ARG_INITIAL_PAYMENT));
    calculator.setInterest(getFloat(ARG_INTEREST));
    calculator.setPayment(getFloat(ARG_PAYMENT));
    calculator.setPeriodTotal(getInt(ARG_PERIOD_TOTAL));
    calculator.setPeriodElapsed(getInt(ARG_PERIOD_ELAPSED));
    calculator.setOpeningFee(getFloat(ARG_OPENFEE));
    calculator.setOpeningPercent(getFloat(ARG_OPENPERCENT));

    auto *m = clp.getMutExclOption();
    if (!m) return CALC_UNKNOWN;

    auto *flag = dynamic_cast<CmdLineOptionFlag *>(m);
    return flag ? static_cast<CALC_TYPE>(flag->getValueKey()) : CALC_UNKNOWN;
}


// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main(int argc, char **argv)
{
    LoanCalculator calculator;

    // Launch GUI if no CLI arguments
    if (argc == 1)
    {
        QApplication app(argc, argv);
        LoanCalcQtMainWindow mainWindow(&calculator);
        mainWindow.show();
        return app.exec();
    }

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
                     << "Total amt paid     = "
                     << static_cast<float>(payment * calculator.getPeriodTotal())
                     << endl;

                if (calculator.getOpeningPercent() != 0.0 ||
                    calculator.getOpeningFee()     != 0.0)
                {
                    cout << "Interest with fees = "
                         << static_cast<float>(calculator.calculateEffectiveInterestRate())
                         << "%" << endl;
                }
                break;
            }

            case CALC_NUMPAYMENTS:
                cout << "Number of payments = "
                     << static_cast<float>(calculator.calculateNumberPayments()) << endl;
                break;

            case CALC_AMOUNT:
                cout << "Initial Loan amount = "
                     << static_cast<float>(calculator.calculateLoanAmount()) << endl;
                break;

            case CALC_INTEREST:
                cout << "Yearly Interest Rate = "
                     << static_cast<float>(calculator.calculateInterestRate())
                     << "%" << endl;
                break;

            case CALC_UNKNOWN:
                return 1;

            default:
                cerr << "Unrecognized calculation type, exiting" << endl;
                return 0;
        }

        // Print calculator summary
        cout << calculator.toString() << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error executing loan calculator: " << e.what() << endl;
    }

    cout << endl;
    return 0;
}
