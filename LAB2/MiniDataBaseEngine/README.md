# Mini Database Engine

## Overview
A simplified SQL-like in-memory database engine implemented in C++. Supports table creation, data insertion, basic querying, and file persistence.

## Concepts Covered
- OOP (Classes, Encapsulation)
- Pointers and Dynamic Memory
- File Handling (fstream)
- Arrays
- Vectors
- Bitwise Operators
- Constraint Validation

## Features

### 1. CREATE TABLE
Create tables with multiple columns. Each column has:
- Name
- Data type (int/string)
- Constraint flags (bitwise)

Example:
```
CREATE TABLE users
```

### 2. Column Constraints (Bitwise Flags)
Uses `unsigned int` to store constraints:
- Bit 1 → Primary Key
- Bit 2 → Not Null
- Bit 4 → Unique

Example: Primary Key + Not Null = 1 | 2 = 3

Uses bitwise OR (|) to set flags and bitwise AND (&) to check flags.

### 3. INSERT
Insert rows dynamically:
```
INSERT INTO users VALUES (1, Ali, 20)
```
Each row is dynamically allocated using `new` and stored in `vector<Row*>`.

### 4. SELECT
Display all records:
```
SELECT * FROM users
```

### 5. File Handling
- SAVE TO FILE
- LOAD FROM FILE

File format example (users.db):
```
TABLE users
id int 3
name string 2
age int 0
DATA
1 Ali 20
2 Sara 21
```
Where: 3 = PrimaryKey + NotNull, 2 = NotNull, 0 = No constraint

## Class Structure

```cpp
class Column {
    string name;
    string type;
    unsigned int constraints;
};

class Row {
    vector<string> values;
};

class Table {
    string tableName;
    vector<Column> columns;
    vector<Row*> rows;
};
```

## Array Usage
Fixed-size arrays used for query parsing:
```cpp
char buffer[256];
```

## Memory Management
- All rows dynamically allocated
- Destructor in Table class frees memory
- No memory leaks

## Compilation and Usage

Compile:
```bash
g++ -o mini_db_engine mini_db_engine.cpp
```

Run:
```bash
./mini_db_engine
```

## Example Session

```
CREATE TABLE users
Table created successfully.

INSERT INTO users VALUES (1, Ali, 20)
Record inserted.

SELECT * FROM users
ID             Name           Age            
1              Ali            20             

SAVE users
Table saved to users.db
```
