
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

class Transaction {
public:
    static int transaction_counter; // 각 거래의 고유 ID (REQ2.4)
    int transactionID;
    string cardNumber;
    string transactionType;
    int amount;
    string additionalInfo;

    Transaction(int id, const string& card, const string& type, int amt, const string& info = "")
        : transactionID(id), cardNumber(card), transactionType(type), amount(amt), additionalInfo(info) {
    }
};

int Transaction::transaction_counter = 1;

class Account { //(REQ1.6) (REQ1.7)
private:
    string bankName;
    string cardNumber;// cardNumber랑 동일
    string AccountNumber;
    string username;
    string password;
    int balance;

    vector<Transaction> transaction;
public:
    Account(const string& bank_name, const string& user_name, const string& card_number, const string& account_number, const string& password, int initial_balance) {
        this->bankName = bank_name;
        this->username = user_name;
        this->cardNumber = card_number;
        this->AccountNumber = account_number;
        this->password = password;
        this->balance = initial_balance;
    }
    Account() : bankName(""), cardNumber(""), username(""), password(""), balance(0) {}

    string getcardNumber() const {
        return cardNumber;
    }

    string getAccountNumber() const {
        return AccountNumber;
    }


    string getPassword() const {
        return password;
    }

    int getBalance() const {
        return balance;
    }

    string getUserName() const {
        return username;
    }

    void setBalance(int money) {
        this->balance = money;
    }


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
        return ""; // 계좌 번호를 찾지 못한 경우 빈 문자열 반환
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
        if (bank->getName() == bankName) { // 은행 이름이 일치하면 반환
            return bank;
        }
    }
    return nullptr; // 찾지 못하면 nullptr 반환
}




class ATM {
private:
    string serialNumber; // ATM 시리얼 번호 (REQ1.1)
    string atmType;      // "Single" or "Multi" (REQ1.2)
    string language; // ATM 언어 설정: English/Bilingual (REQ1.3) //uni Bi
    Bank* primaryBank;   // Primary bank associated with the ATM (REQ1.2)
    int cash1000, cash5000, cash10000, cash50000;
    string adminCardNumber = "0000"; //adm card면 접근(REQ1.9)

    vector<Transaction> session_transactions; // 세션 중 수행된 거래들 (REQ2.3)
    bool session_active = false;              // 세션 상태 표시 (REQ2.1, REQ2.2)

    vector<Transaction> allTransactions; //11.18 20:57

public:
    ATM(string type, Bank* bank, string serial, string lang, int c1000, int c5000, int c10000, int c50000)
        : atmType(type), primaryBank(bank), serialNumber(serial), language(lang),
        cash1000(c1000), cash5000(c5000), cash10000(c10000), cash50000(c50000) {
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
            cout << "The deposit fee is KRW " << fee << ". Would you like to proceed?\n1. Yes\n2. Cancel (press any key)\n-> ";
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
            int count1000, count5000, count10000, count50000;
            int totalBills = 0;

            if (language_signal == 1) {
                cout << "Enter the number of KRW 1,000 bills you want to deposit: ";
            }
            else {
                cout << "천 원권 몇 장을 입금하시겠습니까? ";
            }
            cin >> count1000;
            totalBills += count1000;

            if (language_signal == 1) {
                cout << "Enter the number of KRW 5,000 bills you want to deposit: ";
            }
            else {
                cout << "오천 원권 몇 장을 입금하시겠습니까? ";
            }
            cin >> count5000;
            totalBills += count5000;

            if (language_signal == 1) {
                cout << "Enter the number of KRW 10,000 bills you want to deposit: ";
            }
            else {
                cout << "만원권 몇 장을 입금하시겠습니까? ";
            }
            cin >> count10000;
            totalBills += count10000;

            if (language_signal == 1) {
                cout << "Enter the number of KRW 50,000 bills you want to deposit: ";
            }
            else {
                cout << "오만 원권 몇 장을 입금하시겠습니까? ";
            }
            cin >> count50000;
            totalBills += count50000;

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
                cout << "The total deposit amount is KRW " << depositAmount << "\n.Would you like to proceed?\n1. Yes\n2. Cancel (press any key)\n-> ";
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
            }
            else {
                cout << "입금이 완료되었습니다!\n계좌 잔액: " << account->getBalance() << "원\n";
            }

            // ATM 현금 보유량 반영
            cash1000 += count1000;
            cash5000 += count5000;
            cash10000 += count10000;
            cash50000 += count50000;

            addTransaction(Transaction::transaction_counter++, account->getcardNumber(), "Deposit", depositAmount, ""); //11.18 21:36
        }

        else if (isCheckDeposit) {
            int paperNum = 0;
            int checkContinue;
            int CheckValue;
            int depositAmount = 0;
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

            for (int i = 0; i < paperNum; i++) {
                while (true) {
                    if (language_signal == 1) {
                        cout << "Check #" << (i + 1) << ": Enter the check amount (Minimum: KRW 100,000): ";
                    }
                    else {
                        cout << "수표 #" << (i + 1) << ": 수표 금액을 입력하세요 (최소 100,000원 이상)" << endl;
                    }

                    cin >> CheckValue;

                    if (CheckValue >= 100000) {
                        //account->setBalance(account->getBalance() + CheckValue);
                        depositAmount += CheckValue;
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

            account->setBalance(account->getBalance() + depositAmount);
            if (language_signal == 1) {
                cout << "Deposit successful! Your new balance is KRW " << account->getBalance() << ".\n";
            }
            else {
                cout << "입금이 완료되었습니다!\n계좌 잔액: " << account->getBalance() << "원\n";
            }
            addTransaction(Transaction::transaction_counter++, account->getcardNumber(), "Deposit", depositAmount, ""); //
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
                cout << "\n출금을 계속 하시겠습니까? \n1. 네\n2. 아니 (출금 세션을 종료)\n-> ";
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
                return; // 사용자가 출금을 원하지 않으면 함수 종료
            }

            int withdrawalAmount = 0;
            if (language_signal == 1) {
                cout << "Enter the amount to withdraw: ";
            }
            else {
                cout << "출금할 금액을 입력하시오: ";
            }
            cin >> withdrawalAmount;

            // REQ5.1: 사용자가 금액을 입력하면 각 지폐 단위를 계산하여 가장 적은 지폐 개수를 사용하도록 한다.
            int count50000 = 0, count10000 = 0, count5000 = 0, count1000 = 0;
            int remainingAmount = withdrawalAmount;

            if (remainingAmount >= 50000) { count50000 = remainingAmount / 50000; remainingAmount %= 50000; }
            if (remainingAmount >= 10000) { count10000 = remainingAmount / 10000; remainingAmount %= 10000; }
            if (remainingAmount >= 5000) { count5000 = remainingAmount / 5000;   remainingAmount %= 5000; }
            if (remainingAmount >= 1000) { count1000 = remainingAmount / 1000;   remainingAmount %= 1000; }

            // REQ5.7: 출금 한도를 초과할 경우 에러 메시지 출력 후 종료
            if (withdrawalAmount > maxWithdrawAmount) {
                if (language_signal == 1) {
                    cout << "The maximum withdrawal amount per transaction is KRW " << maxWithdrawAmount << ".\n";
                }
                else {
                    cout << "한 번에 출금할 수 있는 최대 금액은 KRW " << maxWithdrawAmount << "원입니다.\n";
                }
                return;
            }

            // REQ5.2: 계좌 잔액 및 ATM 잔액 부족 체크
            bool isPrimaryBank = (primaryBank == userBank);
            // 2024/11/21 if else문 고침 fee 부분
            bool requiresFee = (primaryBank->getName() != userBank->getName());
            int fee = requiresFee ? 2000 : 1000;


            if (withdrawalAmount + fee > account->getBalance()) {
                if (language_signal == 1) {
                    cout << "Your account balance is insufficient to complete this withdrawal.\n";
                }
                else {
                    cout << "계좌 잔액이 부족하여 출금할 수 없습니다. \n";
                }
                return;
            }

            if (count50000 > cash50000 || count10000 > cash10000 || count5000 > cash5000 || count1000 > cash1000) {
                if (language_signal == 1) {
                    cout << "The ATM does not have enough cash for this withdrawal.\n";
                }
                else {
                    cout << "ATM에 출금할 현금이 부족합니다. \n";
                }
                return;
            }

            // REQ5.3: 출금 성공 시 계좌 잔액에서 차감 및 ATM 잔액 차감
            account->setBalance(account->getBalance() - (withdrawalAmount + fee));
            cash50000 -= count50000;
            cash10000 -= count10000;
            cash5000 -= count5000;
            cash1000 -= count1000;

            if (language_signal == 1) {
                cout << "The withdrawal amount is KRW " << withdrawalAmount << " with a fee of KRW " << fee << ".\n";
                cout << "Would you like to proceed?\n1. OK\n2. Cancel (push any key)\n-> ";
            }
            else {
                cout << "출금 금액은 " << withdrawalAmount << "원이며, 수수료는 " << fee << "원입니다.\n";
                cout << "계속 진행하시겠습니까?\n1. 예\n2. 취소 (아무 키나 누르세요)\n-> ";
            }

            int confirm;
            cin >> confirm;
            if (confirm != 1) {
                if (language_signal == 1) {
                    cout << "Withdrawal canceled.\n";
                }
                else {
                    cout << "출금 취소되었습니다. \n";
                }
                return;
            }

            if (language_signal == 1) {
                cout << "Successfully withdrawn!\n";
                cout << "Remaining balance: KRW " << account->getBalance() << "\n";
                addTransaction(Transaction::transaction_counter++, account->getcardNumber(), "Withdraw", withdrawalAmount, ""); //11.18 21:49

                // REQ5.5: 다른 사용자가 사용할 수 있도록 ATM의 잔액을 감소
                cout << "ATM remaining cash:\n"
                    << "50000: " << cash50000 << ", 10000: " << cash10000
                    << ", 5000: " << cash5000 << ", 1000: " << cash1000 << "\n";
            }
            else {
                cout << "출금 성공!\n";
                cout << "계좌 잔액: KRW " << account->getBalance() << "\n";
                addTransaction(Transaction::transaction_counter++, account->getcardNumber(), "출금", withdrawalAmount, ""); //11.18 21:49
            }

            withdrawalsThisSession++;  // 출금 횟수 증가

            // REQ5.6: 세션당 출금 횟수가 최대치를 초과할 경우 세션 종료
            if (withdrawalsThisSession >= maxWithdrawalsPerSession) {
                if (language_signal == 1) {
                    cout << "The maximum number of withdrawals per session is " << maxWithdrawalsPerSession << ". Please restart another session for more withdrawals.\n";
                }
                else {
                    cout << "세션당 출금 최대 횟수: " << maxWithdrawalsPerSession << ". 추가 출금을 하고 싶으시다면 세션을 새로 시작하십시오. \n";
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

            // Prompt user to confirm inserting the transfer fee first
            if (language_signal == 1) {
                cout << "A transfer fee of KRW " << transferFee << " will be deducted.\n";
                cout << "1. Confirm\n2. Cancel\n-> ";
            }
            else {
                cout << "송금 수수료는 KRW " << transferFee << " 원입니다. \n";
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
            transferAmount = insertedAmount - transferFee;

            // Check if sufficient funds are available after deducting fee
            if (transferAmount < 0) {
                if (language_signal == 1) {
                    cout << "The amount entered is insufficient to cover the transfer fee. Transfer canceled.\n";
                }
                else {
                    cout << "송금 수수료보다 적은 금액을 입력하셨습니다. 송금을 취소합니다. \n";
                }
                return; // End process
            }
            if (language_signal == 1) {
                cout << "Successfully transferred " << transferAmount << " KRW.\n";
            }
            else {
                cout << transferAmount << " 원이 성공적으로 송금되었습니다. \n";
            }
            addTransaction(Transaction::transaction_counter++, sourceAccount->getcardNumber(), "Transfer", transferAmount, "To: " + destinationAccountNumber);
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
                cout << "Your transfer fee is KRW " << transferFee << ". Would you like to proceed?\n1. Yes\n2. Cancel (press any key)\n-> ";
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
                    addTransaction(Transaction::transaction_counter++, sourceAccount->getcardNumber(), "Transfer", transferAmount, "To: " + destinationAccountNumber); //11.18 21:52
                }
                else {
                    cout << "당신 계좌에" << sourceAccount->getBalance() << "원이 남아있습니다. \n";
                    addTransaction(Transaction::transaction_counter++, sourceAccount->getcardNumber(), "Transfer", transferAmount, "To: " + destinationAccountNumber); //11.18 21:52
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




    void startSession(const vector<Bank*>& allBanks) {
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
                return startAdminSession();
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

            vector<string> transactions;
            while (true) {
                if (language_signal == 1) {
                    cout << "\nSelect Transaction:\n1. Deposit\n2. Withdraw\n3. Transfer\n4. Exit\n:";
                }
                else {
                    cout << "\n거래를 선택하세요:\n1. 입금\n2. 출금\n3. 송금\n4. 종료\n:";
                }

                int choice;
                cin >> choice;

                if (choice == 4) break;

                // 거래 처리 예시 (입금, 출금, 송금에 대한 간단한 메시지)
                switch (choice) {
                case 1:
                    deposit(authenticatedAccount, cardBank, language_signal);
                    transactions.push_back(language_signal == 1 ? "Deposit completed." : "입금 완료.");
                    break;
                case 2:
                    withdraw(authenticatedAccount, cardBank, language_signal);
                    transactions.push_back(language_signal == 1 ? "Withdrawal completed." : "출금 완료.");                  
                    break;
                case 3:
                    transfer(authenticatedAccount, cardBank, allBanks, language_signal);
                    transactions.push_back(language_signal == 1 ? "Transfer completed." : "송금 완료.");
                    break;
                case 4:
                    break;
                default:
                    if (language_signal == 1) {
                        cout << "Invalid choice.\n";
                    }
                    else {
                        cout << "잘못된 선택입니다.\n";
                    }
                }
            }

            if (!transactions.empty()) {
                if (language_signal == 1) {
                    cout << "\nTransaction Summary:\n";
                }
                else {
                    cout << "\n거래 요약:\n";
                }
                for (const auto& t : transactions) {
                    cout << "- " << t << "\n";
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

    void startAdminSession() {
        string command;
        cout << "Admin session started. Displaying Transaction History.\n";
        cout << "Would you like to view the Transaction History? (Enter 'JJass' or 'yes') :";
        cin >> command;
        if (command == "JJass" || command == "yes") {
            displayTransactionHistory();
        }
        else {
            return;
        }
    }

    //11/18 20:41
    void addTransaction(int id, const string& card, const string& type, int amt, const string& info = "") {
        allTransactions.push_back(Transaction(id, card, type, amt, info));
    }

    void displayTransactionHistory() {
        cout << "\n===== Transaction History =====\n";
        for (const auto& transaction : allTransactions) {
            cout << "ID: " << transaction.transactionID
                << ", Card: " << transaction.cardNumber
                << ", Type: " << transaction.transactionType
                << ", Amount: " << transaction.amount;
            if (!transaction.additionalInfo.empty()) {
                cout << ", Info: " << transaction.additionalInfo;
            }
            cout << endl;
        }

        // 외부 파일에 거래 내역 저장
        ofstream outFile("transaction_history.txt");
        for (const auto& transaction : allTransactions) {
            outFile << "ID: " << transaction.transactionID
                << ", Card: " << transaction.cardNumber
                << ", Type: " << transaction.transactionType
                << ", Amount: " << transaction.amount;
            if (!transaction.additionalInfo.empty()) {
                outFile << ", Info: " << transaction.additionalInfo;
            }
            outFile << endl;
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


int main() {
    // Bank Initializing Step
    cout << "======< 'Bank Initialization' >======\n";
    vector<Bank> banks;
    vector<Bank*> allBanks;
    string bankInput;

    banks.reserve(100);


    // 사용자로부터 은행 이름을 반복적으로 입력받아 초기화
    while (true) {
        cout << "Enter bank name for initialization(or type 'done' to finish): ";
        cin >> bankInput;

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


            cout << "Balance(KRW): ";
            cin >> balance;

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

        if (createATMInput == "yes") {
            string atmType, primaryBankName, serialNumber, language;
            int cash1000, cash5000, cash10000, cash50000;
            Bank* primaryBank = nullptr;

            cout << "Primary Bank Name: ";
            cin >> primaryBankName;

            // 2024/11/21 고침
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
                };

                // Check for duplicate serialNumber
                /*isDuplicate = false;*/
                for (const ATM& atm : atms) {
                    if (atm.getSerialNumber() == serialNumber) {
                        cout << "Duplicate serial number has been detected. Please enter a unique serial number.\n";
                        isDuplicate = true;
                        break;
                    }
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
            cin >> cash1000;
            cout << "Number of initial 5,000 Cash?: ";
            cin >> cash5000;
            cout << "Number of initial 10,000 Cash?: ";
            cin >> cash10000;
            cout << "Number of initial 50,000 Cash?: ";
            cin >> cash50000;

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
                    atms[atmIndex - 1].startSession(allBanks);
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
