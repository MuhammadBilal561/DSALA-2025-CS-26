//2025(S)-CS-26

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// PERMISSION FLAGS (Bitwise)


const unsigned int PERM_WITHDRAW = 1;   
const unsigned int PERM_DEPOSIT  = 2;   
const unsigned int PERM_TRANSFER = 4;   
const unsigned int PERM_VIP      = 8;   

// ABSTRACT CLASS  

class Account
{
protected:        int accountId;
    string name;
    double balance;
    unsigned int permissions;
    vector<double> transactions;   

public:
    Account(int id, string n, double bal, unsigned int perms)
    {
        accountId = id;
        name = n;
        balance = bal;
        permissions = perms;
    }

    // Default 
    Account()
    {
        accountId = 0;
        name = "";
        balance = 0;
        permissions = 0;
    }

    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual string getType() = 0;

    // Virtual destructor 
    virtual ~Account() {}

    int getId()               { return accountId; }
    string getName()          { return name; }
    double getBalance()       { return balance; }
    unsigned int getPerms()   { return permissions; }

    vector<double>& getTransactions() { return transactions; }

    void display()
    {
        cout << "\n  ================================" << endl;
        cout << "  Type        : " << getType() << endl;
        cout << "  Account ID  : " << accountId << endl;
        cout << "  Name        : " << name << endl;
        cout << "  Balance     : Rs. " << balance << endl;
        cout << "  Permissions : " << permissions << " -> ";

        if (permissions & PERM_WITHDRAW)  cout << "[Withdraw] ";
        if (permissions & PERM_DEPOSIT)   cout << "[Deposit] ";
        if (permissions & PERM_TRANSFER)  cout << "[Transfer] ";
        if (permissions & PERM_VIP)       cout << "[VIP] ";
        cout << endl;

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

    void transferTo(Account* other, double amount)
    {
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

    void loadFromFile(ifstream& in)
    {
        in >> accountId >> name >> balance >> permissions;

        string line;
        getline(in, line);   
        getline(in, line); 

        transactions.clear();

        while (getline(in, line))
        {
            if (line == "END")
                break;
            if (line.empty())
                continue;

            double val = stod(line);  
            transactions.push_back(val);
        }
    }
};



class SavingsAccount : public Account
{
public:
    SavingsAccount(int id, string n, double bal, unsigned int perms)
        : Account(id, n, bal, perms) {}    

    SavingsAccount() : Account() {}          

    ~SavingsAccount() {}

    string getType()
    {
        return "Savings";
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
        if (amount > balance)
        {
            cout << "  [ERROR] Not enough balance!" << endl;
            return;
        }

        balance -= amount;
        transactions.push_back(-amount);  
        cout << "  [OK] Withdrew Rs. " << amount
             << " | Balance: Rs. " << balance << endl;
    }

    void applyInterest()
    {
        double interest = balance * 0.05;  
        balance += interest;
        transactions.push_back(interest);
        cout << "  [OK] Interest Rs. " << interest << " added!" << endl;
    }
};


class CurrentAccount : public Account
{
private:
    double overdraftLimit;

public:
    CurrentAccount(int id, string n, double bal, unsigned int perms)
        : Account(id, n, bal, perms)
    {
        overdraftLimit = 1000;   
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




vector<Account*> accounts;   
int nextId = 1001;           



Account* findAccount(int id)
{
    for (int i = 0; i < accounts.size(); i++)
    {
        if (accounts[i]->getId() == id)
            return accounts[i];
    }
    return NULL;
}



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




void loadFromFile()
{
    ifstream in("accounts.txt");

    if (!in.is_open())
    {
        cout << "  [ERROR] No saved file found!" << endl;
        return;
    }

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
        in >> word >> type;    

        Account* acc = NULL;

        if (type == "Savings")
            acc = new SavingsAccount();
        else
            acc = new CurrentAccount();

        acc->loadFromFile(in);
        accounts.push_back(acc);

        if (acc->getId() >= nextId)
            nextId = acc->getId() + 1;
    }

    in.close();
    cout << "  [OK] Loaded " << accounts.size() << " accounts!" << endl;
}




void monthlySummary()
{
    double deposits[12];
    double withdrawals[12];

    for (int i = 0; i < 12; i++)
    {
        deposits[i] = 0;
        withdrawals[i] = 0;
    }

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



void compressionDemo()
{
    cout << "\n  === TRANSACTION COMPRESSION DEMO ===" << endl;
    cout << "  (Packing type + amount into one number)\n" << endl;

    unsigned int type1 = 1;      // 1 = Deposit
    unsigned int amount1 = 5000;
    unsigned int encoded1 = (type1 << 28) | (amount1 & 0x0FFFFFFF);


    unsigned int decodedType1   = encoded1 >> 28;
    unsigned int decodedAmount1 = encoded1 & 0x0FFFFFFF;

    cout << "  Deposit Rs.5000" << endl;
    cout << "  Encoded number : " << encoded1 << endl;
    cout << "  Decoded type   : " << decodedType1 << " (1=Deposit)" << endl;
    cout << "  Decoded amount : " << decodedAmount1 << endl;

    cout << endl;

    unsigned int type2 = 2;      
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




void cleanupMemory()
{
    for (int i = 0; i < accounts.size(); i++)
    {
        delete accounts[i];     // free heap memory
        accounts[i] = NULL;     // safety
    }
    accounts.clear();
}




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
                cout << "\n  Cleaning up memory..." << endl;
                break;
            default:
                cout << "  Invalid choice!" << endl;
        }

    } while (choice != 0);

    // FREE ALL DYNAMIC MEMORY before program ends
    cleanupMemory();

    return 0;
}