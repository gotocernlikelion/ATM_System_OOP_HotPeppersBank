#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace std;

class ATM;

class User {
protected:
    string username;
    string password;

public:
    // 생성자
    User(const string& username, const string& password)
        : username(username), password(password) {
    }

    User() : username(""), password("") {}

    // Getter 메서드
    string getUserName() const { return username; }
    string getPassword() const { return password; }

    // Setter 메서드
    void setUserName(const string& username) { this->username = username; }
    void setPassword(const string& password) { this->password = password; }
};


class Transaction {
private:
    int transactionID;
    string cardNumber;
    string accountNumber;
    string transactionType;
    int amount;

    string additionalInfo; // 추가 정보 필드

public:
    static int transaction_counter; // 각 거래의 고유 ID (REQ2.4)
    Transaction(int id, const string& accountNb, const string& card, const string& type, int amt, const string& info = "")
        : transactionID(id), accountNumber(accountNb), cardNumber(card), transactionType(type), amount(amt), additionalInfo(info) {
    }

    // 소멸자
    virtual ~Transaction() {}

    // Getter 메서드
    int getTransactionID() const { return transactionID; }
    string getCardNumber() const { return cardNumber; }
    string getAccountNumber() const { return accountNumber; }
    string getTransactionType() const { return transactionType; }
    int getAmount() const { return amount; }
    string getAdditionalInfo() const { return additionalInfo; }
};

vector<Transaction*> allTransactions;
class DepositTransaction : public Transaction {
public:
    DepositTransaction(int id, const string& accountNb, const string& card, int amt)
        : Transaction(id, accountNb, card, "Deposit", amt, "Won") {
    }
    DepositTransaction(const string& accountNb, const string& card, int amt)
        : Transaction(0, accountNb, card, "입금", amt, "원") {
    }
};

class WithdrawTransaction : public Transaction {
public:
    WithdrawTransaction(int id, const string& accountNb, const string& card, int amt)
        : Transaction(id, accountNb, card, "Withdraw", amt, "Won") {
    }
    WithdrawTransaction(const string& accountNb, const string& card, int amt)
        : Transaction(0, accountNb, card, "출금", amt, "원") {
    }
};

class TransferTransaction : public Transaction {
private:
    string targetAccountNumber;

public:
    TransferTransaction(int id, const string& accountNb, const string& card, int amt, const string& target_account)
        : Transaction(id, accountNb, card, "Transfer", amt, "Won, target account: " + target_account) {
    }
    TransferTransaction(const string& accountNb, const string& card, int amt, const string& target_account)
        : Transaction(0, accountNb, card, "계좌이체", amt, "원 도착 계좌: " + target_account) {
    }

    string getTargetAccountNumber() const { return targetAccountNumber; }
};


class Account : public User { // User 클래스 상속
private:
    string bankName;
    string cardNumber;
    string AccountNumber;
    int balance;
    vector<Transaction> transaction;

public:
    Account(const string& bank_name, const string& username, const string& card_number,
        const string& account_number, const string& password, int initial_balance)
        : User(username, password), bankName(bank_name), cardNumber(card_number),
        AccountNumber(account_number), balance(initial_balance) {
    }

    Account() : User(), bankName(""), cardNumber(""), AccountNumber(""), balance(0) {}

    string getcardNumber() const { return cardNumber; }
    string getAccountNumber() const { return AccountNumber; }
    int getBalance() const { return balance; }
    string getBankName() const { return bankName; }

    void setBalance(int money) { this->balance = money; }
};

class Bank {
private:
    string name;
    unordered_map<string, Account> accounts;

public:
    Bank(const string& bankName) : name(bankName) {}
    string getName() const { return name; }

    void addAccount(const Account& account) {
        accounts[account.getcardNumber()] = account;
    }

    Account* authenticate(const string& cardNumber, const string& password) {
        if (accounts.count(cardNumber) && accounts[cardNumber].getPassword() == password) {
            return &accounts[cardNumber];
        }
        //cout << "Wrong  Password." << endl;
        return nullptr;
    }

    // 비밀번호 없이 카드 번호만으로 찾는 메서드 추가
    Account* findAccountByCardNumber(const string& cardNumber) {
        if (accounts.count(cardNumber)) {
            return &accounts[cardNumber];
        }
        return nullptr;
    }

    string getCardNumByAccNum(const string& accountNumber) const {
        for (const auto& entry : accounts) {
            const Account& account = entry.second;
            if (account.getAccountNumber() == accountNumber) {
                return account.getcardNumber();
            }
        }
        return "0"; // 계좌 번호를 찾지 못한 경우 빈 문자열 반환
    }

    Account* findAccountByAccountNumber(const string& AccountNumber) {

        if (accounts.count(AccountNumber)) {
            return &accounts[AccountNumber];
        }
        return nullptr;
    }

    void displayAccounts() const {
        for (const auto& entry : accounts) {
            const Account& account = entry.second;
            cout << "Account [Bank: " << name
                << ", Card Number: " << account.getcardNumber()
                << ", Username: " << account.getUserName()
                << "] balance: " << account.getBalance() << "Won\n";
        }
    }
};

// 2024/11/21 고침
Bank* findBankByName(const string& bankName, const vector<Bank*>& allBanks) {
    for (Bank* bank : allBanks) {
        if (bank->getName() == bankName) {
            return bank;
        }
    }
    return nullptr; // 은행을 찾지 못한 경우 nullptr 반환
}


void displaySnapshot(const vector<Bank>& banks, const vector<ATM>& atms);

class ATM {
private:
    string serialNumber; // ATM 시리얼 번호 (REQ1.1)
    string atmType;      // "Single" or "Multi" (REQ1.2)
    string language; // ATM 언어 설정: English/Bilingual (REQ1.3) //uni Bi
    Bank* primaryBank;   // Primary bank associated with the ATM (REQ1.2)
    int cash1000, cash5000, cash10000, cash50000;
    string adminCardNumber = "0000"; //adm card면 접근(REQ1.9)
    bool session_active = false;              // 세션 상태 표시 (REQ2.1, REQ2.2)
    vector<Transaction*> session_transactions; // 세션 내 거래


public:
    ATM(string type, Bank* bank, string serial, string lang, int c1000, int c5000, int c10000, int c50000)
        : atmType(type), primaryBank(bank), serialNumber(serial), language(lang),
        cash1000(c1000), cash5000(c5000), cash10000(c10000), cash50000(c50000) {
    }
    ~ATM() {
        for (Transaction* transaction : session_transactions) {
            delete transaction;
        }
    }
    string getSerialNumber() const {
        return serialNumber;
    }

    bool authenticateUser(const vector<Bank*>& allBanks, const string& cardNumber, string& password, int language_signal) {
        int attemptCount = 0;
        while (attemptCount < 3) {
            if (atmType == "Single") {
                Account* account = primaryBank->authenticate(cardNumber, password);
                if (account) {
                    if (language_signal == 1) {
                        cout << "Access granted for Single-Bank ATM. You are authorized to access " << primaryBank->getName() << " Bank." << endl;
                    }
                    else {
                        cout << "단일 은행 ATM에 대한 접근 권한이 부여되었습니다. " << primaryBank->getName() << " 은행에 접근할 수 있습니다." << endl;
                    }
                    return true;
                }
                else if (primaryBank->findAccountByCardNumber(cardNumber)) {
                    // primary bank에 계좌는 있지만 비밀번호가 틀린 경우
                    if (language_signal == 1) {
                        cout << "Incorrect password. Please try again (" << 3 - attemptCount - 1 << " attempts left).\n";
                    }
                    else {
                        cout << "비밀번호가 틀렸습니다. 다시 시도하십시오. (" << 3 - attemptCount - 1 << "회 남음)\n";
                    }
                }
                else {
                    if (language_signal == 1) {
                        cout << "Invalid card. Please note that this ATM is associated with " << primaryBank->getName() << " Bank." << endl;
                    }
                    else {
                        cout << "유효하지 않은 카드입니다. 이 ATM은 " << primaryBank->getName() << " 은행과 연결되어 있음을 참고하십시오." << endl;
                    }
                    return false;
                }
            }
            else if (atmType == "Multi") {
                for (Bank* bank : allBanks) {
                    Account* account = bank->authenticate(cardNumber, password);
                    if (account) {
                        if (language_signal == 1) {
                            cout << "Access granted for Multi-Bank ATM. You are authorized to access " << bank->getName() << " Bank." << endl;
                        }
                        else {
                            cout << "다중 은행 ATM에 대한 접근 권한이 부여되었습니다. " << bank->getName() << " 은행에 접근할 수 있습니다." << endl;
                        }
                        return true;
                    }
                }
                if (language_signal == 1) {
                    cout << "Authorization failed. Please try again (" << 3 - attemptCount - 1 << " attempts remaining).\n";
                }
                else {
                    cout << "인증 실패. 다시 시도하십시오. (" << 3 - attemptCount - 1 << "회 남음)\n";
                }
            }

            attemptCount++;
            if (attemptCount < 3) {
                cout << "Please re-enter your password: ";
                cin >> password;
            }
        }
        if (language_signal == 1) {
            cout << "Maximum attempts reached. Session terminated for security.\n";
        }
        else {
            cout << "최대 시도 횟수에 도달했습니다. 보안을 위해 세션이 종료되었습니다.\n";
        }
        return false;
    }


    void displayRemainingCash() const {
        cout << "ATM [SerialNum: " << serialNumber << "] remaining cash: {"
            << "KRW 50000 : " << cash50000 << ", "
            << "KRW 10000 : " << cash10000 << ", "
            << "KRW 5000 : " << cash5000 << ", "
            << "KRW 1000 : " << cash1000 << "}\n";
    }

    void deposit(Account* account, Bank* userBank, int language_signal) {
        int depositType;
        if (language_signal == 1) {
            cout << "\n<< Choose deposit type >>\n1. Cash deposit\n2. Check deposit\n3. Cancel (press any key)\n-> ";
        }
        else {
            cout << "\n<< 입금 유형 선택 >>\n1. 현금 입금\n2. 수표 입금\n3. 취소 (아무 키나 누르세요)\n-> ";
        }
        cin >> depositType;

        if (depositType != 1 && depositType != 2) {
            if (language_signal == 1) {
                cout << "Deposit has been canceled.\n";
            }
            else {
                cout << "입금이 취소되었습니다.\n";
            }
            return;
        }

        bool isCashDeposit = (depositType == 1);
        bool isCheckDeposit = (depositType == 2);

        bool requiresFee = (primaryBank->getName() != userBank->getName());
        int fee = requiresFee ? 2000 : 1000;



        if (language_signal == 1) {
            cout << "The deposit fee is KRW " << fee << ".\nWould you like to proceed?\n1. Yes\n2. Cancel (press any key)\n-> ";
        }
        else {
            cout << "입금 수수료는 " << fee << "원입니다. 수수료를 지불하시겠습니까?\n1. 예\n2. 취소 (아무 키나 누르세요)\n-> ";
        }
        int confirm;
        cin >> confirm;

        if (confirm != 1) {
            if (language_signal == 1) {
                cout << "Deposit has been canceled.\n";
            }
            else {
                cout << "입금이 취소되었습니다.\n";
            }
            return;
        }

        // 수수료 반영
        if (fee == 1000) {
            cash1000++; // 수수료 1000원 지폐 1장 추가
            if (language_signal == 1) {
                cout << "A fee of KRW 1,000 has been accepted.\n";
            }
            else {
                cout << "1,000원의 수수료가 처리되었습니다.\n";
            }
        }
        else if (fee == 2000) {
            cash1000 += 2; // 수수료 2000원 지폐 2장 추가
            if (language_signal == 1) {
                cout << "A fee of KRW 2,000 has been accepted.\n";
            }
            else {
                cout << "2,000원의 수수료가 처리되었습니다.\n";
            }
        }

        if (isCashDeposit) {
            // 입금할 금액 입력
            int count1000 = 0, count5000 = 0, count10000 = 0, count50000 = 0;
            int totalBills = 0;

            // 입력 함수 생성
            auto getPositiveInteger = [&](const string& prompt) -> int {
                int count;
                while (true) {
                    cout << prompt;
                    cin >> count;

                    if (cin.fail() || count < 0) { // 유효하지 않은 입력 처리
                        cin.clear(); // 에러 플래그 초기화
                        cin.ignore(1000, '\n'); // 입력 버퍼 비우기
                        if (language_signal == 1) {
                            cout << "Invalid input. Please enter a non-negative integer.\n";
                        }
                        else {
                            cout << "잘못된 입력입니다. 양의 정수를 입력하세요.\n";
                        }
                    }
                    else {
                        return count; // 유효한 입력 반환
                    }
                }
                };

            // 천 원권
            if (language_signal == 1) {
                count1000 = getPositiveInteger("Enter the number of KRW 1,000 bills you want to deposit: ");
            }
            else {
                count1000 = getPositiveInteger("천 원권 몇 장을 입금하시겠습니까? ");
            }
            totalBills += count1000;

            // 오천 원권
            if (language_signal == 1) {
                count5000 = getPositiveInteger("Enter the number of KRW 5,000 bills you want to deposit: ");
            }
            else {
                count5000 = getPositiveInteger("오천 원권 몇 장을 입금하시겠습니까? ");
            }
            totalBills += count5000;

            // 만원권
            if (language_signal == 1) {
                count10000 = getPositiveInteger("Enter the number of KRW 10,000 bills you want to deposit: ");
            }
            else {
                count10000 = getPositiveInteger("만원권 몇 장을 입금하시겠습니까? ");
            }
            totalBills += count10000;

            // 오만 원권
            if (language_signal == 1) {
                count50000 = getPositiveInteger("Enter the number of KRW 50,000 bills you want to deposit: ");
            }
            else {
                count50000 = getPositiveInteger("오만 원권 몇 장을 입금하시겠습니까? ");
            }
            totalBills += count50000;

            // 총 합계 출력
            if (language_signal == 1) {
                cout << "Total number of bills deposited: " << totalBills << "\n";
            }
            else {
                cout << "총 입금한 지폐 수: " << totalBills << "\n";
            }


            // 지폐 수량 제한 확인
            const int limit = 50;
            if (totalBills > limit) {
                if (language_signal == 1) {
                    cout << "You cannot deposit more than " << limit << " bills at a time.\n!!Session terminated!!\n";
                }
                else {
                    cout << limit << "장을 초과하여 입금할 수 없습니다.\n!!세션 종료!!\n";
                }
                return;
            }

            // 총 입금 금액 계산
            int depositAmount = count1000 * 1000 + count5000 * 5000 + count10000 * 10000 + count50000 * 50000;

            if (depositAmount == 0) {
                if (language_signal == 1) {
                    cout << "Deposit amount cannot be zero. The fee will be refunded.\n";
                }
                else {
                    cout << "입금 금액이 0원입니다. 수수료가 환불됩니다.\n";
                }
                cash1000 -= (fee / 1000); // 수수료 환불
                if (language_signal == 1) {
                    cout << "The fee of KRW " << fee << " has been refunded.\n";
                }
                else {
                    cout << fee << "원이 환불되었습니다.\n";
                }
                return;
            }

            if (language_signal == 1) {
                cout << "The total deposit amount is KRW " << depositAmount << ".\nWould you like to proceed?\n1. Yes\n2. Cancel (press any key)\n-> ";
            }
            else {
                cout << "입금 금액은 " << depositAmount << "원입니다.\n계속 진행하시겠습니까?\n1. 예\n2. 취소 (아무 키나 누르세요)\n-> ";
            }
            cin >> confirm;
            if (confirm != 1) {
                if (language_signal == 1) {
                    cout << "Deposit has been canceled.\n";
                }
                else {
                    cout << "입금이 취소되었습니다.\n";
                }

                return;
            }

            // 계좌 잔액 반영
            account->setBalance(account->getBalance() + depositAmount);
            if (language_signal == 1) {
                cout << "Deposit successful! Your new balance is KRW " << account->getBalance() << ".\n";
                addTransaction(-1, account->getAccountNumber(), account->getcardNumber(), "Deposit", depositAmount, "");
            }
            else {
                cout << "입금이 완료되었습니다!\n계좌 잔액: " << account->getBalance() << "원\n";
                addTransaction(0, account->getAccountNumber(), account->getcardNumber(), "Deposit", depositAmount, "");
            }

            // ATM 현금 보유량 반영
            cash1000 += count1000;
            cash5000 += count5000;
            cash10000 += count10000;
            cash50000 += count50000;

            addTransaction(Transaction::transaction_counter++, account->getAccountNumber(), account->getcardNumber(), "Deposit", depositAmount, ""); //11.18 21:36
        }

        else if (isCheckDeposit) {
            int paperNum = 0;
            double CheckValue = 0;
            int depositAmount = 0;

            // 수표 장 수 입력
            if (language_signal == 1) {
                cout << "Enter the number of checks you want to deposit (Maximum 30): ";
            }
            else {
                cout << "몇 장의 수표를 입금하시겠습니까? (최대 30장)" << endl;
            }

            cin >> paperNum;

            if (paperNum > 30) {
                if (language_signal == 1) {
                    cout << "You cannot deposit more than 30 checks at a time.\n";
                }
                else {
                    cout << "30장을 초과하여 입금할 수 없습니다." << endl;
                }
                return;
            }
            int finalCheck;

            // 각 수표 금액 입력
            for (int i = 0; i < paperNum; i++) {
                while (true) {
                    if (language_signal == 1) {
                        cout << "Check #" << (i + 1) << ": Enter the check amount (Minimum: KRW 100,000): ";
                    }
                    else {
                        cout << "수표 #" << (i + 1) << ": 수표 금액을 입력하세요 (최소 100,000원 이상)" << endl;
                    }

                    cin >> CheckValue;

                    if (cin.fail() || CheckValue < 0) { // 입력 검증: 음수 및 잘못된 입력 처리
                        cin.clear(); // cin 상태 초기화
                        // cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 잘못된 입력 버림
                        if (language_signal == 1) {
                            cout << "Invalid input. Please enter a positive integer amount of at least KRW 100,000.\n";
                        }
                        else {
                            cout << "잘못된 입력입니다. 양의 정수로 100,000원 이상의 금액을 입력하세요." << endl;
                        }
                    }
                    else if (CheckValue >= 100000) {
                        if (language_signal == 1) {// 11.24 여기 바꿈
                            cout << "The Check amount is KRW " << CheckValue << ".\nWould you like to proceed?\n1. Yes\n2. Cancel (press any key)\n->";
                            cin >> finalCheck;
                            if (finalCheck != 1) {
                                return;
                            }

                        }
                        else {
                            cout << "수표 금액은 " << CheckValue << "원입니다.\n계속 진행하시겠습니까?\n1. 예\n2. 취소 (아무 키나 누르세요)\n-> ";
                            cin >> finalCheck;
                            if (finalCheck != 1) {
                                return;
                            }
                        }
                        depositAmount += CheckValue; // 유효한 수표 금액 누적
                        break;
                    }
                    else {
                        if (language_signal == 1) {
                            cout << "Invalid amount. Please enter an amount of at least KRW 100,000.\n";
                        }
                        else {
                            cout << "잘못된 금액입니다. 100,000원 이상의 금액을 입력하세요." << endl;
                        }
                    }
                }
            }

            // 계좌 잔액 업데이트 및 입금 성공 메시지 출력
            account->setBalance(account->getBalance() + depositAmount);
            if (language_signal == 1) {
                cout << "Deposit successful! Your new balance is KRW " << account->getBalance() << ".\n";
                addTransaction(-1, account->getAccountNumber(), account->getcardNumber(), "Deposit", depositAmount, "");
            }
            else {
                cout << "입금이 완료되었습니다!\n계좌 잔액: " << account->getBalance() << "원\n";
                addTransaction(0, account->getAccountNumber(), account->getcardNumber(), "Deposit", depositAmount, "");
            }

            // 거래 기록 추가
            addTransaction(Transaction::transaction_counter++, account->getAccountNumber(), account->getcardNumber(), "Deposit", depositAmount, "");
        }

    }



    void withdraw(Account* account, Bank* userBank, int language_signal) {
        const int maxWithdrawAmount = 500000;    // REQ5.7: 출금 한도
        const int maxWithdrawalsPerSession = 3;  // REQ5.6: 세션당 최대 출금 횟수
        int withdrawalsThisSession = 0;          // 세션당 출금 횟수를 카운트할 변수

        while (withdrawalsThisSession < maxWithdrawalsPerSession) {
            if (language_signal == 1) {
                cout << "\nWould you like to proceed with a withdrawal?\n1. Yes\n2. No (End withdrawal session)\n-> ";
            }
            else {
                cout << "\n출금 계속 진행하시겠습니까? \n1. 네\n2. 아니 (출금 세션을 종료)\n-> ";
            }

            int proceed;
            cin >> proceed;

            if (proceed != 1) {
                if (language_signal == 1) {
                    cout << "Withdrawal session has ended.\n";
                }
                else {
                    cout << "출금 세션을 종료합니다.\n";
                }
                return;
            }

            int withdrawalAmount = 0;
            if (language_signal == 1) {
                cout << "Enter the amount to withdraw: ";
            }
            else {
                cout << "출금할 금액을 입력하시오: ";
            }
            cin >> withdrawalAmount;

            // 출금 한도 확인
            if (withdrawalAmount > maxWithdrawAmount) {
                if (language_signal == 1) {
                    cout << "The maximum withdrawal amount per transaction is KRW " << maxWithdrawAmount << ".\n";
                }
                else {
                    cout << "한 번에 출금할 수 있는 최대 금액은 KRW " << maxWithdrawAmount << "원입니다.\n";
                }
                return;
            }

            // 수수료 계산
            bool requiresFee = (primaryBank->getName() != userBank->getName());
            int fee = requiresFee ? 2000 : 1000;

            // 계좌 잔액 확인
            if (withdrawalAmount + fee > account->getBalance()) {
                if (language_signal == 1) {
                    cout << "Your account balance is insufficient to complete this withdrawal.\n";
                }
                else {
                    cout << "계좌 잔액이 부족하여 출금할 수 없습니다. \n";
                }
                return;
            }

            // 지폐 분배 로직
            int remainingAmount = withdrawalAmount;
            int withdraw50000 = 0, withdraw10000 = 0, withdraw5000 = 0, withdraw1000 = 0;

            if (remainingAmount >= 50000 && cash50000 > 0) {
                withdraw50000 = std::min(remainingAmount / 50000, cash50000);
                remainingAmount -= withdraw50000 * 50000;
            }
            if (remainingAmount >= 10000 && cash10000 > 0) {
                withdraw10000 = std::min(remainingAmount / 10000, cash10000);
                remainingAmount -= withdraw10000 * 10000;
            }
            if (remainingAmount >= 5000 && cash5000 > 0) {
                withdraw5000 = std::min(remainingAmount / 5000, cash5000);
                remainingAmount -= withdraw5000 * 5000;
            }
            if (remainingAmount >= 1000 && cash1000 > 0) {
                withdraw1000 = std::min(remainingAmount / 1000, cash1000);
                remainingAmount -= withdraw1000 * 1000;
            }

            // 남은 금액이 0이 아니라면 ATM 현금 부족
            if (remainingAmount > 0) {
                if (language_signal == 1) {
                    cout << "The ATM does not have enough cash to fulfill your withdrawal.\n";
                }
                else {
                    cout << "ATM에 출금할 현금이 부족합니다.\n";
                }
                return;
            }

            // 사용자 최종 확인 메시지
            if (language_signal == 1) {
                cout << "The withdrawal amount is KRW " << withdrawalAmount
                    << " with a fee of KRW " << fee << ".\n";
                cout << "50000: " << withdraw50000 << ", 10000: " << withdraw10000
                    << ", 5000: " << withdraw5000 << ", 1000: " << withdraw1000 << "\n";
                cout << "Would you like to confirm the withdrawal?\n1. Confirm\n2. Cancel\n-> ";
            }
            else {
                cout << "출금 금액은 " << withdrawalAmount << "원이며, 수수료는 "
                    << fee << "원입니다.\n";
                cout << "50000원권: " << withdraw50000 << "장, 10000원권: " << withdraw10000
                    << "장, 5000원권: " << withdraw5000 << "장, 1000원권: " << withdraw1000 << "장\n";
                cout << "출금을 확인하시겠습니까?\n1. 확인\n2. 취소\n-> ";
            }

            int confirm;
            cin >> confirm;
            if (confirm != 1) {
                if (language_signal == 1) {
                    cout << "Withdrawal canceled. Returning to session.\n";
                }
                else {
                    cout << "출금이 취소되었습니다. 세션으로 돌아갑니다.\n";
                }
                return;
            }

            // 계좌 잔액 차감 및 ATM 잔액 업데이트
            account->setBalance(account->getBalance() - (withdrawalAmount + fee));
            cash50000 -= withdraw50000;
            cash10000 -= withdraw10000;
            cash5000 -= withdraw5000;
            cash1000 -= withdraw1000;

            // 성공 메시지
            if (language_signal == 1) {
                cout << "Successfully withdrawn! Remaining balance: KRW "
                    << account->getBalance() << "\n";
                addTransaction(Transaction::transaction_counter++, account->getAccountNumber(), account->getcardNumber(), "Withdraw", withdrawalAmount, ""); //11.18 21:49
                addTransaction(-1, account->getAccountNumber(), account->getcardNumber(), "Withdraw", withdrawalAmount, "");

            }
            else {
                cout << "출금 성공! 계좌 잔액: KRW " << account->getBalance() << "\n";
                addTransaction(Transaction::transaction_counter++, account->getAccountNumber(), account->getcardNumber(), "Withdraw", withdrawalAmount, ""); //11.18 21:49
                addTransaction(0, account->getAccountNumber(), account->getcardNumber(), "Withdraw", withdrawalAmount, "");
            }
            if (cash1000 + cash5000 + cash10000 + cash50000 == 0) {
                return;
            }

            // 출금 세션 종료 조건 체크
            withdrawalsThisSession++;
            if (withdrawalsThisSession >= maxWithdrawalsPerSession) {
                if (language_signal == 1) {
                    cout << "The maximum number of withdrawals per session is "
                        << maxWithdrawalsPerSession << ". Please restart another session for more withdrawals.\n";
                }
                else {
                    cout << "세션당 출금 최대 횟수: " << maxWithdrawalsPerSession
                        << ". 추가 출금을 하려면 세션을 새로 시작하십시오.\n";
                }
                break;
            }
        }
    }





    void transfer(Account* sourceAccount, Bank* sourceBank, const vector<Bank*>& allBanks, int language_signal) {
        int transferType;
        if (language_signal == 1) {
            cout << "\n<< Choose transfer type >>\n1. Cash transfer\n2. Account transfer\n3. Cancel (press any key)\n-> ";
        }
        else {
            cout << "\n<< 송금 유형 선택 >>\n1. 현금 송금\n2. 계좌 송금\n3. 취소 (아무 키나 입력)\n-> ";
        }
        cin >> transferType;

        if (transferType == 3) {
            if (language_signal == 1) {
                cout << "Transfer has been canceled.\n";
            }
            else {
                cout << "송금이 취소되었습니다.\n";
            }
            return;
        }

        // REQ6.2: 목적 계좌 정보 입력
        string destinationAccountNumber, destinationCardNumber, destinationBankName;
        if (language_signal == 1) {
            cout << "Enter the name of the destination bank: "; //은행 이름 입력 안받고 계좌번호만으로도 알 수 있게 해야할려나
        }
        else {
            cout << "송금 받을 은행 이름을 입력하세요: "; //은행 이름 입력 안받고 계좌번호만으로도 알 수 있게 해야할려나
        }
        cin >> destinationBankName;


        Bank* destinationBank = nullptr;
        for (Bank* bank : allBanks) {
            if (bank->getName() == destinationBankName) {
                destinationBank = bank;
                break;
            }
        }

        if (!destinationBank) {
            if (language_signal == 1) {
                cout << "The destination bank could not be found. Transfer canceled.\n";
            }
            else {
                cout << "송금 받을 은행을 찾을 수 없습니다. 송금을 취소합니다.\n";
            }
            return;
        }

        //계좌 번호 입력
        bool isAcc{ false };
        while (!isAcc) {
            do {
                if (language_signal == 1) {
                    cout << "Input destination account number (if you want to cancel, press 0) :";
                    cin >> destinationAccountNumber;
                    if (destinationAccountNumber == "0") {
                        return;
                    }
                }
                else {
                    cout << "도착 계좌번호를 입력하세요 (취소를 원하면 0을 치세요.) :";
                    cin >> destinationAccountNumber;
                    if (destinationAccountNumber == "0") {
                        return;
                    }
                }
                if (destinationAccountNumber == sourceAccount->getAccountNumber()) {
                    if (language_signal == 1) {
                        cout << "source account and destionation account cannot be the same.\n";
                    }
                    else {
                        cout << "송금계좌와 도착계좌가 같은 수는 없습니다.\n";
                    }
                }
            } while (destinationAccountNumber == sourceAccount->getAccountNumber());

            if (destinationBank->getCardNumByAccNum(destinationAccountNumber) == "0") {
                if (language_signal == 1) {
                    cout << "There is no such account number in the Bank." << endl;
                }
                else {
                    cout << "해당 은행에 없는 계좌입니다." << endl;
                }
            }
            else {
                isAcc = true;
            }
        };


        destinationCardNumber = destinationBank->getCardNumByAccNum(destinationAccountNumber);


        int transferFee = 0;

        // Transfer 수수료 설정 (여기 밑에 if else문 2024/11/20 고침. transfer account 계좌 수수료)
        if (sourceBank == primaryBank && destinationBank == primaryBank) {
            transferFee = 2000;
        }
        else if (sourceBank == primaryBank || destinationBank == primaryBank) {
            transferFee = 3000;
        }
        else {
            transferFee = 4000;
        }

        int transferAmount = 0;

        if (transferType == 1) { // Cash transfer

            int cash1000, cash5000, cash10000, cash50000;
            int command;
            int insertedAmount;
            transferFee = 1000;
            // Prompt user to confirm inserting the transfer fee first
            if (language_signal == 1) {
                cout << "Cash transfer fee costs KRW " << transferFee << ". Please insert the same amount of cash to the ATM.\n";
                cout << "1. Confirm\n2. Cancel\n-> ";
            }
            else {
                cout << "송금 수수료는 KRW " << transferFee << " 원입니다. 수수료만큼의 현금을 투입해주세요. \n";
                cout << "1. 확인\n2. 취소\n-> ";
            }
            cin >> command;

            if (command == 2) {
                if (language_signal == 1) {
                    cout << "Transaction has been canceled.\n";
                }
                else {
                    cout << "송금이 취소되었습니다.\n";
                }
                return; // End the process if user cancels
            }

            if (language_signal == 1) {
                cout << "Please insert the cash to complete the transfer.\n";
            }
            else {
                cout << "송금을 완료하려면 송금할 금액을 입력하십시오.\n";
            }

            do {
                if (language_signal == 1) {
                    cout << "Enter the number of KRW 1,000 bills: ";
                }
                else {
                    cout << "천 원권 개수: ";
                }
                cin >> cash1000;

                if (language_signal == 1) {
                    cout << "Enter the number of KRW 5,000 bills: ";
                }
                else {
                    cout << "오천 원권 개수: ";
                }
                cin >> cash5000;

                if (language_signal == 1) {
                    cout << "Enter the number of KRW 10,000 bills: ";
                }
                else {
                    cout << "만원권 개수: ";
                }
                cin >> cash10000;

                if (language_signal == 1) {
                    cout << "Enter the number of KRW 50,000 bills: ";
                }
                else {
                    cout << "오만 원권 개수: ";
                }
                cin >> cash50000;

                insertedAmount = cash1000 * 1000 + cash5000 * 5000 + cash10000 * 10000 + cash50000 * 50000;

                if (language_signal == 1) {
                    cout << "The total amount entered is: " << insertedAmount << " KRW.\n";
                    cout << "1. Confirm\n2. Re-enter cash\n-> ";
                }
                else {
                    cout << "총 입력 금액: " << insertedAmount << "원.\n";
                    cout << "1. 확인\n2. 다시 입력\n-> ";
                }
                cin >> command;


            } while (command != 1);

            // ATM cash update (REQ6.6)
            this->cash1000 += (cash1000 + 1);
            this->cash5000 += cash5000;
            this->cash10000 += cash10000;
            this->cash50000 += cash50000;

            // Deduct transfer fee from inserted cash
            transferAmount = insertedAmount;

            
            if (language_signal == 1) {
                cout << "Successfully transferred " << transferAmount << " KRW.\n";
                addTransaction(-1, sourceAccount->getAccountNumber(), sourceAccount->getcardNumber(), "Transfer", transferAmount, destinationAccountNumber);
            }
            else {
                cout << transferAmount << " 원이 성공적으로 송금되었습니다. \n";
                addTransaction(0, sourceAccount->getAccountNumber(), sourceAccount->getcardNumber(), "Transfer", transferAmount, destinationAccountNumber);
            }
            addTransaction(Transaction::transaction_counter++, sourceAccount->getAccountNumber(), sourceAccount->getcardNumber(), "Transfer", transferAmount, destinationAccountNumber);
        }

        else if (transferType == 2) { // Account transfer


            if (language_signal == 1) {
                cout << "Enter the amount you wish to transfer: ";
            }
            else {
                cout << "송금할 금액을 입력하세요: ";
            }
            cin >> transferAmount;

            // Account transfer는 원천 계좌에서 금액과 수수료가 차감됨
            if (sourceAccount->getBalance() < transferAmount + transferFee) {
                if (language_signal == 1) {
                    cout << "Insufficient funds in your account. Transfer canceled.\n";
                }
                else {
                    cout << "계좌에 잔액이 부족합니다. 송금이 취소됩니다.\n";
                }
                return;
            }

            // 사용자에게 수수료 안내
            if (language_signal == 1) {
                cout << "Your transfer fee is KRW " << transferFee << ". \nWould you like to proceed?\n1. Yes\n2. Cancel (press any key)\n-> ";
                int confirm;
                cin >> confirm;
                if (confirm != 1) {
                    cout << "Transfer canceled.\n";
                    return;
                }
            }
            else {
                cout << "수수료 : KRW " << transferFee << ".송금 진행하시겠습니까?\n1. 네\n2. 아니요 (아무키나 입력)\n-> ";
                int confirm;
                cin >> confirm;
                if (confirm != 1) {
                    cout << "송금 취소.\n";
                    return;
                }
            }
            sourceAccount->setBalance(sourceAccount->getBalance() - transferAmount - transferFee); // REQ6.7: 원천 계좌 잔액 차감
        }

        // 목적 계좌에 금액 추가
        Account* destinationAccount = destinationBank->findAccountByCardNumber(destinationCardNumber);
        if (destinationAccount) {
            destinationAccount->setBalance(destinationAccount->getBalance() + transferAmount);
            if (language_signal == 1) {
                cout << "Successfully transferred " << transferAmount << " KRW to account " << destinationAccountNumber << ".\n";
            }
            else {
                cout << destinationAccountNumber << " 계좌에 " << transferAmount << "원이 성공적으로 송금되었습니다.\n";
            }
            if (transferType == 2) {
                if (language_signal == 1) {
                    cout << "There is KRW " << sourceAccount->getBalance() << " in your account.\n";
                    addTransaction(Transaction::transaction_counter++, sourceAccount->getAccountNumber(), sourceAccount->getcardNumber(), "Transfer", transferAmount, destinationAccountNumber); //11.18 21:52
                    addTransaction(-1, sourceAccount->getAccountNumber(), sourceAccount->getcardNumber(), "Transfer", transferAmount, destinationAccountNumber);
                }
                else {
                    cout << "당신 계좌에" << sourceAccount->getBalance() << "원이 남아있습니다. \n";
                    addTransaction(Transaction::transaction_counter++, sourceAccount->getAccountNumber(), sourceAccount->getcardNumber(), "Transfer", transferAmount, destinationAccountNumber); //11.18 21:52
                    addTransaction(0, sourceAccount->getAccountNumber(), sourceAccount->getcardNumber(), "Transfer", transferAmount, destinationAccountNumber);
                }
            }
        }
        else {
            if (language_signal == 1) {
                cout << "Destination account could not be found. Transfer canceled.\n";
            }
            else {
                cout << "도착 계좌가 조회되지 않습니다. 송금이 취소됩니다.\n";
            }
        }
    }




    void startSession(const vector<Bank*>& allBanks, const vector<Bank>& banks, const vector<ATM>& atms) {
        if (cash1000 == 0 && cash5000 == 0 && cash10000 == 0 && cash50000 == 0) {
            cout << "ATM has no cash available. Session terminated.\n";
            return;
        } //WH 11.22 23:26

        string cardNumber, password;
        int language_signal = 1;
        //L
        // Bi-lingual ATM에서 언어 선택
        if (language == "Bi") {
            cout << "\nSelect language / 언어를 선택하세요:\n1. English\n2. 한국어\n-> ";
            cin >> language_signal;
        }//L

        while (true) {
            if (language_signal == 1) {
                cout << "\nPlease Enter your Card number: ";
            }
            else {
                cout << "\n카드 번호를 입력해주세요: ";
            }//L
            cin >> cardNumber;

            if (cardNumber == adminCardNumber) {
                if (language_signal == 1) {
                    cout << "Admin session has been started.\n";
                }
                else {
                    cout << "관리자 세션이 시작되었습니다.\n";
                }//L
                return startAdminSession(allTransactions);
            }

            // Check if account exists in any bank
            bool accountExists = false;
            for (Bank* bank : allBanks) {
                if (bank->findAccountByCardNumber(cardNumber)) {
                    accountExists = true;
                    break;
                }
            }
            // 존재하지 않는 번호를 입력했을 때 다시 입력하라 맨이야 -JH- 
            if (!accountExists) {
                if (language_signal == 1) {
                    cout << "There's no account available with that number. Please try again.\n";
                }
                else {
                    cout << "해당 번호의 계좌가 존재하지 않습니다. 다시 시도하세요.\n";
                }
                continue;//L
            }
            break;
        }

        if (language_signal == 1) {
            cout << "Enter Password for your account : ";
        }
        else {
            cout << "비밀번호를 입력하세요: ";
        }//L

        cin >> password;

        if (authenticateUser(allBanks, cardNumber, password, language_signal)) {
            Account* authenticatedAccount = nullptr;
            Bank* cardBank = nullptr;

            // 계좌 인증 후 은행 찾기
            for (Bank* bank : allBanks) {
                authenticatedAccount = bank->authenticate(cardNumber, password);
                if (authenticatedAccount) {
                    cardBank = bank;
                    if (language_signal == 1) {
                        cout << "Authentication successful. Welcome, " << authenticatedAccount->getUserName() << "!\n";
                    }
                    else {
                        cout << "인증 성공. 환영합니다, " << authenticatedAccount->getUserName() << "!\n";
                    }
                    break;
                }
            }

            if (!authenticatedAccount) {
                if (language_signal == 1) {
                    cout << "Authentication has failed. Ending session.\n";
                }
                else {
                    cout << "인증에 실패했습니다. 세션을 종료합니다.\n";
                }
                return;
            }

            session_transactions.clear();
            while (true) {
                //11.29
                // ATM에 현금이 모두 소진된 경우 세션 종료
                if (cash1000 + cash5000 + cash10000 + cash50000 == 0) {
                    if (language_signal == 1) {
                        cout << "ATM has run out of cash. Ending session.\n";
                    }
                    else {
                        cout << "ATM에 현금이 모두 소진되었습니다. 세션을 종료합니다.\n";
                    }
                    return;
                }
                char choice;
                bool validInput = false;

                while (!validInput) {
                    if (language_signal == 1) {
                        cout << "\nSelect Transaction:\n1. Deposit\n2. Withdraw\n3. Transfer\n4. Exit\n:";
                    }
                    else {
                        cout << "\n거래를 선택하세요:\n1. 입금\n2. 출금\n3. 송금\n4. 종료\n:";
                    }

                    cin >> choice;

                    if (choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == '/') {
                        validInput = true; // 유효한 입력
                    }
                    else {
                        cin.clear(); // 에러 플래그 초기화
                        cin.ignore(1000, '\n'); // 버퍼 비우기
                        if (language_signal == 1) {
                            cout << "Invalid choice. Please enter a valid option (1-4 or '/').\n";
                        }
                        else {
                            cout << "잘못된 선택입니다. 유효한 옵션(1-4 또는 '/')을 입력하세요.\n";
                        }
                    }
                }

                if (choice == '4') break;

                // 거래 처리 예시 (입금, 출금, 송금에 대한 간단한 메시지)
                switch (choice) {
                case '1':
                    deposit(authenticatedAccount, cardBank, language_signal);
                    break;
                case '2':
                    withdraw(authenticatedAccount, cardBank, language_signal);
                    break;
                case '3':
                    transfer(authenticatedAccount, cardBank, allBanks, language_signal);
                    break;
                case '/':
                    displaySnapshot(banks, atms);
                }
            }


            if (!session_transactions.empty()) {
                if (language_signal == 1) {
                    cout << "\nTransaction Summary:\n";
                    for (const auto& transaction : session_transactions) {
                        cout << "Card Number: " << transaction->getCardNumber()
                            << ", Account Number: " << transaction->getAccountNumber()
                            << ", Type: " << transaction->getTransactionType()
                            << ", Amount: " << transaction->getAmount();
                        if (!transaction->getAdditionalInfo().empty()) {
                            cout << transaction->getAdditionalInfo();
                        }
                        cout << endl;
                    }
                }
                else {
                    cout << "\n거래 요약:\n";
                    for (const auto& transaction : session_transactions) {
                        cout << "카드 번호: " << transaction->getCardNumber()
                            << ", 계좌 번호: " << transaction->getAccountNumber()
                            << ", 거래 타입: " << transaction->getTransactionType()
                            << ", 거래량: " << transaction->getAmount();
                        if (!transaction->getAdditionalInfo().empty()) {
                            cout << transaction->getAdditionalInfo();
                        }
                        cout << endl;
                    }
                }
            }
            else {
                if (language_signal == 1) {
                    cout << "\nNo transactions completed in this session.\n";
                }
                else {
                    cout << "\n이번 세션에서 완료된 거래가 없습니다.\n";
                }
            }
        }
    }


    void startAdminSession(vector<Transaction*>& allTransactions) {
        string command;
        cout << "Admin session started. Displaying Transaction History.\n";
        cout << "Would you like to view the Transaction History? (Enter 'yes') :";
        cin >> command;
        if (command == "yes") {
            displayTransactionHistory(allTransactions);
        }
        else {
            return;
        }
    }

    //11/18 20:41
    void addTransaction(int id, const string& account, const string& card, const string& type, int amt, const string& info = "", vector<Transaction*>& allTransactions = ::allTransactions) {
        if (id == 0) {
            if (type == "Deposit") {
                session_transactions.push_back(new DepositTransaction(account, card, amt));
            }
            else if (type == "Withdraw") {
                session_transactions.push_back(new WithdrawTransaction(account, card, amt));
            }
            else if (type == "Transfer") {
                session_transactions.push_back(new TransferTransaction(account, card, amt, info));
            }
        }
        else if (id == -1) {
            if (type == "Deposit") {
                session_transactions.push_back(new DepositTransaction(0, account, card, amt));
            }
            else if (type == "Withdraw") {
                session_transactions.push_back(new WithdrawTransaction(0, account, card, amt));
            }
            else if (type == "Transfer") {
                session_transactions.push_back(new TransferTransaction(0, account, card, amt, info));
            }

        }
        else {
            if (type == "Deposit") {
                allTransactions.push_back(new DepositTransaction(id, account, card, amt));
            }
            else if (type == "Withdraw") {
                allTransactions.push_back(new WithdrawTransaction(id, account, card, amt));
            }
            else if (type == "Transfer") {
                allTransactions.push_back(new TransferTransaction(id, account, card, amt, info));
            }
        }

    }
    void displayTransactionHistory(const vector<Transaction*>& allTransactions) {
        cout << "\n===== Transaction History =====\n";

        // 모든 거래 내역 출력
        for (const auto& transaction : allTransactions) {
            cout << "ID: " << transaction->getTransactionID()
                << ", Card: " << transaction->getCardNumber()
                << ", Type: " << transaction->getTransactionType()
                << ", Amount: " << transaction->getAmount();
            if (!transaction->getAdditionalInfo().empty()) {
                cout << transaction->getAdditionalInfo();
            }
            cout << endl; // 개별 거래 출력
        }

        // 외부 파일에 거래 내역 저장
        ofstream outFile("transaction_history.txt");
        for (const auto& transaction : allTransactions) {
            outFile << "ID: " << transaction->getTransactionID()
                << ", Card: " << transaction->getCardNumber()
                << ", Type: " << transaction->getTransactionType()
                << ", Amount: " << transaction->getAmount();
            if (!transaction->getAdditionalInfo().empty()) {
                outFile << transaction->getAdditionalInfo();
            }
            outFile << endl; // 파일에 기록
        }
        outFile.close();

        cout << "Transaction history has been saved to transaction_history.txt text file.\n";
    }

};

void displaySnapshot(const vector<Bank>& banks, const vector<ATM>& atms) {
    cout << "\n\n======< 'Account/ATM Snapshot' >======\n";

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

int Transaction::transaction_counter = 1; // main() 함수 위에 한 번만 선언


int main() {
    // Bank Initializing Step
    cout << "======< 'Bank Initialization' >======\n";
    vector<Bank> banks;
    vector<Bank*> allBanks;
    string bankInput;
    vector<string> banknames;
    bool vaildbankname;
    banks.reserve(100);
    vector<Transaction*> allTransactions; // 전체 거래


    // 사용자로부터 은행 이름을 반복적으로 입력받아 초기화
    while (true) {

        cout << "Enter bank name for initialization(or type 'done' to finish): ";
        cin >> bankInput;
        vaildbankname = true;

        for (const string& bankname : banknames) {
            if (bankInput == bankname) {
                cout << "This bank name has already been taken. Please try to enter a different name.\n";
                vaildbankname = false;
                break;
            }
        }
        if (vaildbankname == false) {
            continue;
        }
        banknames.push_back(bankInput);


        if (bankInput == "done") {
            if (banks.empty()) { // Bank가 하나도 생성되지 않았을 때 프로그램 종료
                cout << "No banks has been created. Exiting program.\n";
                return 0;
            }
            else {
                cout << "Bank initialization has been completed. Proceeding to account Initializaiton...\n";
                break; // Bank가 최소 하나 이상 생성된 경우 다음 단계로 이동
            }
        }

        Bank newBank(bankInput); // 새로운 Bank 객체 생성
        banks.push_back(newBank); // banks 벡터에 추가
        allBanks.push_back(&banks.back()); // 포인터 벡터에도 추가
        cout << "[" << bankInput << "] bank has been created!\n";
    }


    // Account Creation Step
    cout << "\n======< 'Account Creation' >======\n";
    string createAccountInput;
    int accountCount = 1;
    vector<string> cardNumbers;
    vector<string> accountNumbers;
    cardNumbers.push_back("0000");

    do {
        cout << "\nWould you like to create Account " << accountCount++ << "? (yes or no) -> ";
        cin >> createAccountInput;

        while (createAccountInput != "yes" && createAccountInput != "no") {
            cout << "Invalid input. Please type 'yes' or 'no': ";
            cin >> createAccountInput;
        }

        if (createAccountInput == "yes") {
            string bankName, username, cardNumber, AccountNumber, password;
            int balance;
            // Bank 이름 입력
            bool bankFound = false;
            cout << "Bank Name: ";
            while (!bankFound) {
                cin >> bankName;
                for (Bank* bank : allBanks) { //Bank 있는거냐?
                    if (bank->getName() == bankName) {
                        bankFound = true;
                        break;
                    }
                }
                if (!bankFound) {
                    cout << "Bank not found. Please type again: ";
                }
            }

            cout << "User Name: ";
            cin >> username;


            while (true) {
                cout << "Card number (4 digits): ";
                cin >> cardNumber;

                bool validcardNumber = true;

                if (cardNumber.length() != 4) {
                    cout << "Invalid Card number. It must be exactly 4 digits.\n";
                    validcardNumber = false;
                }

                // 기존의 cardNumbers에 중복된 번호가 있는지 확인
                for (const string& name : cardNumbers) {
                    if (cardNumber == name) {
                        validcardNumber = false;
                        cout << "This Card number has already been taken. Please try to enter a different number.\n";
                        break;
                    }
                }

                if (validcardNumber) {
                    cardNumbers.push_back(cardNumber);
                    // cout << "Card number " << cardNumber << " has been successfully added.\n";
                    break;
                }
            }

            // Account  number 확인
            while (true) {
                cout << "Account Number (12 digits): ";
                cin >> AccountNumber;

                bool validAccountNumber = true;

                // 유효성 검사: AccountNumber는 정확히 12자리여야 함
                if (AccountNumber.length() != 12) {
                    cout << "Invalid Account number. It must be exactly 12 digits.\n";
                    validAccountNumber = false;
                }

                // 기존의 AccountNumbers에 중복된 번호가 있는지 확인
                for (const string& existingAccNumber : accountNumbers) {
                    if (AccountNumber == existingAccNumber) {
                        validAccountNumber = false;
                        cout << "This Account number has already been taken. Please try to enter a different number.\n";
                        break;
                    }
                }

                if (validAccountNumber) {
                    accountNumbers.push_back(AccountNumber);
                    // cout << "Account number " << AccountNumber << " has been successfully added.\n";
                    break;
                }
            }


            while (true) {
                try {
                    cout << "Balance(KRW): ";
                    cin >> balance;

                    if (cin.fail()) { // 잘못된 입력 처리
                        throw runtime_error("Invalid input. Please enter a positive integer.");
                    }

                    if (balance < 0) { // 음수 처리
                        throw runtime_error("Balance cannot be negative.");
                    }
                }
                catch (const runtime_error& e) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Error: " << e.what() << endl;
                    continue; // 다시 입력 받기
                }

                // 조건 검사: 양의 정수인지 확인
                if (balance >= 0) {
                    break; // 조건을 만족하면 반복 종료
                }
            }

            cout << "Password: ";
            cin >> password;

            // 은행 이름에 따라 계좌 추가
            //bool bankFound = false;
            for (Bank* bank : allBanks) {
                if (bank->getName() == bankName) {
                    bank->addAccount(Account(bankName, username, cardNumber, AccountNumber, password, balance));
                    bankFound = true;
                    break;
                }
            }
            if (!bankFound) {
                cout << "Bank not found. Account creation has been failed.\n";
            }
        }
    } while (createAccountInput == "yes");

    // ATM Creation Step
    cout << "\n======< 'ATM Creation Step' >======\n";
    vector<ATM> atms;
    string createATMInput;
    int atmCount = 1;
    do {
        cout << "\nWould you like to create ATM " << atmCount << "? (yes or no) -> ";
        cin >> createATMInput;

        while (createATMInput != "yes" && createATMInput != "no") {
            cout << "Invalid input. Please type 'yes' or 'no': ";
            cin >> createATMInput;
        }

        if (createATMInput == "yes") {
            string atmType, primaryBankName, serialNumber, language;
            int cash1000, cash5000, cash10000, cash50000;
            Bank* primaryBank = nullptr;

            cout << "Primary Bank Name: ";
            cin >> primaryBankName;

            primaryBank = findBankByName(primaryBankName, allBanks);

            // Loop to ensure unique 6-digit serial number
            bool isDuplicate{ false };
            do {
                bool serial_is_6digits{ false };

                while (!serial_is_6digits) { // 6 자리 넘지 않으면 while 을 빠져나갈수가 없으셈 ㅇㅇ. - JH- 
                    cout << "Serial Number(6-digit): ";
                    cin >> serialNumber;
                    // Check if serial number is exactly 6 digits
                    if (serialNumber.length() != 6) {
                        cout << "Invalid serial number. It must be exactly 6 digits.\n";
                        continue;
                        // Prompt for serial number again

                    }
                    else {
                        serial_is_6digits = true;
                    }
                }

                // Check for duplicate serialNumber
                /*isDuplicate = false;*/
                try {
                    for (const ATM& atm : atms) {
                        if (atm.getSerialNumber() == serialNumber) {
                            throw runtime_error("Duplicate serial number detected.");
                        }
                    }
                    isDuplicate = false;
                }
                catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                    isDuplicate = true;
                    continue; // 다시 입력
                }
            } while (isDuplicate);

            do {
                cout << "Type(Single or Multi): ";
                cin >> atmType;

                for (int i = 0; i < atmType.size(); i++) {
                    if ('A' <= atmType[i] && atmType[i] <= 'Z') {
                        atmType[i] += 32;
                    }
                }
                if (atmType != "single" && atmType != "multi") {
                    cout << "Incorrect ATM Type. Please enter within 'Single' or 'Multi'.\n";
                }
            } while (atmType != "single" && atmType != "multi");
            atmType[0] -= 32;

            do {
                cout << "Language(Uni or Bi): ";
                cin >> language;

                for (int i = 0; i < language.size(); i++) {
                    if ('A' <= language[i] && language[i] <= 'Z') {
                        language[i] += 32;
                    }
                }
                if (language != "uni" && language != "bi") {
                    cout << "Incorrect Language Type. Please enter within 'Uni' or 'Bi'.\n";
                }
            } while (language != "uni" && language != "bi");
            language[0] -= 32;



            cout << "Number of initial 1,000 Cash?: ";
            while (!(cin >> cash1000) || cash1000 < 0) {
                cin.clear(); // 에러 플래그 초기화
                cin.ignore(1000, '\n'); // 입력 버퍼 비우기
                cout << "Invalid input. Please enter a non-negative integer for 1,000 Cash: ";
            }

            cout << "Number of initial 5,000 Cash?: ";
            while (!(cin >> cash5000) || cash5000 < 0) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input. Please enter a non-negative integer r for 5,000 Cash: ";
            }

            cout << "Number of initial 10,000 Cash?: ";
            while (!(cin >> cash10000) || cash10000 < 0) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input. Please enter a non-negative integer  for 10,000 Cash: ";
            }

            cout << "Number of initial 50,000 Cash?: ";
            while (!(cin >> cash50000) || cash50000 < 0) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input. Please enter a non-negative integer  for 50,000 Cash: ";
            }


            // Find the primary bank
            if (atmType == "Single") {
                for (Bank* bank : allBanks) {
                    if (bank->getName() == primaryBankName) {
                        primaryBank = bank;
                        break;
                    }
                }
                if (!primaryBank) {
                    cout << "Primary bank has not been found. Skipping ATM creation.\n";
                    continue;
                }
            }

            // Create and add the ATM
            atms.emplace_back(atmType, primaryBank, serialNumber, language, cash1000, cash5000, cash10000, cash50000);
            cout << "ATM " << atmCount << " created successfully.\n";

            // Increment atmCount only on successful creation
            atmCount++;
        }
    } while (createATMInput == "yes");



    // Program execution loop to check for '/' input
    string command;
    while (true) {
        cout << "\nEnter '/' to view display snapshot, 'session' to start an ATM session, or 'exit' to quit program: ";
        cin >> command;

        if (command == "/") {
            displaySnapshot(banks, atms);
        }
        else if (command == "session") {
            if (atms.empty()) {
                cout << "No ATMs are available. Please create an ATM first.\n";
            }
            else {
                int atmIndex;
                cout << "Enter ATM index (1 to " << atms.size() << ") to start session: ";
                cin >> atmIndex;

                if (atmIndex >= 1 && atmIndex <= atms.size()) {
                    atms[atmIndex - 1].startSession(allBanks, banks, atms);
                }
                else {
                    cout << "Invalid ATM index.\n";
                }
            }
        }
        else if (command == "exit") {
            break;
        }
    }

    return 0;
}
