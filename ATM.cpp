#include <iostream> //test
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Account {
private:
    string username;
    string accountNumber;
    int balance;
    string password;
    vector<string> transactionHistory;

public:
    Account(const string& user, const string& acctNum, int initialBalance, const string& pwd)
        : username(user), accountNumber(acctNum), balance(initialBalance), password(pwd) {}

    string getUsername() const { return username; }
    string getAccountNumber() const { return accountNumber; }
    int getBalance() const { return balance; }
    string getPassword() const { return password; }

    void deposit(int amount) {
        balance += amount;
        transactionHistory.push_back("Deposit: " + to_string(amount));
    }

    void withdraw(int amount) {
        if (amount <= balance) {
            balance -= amount;
            transactionHistory.push_back("Withdraw: " + to_string(amount));
        }
        else {
            cout << "Insufficient funds for withdrawal." << endl;
        }
    }

    void displayTransactionHistory() const {
        cout << "Transaction History for " << accountNumber << ":" << endl;
        for (const auto& record : transactionHistory) {
            cout << record << endl;
        }
    }
};

class ATM {
private:
    int serialnumber;
    string bankname;
    string atmtype;
    string atmlanguage;
    int num_KRW1000;
    int num_KRW5000;
    int num_KRW10000;
    int num_KRW50000;
    int total_KRW = 0;

public:
    ATM(const string& bank, int serial, const string& type, const string& language,
        int krw1000, int krw5000, int krw10000, int krw50000)
        : bankname(bank), serialnumber(serial), atmtype(type), atmlanguage(language),
        num_KRW1000(krw1000), num_KRW5000(krw5000), num_KRW10000(krw10000), num_KRW50000(krw50000) {
        total_KRW = num_KRW1000 * 1000 + num_KRW5000 * 5000 + num_KRW10000 * 10000 + num_KRW50000 * 50000;
    }

    string getBankname() const { return bankname; }
    int getSerialnumber() const { return serialnumber; }
    string getAtmtype() const { return atmtype; }
    string getAtmlanguage() const { return atmlanguage; }
    int getTotalCash() const { return total_KRW; }

    void depositCash(int amount) {
        total_KRW += amount;
    }

    void displayCashInfo() const {
        cout << "1000원 Cash 장수: " << num_KRW1000 << endl;
        cout << "5000원 Cash 장수: " << num_KRW5000 << endl;
        cout << "10000원 Cash 장수: " << num_KRW10000 << endl;
        cout << "50000원 Cash 장수: " << num_KRW50000 << endl;
        cout << "Total Cash in ATM: " << total_KRW << "원" << endl;
    }
};

class Bank {
private:
    string name;
    unordered_map<string, vector<Account>> userAccounts;

public:
    Bank(const string& bankName) : name(bankName) {}

    string getBankName() const { return name; }

    Account* openAccount(const string& username, const string& accountNumber, int initialDeposit, const string& password) {
        Account newAccount(username, accountNumber, initialDeposit, password);
        userAccounts[username].push_back(newAccount);
        cout << "Account created for " << username << " with account number " << accountNumber << endl;
        return &userAccounts[username].back();
    }

    void displayUserAccounts(const string& username) const {
        auto it = userAccounts.find(username);
        if (it != userAccounts.end()) {
            cout << "Accounts for " << username << " in " << name << " Bank:" << endl;
            for (const auto& account : it->second) {
                cout << "- Account Number: " << account.getAccountNumber()
                    << ", Balance: " << account.getBalance() << "원" << endl;
            }
        }
        else {
            cout << "No accounts found for " << username << " in " << name << " Bank." << endl;
        }
    }

    Account* authenticateAccount(const string& accountNumber, const string& password) {
        for (auto& pair : userAccounts) {
            for (auto& account : pair.second) {
                if (account.getAccountNumber() == accountNumber && account.getPassword() == password) {
                    cout << "User Authorization is successful!" << endl;
                    return &account;
                }
            }
        }
        cout << "Authorization failed: Invalid account number or password." << endl;
        return nullptr;
    }
};

int main() {
    // ATM 생성
    string bankname, atmtype, atmlanguage;
    int serialnumber;

    cout << "Do you want to create ATM 1? (yes or no) -> ";
    string atmInput;
    cin >> atmInput;

    if (atmInput == "yes") {
        cout << "Primary Bank Name: ";
        cin >> bankname;

        cout << "Serial Number: ";
        cin >> serialnumber;

        cout << "Type(Single or Multi): ";
        cin >> atmtype;

        cout << "Language(Uni or Bi): ";
        cin >> atmlanguage;

        cout << "1000원 Cash 몇 장?: ";
        int num_KRW1000; cin >> num_KRW1000;
        cout << "5000원 Cash 몇 장?: ";
        int num_KRW5000; cin >> num_KRW5000;
        cout << "10000원 Cash 몇 장?: ";
        int num_KRW10000; cin >> num_KRW10000;
        cout << "50000원 Cash 몇 장?: ";
        int num_KRW50000; cin >> num_KRW50000;

        ATM atm(bankname, serialnumber, atmtype, atmlanguage, num_KRW1000, num_KRW5000, num_KRW10000, num_KRW50000);

        cout << "ATM created successfully with the following details:" << endl;
        cout << "Primary Bank Name: " << atm.getBankname() << endl;
        cout << "Serial Number: " << atm.getSerialnumber() << endl;
        cout << "Type(Single or Multi): " << atm.getAtmtype() << endl;
        cout << "Language(Uni or Bi): " << atm.getAtmlanguage() << endl;
        atm.displayCashInfo();
    }
    else {
        cout << "ATM creation canceled." << endl;
        return 0;
    }

    // 여러 은행 초기화
    vector<Bank> banks = { Bank("Kakao"), Bank("Shinhan"), Bank("Daegu") };
    string username = "U1";  // 고정 사용자 이름

    // 각 은행에 대해 계좌 개설
    for (int i = 0; i < 3; i++) {
        string createAccountInput;
        cout << "\nDo you want to create Account " << (i + 1) << "? (yes or no) -> ";
        cin >> createAccountInput;

        if (createAccountInput == "yes") {
            string accountNumber, password;
            int initialFunds;

            cout << "Bank Name: " << banks[i].getBankName() << endl;
            cout << "User Name: " << username << endl;

            cout << "Account Number: ";
            cin >> accountNumber;

            cout << "Available Funds(KRW): ";
            cin >> initialFunds;

            cout << "Password: ";
            cin >> password;

            banks[i].openAccount(username, accountNumber, initialFunds, password);
        }
    }

    // 각 은행에서 사용자의 계좌를 출력
    for (const auto& bank : banks) {
        bank.displayUserAccounts(username);
    }

    // 사용자 인증
    string cardNumber, inputPassword;
    cout << "\nPlease insert the card into the card insert slot.\nCard number(Account number): ";
    cin >> cardNumber;

    cout << "Please enter your password on the keypad.\nPassword: ";
    cin >> inputPassword;

    // 모든 은행에서 카드 인증 시도
    bool authenticated = false;
    for (auto& bank : banks) {
        if (bank.authenticateAccount(cardNumber, inputPassword) != nullptr) {
            authenticated = true;
            break;
        }
    }
    if (!authenticated) {
        cout << "Authentication failed." << endl;
    }

    // 프로그램 종료 대기
    cout << "Press Enter to exit...";
    cin.ignore();
    cin.get();

    return 0;
}
