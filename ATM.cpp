#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Account {
public:
    string accountNumber;
    string username;
    string password;
    int balance;

    Account(string accNum, string user, string pwd, int bal)
        : accountNumber(accNum), username(user), password(pwd), balance(bal) {}

    Account() : accountNumber(""), username(""), password(""), balance(0) {}
};



class Bank {
private:
    string name;
    unordered_map<string, Account> accounts;

public:
    Bank(string bankName) : name(bankName) {}

    string getName() const { return name; }

    void addAccount(const Account& account) {
        accounts[account.accountNumber] = account;
    }

    Account* authenticate(const string& accountNumber, const string& password) {
        if (accounts.count(accountNumber) && accounts[accountNumber].password == password) {
            return &accounts[accountNumber];
        }
        return nullptr;
    }

    void displayAccounts() const {
        for (const auto& entry : accounts) {
            const Account& account = entry.second;
            cout << "Account [Bank: " << name
                << ", No: " << account.accountNumber
                << ", Owner: " << account.username
                << "] balance: " << account.balance << "원\n";
        }
    }
};



class ATM {
private:
    string atmType;      // "Single" or "Multi"
    Bank* primaryBank;   // Primary bank associated with the ATM
    string serialNumber;
    string language;
    int cash1000, cash5000, cash10000, cash50000;
    string adminCardNumber;

public:
    ATM(string type, Bank* bank, string serial, string lang, int c1000, int c5000, int c10000, int c50000, string adminCard)
        : atmType(type), primaryBank(bank), serialNumber(serial), language(lang),
        cash1000(c1000), cash5000(c5000), cash10000(c10000), cash50000(c50000), adminCardNumber(adminCard) {}

    bool authenticateUser(const vector<Bank*>& allBanks, const string& accountNumber, const string& password) {
        if (atmType == "Single") {
            Account* account = primaryBank->authenticate(accountNumber, password);
            if (account) {
                cout << "User authorized for Single-Bank ATM in " << primaryBank->getName() << " Bank." << endl;
                return true;
            }
        }
        else if (atmType == "Multi") {
            for (Bank* bank : allBanks) {
                Account* account = bank->authenticate(accountNumber, password);
                if (account) {
                    cout << "User authorized for Multi-Bank ATM. Access granted for " << bank->getName() << " Bank." << endl;
                    return true;
                }
            }
        }
        cout << "Authorization failed." << endl;
        return false;
    }

    void displayRemainingCash() const {
        cout << "ATM [SN: " << serialNumber << "] remaining cash: {"
            << "KRW 50000 : " << cash50000 << ", "
            << "KRW 10000 : " << cash10000 << ", "
            << "KRW 5000 : " << cash5000 << ", "
            << "KRW 1000 : " << cash1000 << "}\n";
    }
};

void displaySnapshot(const vector<Bank>& banks, const vector<ATM>& atms) {
    cout << "\n\n=====<< Account/ATM Snapshot >>=====\n";

    // Display all ATMs' information
    cout << "\n[ATM Information - Remaining Cash]\n";
    for (const auto& atm : atms) {
        atm.displayRemainingCash();
    }

    // Display all accounts' information
    cout << "\n[Account Information - Remaining Balance]\n";
    for (const auto& bank : banks) {
        bank.displayAccounts();
    }
    cout << "=====================================\n";
}

int main() {
    // Bank Initializing Step
    cout << "=====<<Bank Initialization>>=====\n";
    vector<Bank> banks = { Bank("Kakao"), Bank("Woori"), Bank("Toss") };
    vector<Bank*> allBanks;

    for (Bank& bank : banks) {
        cout << "[" << bank.getName() << "] bank created!\n";
        allBanks.push_back(&bank);
    }

    // Account Creation Step
    cout << "\n=====<<Account Creation>>=====\n";
    string createAccountInput;
    int accountCount = 1;
    do {
        cout << "\nDo you want to create Account " << accountCount++ << "? (yes or no) -> ";
        cin >> createAccountInput;

        if (createAccountInput == "yes") {
            string bankName, username, accountNumber, password;
            int balance;
            cout << "Bank Name: ";
            cin >> bankName;

            cout << "User Name: ";
            cin >> username;

            cout << "Account Number(12digit): ";
            cin >> accountNumber;

            cout << "Balance(KRW): ";
            cin >> balance;

            cout << "Password: ";
            cin >> password;

            // 은행 이름에 따라 계좌 추가
            bool bankFound = false;
            for (Bank* bank : allBanks) {
                if (bank->getName() == bankName) {
                    bank->addAccount(Account(accountNumber, username, password, balance));
                    bankFound = true;
                    break;
                }
            }
            if (!bankFound) {
                cout << "Bank not found. Account creation failed.\n";
            }
        }
    } while (createAccountInput == "yes");

    // ATM Creation Step
    cout << "\n=====<<ATM Creation Step>>=====\n";
    vector<ATM> atms;
    string createATMInput;
    int atmCount = 1;
    do {
        cout << "\nDo you want to create ATM " << atmCount++ << "? (yes or no) -> ";
        cin >> createATMInput;

        if (createATMInput == "yes") {
            string atmType, primaryBankName, serialNumber, language, adminCardNumber;
            int cash1000, cash5000, cash10000, cash50000;
            Bank* primaryBank = nullptr;

            cout << "Primary Bank Name: ";
            cin >> primaryBankName;

            cout << "Serial Number(6digit): ";
            cin >> serialNumber;

            cout << "Type(Single or Multi): ";
            cin >> atmType;

            cout << "Language(Uni or Bi): ";
            cin >> language;

            cout << "1000원 Cash 몇 장?: ";
            cin >> cash1000;
            cout << "5000원 Cash 몇 장?: ";
            cin >> cash5000;
            cout << "10000원 Cash 몇 장?: ";
            cin >> cash10000;
            cout << "50000원 Cash 몇 장?: ";
            cin >> cash50000;

            cout << "Admin card Number: ";
            cin >> adminCardNumber;

            // Primary bank 찾기
            if (atmType == "Single") {
                for (Bank* bank : allBanks) {
                    if (bank->getName() == primaryBankName) {
                        primaryBank = bank;
                        break;
                    }
                }
                if (!primaryBank) {
                    cout << "Primary bank not found. Skipping ATM creation.\n";
                    continue;
                }
            }

            // ATM 생성 및 추가
            atms.emplace_back(atmType, primaryBank, serialNumber, language, cash1000, cash5000, cash10000, cash50000, adminCardNumber);
            cout << "ATM " << atmCount - 1 << " created successfully.\n";
        }
    } while (createATMInput == "yes");

    // Program execution loop to check for '/' input
    string command;
    while (true) {
        cout << "\nEnter '/' to display snapshot or 'exit' to quit: ";
        cin >> command;

        if (command == "/") {
            displaySnapshot(banks, atms);
        }
        else if (command == "exit") {
            break;
        }
    }

    return 0;
}
