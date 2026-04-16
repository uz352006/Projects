#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <fstream>
#include <cmath>
using namespace std;
class SecurityConfig {
public:
    static const int MAX_LOGIN_ATTEMPTS = 3;
    static const int ACCOUNT_LOCKOUT_TIME = 10;
    static const int UNDO_WINDOW = 60;
};

class TransactionStack {
public:
    struct StackNode {
        string id;
        string type;
        double amount;
        double balanceBefore;
        double balanceAfter;
        string otherAccount;
        time_t timestamp;
        StackNode* next;
        StackNode(string i, string t, double a, double bBefore, double bAfter, string o, long ts): id(i), type(t), amount(a), balanceBefore(bBefore), balanceAfter(bAfter), otherAccount(o), timestamp(ts), next(nullptr) {}
    };
    StackNode* top;
    int count;
    TransactionStack() : top(nullptr), count(0) {}
    ~TransactionStack() {
        clear();
    }
    void push(string id, string type, double amount, double balanceBefore, double balanceAfter, string otherAccount, long timestamp) {
        StackNode* newNode = new StackNode(id, type, amount, balanceBefore, balanceAfter, otherAccount, timestamp);
        newNode->next = top;
        top = newNode;
        count++;
    }
    StackNode* pop() {
        if (top == nullptr) return nullptr;
        StackNode* temp = top;
        top = top->next;
        count--;
        return temp;
    }
    StackNode* peek() {
        return top;
    }
    bool isEmpty() const {
        return top == nullptr;
    }
    int getCount() const {
        return count;
    }
    void clear() {
        while (top != nullptr) {
            StackNode* temp = top;
            top = top->next;
            delete temp;
        }
        count = 0;
    }
};

class TransactionLinkedList {
public:
    class TransactionNode {
        public:
        string id;
        string type;
        double amount;
        double balanceAfter;
        string otherAccount;
        time_t timestamp;
        TransactionNode* next;
        TransactionNode(string i, string t, double a, double b, string o, long ts): id(i), type(t), amount(a), balanceAfter(b), otherAccount(o), timestamp(ts), next(nullptr) {}
    };
    TransactionNode* head;
    TransactionNode* tail;
    int count;
    TransactionLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~TransactionLinkedList() {
        clear();
    }
    void addTransaction(string id, string type, double amount, double balanceAfter, string otherAccount, long timestamp) {
        TransactionNode* newNode = new TransactionNode(id, type, amount, balanceAfter, otherAccount, timestamp);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }
    TransactionNode* getLastTransaction() {
        return tail;
    }
    void removeLastTransaction() {
        if (head == nullptr) return; 
        if (head == tail) {
            delete head;
            head = nullptr;
            tail = nullptr;
        } else {
            TransactionNode* current = head;
            while (current->next != tail) {
                current = current->next;
            }
            delete tail;
            tail = current;
            tail->next = nullptr;
        }
        count--;
    }
    bool removeMatchingFromBack(const string& typeSubstr, double amount, const string& other) {
        if (head == nullptr) return false;
        TransactionNode* toDelete = nullptr;
        TransactionNode* current = head;
        TransactionNode* prev = nullptr;
        TransactionNode* matchPrev = nullptr;
        while (current != nullptr) {
            if (current->type.find(typeSubstr) != string::npos &&
                fabs(current->amount - amount) < 0.0001 &&
                current->otherAccount == other) {
                toDelete = current;
                matchPrev = prev;
            }
            prev = current;
            current = current->next;
        }
        if (toDelete != nullptr) {
            if (toDelete == head) {
                head = head->next;
                if (head == nullptr) {
                    tail = nullptr;
                }
            } else {
                matchPrev->next = toDelete->next;
                if (toDelete == tail) {
                    tail = matchPrev;
                }
            }
            delete toDelete;
            count--;
            return true;
        }
        return false;
    }
    int getCount() const { return count; }
    TransactionNode* getHead() const { return head; }
    void clear() {
        while (head != nullptr) {
            TransactionNode* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr;
        count = 0;
    }
};

class PaymentPriorityQueue {
public:
    struct PQNode {
        string id;
        time_t executeAt;
        string fromAccount;
        string toAccount;
        double amount;
        PQNode* next;
        PQNode(string i, long e, string f, string t, double a): id(i), executeAt(e), fromAccount(f), toAccount(t), amount(a), next(nullptr) {}
    };
    PQNode* head;
    int count;
    PaymentPriorityQueue() : head(nullptr), count(0) {}
    ~PaymentPriorityQueue() {
        clear();
    }
    void enqueue(string id, long executeAt, string fromAccount, string toAccount, double amount) {
        PQNode* newNode = new PQNode(id, executeAt, fromAccount, toAccount, amount);  
        if (head == nullptr || executeAt < head->executeAt) {
            newNode->next = head;
            head = newNode;
        } else {
            PQNode* current = head;
            while (current->next != nullptr && current->next->executeAt < newNode->executeAt) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
        count++;
    }
    PQNode* dequeue() {
        if (head == nullptr) return nullptr;
        PQNode* temp = head;
        head = head->next;
        count--;
        return temp;
    }
    PQNode* peek() {
        return head;
    }
    bool isEmpty() const {
        return head == nullptr;
    }
    int size() const {
        return count;
    }
    void clear() {
        while (head != nullptr) {
            PQNode* temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
        count = 0;
    }
    PQNode* getHead() const {
        return head;
    }
};

class User {
public:
    string name;
    string userID;
    string password;
    string pin;
    string accountNumber;
    string email;
    string phone;
    string address;
    double balance;
    string accountType;
    string dateCreated;
    int loginAttempts;
    long lastLoginAttempt;
    bool isLocked;
    TransactionLinkedList transactions;
    TransactionStack undoStack;

    User() : name(""), userID(""), password(""), pin(""), accountNumber(""), email(""), phone(""), address(""), balance(0.0), accountType(""), dateCreated(""), loginAttempts(0), lastLoginAttempt(0), isLocked(false) {}

    User(string n, string id, string pwd, string userPin, string email_, string phone_, string addr, string accType, double initialBalance): name(n), userID(id), password(pwd), pin(userPin), accountNumber(""), email(email_), phone(phone_), address(addr), balance(initialBalance), accountType(accType), dateCreated(""), loginAttempts(0), lastLoginAttempt(0), isLocked(false) {
        accountNumber = generateAccountNumber();
        dateCreated = getCurrentDate();
    }

    string generateAccountNumber() {
        string accNum = "ACC";
        for (int i = 0; i < 10; i++) {
            accNum += to_string(rand() % 10);
        }
        return accNum;
    }

    string getCurrentDate() {
        time_t t = time(nullptr);
        struct tm *tm_info = localtime(&t);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
        return string(buf);
    }

    bool isWeakPassword(const string& pwd) {
        if (pwd.length() < 4) return true;
        const char* weakPasswords[] = {
            "password", "123456", "1234", "0000", "admin",
            "qwerty", "letmein", "welcome", "pass", "1111"
        };
        int weakCount = 10;
        for (int i = 0; i < weakCount; i++) {
            if (pwd == weakPasswords[i]) return true;
        }
        bool allSame = true;
        for (int i = 1; i < (int)pwd.length(); i++) {
            if (pwd[i] != pwd[0]) {
                allSame = false;
                break;
            }
        }
        if (allSame) return true;
        return false;
    }
   
    bool isWeakPIN(const string& pin) {
        if (pin.length() != 4) return false;
        bool sequentialAsc = true;
        bool sequentialDesc = true;
        for (int i = 1; i < 4; i++) {
            if (pin[i] != pin[i-1] + 1) sequentialAsc = false;
            if (pin[i] != pin[i-1] - 1) sequentialDesc = false;
        }
        if (sequentialAsc || sequentialDesc) return true;
        bool allSame = true;
        for (int i = 1; i < 4; i++) {
            if (pin[i] != pin[0]) {
                allSame = false;
                break;
            }
        }
        if (allSame) return true;
        const char* weakPINs[] = {"0000", "1111", "2222", "3333", "4444","5555", "6666", "7777", "8888", "9999","1234", "4321", "1122", "1212"};
        int weakCount = 14;
        for (int i = 0; i < weakCount; i++) {
            if (pin == weakPINs[i]) return true;
        }
        return false;
    }

    bool setEmail(const string &e) {
        if (e.empty() || e.length() > 50) return false;
        size_t at_pos = e.find('@');
        if (at_pos == string::npos) return false;
        size_t dot_pos = e.find('.', at_pos + 1);
        if (dot_pos == string::npos) return false;
        if (dot_pos <= at_pos + 1) return false;
        email = e;
        return true;
    }
   
    bool setPhone(const string &p) {
        if (p.empty() || p.length() < 10 || p.length() > 15) return false;
        for (int i = 0; i < (int)p.length(); i++) {
            char c = p[i];
            if (!isdigit(c) && c != '+' && c != '-' && c != ' ' && c != '(' && c != ')') return false;
        }
        phone = p;
        return true;
    }
   
    bool setAddress(const string &a) {
        if (a.empty() || a.length() > 100) return false;
        address = a;
        return true;
    }
   
    bool setPassword(const string &pwd) {
        if (pwd.length() < 4 || pwd.length() > 20) return false;
        if (isWeakPassword(pwd)) return false;
        password = pwd;
        return true;
    }
   
    bool setPIN(const string &p) {
        if (p.length() != 4) return false;
        for (int i = 0; i < 4; i++) {
            if (!isdigit(p[i])) return false;
        }
        if (isWeakPIN(p)) return false;
        pin = p;
        return true;
    }

    bool verifyPIN(string inputPIN) {
        if (isLocked) {
            long now = static_cast<long>(time(nullptr));
            if (now - lastLoginAttempt > SecurityConfig::ACCOUNT_LOCKOUT_TIME) {
                isLocked = false;
                loginAttempts = 0;
            } else {
                return false;
            }
        }
        if (pin == inputPIN) {
            loginAttempts = 0;
            return true;
        } else {
            loginAttempts++;
            lastLoginAttempt = static_cast<long>(time(nullptr));
            if (loginAttempts >= SecurityConfig::MAX_LOGIN_ATTEMPTS) {
                isLocked = true;
            }
            return false;
        }
    }
   
    bool verifyPassword(string inputPassword) {
        if (isLocked) {
            long now = static_cast<long>(time(nullptr));
            if (now - lastLoginAttempt > SecurityConfig::ACCOUNT_LOCKOUT_TIME) {
                isLocked = false;
                loginAttempts = 0;
            } else {
                return false;
            }
        }
        if (password == inputPassword) {
            loginAttempts = 0;
            return true;
        } else {
            loginAttempts++;
            lastLoginAttempt = static_cast<long>(time(nullptr));
            if (loginAttempts >= SecurityConfig::MAX_LOGIN_ATTEMPTS) {
                isLocked = true;
            }
            return false;
        }
    }

    string formatBalance(double amount) {
        long cents = static_cast<long>(round(amount * 100));
        long whole = cents / 100;
        long rem = labs(cents % 100);
        char buf[64];
        sprintf(buf, "%ld.%02ld", whole, rem);
        return string(buf);
    }
   
    bool canUndo() const {
        return !undoStack.isEmpty();
    }
   
    void addTransactionRecord(const string &type, double amount, double balanceAfter, const string &otherAccount = "") {
        string id = "T" + to_string(time(nullptr)) + to_string(rand() % 1000);
        double balanceBefore = balance;
        balance = balanceAfter;
        transactions.addTransaction(id, type, amount, balanceAfter, otherAccount, time(nullptr));
        if (type != "SECURITY:" && type.find("UNDO") != 0) {
            undoStack.push(id, type, amount, balanceBefore, balanceAfter, otherAccount, time(nullptr));
        }
    }
   
    void undoLastTransaction() {
        if (!canUndo()) {
            cout << "No transactions to undo." << endl;
            return;
        }
        TransactionStack::StackNode* last = undoStack.peek();
        long now = time(nullptr);
        long timeDiff = now - last->timestamp;
        if (timeDiff > SecurityConfig::UNDO_WINDOW) {
            cout << "Cannot undo - transaction is older than " << SecurityConfig::UNDO_WINDOW << " seconds." << endl;
            return;
        }
        string type = last->type;
        double amount = last->amount;
        double oldBalance = balance;
        cout << "Undoing last transaction: " << type << " of PKR " << formatBalance(amount) << endl;
        if (type.find("SCHEDULED") != string::npos) {
            cout << "Cannot undo scheduled payments once they are scheduled." << endl;
            cout << "You need to cancel the scheduled payment instead." << endl;
            return;
        }
        balance = last->balanceBefore;
        string undoId = "UNDO" + to_string(time(nullptr)) + to_string(rand() % 1000);
        if (type == "DEPOSIT") {
            transactions.addTransaction(undoId, "UNDO DEPOSIT", amount, balance, "", time(nullptr));
            cout << "Deposit undone. " << formatBalance(amount) << " deducted from account." << endl;
        }
        else if (type == "WITHDRAW") {
            transactions.addTransaction(undoId, "UNDO WITHDRAW", amount, balance, "", time(nullptr));
            cout << "Withdrawal undone. " << formatBalance(amount) << " added back to account." << endl;
        }
        else if (type.find("TRANSFER") != string::npos) {
            transactions.addTransaction(undoId, "UNDO " + type, amount, balance, last->otherAccount, time(nullptr));
            cout << "Transfer undone. " << formatBalance(amount) << " added back to account." << endl;
        }
        else {
            cout << "Cannot undo this type of transaction." << endl;
            return;
        }
        TransactionStack::StackNode* undone = undoStack.pop();
        delete undone;
        cout << "Balance changed from PKR " << formatBalance(oldBalance)
             << " to PKR " << formatBalance(balance) << endl;
        cout << "Undo completed successfully!" << endl;
    }

    void showUndoableTransactions() {
        if (undoStack.isEmpty()) {
            cout << "No transactions available to undo." << endl;
            return;
        }
        TransactionStack::StackNode* last = undoStack.peek();
        long now = time(nullptr);
        long timeDiff = now - last->timestamp;
        bool canUndo = (timeDiff <= SecurityConfig::UNDO_WINDOW);
        cout << "\n+-------------------------------------------------+" << endl;
        cout << "|              LAST TRANSACTION                  |" << endl;
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  Type: " << padString(last->type, 38) << "|" << endl;
        cout << "|  Amount: PKR " << padString(formatBalance(last->amount), 31) << "|" << endl;
        cout << "|  Time: " << padString(to_string(timeDiff) + " seconds ago", 37) << "|" << endl;
        if (canUndo) {
            cout << "|  Status: " << padString("CAN BE UNDONE", 35) << "|" << endl;
            cout << "|  Time left: " << padString(to_string(SecurityConfig::UNDO_WINDOW - timeDiff) + " seconds", 32) << "|" << endl;
        } else {
            cout << "|  Status: " << padString("EXPIRED - CANNOT UNDO", 35) << "|" << endl;
        }
        cout << "+-------------------------------------------------+" << endl;
    }

    string padString(string text, int width) {
        if ((int)text.length() >= width) {
            return text.substr(0, width);
        }
        return text + string(width - text.length(), ' ');
    }

    void displayAccountInfo() {
        cout << "+=================================================+" << endl;
        cout << "|                  ACCOUNT DETAILS                |" << endl;
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  Account No: " << padString(accountNumber, 31) << "|" << endl;
        cout << "|  Name: " << padString(name, 37) << "|" << endl;
        cout << "|  User ID: " << padString(userID, 34) << "|" << endl;
        cout << "|  Email: " << padString(email, 36) << "|" << endl;
        cout << "|  Phone: " << padString(phone, 36) << "|" << endl;
        cout << "|  Address: " << padString(address, 34) << "|" << endl;
        cout << "|  Account Type: " << padString(accountType, 29) << "|" << endl;
        cout << "|  Balance: PKR " << padString(formatBalance(balance), 30) << "|" << endl;
        cout << "|  Created: " << padString(dateCreated, 34) << "|" << endl;
        if (isLocked) {
            cout << "|  STATUS: ACCOUNT TEMPORARILY LOCKED            |" << endl;
        }
        cout << "+=================================================+" << endl;
    }

    void displayMiniInfo() {
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  Account: " << padString(accountNumber, 34) << "|" << endl;
        cout << "|  Name: " << padString(name, 37) << "|" << endl;
        cout << "|  Balance: PKR " << padString(formatBalance(balance), 30) << "|" << endl;
        if (isLocked) {
            cout << "|  STATUS: LOCKED (Too many failed attempts)     |" << endl;
        }
        cout << "+-------------------------------------------------+" << endl;
    }
   
    void addSecurityLog(const string &action, const string &details = "") {
        string id = "SEC" + to_string(time(nullptr)) + to_string(rand() % 1000);
        transactions.addTransaction(id, "SECURITY: " + action, 0, balance, details, time(nullptr));
    }

    int getTransactionCount() const { return transactions.getCount(); }
    bool hasTransactions() const { return transactions.getCount() > 0; }
   
    void displayTransactionHistory() {
        TransactionLinkedList::TransactionNode* current = transactions.getHead();
        if (current == nullptr) {
            cout << "\nNo transactions found." << endl;
            return;
        }
        cout << "\n+----------------------------------------------------------------------------------------+" << endl;
        cout << "|                            TRANSACTION HISTORY                                       |" << endl;
        cout << "+----------------------------------------------------------------------------------------+" << endl;
        while (current != nullptr) {
            time_t ts = static_cast<time_t>(current->timestamp);
            struct tm *ptm = localtime(&ts);
            char buf[32];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ptm);
            cout << "| " << padString(current->id, 12) << " | " << padString(current->type, 15)
                 << " | PKR " << padString(formatBalance(current->amount), 12)
                 << " | Bal: PKR " << padString(formatBalance(current->balanceAfter), 12)
                 << " | " << padString(buf, 19) << " |" << endl;
            if (!current->otherAccount.empty()) {
                cout << "|   -> Other Account: " << padString(current->otherAccount, 73) << " |" << endl;
            }
            current = current->next;
        }
        cout << "+----------------------------------------------------------------------------------------+" << endl;
    }

    void saveToFile(ofstream& file) {
        file << name << endl;
        file << userID << endl;
        file << password << endl;
        file << pin << endl;
        file << accountNumber << endl;
        file << email << endl;
        file << phone << endl;
        file << address << endl;
        file << accountType << endl;
        file << balance << endl;
        file << dateCreated << endl;
        file << loginAttempts << endl;
        file << lastLoginAttempt << endl;
        file << isLocked << endl;
        file << transactions.getCount() << endl;
        TransactionLinkedList::TransactionNode* current = transactions.getHead();
        while (current != nullptr) {
            file << current->id << endl;
            file << current->type << endl;
            file << current->amount << endl;
            file << current->balanceAfter << endl;
            file << current->otherAccount << endl;
            file << current->timestamp << endl;
            current = current->next;
        }
        file << undoStack.getCount() << endl;
        TransactionStack::StackNode* undoCurrent = undoStack.top;
        while (undoCurrent != nullptr) {
            file << undoCurrent->id << endl;
            file << undoCurrent->type << endl;
            file << undoCurrent->amount << endl;
            file << undoCurrent->balanceBefore << endl;
            file << undoCurrent->balanceAfter << endl;
            file << undoCurrent->otherAccount << endl;
            file << undoCurrent->timestamp << endl;
            undoCurrent = undoCurrent->next;
        }
    }

    void loadFromFile(ifstream& file) {
        getline(file, name);
        getline(file, userID);
        getline(file, password);
        getline(file, pin);
        getline(file, accountNumber);
        getline(file, email);
        getline(file, phone);
        getline(file, address);
        getline(file, accountType);
        file >> balance;
        file.ignore();
        getline(file, dateCreated);
        file >> loginAttempts;
        file >> lastLoginAttempt;
        file >> isLocked;
        file.ignore();
        transactions.clear();
        int count = 0;
        if (file >> count) {
            file.ignore();
            for (int i = 0; i < count; ++i) {
                string id, type, otherAccount;
                double amount, balanceAfter;
                long timestamp;
                getline(file, id);
                getline(file, type);
                file >> amount;
                file >> balanceAfter;
                file.ignore();
                getline(file, otherAccount);
                file >> timestamp;
                file.ignore();
                transactions.addTransaction(id, type, amount, balanceAfter, otherAccount, timestamp);
            }
        } else {
            file.clear();
        }
        undoStack.clear();
        int undoCount = 0;
        if (file >> undoCount) {
            file.ignore();
            for (int i = 0; i < undoCount; ++i) {
                string id, type, otherAccount;
                double amount, balanceBefore, balanceAfter;
                long timestamp;
                getline(file, id);
                getline(file, type);
                file >> amount;
                file >> balanceBefore;
                file >> balanceAfter;
                file.ignore();
                getline(file, otherAccount);
                file >> timestamp;
                file.ignore();
                undoStack.push(id, type, amount, balanceBefore, balanceAfter, otherAccount, timestamp);
            }
        } else {
            file.clear();
        }
    }
};

class BankingSystem {
public:
    User** users;
    int capacity;
    int userCount;
    int nextUserID;
    int invalidIDAttempts;
    PaymentPriorityQueue scheduledPayments;
    string dataFileName;

    BankingSystem() : capacity(10), userCount(0), nextUserID(1000), invalidIDAttempts(0), dataFileName("bank_data.txt") {
        users = new User*[capacity];
        for (int i = 0; i < capacity; i++) {
            users[i] = nullptr;
        }
        loadFromFile();
    }
   
    ~BankingSystem() {
        saveToFile();
        for (int i = 0; i < userCount; i++) {
            if (users[i] != nullptr) {
                delete users[i];
                users[i] = nullptr; 
            }
        }
        delete[] users;
    }

    bool isValidName(const string& name) {
        if (name.empty() || name.length() < 2 || name.length() > 50) return false;
        for (int i = 0; i < (int)name.length(); i++) {
            char c = name[i];
            if (!isalpha(c) && c != ' ' && c != '.' && c != '-') return false;
        }
        return true;
    }

    bool isValidEmail(const string& email) {
        if (email.empty() || email.length() > 50) return false;
        size_t at_pos = email.find('@');
        if (at_pos == string::npos) return false;
        size_t dot_pos = email.find('.', at_pos + 1);
        if (dot_pos == string::npos) return false;
        if (dot_pos <= at_pos + 1) return false;
        return true;
    }

    bool isValidPassword(const string& password) {
        if (password.empty() || password.length() < 4 || password.length() > 20) return false;
        return true;
    }

    bool isValidPIN(const string& pin) {
        if (pin.length() != 4) return false;
        for (int i = 0; i < 4; i++) {
            if (!isdigit(pin[i])) return false;
        }
        return true;
    }

    bool isValidPhone(const string& phone) {
        if (phone.empty() || phone.length() < 10 || phone.length() > 15) return false;
        for (int i = 0; i < (int)phone.length(); i++) {
            char c = phone[i];
            if (!isdigit(c) && c != '+' && c != '-' && c != ' ' && c != '(' && c != ')') return false;
        }
        return true;
    }

    bool isValidAmount(double amount) {
        if (amount < 0.0 || amount > 1000000.0) return false;
        return true;
    }

    bool isEmailUnique(const string& email) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->email == email) return false;
        }
        return true;
    }

    bool isUserIDUnique(const string& userID) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID) return false;
        }
        return true;
    }

    string formatBalance(double amount) {
        long cents = static_cast<long>(round(amount * 100));
        long whole = cents / 100;
        long rem = labs(cents % 100);
        char buf[64];
        sprintf(buf, "%ld.%02ld", whole, rem);
        return string(buf);
    }

    string padString(string text, int width) {
        if (text.length() >= (size_t)width) {
            return text.substr(0, width);
        }
        return text + string(width - text.length(), ' ');
    }

    void resizeArray() {
        int newCapacity = capacity * 2;
        User** newUsers = new User*[newCapacity];
        for (int i = 0; i < userCount; i++) {
            newUsers[i] = users[i];
        }
        for (int i = userCount; i < newCapacity; i++) {
            newUsers[i] = nullptr;
        }
        delete[] users;
        users = newUsers;
        capacity = newCapacity;
    }
   
    int findUserByAccountNumber(const string &accountNumber) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->accountNumber == accountNumber) return i;
        }
        return -1;
    }

    int findUserByUserID(const string &userID) {
        for (int i = 0; i < userCount; i++) {
            if (users[i]->userID == userID) return i;
        }
        return -1;
    }

    string generateUserID() {
        string id;
        do {
            id = "USER" + to_string(nextUserID);
            nextUserID++;
        } while (!isUserIDUnique(id));
        return id;
    }

    void addTransaction(User* user, const string &type, double amount, double balanceAfter, const string &otherAccount = "") {
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  Transaction Recorded:                          |" << endl;
        cout << "|  Type: " << padString(type, 37) << "|" << endl;
        cout << "|  Amount: PKR " << padString(formatBalance(amount), 30) << "|" << endl;
        if (!otherAccount.empty()) {
            cout << "|  Other: " << padString(otherAccount, 33) << "|" << endl;
        }
        cout << "|  New Balance: PKR " << padString(formatBalance(balanceAfter), 27) << "|" << endl;
        cout << "+-------------------------------------------------+" << endl;
    }

    bool verifyTransactionPIN(User* user) {
        string pin;
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  Enter your 4-digit PIN (0 to cancel): ";
        cin >> pin;
        if (pin == "0") {
            cout << "|  Transaction cancelled by user.                 |" << endl;
            cout << "+-------------------------------------------------+" << endl;
            return false;
        }
        if (user->verifyPIN(pin)) {
            return true;
        } else {
            cout << "|  Invalid PIN! ";
            if (user->isLocked) {
                cout << "Account temporarily locked due to too many failed attempts." << endl;
                cout << "|  Please try again after " << SecurityConfig::ACCOUNT_LOCKOUT_TIME << " seconds." << endl;
                user->addSecurityLog("ACCOUNT_LOCKED", "Too many failed PIN attempts");
            } else {
                int attemptsLeft = SecurityConfig::MAX_LOGIN_ATTEMPTS - user->loginAttempts;
                cout << attemptsLeft << " attempts remaining." << endl;
                user->addSecurityLog("FAILED_PIN_ATTEMPT", "Attempts: " + to_string(user->loginAttempts));
            }
            cout << "|  Transaction cancelled.                         |" << endl;
            cout << "+-------------------------------------------------+" << endl;
            return false;
        }
    }

    bool verifyPassword(User* user, const string& purpose = "login") {
        string password;
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  Enter your password for " << padString(purpose, 20) << "|" << endl;
        cout << "|  Password (0 to cancel): ";
        cin >> password;
        if (password == "0") {
            cout << "|  Operation cancelled by user.                   |" << endl;
            cout << "+-------------------------------------------------+" << endl;
            return false;
        }
        if (user->verifyPassword(password)) {
            return true;
        } else {
            cout << "|  Invalid password! ";
            if (user->isLocked) {
                cout << "Account temporarily locked." << endl;
                cout << "|  Please try again after " << SecurityConfig::ACCOUNT_LOCKOUT_TIME << " seconds." << endl;
                user->addSecurityLog("ACCOUNT_LOCKED", "Too many failed password attempts");
            } else {
                int attemptsLeft = SecurityConfig::MAX_LOGIN_ATTEMPTS - user->loginAttempts;
                cout << attemptsLeft << " attempts remaining." << endl;
                user->addSecurityLog("FAILED_PASSWORD_ATTEMPT", "Purpose: " + purpose);
            }
            cout << "|  Operation cancelled.                           |" << endl;
            cout << "+-------------------------------------------------+" << endl;
            return false;
        }
    }

    void processScheduledPayments() {
        long currentTime = time(nullptr);
        while (!scheduledPayments.isEmpty() && scheduledPayments.peek()->executeAt <= currentTime) {
            PaymentPriorityQueue::PQNode* payment = scheduledPayments.dequeue();
            int fromUserIndex = findUserByAccountNumber(payment->fromAccount);
            int toUserIndex = findUserByAccountNumber(payment->toAccount);
            if (fromUserIndex != -1 && toUserIndex != -1) {
                User* fromUser = users[fromUserIndex];
                User* toUser = users[toUserIndex];
                if (fromUser->balance >= payment->amount) {
                    fromUser->balance -= payment->amount;
                    toUser->balance += payment->amount;
                    fromUser->addTransactionRecord("SCHEDULED TRANSFER OUT", payment->amount, fromUser->balance, payment->toAccount);
                    toUser->addTransactionRecord("SCHEDULED TRANSFER IN", payment->amount, toUser->balance, payment->fromAccount);
                    cout << "Scheduled payment processed: " << payment->id << endl;
                }
            }
            delete payment;
        }
    }

    void displayMainMenu() {
        cout << "+-------------------------------------------------+" << endl;
        cout << "|                   MAIN MENU                    |" << endl;
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  1. Create New Account                         |" << endl;
        cout << "|  2. Login to Existing Account                  |" << endl;
        cout << "|  3. Exit System                                |" << endl;
        cout << "+-------------------------------------------------+" << endl;
    }

    void createAccount() {
        cout << "\n=== CREATE NEW ACCOUNT ===" << endl;
        string name, email, phone, address, accountType, password, pin;
        double initialBalance;
        User tempUser;
        while (true) {
            cin.ignore();
            cout << "Enter Full Name (or 0 to cancel): ";
            getline(cin, name);
            if (name == "0") {
                cout << "Account creation cancelled." << endl;
                return;
            }
            if (isValidName(name)) {
                break;
            } else {
                cout << "Invalid name! Name must be 2-50 characters with only letters, spaces, dots, and hyphens." << endl;
                cout << "Please try again." << endl;
            }
        }
        while (true) {
            cout << "Enter Email (or 0 to cancel): ";
            getline(cin, email);
            if (email == "0") {
                cout << "Account creation cancelled." << endl;
                return;
            }
            if (isValidEmail(email) && isEmailUnique(email)) {
                break;
            } else {
                if (!isValidEmail(email)) {
                    cout << "Invalid email format! Please enter a valid email." << endl;
                } else {
                    cout << "Email already exists! Please use a different email." << endl;
                }
                cout << "Please try again." << endl;
            }
        }
        while (true) {
            cout << "Enter Phone (or 0 to cancel): ";
            getline(cin, phone);
            if (phone == "0") {
                cout << "Account creation cancelled." << endl;
                return;
            }
            if (isValidPhone(phone)) {
                break;
            } else {
                cout << "Invalid phone number! Phone must be 10-15 digits with optional +, -, spaces, (, )." << endl;
                cout << "Please try again." << endl;
            }
        }
        while (true) {
            cout << "Enter Address (or 0 to cancel): ";
            getline(cin, address);
            if (address == "0") {
                cout << "Account creation cancelled." << endl;
                return;
            }
            if (!address.empty() && address.length() <= 100) {
                break;
            } else {
                cout << "Invalid address! Address cannot be empty and must be under 100 characters." << endl;
                cout << "Please try again." << endl;
            }
        }
        while (true) {
            cout << "Enter Account Type (Savings/Current) or 0 to cancel: ";
            getline(cin, accountType);
            if (accountType == "0") {
                cout << "Account creation cancelled." << endl;
                return;
            }
            if (accountType == "Savings" || accountType == "Current") {
                break;
            } else {
                cout << "Invalid account type! Please enter either 'Savings' or 'Current'." << endl;
                cout << "Please try again." << endl;
            }
        }
        while (true) {
            cout << "Enter Initial Balance: ";
            cin >> initialBalance;
            if (cin.fail() || !isValidAmount(initialBalance)) {
                cout << "Invalid amount! Amount must be between 0 and 1,000,000." << endl;
                cout << "Please try again." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
            } else {
                break;
            }
        }
        while (true) {
            cout << "Enter Password (or 0 to cancel): ";
            cin >> password;
            if (password == "0") {
                cout << "Account creation cancelled." << endl;
                return;
            }
            if (isValidPassword(password) && !tempUser.isWeakPassword(password)) {
                break;
            } else {
                cout << "Invalid password! Must be 4-20 characters and NOT weak." << endl;
                cout << "Weak passwords like '1234', '0000', 'password', or repeated characters are NOT allowed." << endl;
                cout << "Please try again." << endl;
            }
        }
        while (true) {
            cout << "Enter 4-digit PIN (or 0 to cancel): ";
            cin >> pin;
            if (pin == "0") {
                cout << "Account creation cancelled." << endl;
                return;
            }
            if (isValidPIN(pin) && !tempUser.isWeakPIN(pin)) {
                break;
            } else {
                cout << "Invalid PIN! Must be exactly 4 digits and NOT weak." << endl;
                cout << "Weak PINs like 0000, 1111, 1234, 4321, or repeated digits are NOT allowed." << endl;
                cout << "Please try again." << endl;
            }
        }
        string userID = generateUserID();
        User* newUser = new User(name, userID, password, pin, email, phone, address, accountType, initialBalance);
        if (userCount >= capacity) {
            resizeArray();
        }
        users[userCount] = newUser;
        userCount++;
        newUser->addTransactionRecord("ACCOUNT CREATION", initialBalance, initialBalance);
        newUser->addSecurityLog("ACCOUNT_CREATED");
        cout << "\n+=================================================+" << endl;
        cout << "|          ACCOUNT CREATED SUCCESSFULLY         |" << endl;
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  User ID: " << padString(userID, 34) << "|" << endl;
        cout << "|  Account No: " << padString(newUser->accountNumber, 31) << "|" << endl;
        cout << "+=================================================+" << endl;
    }

    User* login() {
        cout << "\n=== LOGIN ===" << endl;
        string userID, password;
        while (true) {
            cout << "Enter User ID (or 0 to cancel): ";
            cin >> userID;
            if (userID == "0") {
                cout << "Login cancelled." << endl;
                return nullptr;
            }
            int userIndex = findUserByUserID(userID);
            if (userIndex == -1) {
                invalidIDAttempts++;
                cout << "Invalid User ID! Please try again." << endl;
                if (invalidIDAttempts >= 3) {
                    cout << "Too many failed attempts. System will exit." << endl;
                    exit(0);
                }
            } else {
                break;
            }
        }
        User* user = users[findUserByUserID(userID)];
        while (true) {
            cout << "Enter Password (or 0 to cancel): ";
            cin >> password;
            if (password == "0") {
                cout << "Login cancelled." << endl;
                return nullptr;
            }
            if (user->verifyPassword(password)) {
                invalidIDAttempts = 0;
                user->addSecurityLog("LOGIN_SUCCESS");
                cout << "Login successful! Welcome " << user->name << endl;
                return user;
            } else {
                cout << "Invalid password! ";
                if (user->isLocked) {
                    cout << "Account temporarily locked. Please try again after " << SecurityConfig::ACCOUNT_LOCKOUT_TIME << " seconds." << endl;
                    return nullptr;
                } else {
                    int attemptsLeft = SecurityConfig::MAX_LOGIN_ATTEMPTS - user->loginAttempts;
                    cout << attemptsLeft << " attempts remaining." << endl;
                    if (attemptsLeft <= 0) {
                        return nullptr;
                    }
                }
            }
        }
    }

    void depositMoney(User* user) {
        cout << "\n=== DEPOSIT MONEY ===" << endl;
        double amount;
        bool validAmount = false;
        while (!validAmount) {
            cout << "Enter amount to deposit (0 to cancel): PKR ";
            cin >> amount;
            if (!cin.fail() && amount == 0) { 
                cout << "Deposit cancelled." << endl;
                return;
            }
            if (cin.fail() || !isValidAmount(amount)) {
                cout << "Invalid amount! Amount must be between 0 and 1,000,000." << endl;
                cout << "Please try again." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
            } else {
                validAmount = true;
            }
        }
        bool pinVerified = false;
        int pinAttempts = 0;
        while (!pinVerified && pinAttempts < 3) {
            if (verifyTransactionPIN(user)) {
                pinVerified = true;
            } else {
                pinAttempts++;
                if (pinAttempts < 3 && !user->isLocked) {
                    cout << "Try again? (y/n): ";
                    char choice;
                    cin >> choice;
                    if (choice != 'y' && choice != 'Y') {
                        break;
                    }
                }
            }
        }
        if (!pinVerified) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        double newBalance = user->balance + amount;
        user->addTransactionRecord("DEPOSIT", amount, newBalance);
        addTransaction(user, "DEPOSIT", amount, newBalance);
        user->addSecurityLog("DEPOSIT", "Amount: " + formatBalance(amount));
        saveToFile();
        cin.ignore(10000, '\n');
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    void withdrawMoney(User* user) {
        cout << "\n=== WITHDRAW MONEY ===" << endl;
        double amount;
        bool validAmount = false;
        while (!validAmount) {
            cout << "Enter amount to withdraw (0 to cancel): PKR ";
            cin >> amount;
            if (!cin.fail() && amount == 0) { 
                cout << "Withdrawal cancelled." << endl;
                return;
            }
            if (cin.fail() || !isValidAmount(amount)) {
                cout << "Invalid amount! Amount must be between 0 and 1,000,000." << endl;
                cout << "Please try again." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
            } else if (amount > user->balance) {
                cout << "Insufficient balance! Your balance is PKR " << formatBalance(user->balance) << endl;
                cout << "Please enter a smaller amount: ";
            } else {
                validAmount = true;
            }
        }
        bool pinVerified = false;
        int pinAttempts = 0;
        while (!pinVerified && pinAttempts < 3) {
            if (verifyTransactionPIN(user)) {
                pinVerified = true;
            } else {
                pinAttempts++;
                if (pinAttempts < 3 && !user->isLocked) {
                    cout << "Try again? (y/n): ";
                    char choice;
                    cin >> choice;
                    if (choice != 'y' && choice != 'Y') {
                        break;
                    }
                }
            }
        }
        if (!pinVerified) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        double newBalance = user->balance - amount;
        user->addTransactionRecord("WITHDRAW", amount, newBalance);
        addTransaction(user, "WITHDRAW", amount, newBalance);
        user->addSecurityLog("WITHDRAWAL", "Amount: " + formatBalance(amount));
        saveToFile();
        cin.ignore(10000, '\n');
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    void transferFunds(User* user) {
        cout << "\n=== TRANSFER FUNDS ===" << endl;
        string toAccount;
        double amount;
        bool validAccount = false;
        bool validAmount = false;
        while (!validAccount) {
            cout << "Enter recipient account number (or 0 to cancel): ";
            cin >> toAccount;
            if (toAccount == "0") { 
                cout << "Transfer cancelled." << endl;
                return;
            }
            int toUserIndex = findUserByAccountNumber(toAccount);
            if (toUserIndex == -1) {
                cout << "Recipient account not found! Please try again." << endl;
            } else if (toAccount == user->accountNumber) {
                cout << "Cannot transfer to your own account! Please enter a different account." << endl;
            } else {
                validAccount = true;
            }
        }
        while (!validAmount) {
            cout << "Enter amount to transfer (0 to cancel): PKR ";
            cin >> amount;
            if (!cin.fail() && amount == 0) { 
                cout << "Transfer cancelled." << endl;
                return;
            }
            if (cin.fail() || !isValidAmount(amount)) {
                cout << "Invalid amount! Amount must be between 0 and 1,000,000." << endl;
                cout << "Please try again." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
            } else if (amount > user->balance) {
                cout << "Insufficient balance! Your balance is PKR " << formatBalance(user->balance) << endl;
                cout << "Please enter a smaller amount: ";
            } else {
                validAmount = true;
            }
        }
        bool pinVerified = false;
        int pinAttempts = 0;
        while (!pinVerified && pinAttempts < 3) {
            if (verifyTransactionPIN(user)) {
                pinVerified = true;
            } else {
                pinAttempts++;
                if (pinAttempts < 3 && !user->isLocked) {
                    cout << "Try again? (y/n): ";
                    char choice;
                    cin >> choice;
                    if (choice != 'y' && choice != 'Y') {
                        break;
                    }
                }
            }
        }
        if (!pinVerified) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        User* toUser = users[findUserByAccountNumber(toAccount)];
        double userNewBalance = user->balance - amount;
        double toUserNewBalance = toUser->balance + amount;
        user->addTransactionRecord("TRANSFER OUT", amount, userNewBalance, toAccount);
        toUser->addTransactionRecord("TRANSFER IN", amount, toUserNewBalance, user->accountNumber);
        addTransaction(user, "TRANSFER OUT", amount, userNewBalance, toAccount);
        user->addSecurityLog("TRANSFER_OUT", "To: " + toAccount + " Amount: " + formatBalance(amount));
        toUser->addSecurityLog("TRANSFER_IN", "From: " + user->accountNumber + " Amount: " + formatBalance(amount));
        cout << "Transfer completed successfully to account: " << toAccount << endl;
        saveToFile();
        cin.ignore(10000, '\n');
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    void viewTransactionHistory(User* user) {
        cout << "\n=== TRANSACTION HISTORY ===" << endl;
        user->displayTransactionHistory();
        cin.ignore(10000, '\n');
        cout << "\nPress Enter to continue...";
        cin.get();
    }
   
    void undoLastTransaction(User* user) {
        cout << "\n=== UNDO LAST TRANSACTION ===" << endl;
        user->showUndoableTransactions();
        if (!user->canUndo()) {
            cin.ignore(10000, '\n');
            cout << "\nPress Enter to continue...";
            cin.get();
            return;
        } 
        bool pinVerified = false;
        int pinAttempts = 0;
        while (!pinVerified && pinAttempts < 3) {
            if (verifyTransactionPIN(user)) {
                pinVerified = true;
            } else {
                pinAttempts++;
                if (pinAttempts < 3 && !user->isLocked) {
                    cout << "Try again? (y/n): ";
                    char choice;
                    cin >> choice;
                    if (choice != 'y' && choice != 'Y') {
                        break;
                    }
                }
            }
        }
        if (!pinVerified) {
            cout << "Transaction cancelled." << endl;
            cin.ignore(10000, '\n');
            cout << "\nPress Enter to continue...";
            cin.get();
            return;
        }
        user->undoLastTransaction();
        saveToFile();
        cin.ignore(10000, '\n');
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    void schedulePayment(User* user) {
        cout << "\n=== SCHEDULE PAYMENT ===" << endl;
        string toAccount;
        double amount;
        int unitChoice;
        long long value;
        bool validAccount = false;
        bool validAmount = false;
        while (!validAccount) {
            cout << "Enter recipient account number (or 0 to cancel): ";
            cin >> toAccount;
            if (toAccount == "0") { 
                cout << "Payment scheduling cancelled." << endl;
                return;
            }
            int toUserIndex = findUserByAccountNumber(toAccount);
            if (toUserIndex == -1) {
                cout << "Recipient account not found! Please try again." << endl;
            } else if (toAccount == user->accountNumber) {
                cout << "Cannot schedule payment to your own account!" << endl;
            } else {
                validAccount = true;
            }
        }
        while (!validAmount) {
            cout << "Enter amount to schedule (0 to cancel): PKR ";
            cin >> amount;
            if (!cin.fail() && amount == 0) { 
                cout << "Payment scheduling cancelled." << endl;
                return;
            }
            if (cin.fail() || !isValidAmount(amount)) {
                cout << "Invalid amount! Please try again." << endl;
                cin.clear();
                cin.ignore(10000, '\n');
            } else if (amount > user->balance) {
                cout << "Insufficient balance! Your balance: PKR " << formatBalance(user->balance) << endl;
                cout << "Would you like to deposit money now? (yes/no): ";
                string choice;
                cin >> choice;
                if (choice == "yes" || choice == "y") {
                    depositMoney(user);
                    if (amount > user->balance) {
                        cout << "Still insufficient funds. Payment cancelled." << endl;
                        return;
                    } else {
                        validAmount = true;
                    }
                } else {
                    return;
                }
            } else {
                validAmount = true;
            }
        }
        if (amount > 5000) {
            cout << "Large payment detected. Additional verification required." << endl;
            if (!verifyPassword(user, "large scheduled payment")) {
                return;
            }
        }
        bool pinVerified = false;
        int pinAttempts = 0;
        while (!pinVerified && pinAttempts < 3) {
            if (verifyTransactionPIN(user)) {
                pinVerified = true;
            } else {
                pinAttempts++;
                if (pinAttempts < 3 && !user->isLocked) {
                    cout << "Try again? (y/n): ";
                    char choice;
                    cin >> choice;
                    if (choice != 'y' && choice != 'Y') {
                        break;
                    }
                }
            }
        }
        if (!pinVerified) {
            cout << "Transaction cancelled." << endl;
            return;
        }
        cout << "\n+-------------------------------------------------+" << endl;
        cout << "|         WHEN SHOULD THIS PAYMENT RUN?           |" << endl;
        cout << "+-------------------------------------------------+" << endl;
        cout << "|  1. After N minutes                             |" << endl;
        cout << "|  2. After N hours                               |" << endl;
        cout << "|  3. After N days                                |" << endl;
        cout << "|  4. After N months (approx 30 days each)        |" << endl;
        cout << "|  0. Cancel                                      |" << endl;
        cout << "+-------------------------------------------------+" << endl;
        while (true) {
            cout << "Choose option (0-4): ";
            cin >> unitChoice;
            if (unitChoice == 0) {
                cout << "Payment scheduling cancelled." << endl;
                return;
            }
            if (unitChoice >= 1 && unitChoice <= 4) break;
            cout << "Invalid choice!" << endl;
        }
        while (true) {
            cout << "Enter N (how many, or 0 to cancel): ";
            cin >> value;
            if (value == 0) { 
                cout << "Payment scheduling cancelled." << endl;
                return;
            }
            if (value > 0) break;
            cout << "Value must be positive." << endl;
        }
        long long offsetSeconds = 0;
        if (unitChoice == 1) offsetSeconds = value * 60;
        else if (unitChoice == 2) offsetSeconds = value * 60 * 60;
        else if (unitChoice == 3) offsetSeconds = value * 24 * 60 * 60;
        else if (unitChoice == 4) offsetSeconds = value * 30 * 24 * 60 * 60;
        long executeTime = time(nullptr) + offsetSeconds;
        string paymentID = "PAY" + to_string(time(nullptr)) + "_" + user->accountNumber + "_" + to_string(rand() % 10000);
        scheduledPayments.enqueue(paymentID, executeTime, user->accountNumber, toAccount, amount);
        cout << "\nPayment scheduled successfully!" << endl;
        cout << "Payment ID: " << paymentID << endl;
        cout << "Will execute after " << value << " ";
        if (unitChoice == 1) cout << "minute(s)";
        else if (unitChoice == 2) cout << "hour(s)";
        else if (unitChoice == 3) cout << "day(s)";
        else cout << "month(s)";
        cout << endl;
        user->addSecurityLog("PAYMENT_SCHEDULED", "To: " + toAccount);
        saveToFile();
        cin.ignore(10000, '\n');
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    void cancelScheduledPayment(User* user) {
        cout << "\n=== CANCEL SCHEDULED PAYMENT ===" << endl;
        bool hasPayments = false;
        PaymentPriorityQueue::PQNode* temp = scheduledPayments.getHead();
        while (temp != nullptr) {
            if (temp->fromAccount == user->accountNumber) {
                hasPayments = true;
                break;
            }
            temp = temp->next;
        }
        if (!hasPayments) {
            cout << "You have no scheduled payments to cancel." << endl;
            cin.ignore(10000, '\n');
            cout << "\nPress Enter to continue...";
            cin.get();
            return;
        }
        viewScheduledPayments(user);
        string paymentID;
        cout << "Enter Payment ID to cancel (or 0 to cancel): ";
        cin.ignore(); 
        getline(cin, paymentID);
        paymentID.erase(0, paymentID.find_first_not_of(" \t\n\r\f\v"));
        paymentID.erase(paymentID.find_last_not_of(" \t\n\r\f\v") + 1);
        if (paymentID == "0") {
            cout << "Cancellation aborted by user." << endl;
            cout << "\nPress Enter to continue...";
            cin.get();
            return;
        }
        if (paymentID.empty()) {
            cout << "No Payment ID entered. Cancellation aborted." << endl;
            cout << "\nPress Enter to continue...";
            cin.get();
            return;
        }
        cout << "Searching for Payment ID: '" << paymentID << "'" << endl;
        PaymentPriorityQueue newQueue;
        bool found = false;
        PaymentPriorityQueue::PQNode* current = scheduledPayments.getHead();
        while (current != nullptr) {
            string currentID = current->id;
            currentID.erase(0, currentID.find_first_not_of(" \t\n\r\f\v"));
            currentID.erase(currentID.find_last_not_of(" \t\n\r\f\v") + 1);
            if (currentID == paymentID && current->fromAccount == user->accountNumber) {
                found = true;
                cout << "Found payment: " << current->id << " - " << current->toAccount
                     << " - PKR " << formatBalance(current->amount) << endl;
            } else {
                newQueue.enqueue(current->id, current->executeAt, current->fromAccount,
                               current->toAccount, current->amount);
            }
            current = current->next;
        }
        scheduledPayments.clear();
        current = newQueue.getHead();
        while (current != nullptr) {
            scheduledPayments.enqueue(current->id, current->executeAt, current->fromAccount,
                                    current->toAccount, current->amount);
            current = current->next;
        }
        if (found) {
            cout << "\n Scheduled payment cancelled successfully!" << endl;
            cout << "Payment ID: " << paymentID << " has been removed." << endl;
            user->addSecurityLog("PAYMENT_CANCELLED", "Payment ID: " + paymentID);
            saveToFile();
        } else {
            cout << "\n Payment ID '" << paymentID << "' not found or you don't have permission to cancel it." << endl;
            cout << "Please check the Payment ID and try again." << endl;
            cout << "Make sure to copy the EXACT Payment ID shown in the list above." << endl;
        }
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    void updateProfile(User* user) {
        cout << "\n=== UPDATE PROFILE ===" << endl;
        if (!verifyPassword(user, "profile update")) {
            return;
        }
        int choice;
        do {
            cout << "\n+-------------------------------------------------+" << endl;
            cout << "|              UPDATE PROFILE                    |" << endl;
            cout << "+-------------------------------------------------+" << endl;
            cout << "|  1. Update Email                               |" << endl;
            cout << "|  2. Update Phone                               |" << endl;
            cout << "|  3. Update Address                             |" << endl;
            cout << "|  4. Update Password                            |" << endl;
            cout << "|  5. Update PIN                                 |" << endl;
            cout << "|  6. Back to Main Menu                          |" << endl;
            cout << "+-------------------------------------------------+" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();
            switch (choice) {
                case 1: {
                    string newEmail, currentEmail;
                    cout << "Enter current email for verification (or 0 to cancel): ";
                    getline(cin, currentEmail);
                    if (currentEmail == "0") {
                        cout << "Update cancelled." << endl;
                        break;
                    }
                    if (currentEmail != user->email) {
                        cout << "Email verification failed! Cannot update." << endl;
                        break;
                    }
                    cout << "Enter new email (or 0 to cancel): ";
                    getline(cin, newEmail);
                    if (newEmail == "0") {
                        cout << "Update cancelled." << endl;
                        break;
                    }
                    if (user->setEmail(newEmail)) {
                        cout << "Email updated successfully!" << endl;
                    } else {
                        cout << "Invalid email!" << endl;
                    }
                    break;
                }
                case 2: {
                    string newPhone, currentPhone;
                    cout << "Enter current phone for verification (or 0 to cancel): ";
                    getline(cin, currentPhone);
                    if (currentPhone == "0") {
                        cout << "Update cancelled." << endl;
                        break;
                    }
                    if (currentPhone != user->phone) {
                        cout << "Phone verification failed! Cannot update." << endl;
                        break;
                    }
                    cout << "Enter new phone (or 0 to cancel): ";
                    getline(cin, newPhone);
                    if (newPhone == "0") {
                        cout << "Update cancelled." << endl;
                        break;
                    }
                    if (user->setPhone(newPhone)) {
                        cout << "Phone updated successfully!" << endl;
                    } else {
                        cout << "Invalid phone!" << endl;
                    }
                    break;
                }
                case 3: {
                    string newAddress;
                    cout << "Enter new address (or 0 to cancel): ";
                    getline(cin, newAddress);
                    if (newAddress == "0") {
                        cout << "Update cancelled." << endl;
                        break;
                    }
                    if (user->setAddress(newAddress)) {
                        cout << "Address updated successfully!" << endl;
                    } else {
                        cout << "Invalid address!" << endl;
                    }
                    break;
                }
                case 4: {
                    cout << "For security, password updates require calling bank helpline." << endl;
                    cout << "Please call 1-800-BANK-HELP" << endl;
                    break;
                }
                case 5: {
                    cout << "For security, PIN updates require calling bank helpline." << endl;
                    cout << "Please call 1-800-BANK-HELP" << endl;
                    break;
                }
                case 6:
                    cout << "Returning to main menu..." << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        } while (choice != 6);
        cout << "\nPress Enter to continue...";
        cin.get();
    }

    void viewScheduledPayments(User* user) {
        cout << "\n=== SCHEDULED PAYMENTS ===" << endl;
        processScheduledPayments();
        PaymentPriorityQueue::PQNode* current = scheduledPayments.getHead();
        cout << "+----------------------------------------------------------------+" << endl;
        cout << "|                      SCHEDULED PAYMENTS                      |" << endl;
        cout << "+----------------------------------------------------------------+" << endl;
        bool foundUserPayments = false;
        int paymentCount = 0;
        while (current != nullptr) {
            if (current->fromAccount == user->accountNumber) {
                foundUserPayments = true;
                paymentCount++;
                string timeStr = ctime(&current->executeAt);
                timeStr = timeStr.substr(0, timeStr.length()-1); 
                cout << "| Payment #" << paymentCount << "                                          |" << endl;
                cout << "| Payment ID: " << current->id << endl;
                cout << "| To Account: " << current->toAccount << endl;
                cout << "| Amount: PKR " << formatBalance(current->amount) << endl;
                cout << "| Scheduled Time: " << timeStr << endl;
                cout << "+----------------------------------------------------------------+" << endl;
            }
            current = current->next;
        }
        if (!foundUserPayments) {
            cout << "|               No scheduled payments found.                 |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
        } else {
            cout << "| Total: " << paymentCount << " scheduled payment(s) found.               |" << endl;
            cout << "+----------------------------------------------------------------+" << endl;
        }
    }

    void saveToFile() {
        ofstream file(dataFileName);
        if (!file.is_open()) {
            cout << "Error saving data!" << endl;
            return;
        } 
        file << userCount << endl;
        file << nextUserID << endl;
        file << invalidIDAttempts << endl;
        for (int i = 0; i < userCount; i++) {
            users[i]->saveToFile(file);
        }
        file << scheduledPayments.size() << endl;
        PaymentPriorityQueue::PQNode* current = scheduledPayments.getHead();
        while (current != nullptr) {
            file << current->id << endl;
            file << current->executeAt << endl;
            file << current->fromAccount << endl;
            file << current->toAccount << endl;
            file << current->amount << endl;
            current = current->next;
        }
        file.close();
    }

    void loadFromFile() {
        ifstream file(dataFileName);
        if (!file.is_open()) {
            return;
        }
        int loadedUserCount;
        if (!(file >> loadedUserCount)) {
            file.close();
            return;
        }
        if (loadedUserCount < 0 || loadedUserCount > 10000) {
            file.close();
            return;
        }
        while (loadedUserCount >= capacity) {
            resizeArray();
        }
        userCount = loadedUserCount;
        if (!(file >> nextUserID)) {
            userCount = 0;
            file.close();
            return;
        }
        if (!(file >> invalidIDAttempts)) {
            userCount = 0;
            file.close();
            return;
        }
        file.ignore(); 
        for (int i = 0; i < userCount; i++) {
            if (file.eof()) {
                userCount = i; 
                break;
            }
            if (users[i] == nullptr) {
                users[i] = new User();
            }
            try {
                users[i]->loadFromFile(file);
            } catch (...) {
                delete users[i];
                users[i] = nullptr;
                break;
            }
        }
        int scheduledCount = 0;
        if (file >> scheduledCount) {
            file.ignore();
            for (int i = 0; i < scheduledCount; i++) {
                if (file.eof()) break;
                string id, fromAccount, toAccount;
                long executeAt;
                double amount;
                getline(file, id);
                if (file.eof()) break;
                if (!(file >> executeAt)) break;
                file.ignore();
                getline(file, fromAccount);
                if (file.eof()) break;
                getline(file, toAccount);
                if (file.eof()) break;
                if (!(file >> amount)) break;
                file.ignore();
                scheduledPayments.enqueue(id, executeAt, fromAccount, toAccount, amount);
            }
        }
        file.close();
    }
};

void displayBanner() {
    cout << endl;
    cout << "***************************************************" << endl;
    cout << "*        SMART BANKING WALLET SYSTEM             *" << endl;
    cout << "***************************************************" << endl;
}

void displayExitMessage() {
    cout << endl;
    cout << "===================================================" << endl;
    cout << "           THANK YOU FOR BANKING WITH US!         " << endl;
    cout << "                                                  " << endl;
    cout << "             See you again soon!                  " << endl;
    cout << "===================================================" << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    BankingSystem bankSystem;
    int choice;
    do {
        bankSystem.processScheduledPayments();
        displayBanner();
        bankSystem.displayMainMenu();
        cout << "Enter your choice (1-3): ";
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            choice = 0;
        }
        if (choice == 1) {
            bankSystem.createAccount();
        } else if (choice == 2) {
            User* loggedInUser = bankSystem.login();
            if (loggedInUser != nullptr) {
                int dashChoice;
                do {
                    bankSystem.processScheduledPayments();
                    cout << endl;
                    cout << "===================================================" << endl;
                    cout << "               WELCOME DASHBOARD                  " << endl;
                    cout << "===================================================" << endl;
                    loggedInUser->displayMiniInfo();
                    cout << "\n";
                    cout << "+-------------------------------------------------+" << endl;
                    cout << "|              BANKING OPERATIONS                |" << endl;
                    cout << "+-------------------------------------------------+" << endl;
                    cout << "|  1. Deposit Money                               |" << endl;
                    cout << "|  2. Withdraw Money                              |" << endl;
                    cout << "|  3. Transfer Funds                              |" << endl;
                    cout << "|  4. View Account Details                        |" << endl;
                    cout << "|  5. View Transaction History                    |" << endl;
                    cout << "|  6. Undo Last Transaction                       |" << endl;
                    cout << "|  7. Schedule Payment                            |" << endl;
                    cout << "|  8. View Scheduled Payments                     |" << endl;
                    cout << "|  9. Cancel Scheduled Payment                    |" << endl;
                    cout << "| 10. Update Profile Information                  |" << endl;
                    cout << "| 11. Log Out                                     |" << endl;
                    cout << "+-------------------------------------------------+" << endl;
                    cout << "Enter your choice (1-11): ";
                    cin >> dashChoice;
                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(10000, '\n');
                        dashChoice = 0; 
                    }
                    if (dashChoice == 1) {
                        bankSystem.depositMoney(loggedInUser);
                    } else if (dashChoice == 2) {
                        bankSystem.withdrawMoney(loggedInUser);
                    } else if (dashChoice == 3) {
                        bankSystem.transferFunds(loggedInUser);
                    } else if (dashChoice == 4) {
                        loggedInUser->displayAccountInfo();
                    } else if (dashChoice == 5) {
                        bankSystem.viewTransactionHistory(loggedInUser);
                    } else if (dashChoice == 6) {
                        bankSystem.undoLastTransaction(loggedInUser);
                    } else if (dashChoice == 7) {
                        bankSystem.schedulePayment(loggedInUser);
                    } else if (dashChoice == 8) {
                        bankSystem.viewScheduledPayments(loggedInUser);
                    } else if (dashChoice == 9) {
                        bankSystem.cancelScheduledPayment(loggedInUser);
                    } else if (dashChoice == 10) {
                        bankSystem.updateProfile(loggedInUser);
                    } else if (dashChoice == 11) {
                        cout << "\nLogging out..." << endl;
                        loggedInUser->addSecurityLog("LOGOUT");
                    }
                    else {
                        cout << "\nInvalid choice! Please enter a number between 1-11." << endl;
                    }
                    if (dashChoice != 11) {
                        cin.ignore(10000, '\n');
                        cout << "\nPress Enter to continue...";
                        cin.get();
                    }
                } while (dashChoice != 11);
            }
        } else if (choice == 3) {
            displayExitMessage();
        } else {
            cout << "\nInvalid choice! Please enter 1, 2, or 3." << endl;
        }
        if (choice != 3) {
            cin.ignore(10000, '\n');
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while(choice != 3);
    return 0;
}