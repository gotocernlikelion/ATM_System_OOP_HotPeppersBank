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

    Transaction(const string& type, int amount)
        : transaction_id(transaction_counter++), type(type), amount(amount) {}
};

int Transaction::transaction_counter = 1;

class ATM {
private:
    static int serial_counter; // ATM 시리얼 번호 초기화 (REQ1.1)
    int serial_number;
    string atm_type;           // ATM 유형 설정: Single/Multi (REQ1.2)
    string language;           // ATM 언어 설정: English/Bilingual (REQ1.3)
    string primary_bank;       // ATM의 주거래 은행 (REQ1.2)

    int cash_1000_num;
    int cash_5000_num;
    int cash_10000_num;
    int cash_50000_num;
    int total_cash;

    vector<Transaction> session_transactions; // 세션 중 수행된 거래들 (REQ2.3)
    bool session_active = false;              // 세션 상태 표시 (REQ2.1, REQ2.2)

    // 거래 수수료 테이블 (REQ1.8)
    unordered_map<string, int> fees = {
        {"primary_deposit", 1000}, {"non_primary_deposit", 2000},
        {"primary_withdrawal", 1000}, {"non_primary_withdrawal", 2000},
        {"transfer_primary", 2000}, {"transfer_primary_non", 3000},
        {"transfer_non_non", 4000}, {"cash_transfer", 1000}
    };

public:
    // ATM 생성자
    ATM(string type, string lang, string primary_bank)
        : serial_number(serial_counter++), atm_type(type), language(lang),
        primary_bank(primary_bank), cash_1000_num(0), cash_5000_num(0),
        cash_10000_num(0), cash_50000_num(0), total_cash(0) {}

    // 초기 현금 설정 (REQ1.4)
    void initializeCash(int num1000, int num5000, int num10000, int num50000) {
        cash_1000_num = num1000;
        cash_5000_num = num5000;
        cash_10000_num = num10000;
        cash_50000_num = num50000;
        updateTotalCash();
    }

    // 전체 현금 잔액 업데이트 (REQ1.10)
    void updateTotalCash() {
        total_cash = (cash_1000_num * 1000) + (cash_5000_num * 5000) +
            (cash_10000_num * 10000) + (cash_50000_num * 50000);
    }

    // 세션 시작 (REQ2.1)
    void startSession() {
        if (!session_active) {
            session_active = true;
            session_transactions.clear();
            cout << "Session started. Insert card to proceed.\n";
        }
        else {
            cout << "Session is already active.\n";
        }
    }

    // 세션 종료 (REQ2.2, REQ2.3)
    void endSession() {
        if (session_active) {
            cout << "Session ended.\n";
            displaySessionSummary();
            session_active = false;
        }
        else {
            cout << "No active session to end.\n";
        }
    }

    // 거래 수행 (REQ2.3, REQ2.4)
    void performTransaction(const string& type, int amount) {
        if (session_active) {
            Transaction transaction(type, amount);
            session_transactions.push_back(transaction);
            cout << "Performed transaction: " << type << ", Amount: " << amount << endl;
        }
        else {
            cout << "Start a session first to perform transactions.\n";
        }
    }

    // 세션 요약 정보 출력 (REQ2.3)
    void displaySessionSummary() const {
        if (session_transactions.empty()) {
            cout << "No transactions completed during this session.\n";
        }
        else {
            cout << "Session Summary:\n";
            for (const auto& transaction : session_transactions) {
                cout << "Transaction ID: " << transaction.transaction_id
                    << ", Type: " << transaction.type
                    << ", Amount: " << transaction.amount << endl;
            }
        }
    }

    // 사용자 입력을 통한 ATM 생성 (REQ1.11)
    static ATM createATMFromUserInput() {
        string type, lang, primary_bank;
        cout << "Enter ATM type (Single/Multi): ";
        cin >> type;
        cout << "Enter ATM language (English/Bilingual): ";
        cin >> lang;
        cout << "Enter the primary bank: ";
        cin >> primary_bank;

        ATM atm(type, lang, primary_bank);
        int num1000, num5000, num10000, num50000;
        cout << "Initialize cash: \n";
        cout << "Enter number of KRW 1,000 bills: ";
        cin >> num1000;
        cout << "Enter number of KRW 5,000 bills: ";
        cin >> num5000;
        cout << "Enter number of KRW 10,000 bills: ";
        cin >> num10000;
        cout << "Enter number of KRW 50,000 bills: ";
        cin >> num50000;
        atm.initializeCash(num1000, num5000, num10000, num50000);

        return atm;
    }
};

// 초기 시리얼 번호를 111111로 설정 (REQ1.1)
int ATM::serial_counter = 111111;

// 메인 함수
int main() {
    ATM atm = ATM::createATMFromUserInput(); // 사용자 입력에 따른 ATM 생성 (REQ1.11)

    atm.startSession();  // 세션 시작 (REQ2.1)
    atm.performTransaction("Deposit", 50000); // 예시 거래 수행 (REQ2.3, REQ2.4)
    atm.performTransaction("Withdrawal", 20000);
    atm.endSession();  // 세션 종료 및 요약 출력 (REQ2.2, REQ2.3)

    return 0;
}
