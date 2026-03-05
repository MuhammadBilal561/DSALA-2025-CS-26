//2025(S)-CS-26

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
using namespace std;


// BITWISE CONSTRAINT FLAGS


const unsigned int PK       = 1;   
const unsigned int NOT_NULL = 2;   
const unsigned int UNIQUE   = 4;   


// CLASS Column


class Column
{
public:
    string name;
    string type;
    unsigned int constraints;

    Column(string n, string t, unsigned int c)
    {
        name = n;
        type = t;
        constraints = c;
    }
};


// CLASS Row


class Row
{
public:
    vector<string> values;

    Row(vector<string> v)
    {
        values = v;
    }
};


// CLASS Table


class Table
{
public:
    string tableName;
    vector<Column> columns;
    vector<Row*> rows;          // dynamic memory — each Row is on the heap

    Table(string name)
    {
        tableName = name;
    }

    // called automatically when a Table object is destroyed.
    ~Table()
    {
        for (int i = 0; i < rows.size(); i++)
        {
            delete rows[i];     
        }
        rows.clear();
    }


    bool validateConstraint(int colIdx, string value)
    {
        unsigned int flags = columns[colIdx].constraints;

        if ((flags & NOT_NULL) && value.empty())
        {
            cout << "  Error: '" << columns[colIdx].name
                 << "' cannot be empty (NOT NULL constraint)." << endl;
            return false;
        }
    
        if ((flags & PK) || (flags & UNIQUE))
        {
            // Loop through all existing rows to find a duplicate
            for (int i = 0; i < rows.size(); i++)
            {
                if (rows[i]->values[colIdx] == value)
                {
                    string ruleName = (flags & PK) ? "PRIMARY KEY" : "UNIQUE";
                    cout << "  Error: Duplicate '" << value
                         << "' violates " << ruleName
                         << " on '" << columns[colIdx].name << "'." << endl;
                    return false;
                }
            }
        }

        return true;   
    }

    void insertRow(vector<string> values)
    {
        if (values.size() != columns.size())
        {
            cout << "  Error: Expected " << columns.size()
                 << " values, got " << values.size() << "." << endl;
            return;
        }

        for (int i = 0; i < values.size(); i++)
        {
            if (!validateConstraint(i, values[i]))
                return;   
        }

        // All checks passed — allocate new Row on heap
        Row* newRow = new Row(values);
        rows.push_back(newRow);
        cout << "  Record inserted successfully." << endl;
    }


    void selectAll()
    {
        if (rows.empty())
        {
            cout << "  Table '" << tableName << "' is empty." << endl;
            return;
        }

        cout << endl;

        for (int i = 0; i < columns.size(); i++)
        {
            cout << left << setw(15) << columns[i].name;
        }
        cout << endl;

        for (int i = 0; i < columns.size(); i++)
        {
            cout << "-------------- ";
        }
        cout << endl;

        for (int i = 0; i < rows.size(); i++)
        {
            for (int j = 0; j < rows[i]->values.size(); j++)
            {
                cout << left << setw(15) << rows[i]->values[j];
            }
            cout << endl;
        }
    }


    void deleteWhere(string colName, string matchValue)
    {
        int colIdx = -1;
        for (int i = 0; i < columns.size(); i++)
        {
            if (columns[i].name == colName)
            {
                colIdx = i;
                break;
            }
        }

        if (colIdx == -1)
        {
            cout << "  Error: Column '" << colName << "' not found." << endl;
            return;
        }

        int deleted = 0;

       
        for (int i = rows.size() - 1; i >= 0; i--)
        {
            if (rows[i]->values[colIdx] == matchValue)
            {
                delete rows[i];              // free heap memory
                rows.erase(rows.begin() + i); // remove pointer from vector
                deleted++;
            }
        }

        cout << "  Deleted " << deleted << " record(s)." << endl;
    }


    // Save

    void saveToFile()
    {
        ofstream out(tableName + ".db");

        if (!out.is_open())
        {
            cout << "  Error: Cannot create file!" << endl;
            return;
        }

        out << "TABLE " << tableName << endl;

        for (int i = 0; i < columns.size(); i++)
        {
            out << columns[i].name << " "
                << columns[i].type << " "
                << columns[i].constraints << endl;
        }

        out << "DATA" << endl;

        for (int i = 0; i < rows.size(); i++)
        {
            for (int j = 0; j < rows[i]->values.size(); j++)
            {
                out << rows[i]->values[j];
                if (j < rows[i]->values.size() - 1)
                    out << " ";
            }
            out << endl;
        }

        out.close();
        cout << "  Saved to " << tableName << ".db" << endl;
    }

    // LOAD 
  
    void loadFromFile(ifstream& in)
    {
        string line;

        while (getline(in, line))
        {
            if (line == "DATA")
                break;

            stringstream ss(line);
            string cName, cType;
            unsigned int cFlags;
            ss >> cName >> cType >> cFlags;

            columns.push_back(Column(cName, cType, cFlags));
        }

        while (getline(in, line))
        {
            if (line.empty())
                continue;

            stringstream ss(line);
            string val;
            vector<string> rowValues;

            while (ss >> val)
            {
                rowValues.push_back(val);
            }

            if (!rowValues.empty())
            {
                
                Row* newRow = new Row(rowValues);
                rows.push_back(newRow);
            }
        }
    }
};


// CLASS DatabaseEngine


class DatabaseEngine
{
private:
    vector<Table*> tables;     // all tables (dynamic, on heap)
    char buffer[256];         

public:
    ~DatabaseEngine()
    {
        for (int i = 0; i < tables.size(); i++)
        {
            delete tables[i];
        }
        tables.clear();
    }

   
    Table* findTable(string name)
    {
        for (int i = 0; i < tables.size(); i++)
        {
            if (tables[i]->tableName == name)
                return tables[i];
        }
        return NULL;
    }

   
    void processCommand(string query)
    {
       
        strncpy(buffer, query.c_str(), 255);
        buffer[255] = '\0';  

        
        stringstream ss(buffer);
        string cmd;
        ss >> cmd;

        for (int i = 0; i < cmd.length(); i++)
        {
            cmd[i] = toupper(cmd[i]);
        }


        if (cmd == "CREATE")
        {
            handleCreate(ss);
        }
        else if (cmd == "INSERT")
        {
            handleInsert(ss);
        }
        else if (cmd == "SELECT")
        {
            handleSelect(ss);
        }
        else if (cmd == "DELETE")
        {
            handleDelete(ss);
        }
        else if (cmd == "SAVE")
        {
            handleSave(ss);
        }
        else if (cmd == "LOAD")
        {
            handleLoad(ss);
        }
        else if (cmd == "EXIT")
        {
            cout << "  Exiting....." << endl;
            exit(0);
        }
        else
        {
            cout << "  Unknown command. Try: CREATE, INSERT, SELECT, DELETE, SAVE, LOAD, EXIT" << endl;
        }
    }


    
    void handleCreate(stringstream& ss)
    {
        string keyword, tName;
        ss >> keyword >> tName;    
        if (findTable(tName) != NULL)
        {
            cout << "  Error: Table '" << tName << "' already exists!" << endl;
            return;
        }

        Table* newTable = new Table(tName);

        cout << "\n  Define columns for '" << tName
             << "' (type 'done' when finished):\n" << endl;

        string colName, colType;
        char choice;

        while (true)
        {
            cout << "  Column name (or 'done'): ";
            cin >> colName;

            if (colName == "done")
                break;

            cout << "  Data type (int/string): ";
            cin >> colType;

          
            unsigned int flags = 0;

            cout << "  Primary Key?  (y/n): ";
            cin >> choice;
            if (choice == 'y' || choice == 'Y')
                flags = flags | PK;          // turn ON bit 0: 000 -> 001

            cout << "  Not Null?     (y/n): ";
            cin >> choice;
            if (choice == 'y' || choice == 'Y')
                flags = flags | NOT_NULL;    // turn ON bit 1: 001 -> 011

            cout << "  Unique?       (y/n): ";
            cin >> choice;
            if (choice == 'y' || choice == 'Y')
                flags = flags | UNIQUE;      // turn ON bit 2: 011 -> 111

            newTable->columns.push_back(Column(colName, colType, flags));

            cout << "  -> Added '" << colName << "' (flags=" << flags << ")\n" << endl;
        }

        tables.push_back(newTable);
        cout << "\n  Table '" << tName << "' created with "
             << newTable->columns.size() << " columns." << endl;

  
        cin.ignore();
    }


    void handleInsert(stringstream& ss)
    {
        string into, tName, valuesWord;
        ss >> into >> tName >> valuesWord;
       

        Table* t = findTable(tName);
        if (t == NULL)
        {
            cout << "  Error: Table '" << tName << "' not found!" << endl;
            return;
        }

        string remaining;
        getline(ss, remaining);

        int start = remaining.find('(');
        int end = remaining.find(')');

        if (start == string::npos || end == string::npos)
        {
            cout << "  Error: Use format: INSERT INTO table VALUES (v1, v2, v3)" << endl;
            return;
        }

        string data = remaining.substr(start + 1, end - start - 1);

        stringstream dataSS(data);
        string val;
        vector<string> rowValues;

        while (getline(dataSS, val, ','))
        {
            int firstChar = val.find_first_not_of(" ");
            if (firstChar != string::npos)
                val = val.substr(firstChar);

            int lastChar = val.find_last_not_of(" ");
            if (lastChar != string::npos)
                val = val.substr(0, lastChar + 1);

            rowValues.push_back(val);
        }

        t->insertRow(rowValues);
    }



    void handleSelect(stringstream& ss)
    {
        string star, from, tName;
        ss >> star >> from >> tName;

        Table* t = findTable(tName);
        if (t == NULL)
        {
            cout << "  Error: Table '" << tName << "' not found!" << endl;
            return;
        }

        t->selectAll();
    }


   
    void handleDelete(stringstream& ss)
    {
        string from, tName, where, colName, equals, matchValue;
        ss >> from >> tName >> where >> colName >> equals >> matchValue;

        Table* t = findTable(tName);
        if (t == NULL)
        {
            cout << "  Error: Table '" << tName << "' not found!" << endl;
            return;
        }

        t->deleteWhere(colName, matchValue);
    }



    void handleSave(stringstream& ss)
    {
        string tName;
        ss >> tName;

        Table* t = findTable(tName);
        if (t == NULL)
        {
            cout << "  Error: Table '" << tName << "' not found!" << endl;
            return;
        }

        t->saveToFile();
    }


    
    void handleLoad(stringstream& ss)
    {
        string tName;
        ss >> tName;

        if (findTable(tName) != NULL)
        {
            cout << "  Table '" << tName << "' is already loaded!" << endl;
            return;
        }

        ifstream in(tName + ".db");

        if (!in.is_open())
        {
            cout << "  Error: File '" << tName << ".db' not found!" << endl;
            return;
        }

        string line, dummy, tableName;
        getline(in, line);
        stringstream headerSS(line);
        headerSS >> dummy >> tableName;    
        Table* newTable = new Table(tableName);

        newTable->loadFromFile(in);

        in.close();

        tables.push_back(newTable);
        cout << "  Table '" << tableName << "' loaded with "
             << newTable->rows.size() << " records." << endl;
    }
};




int main()
{
    DatabaseEngine engine;
    string query;

    cout << "========================================" << endl;
    cout << "       MINI DATABASE ENGINE (C++)" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "  Commands:" << endl;
    cout << "    CREATE TABLE tablename" << endl;
    cout << "    INSERT INTO tablename VALUES (v1, v2, v3)" << endl;
    cout << "    SELECT * FROM tablename" << endl;
    cout << "    DELETE FROM tablename WHERE column = value" << endl;
    cout << "    SAVE tablename" << endl;
    cout << "    LOAD tablename" << endl;
    cout << "    EXIT" << endl;

    while (true)
    {
        cout << "\n  db> ";
        getline(cin, query);

        if (query.empty())
            continue;

        engine.processCommand(query);
    }

    return 0;
}