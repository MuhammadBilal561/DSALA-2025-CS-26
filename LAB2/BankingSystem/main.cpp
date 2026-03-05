// ========================================================
//  SECURE FILE-BASED BANKING SYSTEM
//  Simple Single-File Version for 3rd Semester Students
// ========================================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// --------------------------------------------------------
// PERMISSION FLAGS (Bitwise)
//
// Think of permissions as ON/OFF switches stored in one number.
//
//   Bit:   3    2    1    0
//          VIP  Transfer  Deposit  Withdraw
//          8    4    2    1
//
//   Example: permissions = 7 means 0111 in binary
//            = Withdraw(1) + Deposit(2) + Transfer(4)
// --------------------------------------------------------

const unsigned int PERM_WITHDRAW = 1;   // 0001
const unsigned int PERM_DEPOSIT  = 2;   // 0010
const unsigned int PERM_TRANSFER = 4;   // 0100
const unsigned int PERM_VIP      = 8;   // 1000

// --------------------------------------------------------
// BASE CLASS: Account (Abstract)
//
// "Abstract" means you CANNOT create an object of this class
// directly. You MUST use SavingsAccount or CurrentAccount.
// The "= 0" makes a function "pure virtual".
// --------------------------------------------------------

class Account
{
protected:    // accessible in this class AND child classes
    int accountId;
    string name;
    double balance;
    unsigned int permissions;
    vector<double> transactions;   // stores history

public:
    // Constructor
    Account(int id, string n, double bal, unsigned int perms)
    {
        accountId = id;
        name = n;
        balance = bal;
        permissions = perms;
    }

    // Default constructor (needed for loading from file)
    Account()
    {
        accountId = 0;
        name = "";
        balance = 0;
        permissions = 0;
    }

    // Pure virtual functions (child classes MUST override these)
    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual string getType() = 0;

    // Virtual destructor (needed when using pointers)
    virtual ~Account() {}

    // --- Simple Getters ---
    int getId()               { return accountId; }
    string getName()          { return name; }
    double getBalance()       { return balance; }
    unsigned int getPerms()   { return permissions; }

    vector<double>& getTransactions() { return transactions; }

    // --- Display Account Info ---
    void display()
    {
        cout << "\n  ================================" << endl;
        cout << "  Type        : " << getType() << endl;
        cout << "  Account ID  : " << accountId << endl;
        cout << "  Name        : " << name << endl;
        cout << "  Balance     : Rs. " << balance << endl;
        cout << "  Permissions : " << permissions << " -> ";

        // Check each permission using BITWISE AND
        if (permissions & PERM_WITHDRAW)  cout << "[Withdraw] ";
        if (permissions & PERM_DEPOSIT)   cout << "[Deposit] ";
        if (permissions & PERM_TRANSFER)  cout << "[Transfer] ";
        if (permissions & PERM_VIP)       cout << "[VIP] ";
        cout << endl;

        // Show transaction history
        cout << "  Transactions: ";
        if (transactions.empty())
        {
            cout << "None";
        }
        else
        {
            for (int i = 0; i < transactions.size(); i++)
            {
                if (transactions[i] >= 0)
                    cout << "+" << transactions[i];
                else
                    cout << transactions[i];

                if (i < transactions.size() - 1)
                    cout << ", ";
            }
        }
        cout << endl;
        cout << "  ================================" << endl;
    }

    // --- Transfer money to another account ---
    void transferTo(Account* other, double amount)
    {
        // Check transfer permission using BITWISE AND
        if (!(permissions & PERM_TRANSFER))
        {
            cout << "  [DENIED] You don't have Transfer permission!" << endl;
            return;
        }
        if (amount <= 0)
        {
            cout << "  [ERROR] Invalid amount!" << endl;
            return;
        }
        if (balance < amount)
        {
            cout << "  [ERROR] Not enough balance!" << endl;
            return;
        }

        balance -= amount;
        transactions.push_back(-amount);

        other->balance += amount;
        other->transactions.push_back(amount);

        cout << "  [OK] Transferred Rs. " << amount
             << " from " << name << " to " << other->name << endl;
    }

    // --- Save this account to file ---
    void saveToFile(ofstream& out)
    {
        out << "ACCOUNT " << getType() << endl;
        out << accountId << " " << name << " "
            << balance << " " << permissions << endl;
        out << "TRANSACTIONS" << endl;

        for (int i = 0; i < transactions.size(); i++)
        {
            out << transactions[i] << endl;
        }

        out << "END" << endl;
    }

    // --- Load this account from file ---
    void loadFromFile(ifstream& in)
    {
        in >> accountId >> name >> balance >> permissions;

        string line;
        getline(in, line);   // skip rest of current line
        getline(in, line);   // read "TRANSACTIONS"

        transactions.clear();

        while (getline(in, line))
        {
            if (line == "END")
                break;
            if (line.empty())
                continue;

            double val = stod(line);   // string to double
            transactions.push_back(val);
        }
    }
};

// --------------------------------------------------------
// CHILD CLASS 1: SavingsAccount
// Inherits everything from Account.
// Adds interest calculation.
// --------------------------------------------------------

class SavingsAccount : public Account
{
public:
    SavingsAccount(int id, string n, double bal, unsigned int perms)
        : Account(id, n, bal, perms) {}     // call parent constructor

    SavingsAccount() : Account() {}          // default

    ~SavingsAccount() {}

    string getType()
    {
        return "Savings";
    }

    void deposit(double amount)
    {
        // BITWISE AND to check permission
        if (!(permissions & PERM_DEPOSIT))
        {
            cout << "  [DENIED] Deposit not allowed!" << endl;
            return;
        }
        if (amount <= 0)
        {
            cout << "  [ERROR] Invalid amount!" << endl;
            return;
        }

        balance += amount;
        transactions.push_back(amount);    // positive = deposit
        cout << "  [OK] Deposited Rs. " << amount
             << " | Balance: Rs. " << balance << endl;
    }

    void withdraw(double amount)
    {
        // BITWISE AND to check permission
        if (!(permissions & PERM_WITHDRAW))
        {
            cout << "  [DENIED] Withdraw not allowed!" << endl;
            return;
        }
        if (amount <= 0)
        {
            cout << "  [ERROR] Invalid amount!" << endl;
            return;
        }
        if (amount > balance)
        {
            cout << "  [ERROR] Not enough balance!" << endl;
            return;
        }

        balance -= amount;
        transactions.push_back(-amount);   // negative = withdrawal
        cout << "  [OK] Withdrew Rs. " << amount
             << " | Balance: Rs. " << balance << endl;
    }

    // BONUS: Interest
    void applyInterest()
    {
        double interest = balance * 0.05;   // 5%
        balance += interest;
        transactions.push_back(interest);
        cout << "  [OK] Interest Rs. " << interest << " added!" << endl;
    }
};

// --------------------------------------------------------
// CHILD CLASS 2: CurrentAccount
// Inherits from Account.
// Adds overdraft (can go negative up to a limit).
// --------------------------------------------------------

class CurrentAccount : public Account
{
private:
    double overdraftLimit;

public:
    CurrentAccount(int id, string n, double bal, unsigned int perms)
        : Account(id, n, bal, perms)
    {
        overdraftLimit = 1000;   // can go Rs.1000 below zero
    }

    CurrentAccount() : Account()
    {
        overdraftLimit = 1000;
    }

    ~CurrentAccount() {}

    string getType()
    {
        return "Current";
    }

    void deposit(double amount)
    {
        if (!(permissions & PERM_DEPOSIT))
        {
            cout << "  [DENIED] Deposit not allowed!" << endl;
            return;
        }
        if (amount <= 0)
        {
            cout << "  [ERROR] Invalid amount!" << endl;
            return;
        }

        balance += amount;
        transactions.push_back(amount);
        cout << "  [OK] Deposited Rs. " << amount
             << " | Balance: Rs. " << balance << endl;
    }

    void withdraw(double amount)
    {
        if (!(permissions & PERM_WITHDRAW))
        {
            cout << "  [DENIED] Withdraw not allowed!" << endl;
            return;
        }
        if (amount <= 0)
        {
            cout << "  [ERROR] Invalid amount!" << endl;
            return;
        }

        // Overdraft: allow balance to go negative up to limit
        if ((balance - amount) < -overdraftLimit)
        {
            cout << "  [ERROR] Exceeds overdraft limit of Rs. "
                 << overdraftLimit << endl;
            return;
        }

        balance -= amount;
        transactions.push_back(-amount);
        cout << "  [OK] Withdrew Rs. " << amount
             << " | Balance: Rs. " << balance << endl;
    }
};


// ========================================================
//  GLOBAL VARIABLES
// ========================================================

vector<Account*> accounts;   // stores ALL accounts as pointers
int nextId = 1001;           // auto-increment ID


// ========================================================
//  HELPER: Find account by ID
// ========================================================

Account* findAccount(int id)
{
    for (int i = 0; i < accounts.size(); i++)
    {
        if (accounts[i]->getId() == id)
            return accounts[i];
    }
    return NULL;
}


// ========================================================
//  FUNCTION: Create Account
// ========================================================

void createAccount()
{
    string name;
    double balance;
    int type;
    char ch;

    cout << "\n  --- Create New Account ---" << endl;
    cout << "  1. Savings" << endl;
    cout << "  2. Current" << endl;
    cout << "  Choose type: ";
    cin >> type;

    cout << "  Enter name (no spaces): ";
    cin >> name;

    cout << "  Enter initial balance: ";
    cin >> balance;

    // BUILD PERMISSIONS USING BITWISE OR
    unsigned int perms = 0;

    cout << "  Allow Withdraw? (y/n): ";
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        perms = perms | PERM_WITHDRAW;     // turns ON bit 0

    cout << "  Allow Deposit?  (y/n): ";
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        perms = perms | PERM_DEPOSIT;      // turns ON bit 1

    cout << "  Allow Transfer? (y/n): ";
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        perms = perms | PERM_TRANSFER;     // turns ON bit 2

    cout << "  VIP Account?    (y/n): ";
    cin >> ch;
    if (ch == 'y' || ch == 'Y')
        perms = perms | PERM_VIP;          // turns ON bit 3

    // DYNAMIC MEMORY: create object with 'new'
    // Store as Account* (base class pointer) = POLYMORPHISM
    Account* newAcc = NULL;

    if (type == 1)
        newAcc = new SavingsAccount(nextId, name, balance, perms);
    else
        newAcc = new CurrentAccount(nextId, name, balance, perms);

    accounts.push_back(newAcc);

    cout << "\n  [OK] Account created! ID: " << nextId << endl;
    cout << "  Permission value: " << perms << endl;

    nextId++;
}


// ========================================================
//  FUNCTION: Deposit
// ========================================================

void doDeposit()
{
    int id;
    double amount;

    cout << "  Enter Account ID: ";
    cin >> id;

    Account* acc = findAccount(id);
    if (acc == NULL)
    {
        cout << "  [ERROR] Account not found!" << endl;
        return;
    }

    cout << "  Enter amount: ";
    cin >> amount;

    acc->deposit(amount);   // POLYMORPHISM: calls correct version
}


// ========================================================
//  FUNCTION: Withdraw
// ========================================================

void doWithdraw()
{
    int id;
    double amount;

    cout << "  Enter Account ID: ";
    cin >> id;

    Account* acc = findAccount(id);
    if (acc == NULL)
    {
        cout << "  [ERROR] Account not found!" << endl;
        return;
    }

    cout << "  Enter amount: ";
    cin >> amount;

    acc->withdraw(amount);   // POLYMORPHISM
}


// ========================================================
//  FUNCTION: Transfer
// ========================================================

void doTransfer()
{
    int fromId, toId;
    double amount;

    cout << "  Enter Sender ID: ";
    cin >> fromId;
    cout << "  Enter Receiver ID: ";
    cin >> toId;

    Account* from = findAccount(fromId);
    Account* to = findAccount(toId);

    if (from == NULL || to == NULL)
    {
        cout << "  [ERROR] Account not found!" << endl;
        return;
    }

    cout << "  Enter amount: ";
    cin >> amount;

    from->transferTo(to, amount);
}


// ========================================================
//  FUNCTION: Show Account
// ========================================================

void showAccount()
{
    int id;
    cout << "  Enter Account ID: ";
    cin >> id;

    Account* acc = findAccount(id);
    if (acc == NULL)
    {
        cout << "  [ERROR] Account not found!" << endl;
        return;
    }

    acc->display();
}


// ========================================================
//  FUNCTION: Show All Accounts
// ========================================================

void showAll()
{
    if (accounts.empty())
    {
        cout << "  No accounts yet!" << endl;
        return;
    }

    for (int i = 0; i < accounts.size(); i++)
    {
        accounts[i]->display();
    }
}


// ========================================================
//  FUNCTION: Save All Accounts to File
// ========================================================

void saveToFile()
{
    ofstream out("accounts.txt");

    if (!out.is_open())
    {
        cout << "  [ERROR] Cannot open file!" << endl;
        return;
    }

    out << accounts.size() << endl;

    for (int i = 0; i < accounts.size(); i++)
    {
        accounts[i]->saveToFile(out);    // POLYMORPHISM
    }

    out.close();
    cout << "  [OK] Saved " << accounts.size() << " accounts!" << endl;
}


// ========================================================
//  FUNCTION: Load All Accounts from File
// ========================================================

void loadFromFile()
{
    ifstream in("accounts.txt");

    if (!in.is_open())
    {
        cout << "  [ERROR] No saved file found!" << endl;
        return;
    }

    // First delete old accounts to avoid MEMORY LEAK
    for (int i = 0; i < accounts.size(); i++)
    {
        delete accounts[i];
    }
    accounts.clear();

    int count;
    in >> count;

    for (int i = 0; i < count; i++)
    {
        string word, type;
        in >> word >> type;    // reads "ACCOUNT" and "Savings"/"Current"

        Account* acc = NULL;

        if (type == "Savings")
            acc = new SavingsAccount();
        else
            acc = new CurrentAccount();

        acc->loadFromFile(in);
        accounts.push_back(acc);

        // Keep nextId updated
        if (acc->getId() >= nextId)
            nextId = acc->getId() + 1;
    }

    in.close();
    cout << "  [OK] Loaded " << accounts.size() << " accounts!" << endl;
}


// ========================================================
//  FUNCTION: Monthly Summary (Uses C-Style Arrays)
// ========================================================

void monthlySummary()
{
    // REQUIREMENT: Must use arrays
    double deposits[12];
    double withdrawals[12];

    // Initialize to zero
    for (int i = 0; i < 12; i++)
    {
        deposits[i] = 0;
        withdrawals[i] = 0;
    }

    // Spread transactions across months (round-robin)
    for (int i = 0; i < accounts.size(); i++)
    {
        vector<double>& txns = accounts[i]->getTransactions();

        for (int j = 0; j < txns.size(); j++)
        {
            int month = j % 12;

            if (txns[j] > 0)
                deposits[month] += txns[j];
            else
                withdrawals[month] += txns[j];
        }
    }

    string monthNames[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    cout << "\n  Month   Deposits   Withdrawals   Net" << endl;
    cout << "  -----   --------   -----------   ----" << endl;

    for (int i = 0; i < 12; i++)
    {
        if (deposits[i] != 0 || withdrawals[i] != 0)
        {
            double net = deposits[i] + withdrawals[i];
            cout << "  " << monthNames[i]
                 << "\t  " << deposits[i]
                 << "\t   " << withdrawals[i]
                 << "\t    " << net << endl;
        }
    }
}


// ========================================================
//  FUNCTION: Transaction Compression Demo (Bitwise)
// ========================================================

void compressionDemo()
{
    cout << "\n  === TRANSACTION COMPRESSION DEMO ===" << endl;
    cout << "  (Packing type + amount into one number)\n" << endl;

    // ENCODE: shift type left 28 bits, OR with amount
    unsigned int type1 = 1;      // 1 = Deposit
    unsigned int amount1 = 5000;
    unsigned int encoded1 = (type1 << 28) | (amount1 & 0x0FFFFFFF);

    // DECODE: shift right to get type, mask to get amount
    unsigned int decodedType1   = encoded1 >> 28;
    unsigned int decodedAmount1 = encoded1 & 0x0FFFFFFF;

    cout << "  Deposit Rs.5000" << endl;
    cout << "  Encoded number : " << encoded1 << endl;
    cout << "  Decoded type   : " << decodedType1 << " (1=Deposit)" << endl;
    cout << "  Decoded amount : " << decodedAmount1 << endl;

    cout << endl;

    // Another example: Withdrawal of 2500
    unsigned int type2 = 2;      // 2 = Withdrawal
    unsigned int amount2 = 2500;
    unsigned int encoded2 = (type2 << 28) | (amount2 & 0x0FFFFFFF);

    unsigned int decodedType2   = encoded2 >> 28;
    unsigned int decodedAmount2 = encoded2 & 0x0FFFFFFF;

    cout << "  Withdrawal Rs.2500" << endl;
    cout << "  Encoded number : " << encoded2 << endl;
    cout << "  Decoded type   : " << decodedType2 << " (2=Withdrawal)" << endl;
    cout << "  Decoded amount : " << decodedAmount2 << endl;

    cout << "\n  =====================================" << endl;
}


// ========================================================
//  FUNCTION: Cleanup Memory
// ========================================================

void cleanupMemory()
{
    for (int i = 0; i < accounts.size(); i++)
    {
        delete accounts[i];     // free heap memory
        accounts[i] = NULL;     // safety
    }
    accounts.clear();
}


// ========================================================
//  MAIN FUNCTION
// ========================================================

int main()
{
    int choice;

    cout << "========================================" << endl;
    cout << "   SECURE BANKING SYSTEM (C++)" << endl;
    cout << "========================================" << endl;

    do
    {
        cout << "\n  -------- MENU --------" << endl;
        cout << "  1.  Create Account" << endl;
        cout << "  2.  Deposit" << endl;
        cout << "  3.  Withdraw" << endl;
        cout << "  4.  Transfer" << endl;
        cout << "  5.  Show Account" << endl;
        cout << "  6.  Show All Accounts" << endl;
        cout << "  7.  Save to File" << endl;
        cout << "  8.  Load from File" << endl;
        cout << "  9.  Monthly Summary" << endl;
        cout << "  10. Compression Demo" << endl;
        cout << "  0.  Exit" << endl;
        cout << "  ----------------------" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:  createAccount();    break;
            case 2:  doDeposit();        break;
            case 3:  doWithdraw();       break;
            case 4:  doTransfer();       break;
            case 5:  showAccount();      break;
            case 6:  showAll();          break;
            case 7:  saveToFile();       break;
            case 8:  loadFromFile();     break;
            case 9:  monthlySummary();   break;
            case 10: compressionDemo();  break;
            case 0:
                cout << "\n  Goodbye! Cleaning up memory..." << endl;
                break;
            default:
                cout << "  Invalid choice!" << endl;
        }

    } while (choice != 0);

    // FREE ALL DYNAMIC MEMORY before program ends
    cleanupMemory();

    return 0;
}