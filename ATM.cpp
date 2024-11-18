#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

class Transaction {
public:
    static int transaction_counter; // 각 거래의 고유 ID (REQ2.4)
    int transaction_id;
    string type;
    int amount;

    Transaction(const string& type, int amount) {
        transaction_id = transaction_counter++;
        this->type = type;
        this->amount = amount;
    }
};

int Transaction::transaction_counter = 1;

class Account { //(REQ1.6) (REQ1.7)
private:
    string bankName;
    string accountNumber;// cardNumber랑 동일
    string username;
    string password;
    int balance;

    vector<Transaction> transaction;
public:
    Account(const string& bank_name, const string& user_name, const string& account_number, const string& password, int initial_balance) {
        this->bankName = bank_name;
        this->username = user_name;
        this->accountNumber = account_number;
        // this->cardNumber=card_number;
        this->password = password;
        this->balance = initial_balance;
    }
    Account() : bankName(""), accountNumber(""), username(""), password(""), balance(0) {}

    string getAccountNumber() const {
        return accountNumber;
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
        accounts[account.getAccountNumber()] = account;
    }

    Account* authenticate(const string& accountNumber, const string& password) {
        if (accounts.count(accountNumber) && accounts[accountNumber].getPassword() == password) {
            return &accounts[accountNumber];
        }
        //cout << "Wrong  Password." << endl;
        return nullptr;
    }

    // 비밀번호 없이 계좌 번호만으로 찾는 메서드 추가
    Account* findAccountByNumber(const string& accountNumber) {
        if (accounts.count(accountNumber)) {
            return &accounts[accountNumber];
        }
        return nullptr;
    }

    void displayAccounts() const {
        for (const auto& entry : accounts) {
            const Account& account = entry.second;
            cout << "Account [Bank: " << name
                << ", No: " << account.getAccountNumber()
                << ", Owner: " << account.getUserName()
                << "] balance: " << account.getBalance() << "Won\n";
        }
    }
};



class ATM {
private:
    string serialNumber; // ATM 시리얼 번호 (REQ1.1)
    string atmType;      // "Single" or "Multi" (REQ1.2)
    string language; // ATM 언어 설정: English/Bilingual (REQ1.3) //uni Bi
    Bank* primaryBank;   // Primary bank associated with the ATM (REQ1.2)
    int cash1000, cash5000, cash10000, cash50000;
    string adminCardNumber; //adm card면 접근(REQ1.9)

    vector<Transaction> session_transactions; // 세션 중 수행된 거래들 (REQ2.3)
    bool session_active = false;              // 세션 상태 표시 (REQ2.1, REQ2.2)

public:
    ATM(string type, Bank* bank, string serial, string lang, int c1000, int c5000, int c10000, int c50000, string adminCard)
        : atmType(type), primaryBank(bank), serialNumber(serial), language(lang),
        cash1000(c1000), cash5000(c5000), cash10000(c10000), cash50000(c50000), adminCardNumber(adminCard) {}

    string getSerialNumber() const {
        return serialNumber;
    }

    bool authenticateUser(const vector<Bank*>& allBanks, const string& accountNumber,  string& password) {
    int attemptCount = 0;
    while (attemptCount < 3) {
        if (atmType == "Single") {
            Account* account = primaryBank->authenticate(accountNumber, password);
            if (account) {
                cout << "User authorized for Single-Bank ATM in " << primaryBank->getName() << " Bank." << endl;
                return true;
            }
            else if (primaryBank->findAccountByNumber(accountNumber)) {
                // primary bank에 계좌는 있지만 비밀번호가 틀린 경우
                cout << "Wrong password. Please try again (" << 3 - attemptCount - 1 << " attempts remaining).\n";
            }
            else {
                cout << "Invalid Card. Primary Bank of ATM: " << primaryBank->getName() << endl;
                return false;
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
            cout << "Authorization failed. Please try again (" << 3 - attemptCount - 1 << " attempts remaining).\n";
        }

        attemptCount++;
        if (attemptCount < 3) {
            cout << "Enter Password again: ";
            cin >> password;
        }
    }
    cout << "Too many failed attempts. Session terminated.\n";
    return false;
}


    void displayRemainingCash() const {
        cout << "ATM [SN: " << serialNumber << "] remaining cash: {"
            << "KRW 50000 : " << cash50000 << ", "
            << "KRW 10000 : " << cash10000 << ", "
            << "KRW 5000 : " << cash5000 << ", "
            << "KRW 1000 : " << cash1000 << "}\n";
    }

    void deposit(Account* account, Bank* userBank) {
        int depositType;
        cout << "\n<< Select deposit type >>\n1. Cash deposit\n2. Check deposit\n3. Cancel (push any key)\n-> ";
        cin >> depositType;

        if (depositType != 1 && depositType != 2) {
            cout << "Deposit canceled.\n";
            return;
        }

        bool isCashDeposit = (depositType == 1);
        bool isCheckDeposit = (depositType == 2);
        bool requiresFee = (primaryBank != userBank); // 주은행 여부 확인
        int fee = requiresFee ? 2000 : 1000;         // 수수료 설정

        cout << "Deposit fee is KRW " << fee << ". Would you like to pay the fee?\n";
        cout << "1. Yes\n2. Cancel (push any key)\n-> ";
        int confirm;
        cin >> confirm;

        if (confirm != 1) {
            cout << "Deposit canceled.\n";
            return;
        }

        // 수수료 반영
        if (fee == 1000) {
            cash1000++; // 수수료 1000원 지폐 1장 추가
            cout << "Fee of KRW 1,000 has been accepted.\n";
        }
        else if (fee == 2000) {
            cash1000 += 2; // 수수료 2000원 지폐 2장 추가
            cout << "Fee of KRW 2,000 has been accepted.\n";
        }

        if (isCashDeposit) {
            // 입금할 금액 입력
            int count1000, count5000, count10000, count50000;
            int totalBills = 0;

            cout << "How many KRW 1000 papers will you deposit? ";
            cin >> count1000;
            totalBills += count1000;

            cout << "How many KRW 5000 papers will you deposit? ";
            cin >> count5000;
            totalBills += count5000;

            cout << "How many KRW 10000 papers will you deposit? ";
            cin >> count10000;
            totalBills += count10000;

            cout << "How many KRW 50000 papers will you deposit? ";
            cin >> count50000;
            totalBills += count50000;

            // 지폐 수량 제한 확인
            const int limit = 50;
            if (totalBills > limit) {
                cout << "You cannot insert over " << limit << " papers.\n!!Session finished!!\n";
                return;
            }

            // 총 입금 금액 계산
            int depositAmount = count1000 * 1000 + count5000 * 5000 + count10000 * 10000 + count50000 * 50000;

            cout << "The deposit amount is KRW " << depositAmount << "\nWould you like to continue?\n1. Yes\n2. Cancel (push any key)\n-> ";
            cin >> confirm;
            if (confirm != 1) {
                cout << "Deposit canceled.\n";
                return;
            }

            // 계좌 잔액 반영
            account->setBalance(account->getBalance() + depositAmount);
            cout << "Successfully deposited!\nThere is KRW " << account->getBalance() << " in your account.\n";

            // ATM 현금 보유량 반영
            cash1000 += count1000;
            cash5000 += count5000;
            cash10000 += count10000;
            cash50000 += count50000;
        }

        else if (isCheckDeposit) {
            int paperNum = 0;
            int checkContinue;
            int CheckValue;
            cout << "How many check papers will you deposit? (30 papers limit)" << endl;
            cin >> paperNum;

            if (paperNum > 30) {
                cout << "You cannot insert over 30 papers" << endl;
                return;
            }

            for (int i = 0; i < paperNum; i++) {
                while (true) {
                    cout << "Check #" << (i + 1) << ": How much will you deposit with your check? (Minimum amount is 100,000 KRW)" << endl;
                    cin >> CheckValue;

                    if (CheckValue >= 100000) {
                        account->setBalance(account->getBalance() + CheckValue);
                        break;
                    }
                    else {
                        cout << "Invalid amount. Please enter a value of 100,000 KRW or more." << endl;
                    }
                }
            }
            cout << "Successfully deposited!\nThere is KRW " << account->getBalance() << " in your account.\n";
        }
    }



    void withdraw(Account* account, Bank* userBank) {
        const int maxWithdrawAmount = 500000;    // REQ5.7: 출금 한도
        const int maxWithdrawalsPerSession = 3;  // REQ5.6: 세션당 최대 출금 횟수
        int withdrawalsThisSession = 0;          // 세션당 출금 횟수를 카운트할 변수

        while (withdrawalsThisSession < maxWithdrawalsPerSession) {
            int withdrawalAmount, fee = 0;
            cout << "Enter amount to withdraw: ";
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
                cout << "The maximum amount of cash withdrawal per transaction is KRW " << maxWithdrawAmount << ".\n";
                return;
            }

            // REQ5.2: 계좌 잔액 및 ATM 잔액 부족 체크
            bool isPrimaryBank = (primaryBank == userBank);
            fee = isPrimaryBank ? 1000 : 2000;  // REQ5.4: 수수료 설정
            if (withdrawalAmount + fee > account->getBalance()) {
                cout << "Insufficient funds in your account.\n";
                return;
            }

            if (count50000 > cash50000 || count10000 > cash10000 || count5000 > cash5000 || count1000 > cash1000) {
                cout << "Insufficient cash in the ATM.\n";
                return;
            }

            // REQ5.3: 출금 성공 시 계좌 잔액에서 차감 및 ATM 잔액 차감
            account->setBalance(account->getBalance() - (withdrawalAmount + fee));
            cash50000 -= count50000;
            cash10000 -= count10000;
            cash5000 -= count5000;
            cash1000 -= count1000;

            cout << "The withdrawal amount is KRW " << withdrawalAmount << " with a fee of KRW " << fee << ".\n";
            cout << "Would you like to continue?\n1. OK\n2. Cancel (push any key)\n-> ";
            int confirm;
            cin >> confirm;
            if (confirm != 1) {
                cout << "Withdrawal canceled.\n";
                return;
            }

            cout << "Successfully withdrawn!\n";
            cout << "Remaining balance: KRW " << account->getBalance() << "\n";

            // REQ5.5: 다른 사용자가 사용할 수 있도록 ATM의 잔액을 감소
            cout << "ATM remaining cash:\n"
                << "50000: " << cash50000 << ", 10000: " << cash10000
                << ", 5000: " << cash5000 << ", 1000: " << cash1000 << "\n";

            withdrawalsThisSession++;  // 출금 횟수 증가

            // REQ5.6: 세션당 출금 횟수가 최대치를 초과할 경우 세션 종료
            if (withdrawalsThisSession >= maxWithdrawalsPerSession) {
                cout << "The maximum number of withdrawals per session is " << maxWithdrawalsPerSession << ". Please restart another session for more withdrawals.\n";
                break;
            }
        }
    }


    void transfer(Account* sourceAccount, Bank* sourceBank, const vector<Bank*>& allBanks) {
        int transferType;
        cout << "\n<< Select transfer type >>\n1. Cash transfer\n2. Account transfer\n3. Cancel (push any key)\n-> ";
        cin >> transferType;

        if (transferType == 3) {
            cout << "Transfer canceled.\n";
            return;
        }

        // REQ6.2: 목적 계좌 정보 입력
        string destinationAccountNumber, destinationBankName;
        cout << "Input destination account number: ";
        cin >> destinationAccountNumber;
        cout << "Input destination bank_name: "; //은행 이름 입력 안받고 계좌번호만으로도 알 수 있게 해야할려나
        cin >> destinationBankName;

        Bank* destinationBank = nullptr;
        for (Bank* bank : allBanks) {
            if (bank->getName() == destinationBankName) {
                destinationBank = bank;
                break;
            }
        }

        if (!destinationBank) {
            cout << "Destination bank not found. Transfer canceled.\n";
            return;
        }

        // Transfer 수수료 설정
        int transferFee = 0;
        if (transferType == 1) { // Cash transfer
            transferFee = 1000; // REQ6.5: Cash transfer 수수료는 1000원
        }
        else if (transferType == 2) { // Account transfer
            transferFee = (sourceBank == destinationBank) ? 2000 : 3000; // REQ6.5: Account transfer 수수료는 은행 간 다름
        }

        int transferAmount = 0;

        if (transferType == 1) { // Cash transfer

            int cash1000, cash5000, cash10000, cash50000;
            int command;
            int insertedAmount;

            // Prompt user to confirm inserting the transfer fee first
            cout << "A transfer fee of KRW " << transferFee << " will be deducted.\n";
            cout << "1. OK\n2. Cancel\n-> ";
            cin >> command;

            if (command == 2) {
                cout << "Transaction canceled.\n";
                return; // End the process if user cancels
            }

            cout << "Transfer fee accepted. Please insert cash for the transfer.\n";

            do {
                // Prompt user to insert cash
                cout << "num of KRW 1000: "; cin >> cash1000;
                cout << "num of KRW 5000: "; cin >> cash5000;
                cout << "num of KRW 10000: "; cin >> cash10000;
                cout << "num of KRW 50000: "; cin >> cash50000;

                // Calculate total inserted cash
                insertedAmount = cash1000 * 1000 + cash5000 * 5000 + cash10000 * 10000 + cash50000 * 50000;

                cout << "Please check the total amount of transfer cash: " << insertedAmount << " KRW\n";
                cout << "1. Confirm\n2. Re-enter cash\n-> ";
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
                cout << "Insufficient funds to cover the transfer fee. Transaction canceled.\n";
                return; // End process
            }

            cout << "Transfer of " << transferAmount << " KRW successfully completed.\n";
        }

        else if (transferType == 2) { // Account transfer
            // Account transfer는 원천 계좌에서 금액과 수수료가 차감됨
            if (sourceAccount->getBalance() < transferAmount + transferFee) {
                cout << "Not enough funds in your account\n!!Session finished!!\n";
                return;
            }

            // 사용자에게 수수료 안내
            cout << "Your transfer fee is KRW " << transferFee << ". Do you want to transfer?\n1. Yes\n2. Cancel (push any key)\n-> ";
            int confirm;
            cin >> confirm;
            if (confirm != 1) {
                cout << "Transfer canceled.\n";
                return;
            }
            sourceAccount->setBalance(sourceAccount->getBalance() - transferAmount - transferFee); // REQ6.7: 원천 계좌 잔액 차감
        }

        // 목적 계좌에 금액 추가
        Account* destinationAccount = destinationBank->findAccountByNumber(destinationAccountNumber);
        if (destinationAccount) {
            destinationAccount->setBalance(destinationAccount->getBalance() + transferAmount);
            cout << "KRW " << transferAmount << " is successfully transferred to account " << destinationAccountNumber << "!\n";
            if (transferType == 2) {
                cout << "There is KRW " << sourceAccount->getBalance() << " in your account.\n";
            }
        }
        else {
            cout << "Destination account not found. Transfer canceled.\n";
        }
    }




    void startSession(const vector<Bank*>& allBanks) {
        string cardNumber, password;
        cout << "\nPlease insert your card (Enter Account Number): ";
        cin >> cardNumber;

        cout << "Enter Password: ";
        cin >> password;

        if (authenticateUser(allBanks, cardNumber, password)) {
            Account* authenticatedAccount = nullptr;
            Bank* cardBank = nullptr;

            // 계좌 인증 후 은행 찾기
            for (Bank* bank : allBanks) {
                authenticatedAccount = bank->authenticate(cardNumber, password);
                if (authenticatedAccount) {
                    cardBank = bank;
                    cout << "Authentication successful. Welcome, " << authenticatedAccount->getUserName() << "!\n";
                    break;
                }
            }

            if (!authenticatedAccount) {
                cout << "Authentication failed. Ending session.\n";
                return;
            }

            vector<string> transactions;
            while (true) {
                cout << "\nSelect Transaction:\n1. Deposit\n2. Withdraw\n3. Transfer\n4. Exit\n:";
                int choice;
                cin >> choice;

                if (choice == 4) break;

                // 거래 처리 예시 (입금, 출금, 송금에 대한 간단한 메시지)
                switch (choice) {
                case 1:
                    deposit(authenticatedAccount, cardBank);
                    transactions.push_back("Deposit completed.");
                    cout << "Deposit completed.\n";
                    break;
                case 2:
                    withdraw(authenticatedAccount, cardBank);
                    transactions.push_back("Withdrawal completed.");
                    cout << "Withdrawal completed.\n";
                    break;
                case 3:
                    transfer(authenticatedAccount, cardBank, allBanks);
                    transactions.push_back("Transfer completed.");
                    cout << "Transfer completed.\n";
                    break;
                case 4:
                    break;
                default:
                    cout << "Invalid choice.\n";
                }
            }

            if (!transactions.empty()) {
                cout << "\nTransaction Summary:\n";
                for (const auto& t : transactions) {
                    cout << "- " << t << "\n";
                }
            }
            else {
                cout << "\nNo transactions completed in this session.\n";
            }
        }
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
    vector<Bank> banks;
    vector<Bank*> allBanks;
    string bankInput;

    banks.reserve(100);


    // 사용자로부터 은행 이름을 반복적으로 입력받아 초기화
    while (true) {
        cout << "Enter bank name (or type 'done' to finish): ";
        cin >> bankInput;

        if (bankInput == "done") break; // 'done' 입력 시 은행 초기화 종료

        Bank newBank(bankInput); // 새로운 Bank 객체 생성
        banks.push_back(newBank); // banks 벡터에 추가
        allBanks.push_back(&banks.back()); // 포인터 벡터에도 추가
        cout << "[" << bankInput << "] bank created!\n";
    }


    // Account Creation Step
    cout << "\n=====<<Account Creation>>=====\n";
    string createAccountInput;
    int accountCount = 1;
    vector<string> accountNumbers;

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


            while (true) {
                cout << "Account Number (12 digits): ";
                cin >> accountNumber;

                bool validAccountNumber = true;

                if (accountNumber.length() != 12) {
                    cout << "Invalid account number. It must be exactly 12 digits.\n";
                    validAccountNumber = false;
                }

                // 기존의 accountNumbers에 중복된 번호가 있는지 확인
                for (const string& name : accountNumbers) {
                    if (accountNumber == name) {
                        validAccountNumber = false;
                        cout << "This account number is already taken. Please enter a different number.\n";
                        break;
                    }
                }

                if (validAccountNumber) {
                    accountNumbers.push_back(accountNumber);
                    // cout << "Account number " << accountNumber << " has been successfully added.\n";
                    break;
                }
            }

            cout << "Balance(KRW): ";
            cin >> balance;

            cout << "Password: ";
            cin >> password;

            // 은행 이름에 따라 계좌 추가
            bool bankFound = false;
            for (Bank* bank : allBanks) {
                if (bank->getName() == bankName) {
                    bank->addAccount(Account(bankName, username, accountNumber, password, balance));
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
        cout << "\nDo you want to create ATM " << atmCount << "? (yes or no) -> ";
        cin >> createATMInput;

        if (createATMInput == "yes") {
            string atmType, primaryBankName, serialNumber, language, adminCardNumber;
            int cash1000, cash5000, cash10000, cash50000;
            Bank* primaryBank = nullptr;

            cout << "Primary Bank Name: ";
            cin >> primaryBankName;

            // Loop to ensure unique 6-digit serial number
            bool isDuplicate;
            do {
                cout << "Serial Number(6-digit): ";
                cin >> serialNumber;

                // Check if serial number is exactly 6 digits
                if (serialNumber.length() != 6) {
                    cout << "Invalid serial number. It must be exactly 6 digits.\n";
                    continue;  // Prompt for serial number again
                }

                // Check for duplicate serialNumber
                isDuplicate = false;
                for (const ATM& atm : atms) {
                    if (atm.getSerialNumber() == serialNumber) {
                        cout << "Duplicate serial number detected. Please enter a unique serial number.\n";
                        isDuplicate = true;
                        break;
                    }
                }
            } while (isDuplicate);

            cout << "Type(Single or Multi): ";
            cin >> atmType;

            cout << "Language(Uni or Bi): ";
            cin >> language;

            cout << "1000 Cash ?: ";
            cin >> cash1000;
            cout << "5000 Cash?: ";
            cin >> cash5000;
            cout << "10000 Cash?: ";
            cin >> cash10000;
            cout << "50000 Cash ?: ";
            cin >> cash50000;

            cout << "Admin card Number: ";
            cin >> adminCardNumber;

            // Find the primary bank
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

            // Create and add the ATM
            atms.emplace_back(atmType, primaryBank, serialNumber, language, cash1000, cash5000, cash10000, cash50000, adminCardNumber);
            cout << "ATM " << atmCount << " created successfully.\n";

            // Increment atmCount only on successful creation
            atmCount++;
        }
    } while (createATMInput == "yes");



    // Program execution loop to check for '/' input
    string command;
    while (true) {
        cout << "\nEnter '/' to display snapshot, 'session' to start an ATM session, or 'exit' to quit: ";
        cin >> command;

        if (command == "/") {
            displaySnapshot(banks, atms);
        }
        else if (command == "session") {
            if (atms.empty()) {
                cout << "No ATMs available. Please create an ATM first.\n";
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
