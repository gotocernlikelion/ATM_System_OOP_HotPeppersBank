#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class ATM;
class Bank;
class Account;

// Bank Class
class Bank {
private:
    string bank_name;
    unordered_map<string, Account*> accounts; //계좌 관리 map

public:
    Bank(string bank_name);
    ~Bank();
    string getBankName();
    void openAccount(const string& userName, const string& accountNumber, int initialFund, const string& password);
    Account* getAccount(const string& accountNumber);
    bool authenticate(const string& accountNumber, const string& password); //사용자 인증
};

Bank::Bank(string b_name){
    bank_name=b_name;
    cout<<"Bank "<<bank_name<<" created"<<endl;
}
Bank::~Bank(){}

void openAccount(const string& userName, const string& accountNumber, int initialFund, const string& password){
    Account()
};

// Account Class
class Account {
private:
    string user_name;
    Bank* bank;
    string account_number;
    int available_fund;
    string password;
    vector<pair<string, int>> transactionHistory;
    

public:
    Account(string a_bank, string u_name, string a_num, int init_fund, string ps);
    ~Account();
    void deposit(int amount);
    bool withdraw(int amount);
    void addTransaction(const string& type, int amount);
    void printTransactionHistory() const;
};
Account::Account(string a_bank, string u_name, string a_num, int init_fund, string ps){
    // Bank=a_bank;
    user_name=u_name;
    account_number=a_num;
    available_fund=init_fund;
    password=ps;
}

// ATM Class
class ATM {
private:
    int serial_number; //REQ1.1
    string atm_type; //REQ1.2
    string language; //REQ1.3
    int cash_1000_num;
    int cash_5000_num;
    int cash_10000_num;
    int cash_50000_num; //REQ1.4 / 1.8
    int total_cash;
    int transactionFeePrimaryBank;
    int transactionFeeNonPrimaryBank;
    bool isSessionActive;
    string currentCardNumber;

public:
    static int numOfATMs;
    static int withdrawal_count;
    static int numOfTransactions;

    ATM(int s_num, string type, string lang, int cash_1000, int cash_5000, int cash_10000, int cash_50000);
    ~ATM();
    void startSession(const string& cardNumber);
    void endSession();
    bool checkCardValidity(const string& cardNumber);
    bool performTransaction(const string& type, Account* account, int amount);
    void displayTransactionSummary();
};

ATM::ATM(int s_num, string type, string lang, int cash_1000, int cash_5000, int cash_10000, int cash_50000){
    
};






int main() {
//     // System setup example
    Bank bank("hana");
    bank.openAccount("Jenny", "123456789012", 5000.0,'1234');

// void openAccount(const string& userName, const string& accountNumber, int initialFund, const string& password);
//     ATM atm(123456, "Single-Bank");

//     // Start a session
//     atm.startSession("123456789012");

//     // Perform transactions
//     Account* account = bank.getAccount("123456789012");
//     if (account) {
//         atm.performTransaction("deposit", account, 2000.0);
//         atm.performTransaction("withdraw", account, 1000.0);
//     }

//     // End session
//     atm.endSession();

//     return 0;
}