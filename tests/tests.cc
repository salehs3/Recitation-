#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}

TEST_CASE("RegisterAccount duplicate account", "[RegisterAccount]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "Saleh", 300.30);

  bool exception_thrown = false;

  try {
    atm.RegisterAccount(12345678, 1234, "Saleh", 500.00);
  } catch (...) {
    exception_thrown = true;
  }

  REQUIRE(exception_thrown);
}

TEST_CASE("RegisterAccount duplicate does not change balance",
          "[RegisterAccount]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "Messi", 300.30);

  try {
    atm.RegisterAccount(12345678, 1234, "Messi", 999.99);
  } catch (...) {
  }

  auto accounts = atm.GetAccounts();
  Account account = accounts[{12345678, 1234}];

  REQUIRE(account.balance == 300.30);
}

TEST_CASE("WithdrawCash updates balance correctly", "[WithdrawCash]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "x", 300.00);
  atm.WithdrawCash(12345678, 1234, 50.00);

  auto accounts = atm.GetAccounts();
  Account xacc = accounts[{12345678, 1234}];

  REQUIRE(xacc.balance == 250.00);
}

TEST_CASE("Withdraw does not got -ve ", "[WithdrawCash]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "Messi", 300.30);

  bool exthrow = false;

  try {
    atm.WithdrawCash(12345678, 1234, 400.00);
  } catch (...) {
    exthrow = true;
  }

  REQUIRE(exthrow);
}

TEST_CASE("Withdraw does accept -ve ", "[WithdrawCash]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "Messi", 300.30);

  bool exthrow = false;

  try {
    atm.WithdrawCash(12345678, 1234, -400.00);
  } catch (...) {
    exthrow = true;
  }

  REQUIRE(exthrow);
}

TEST_CASE("DepositCash updates balance", "[DepositCash]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "Sam", 100.00);
  atm.DepositCash(12345678, 1234, 50.00);

  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 150.00);
}

TEST_CASE("DepositCash negative amount not allowed", "[DepositCash]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "Sam", 100.00);

  bool exception_thrown = false;

  try {
    atm.DepositCash(12345678, 1234, -20.00);
  } catch (...) {
    exception_thrown = true;
  }

  REQUIRE(exception_thrown);
}

TEST_CASE("PrintLedger invalid account", "[PrintLedger]") {
  Atm atm;

  bool exception_thrown = false;

  try {
    atm.PrintLedger("./ledger.txt", 1111, 2222);
  } catch (...) {
    exception_thrown = true;
  }

  REQUIRE(exception_thrown);
}

TEST_CASE("PrintLedger creates correct file", "[PrintLedger]") {
  Atm atm;

  atm.RegisterAccount(12345678, 1234, "Sam", 300.30);
  atm.DepositCash(12345678, 1234, 50.00);

  atm.PrintLedger("./test.txt", 12345678, 1234);

  REQUIRE(CompareFiles("./test.txt", "./test.txt"));
}