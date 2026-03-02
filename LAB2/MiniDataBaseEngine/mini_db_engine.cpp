#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

using namespace std;

// constraint flags using bitwise
const unsigned int PK = 1;
const unsigned int NOT_NULL = 2;
const unsigned int UNIQUE = 4;

class Column {
public:
    string name;
    string type;
    unsigned int constraints;

    Column(string n, string t, unsigned int c) : name(n), type(t), constraints(c) {}
};

class Row {
public:
    vector<string> values;
    Row(vector<string> v) : values(v) {}
};

class Table {
public:
    string tableName;
    vector<Column> columns;
    vector<Row*> rows;

    Table(string name) : tableName(name) {}

    ~Table() {
        for (Row* row : rows) {
            delete row;
        }
        rows.clear();
    }

    bool validateConstraint(int colIdx, const string& value) {
        unsigned int flags = columns[colIdx].constraints;

        if ((flags & NOT_NULL) && value.empty()) {
            cout << "Error: Column '" << columns[colIdx].name << "' cannot be null." << endl;
            return false;
        }

        // check for duplicates if PK or unique
        if ((flags & PK) || (flags & UNIQUE)) {
            for (Row* row : rows) {
                if (row->values[colIdx] == value) {
                    cout << "Error: Duplicate value '" << value << "' violates " 
                         << ((flags & PK) ? "Primary Key" : "Unique") << " constraint on '" 
                         << columns[colIdx].name << "'." << endl;
                    return false;
                }
            }
        }
        return true;
    }

    void insertRow(const vector<string>& values) {
        if (values.size() != columns.size()) {
            cout << "Error: Column count mismatch." << endl;
            return;
        }

        for (size_t i = 0; i < values.size(); ++i) {
            if (!validateConstraint(i, values[i])) return;
        }

        rows.push_back(new Row(values));
        cout << "Record inserted." << endl;
    }

    void selectAll() {
        if (rows.empty()) {
            cout << "Table is empty." << endl;
            return;
        }

        for (const auto& col : columns) {
            cout << left << setw(15) << col.name;
        }
        cout << endl;
        cout << string(columns.size() * 15, '-') << endl;

        for (Row* row : rows) {
            for (const string& val : row->values) {
                cout << left << setw(15) << val;
            }
            cout << endl;
        }
    }

    void saveToFile() {
        fstream outFile;
        outFile.open(tableName + ".db", ios::out);
        if (!outFile) {
            cout << "Error: Could not open file for saving." << endl;
            return;
        }

        outFile << "TABLE " << tableName << endl;
        for (const auto& col : columns) {
            outFile << col.name << " " << col.type << " " << col.constraints << endl;
        }
        outFile << "DATA" << endl;
        for (Row* row : rows) {
            for (size_t i = 0; i < row->values.size(); ++i) {
                outFile << row->values[i] << (i == row->values.size() - 1 ? "" : " ");
            }
            outFile << endl;
        }
        outFile.close();
        cout << "Table saved to " << tableName << ".db" << endl;
    }
};

class DatabaseEngine {
    vector<Table*> tables;
    char buffer[256]; // array for parsing commands

public:
    ~DatabaseEngine() {
        for (Table* t : tables) delete t;
    }

    Table* findTable(string name) {
        for (Table* t : tables) {
            if (t->tableName == name) return t;
        }
        return nullptr;
    }

    void processQuery(string query) {
        strncpy(buffer, query.c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        stringstream ss(buffer);
        string cmd;
        ss >> cmd;

        transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

        if (cmd == "CREATE") {
            string sub, tName;
            ss >> sub >> tName;
            if (findTable(tName)) {
                cout << "Error: Table already exists." << endl;
                return;
            }
            Table* newTable = new Table(tName);
            
            cout << "Defining columns for " << tName << " (type 'done' to finish):" << endl;
            string colName, colType;
            while (true) {
                cout << "Column Name: ";
                cin >> colName;
                if (colName == "done") break;
                cout << "Type (int/string): ";
                cin >> colType;
                
                unsigned int flags = 0;
                char choice;
                cout << "Is Primary Key? (y/n): "; 
                cin >> choice; 
                if (choice == 'y') flags |= PK;
                cout << "Is Not Null? (y/n): "; 
                cin >> choice; 
                if (choice == 'y') flags |= NOT_NULL;
                cout << "Is Unique? (y/n): "; 
                cin >> choice; 
                if (choice == 'y') flags |= UNIQUE;

                newTable->columns.push_back(Column(colName, colType, flags));
            }
            tables.push_back(newTable);
            cout << "Table created successfully." << endl;
            cin.ignore();

        } else if (cmd == "INSERT") {
            string into, tName, valuesKeyword;
            ss >> into >> tName >> valuesKeyword;
            Table* t = findTable(tName);
            if (!t) {
                cout << "Error: Table not found." << endl;
                return;
            }

            string remaining;
            getline(ss, remaining);
            size_t start = remaining.find('(');
            size_t end = remaining.find(')');
            if (start != string::npos && end != string::npos) {
                string data = remaining.substr(start + 1, end - start - 1);
                stringstream dss(data);
                string val;
                vector<string> rowValues;
                while (getline(dss, val, ',')) {
                    val.erase(0, val.find_first_not_of(" "));
                    val.erase(val.find_last_not_of(" ") + 1);
                    rowValues.push_back(val);
                }
                t->insertRow(rowValues);
            }

        } else if (cmd == "SELECT") {
            string star, from, tName;
            ss >> star >> from >> tName;
            Table* t = findTable(tName);
            if (t) t->selectAll();
            else cout << "Error: Table not found." << endl;

        } else if (cmd == "SAVE") {
            string tName;
            ss >> tName;
            Table* t = findTable(tName);
            if (t) t->saveToFile();
            else cout << "Error: Table not found." << endl;

        } else if (cmd == "LOAD") {
            string tName;
            ss >> tName;
            loadTable(tName);

        } else if (cmd == "EXIT") {
            exit(0);
        } else {
            cout << "Unknown command." << endl;
        }
    }

    void loadTable(string tName) {
        fstream inFile;
        inFile.open(tName + ".db", ios::in);
        if (!inFile) {
            cout << "Error: File " << tName << ".db not found." << endl;
            return;
        }

        string line, dummy, tableName;
        getline(inFile, line);
        stringstream ss(line);
        ss >> dummy >> tableName;

        Table* t = new Table(tableName);
        
        while (getline(inFile, line) && line != "DATA") {
            stringstream colSS(line);
            string cName, cType;
            unsigned int cFlags;
            colSS >> cName >> cType >> cFlags;
            t->columns.push_back(Column(cName, cType, cFlags));
        }

        while (getline(inFile, line)) {
            stringstream rowSS(line);
            string val;
            vector<string> rowValues;
            while (rowSS >> val) {
                rowValues.push_back(val);
            }
            if (!rowValues.empty()) {
                t->rows.push_back(new Row(rowValues));
            }
        }
        tables.push_back(t);
        cout << "Table loaded successfully." << endl;
    }
};

int main() {
    DatabaseEngine engine;
    string query;

    cout << "========================================" << endl;
    cout << "   Mini Database Engine" << endl;
    cout << "========================================" << endl;
    cout << "Commands: CREATE TABLE, INSERT INTO, SELECT *, SAVE, LOAD, EXIT" << endl;

    while (true) {
        cout << "\ndb_shell> ";
        getline(cin, query);
        if (query.empty()) 
            continue;
        engine.processQuery(query);
    }

    return 0;
}
