#include <iostream>
#include <cstring>

using namespace std;

const int ERROR_RECORD_NOT_FOUND = -2;
const int ERROR_KEY_IS_NOT_UNIQUE = -3;
const int ERROR_TABLE_IS_EMPTY = -4;

struct Record {
    int key;
    string fullName;
};

class Stack {
public:
    Record data;
    Stack *Next, *Head;
public:
    Stack() {
        Head = Next = nullptr;
    }

    explicit Stack(Stack *copy) {
        Head = copy->Head;
        Next = copy->Next;
        data = copy->data;
        auto *dubStack = this;
        while (!dubStack->isStackEmpty()) {
            copy->push(dubStack->pop());
        }
    }


    Stack dubStack(Stack &stack) {
        auto *dubStack = this;
        while (!dubStack->isStackEmpty()) {
            stack.push(dubStack->pop());
        }
        return stack;
    }

    void push(const Record& record) {
        if (Head == nullptr && Next == nullptr) {
            data.key = record.key;
            data.fullName = record.fullName;
            Next = nullptr;
            Head = this;
        }
        else {
            auto *temp = new Stack;
            temp->data.key = record.key;
            temp->data.fullName = record.fullName;
            temp->Next = Head;
            Head = temp;
        }
    }

    Record pop() {
        if (Head == nullptr) {
            cout << "ERROR: Stack empty!" << endl;
        }
        else {
            Record popRecord;
            popRecord.key = Head->data.key;
            popRecord.fullName = Head->data.fullName;
            Stack *CurHead = Head->Next;
            Head = CurHead;
            return popRecord;
        }
    }

    Record searchAndPop(const Record& record){
        Record popRecord = Record();
        auto *forBusting = new Stack;
        bool search = false;
        while ((!isStackEmpty()) && !search) {
            if ((Head->data.key == record.key) && (Head->data.fullName == record.fullName)){
                search = true;
                popRecord = pop();
            }
            if (!search) {
                forBusting->push(pop());
            }
        }
        while (!forBusting->isStackEmpty()) {
            push(forBusting->pop());
        }
        if (!search)
            cout << "ERROR: Record \"" << record.key << " : " << record.fullName << "\" not found!" << endl;
        return popRecord;
    }

    int searchRecord(const Record& record) {
        int search_record = 0;
        auto *forBusting = new Stack;
        bool search = false;
        while ((!isStackEmpty()) && !search) {
            if ((Head->data.key == record.key) && (Head->data.fullName == record.fullName)){
                search = true;
                forBusting->push(pop());
            }
            if (!search) {
                forBusting->push(pop());
            }
        }
        while (!forBusting->isStackEmpty()) {
            push(forBusting->pop());
        }
        if (!search)
            return ERROR_RECORD_NOT_FOUND;
        return search_record;
    }

    void print() {
        Stack *temp = Head;
        while (temp != nullptr) {
            cout << "\"" << temp->data.key << " : " << temp->data.fullName << "\" --> ";
            temp = temp->Next;
        }
        cout << "NULL.";
    }

    bool isStackEmpty() {
        bool empty = false;
        if (Head == nullptr) {
            empty = true;
            return empty;
        }
        return empty;
    }

    Stack &operator+(const Stack &right) {
        Stack *result = this;
        auto popStackRight = right;
        while (!popStackRight.isStackEmpty()) {
            result->push(popStackRight.pop());
        }
        return *result;
    }

};

class HashTable : public error_code {
    unsigned int size, countOfElem;
    bool *status;
    Stack *stack;

public:
    explicit HashTable(unsigned int N) {
        size = N;
        countOfElem = 0;
        stack = new Stack[size];
        status = new bool[size];
        for (unsigned int i = 0; i < size; i++) {
            status[i] = false;
        }
    }

    ~HashTable() {
        delete[] stack;
        delete[] status;
    }

    unsigned int hashFunction(unsigned int key) {
        unsigned int address = 0;
        while (key != 0) {
            address = address + (key % 10);
            key = key / 10;
        }
        address = address % size;
        return address;
    }

    int add(const Record& record) {
        size_t i = 0;
        while (i < size) {
            if ((status[i] == 1) && (stack[i].searchRecord(record) == 0)) {
                cout << "Нельзя добавить данную запись : \"" << record.key << " | " << record.fullName << "\"" << endl;
                cout << "Ключ \"" << record.key << "\" не уникальный! В записи под номером \"" << i
                     << "\" находится абсолютно идентичный ключ." << endl;
                return ERROR_KEY_IS_NOT_UNIQUE;
            }
            i++;
        }

        stack[hashFunction(record.key)].push(record);
        status[hashFunction(record.key)] = true;
        countOfElem++;
        cout << "Запись \"" << record.key << " : " << record.fullName << "\" успешно добавленна!" << endl;
        return 0;
    }

    int deleteRecord(const Record& record) {
        if (countOfElem == 0) {
            cout << "Невозможно найти данную запись : \"" << record.key << " | " << record.fullName << "\"" << endl;
            cout << "Таблица пустая" << endl;
            return ERROR_TABLE_IS_EMPTY;
        }
        if (stack[hashFunction(record.key)].searchRecord(record) == ERROR_RECORD_NOT_FOUND) {
            cout << "Неудалось удалить. Данная запись не найдена : \"" << record.key << " | " << record.fullName << "\""
                 << endl;
            return ERROR_RECORD_NOT_FOUND;
        }
        stack[hashFunction(record.key)].searchAndPop(record);
        if (stack[hashFunction(record.key)].isStackEmpty())
            status[hashFunction(record.key)] = false;
        cout << "Запись \"" << record.key << " : " << record.fullName << "\" успешно удалена!" << endl;
        countOfElem--;
        return 0;
    }

    void initialize(unsigned int size) {
        stack = new Stack[size];
        status = new bool[size];
        memset(status, 0, (size_t) size);
    }

    HashTable &operator=(const HashTable &right) {
        if (this == &right) {
            return *this;
        }

        delete[] this->stack;
        delete[] this->status;

        initialize(this->size);
        countOfElem = 0;

        for (size_t i = 0; i < right.size; i++) {
            if (right.status[i]) {
                Stack popStackRight(right.stack[i]);
                while (!popStackRight.isStackEmpty()){
                    this->add(popStackRight.pop());
                }
            }
        }
        return *this;
    }

    HashTable &operator+(const Record &right) {
        this->add(right);
        return *this;
    }

    HashTable &operator+(const HashTable &right) {
        for (int i = 0; i < right.size; i++) {
            if (right.status[i]) {
                Stack popStack(right.stack[i]);
                while (!popStack.isStackEmpty()) {
                    this->add(popStack.pop());
                }
            }
        }
        return *this;
    }


    friend std::ostream &operator<<(std::ostream &out, HashTable &table) {
        out << "---------------------------------------------------------------------------------" << endl;
        for (size_t i = 0; i < table.size; i++) {
            if (table.status[i] == 1) {
                out << " | " << i << " | ";
                table.stack[i].print();
                out << " | " << endl;
            }
        }
        out << "---------------------------------------------------------------------------------" << endl;
        out << "Элементов в таблице: \"" << table.countOfElem << "\"." << endl;
    }

};

int main() {
    HashTable table(10);
    Record record;

    record.key = 1009;
    record.fullName = "Kirill Mokhovichenko10";

    for (int i = 0; i < 4; i++) {
        table.add(record);
        record.key = record.key + 1000 - 1;
        record.fullName[0] += 1;
    }
    cout << table;

    for (int i = 0; i < 5; i++) {
        table + record;
        record.key = record.key + 1234 - 1;
        record.fullName[0] += 1;
    }
    cout << table;

    HashTable table2(5);

    table2 = table;

    cout << table2;

    cout << table;

    table2 + table;

    cout << table2;

    cout << table;

    HashTable table1 = table2 + table2;

    cout << table1;

    return 0;
}