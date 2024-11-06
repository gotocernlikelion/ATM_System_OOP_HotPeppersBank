#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>  // 파일 출력을 위한 라이브러리

using namespace std;

// 다국어 지원용 Enum 타입 (영어 또는 한국어)
enum Language {
    ENGLISH,
    KOREAN
};

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
    string atmType;
    Bank* primaryBank;
    string serialNumber;
    Language language;
    int cash1000, cash5000, cash10000, cash50000;
    string adminCardNumber;
    vector<string> transactionHistory;  // 거래 내역 저장을 위한 벡터

public:
    ATM(string type, Bank* bank, string serial, Language lang, int c1000, int c5000, int c10000, int c50000, string adminCard)
        : atmType(type), primaryBank(bank), serialNumber(serial), language(lang),
        cash1000(c1000), cash5000(c5000), cash10000(c10000), cash50000(c50000), adminCardNumber(adminCard) {}

    string getSerialNumber() const { return serialNumber; }

    // 다국어 지원을 위해 언어 설정을 변경하는 메서드
    void setLanguage(Language lang) {
        language = lang;
    }

    // 언어에 따라 출력 메시지를 선택하는 메서드
    string translate(const string& eng, const string& kor) const {
        return language == ENGLISH ? eng : kor;
    }

    // 거래 내역을 파일에 출력하는 메서드
    void saveTransactionHistoryToFile() const {
        ofstream outFile("TransactionHistory.txt");  // 텍스트 파일로 저장
        if (!outFile) {
            cerr << translate("Failed to open file.", "파일을 열 수 없습니다.") << endl;
            return;
        }

        outFile << translate("Transaction History\n", "거래 내역\n");
        for (const string& record : transactionHistory) {
            outFile << record << endl;
        }
        outFile.close();
        cout << translate("Transaction history saved to TransactionHistory.txt\n",
                          "거래 내역이 TransactionHistory.txt에 저장되었습니다.\n");
    }

    // 에러 메시지를 언어에 맞춰 출력하는 메서드
    void showError(const string& engMsg, const string& korMsg) const {
        cout << translate(engMsg, korMsg) << endl;
    }

    void deposit(Account* account) {
        int depositAmount;
        cout << translate("Enter deposit amount: ", "입금 금액을 입력하세요: ");
        if (!(cin >> depositAmount) || depositAmount <= 0) {  // 잘못된 입력에 대한 예외 처리
            showError("Invalid deposit amount.", "잘못된 입금 금액입니다.");
            return;
        }

        // 거래 내역에 입금 기록 추가
        transactionHistory.push_back(translate("Deposit of ", "입금: ") + to_string(depositAmount) + translate(" KRW", "원"));
        account->balance += depositAmount;  // 계좌 잔액 증가
        cout << translate("Deposit successful!", "입금 성공!") << endl;
    }

    void withdraw(Account* account) {
        int withdrawAmount;
        cout << translate("Enter withdrawal amount: ", "출금 금액을 입력하세요: ");
        if (!(cin >> withdrawAmount) || withdrawAmount <= 0) {  // 잘못된 입력에 대한 예외 처리
            showError("Invalid withdrawal amount.", "잘못된 출금 금액입니다.");
            return;
        }

        // ATM과 계좌 잔액 확인
        if (withdrawAmount > account->balance) {
            showError("Insufficient account balance.", "계좌 잔액이 부족합니다.");
            return;
        }

        // 거래 내역에 출금 기록 추가
        transactionHistory.push_back(translate("Withdrawal of ", "출금: ") + to_string(withdrawAmount) + translate(" KRW", "원"));
        account->balance -= withdrawAmount;  // 계좌 잔액 감소
        cout << translate("Withdrawal successful!", "출금 성공!") << endl;
    }

    void transfer(Account* sourceAccount, Bank* destinationBank, const string& destinationAccountNumber, int transferAmount) {
        if (transferAmount <= 0) {
            showError("Invalid transfer amount.", "잘못된 이체 금액입니다.");
            return;
        }

        if (transferAmount > sourceAccount->balance) {
            showError("Insufficient account balance.", "계좌 잔액이 부족합니다.");
            return;
        }

        Account* destinationAccount = destinationBank->authenticate(destinationAccountNumber, "");  // 비밀번호 생략
        if (!destinationAccount) {
            showError("Destination account not found.", "목적 계좌를 찾을 수 없습니다.");
            return;
        }

        // 거래 내역에 이체 기록 추가
        transactionHistory.push_back(translate("Transfer of ", "이체: ") + to_string(transferAmount) + translate(" KRW to ", "원 -> ") + destinationAccountNumber);
        sourceAccount->balance -= transferAmount;
        destinationAccount->balance += transferAmount;

        cout << translate("Transfer successful!", "이체 성공!") << endl;
    }

    void startSession(Account* authenticatedAccount, Bank* cardBank) {
        int choice;
        while (true) {
            cout << translate("\nSelect Transaction:\n1. Deposit\n2. Withdraw\n3. Transfer\n4. Save Transaction History to File\n5. Exit\n",
                              "\n거래 선택:\n1. 입금\n2. 출금\n3. 이체\n4. 거래 내역 파일 저장\n5. 종료\n");
            cout << translate("-> ", "-> ");
            cin >> choice;

            if (choice == 5) break;

            switch (choice) {
                case 1:
                    deposit(authenticatedAccount);
                    break;
                case 2:
                    withdraw(authenticatedAccount);
                    break;
                case 3: {
                    string destAccount;
                    int amount;
                    cout << translate("Enter destination account number: ", "목적 계좌 번호를 입력하세요: ");
                    cin >> destAccount;
                    cout << translate("Enter transfer amount: ", "이체 금액을 입력하세요: ");
                    cin >> amount;
                    transfer(authenticatedAccount, cardBank, destAccount, amount);
                    break;
                }
                case 4:
                    saveTransactionHistoryToFile();
                    break;
                default:
                    showError("Invalid choice.", "잘못된 선택입니다.");
            }
        }
    }
};


int main() {
    Bank bank("Example Bank");
    Account userAccount("123456789012", "User", "pass", 10000);
    bank.addAccount(userAccount);

    ATM atm("Single", &bank, "123456", ENGLISH, 10, 10, 10, 10, "admin123");

    // 세션을 시작하여 거래를 시뮬레이션
    Account* authenticatedAccount = bank.authenticate("123456789012", "pass");
    if (authenticatedAccount) {
        atm.startSession(authenticatedAccount, &bank);
    } else {
        cout << "Authentication failed.\n";
    }

    return 0;
}
