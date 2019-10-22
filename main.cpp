#include <iostream>
#include <cstring>

using namespace std;

const int ERROR_RECORD_NOT_FOUND = -2;
const int ERROR_KEY_IS_NOT_UNIQUE = -3;
const int ERROR_TABLE_IS_EMPTY = -4;

struct Record {
    int key{};
    string fullName;
};

class Stack {
private:
    Record data;
    Stack *Next,*Head;
public:
    Stack() {
        Head = Next = nullptr;
    }

    void push(const Record& record) {
        auto *temp = new Stack;
        temp->data.key = record.key;
        temp->data.fullName = record.fullName;
        temp->Next = Head;
        Head = temp;
    }

    Record pop() {
        if (Head == nullptr) {
            cout << "ERROR: Stack empty!" << endl;
            Record errorRecord = Record();
            return errorRecord;
        }
        Record popRecord;
        popRecord.key = Head->data.key;
        popRecord.fullName = Head->data.fullName;
        Stack *CurHead = Head->Next;
        delete Head;
        Head = CurHead;
        return popRecord;
    }

    Record searchAndPop(const Record& record){
        Record popRecord = Record();
        auto *forBusting = new Stack;
        bool search = false;
        while ((Head != nullptr) && !search) {
            if ((Head->data.key == record.key) && (Head->data.fullName == record.fullName)){
                search = true;
                popRecord = pop();
            }
            if (!search) {
                forBusting->push(pop());
            }
        }
        while (forBusting->Head != nullptr) {
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
        while ((Head != nullptr) && !search) {
            if ((Head->data.key == record.key) && (Head->data.fullName == record.fullName)){
                search = true;
                forBusting->push(pop());
            }
            if (!search) {
                forBusting->push(pop());
            }
        }
        while (forBusting->Head != nullptr) {
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

    Stack dubStack(Stack &stack) {
        auto *dubStack = new Stack;
        while (!dubStack->isStackEmpty()) {
            stack.push(dubStack->pop());
        }
        return stack;
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

    void initialize(unsigned int sizeNewTable) {
        stack = new Stack[sizeNewTable];
        status = new bool[sizeNewTable];
        memset(status, 0, (size_t) sizeNewTable);
    }

    HashTable &operator+(const Record &right) {
        HashTable *result = this;
        result->add(right);
        return *result;
    }

    HashTable &operator+(const HashTable &right) {
        auto *result = new HashTable(this->size);

        for (int i = 0; i < right.size; i++) {
            if (!this->stack[i].isStackEmpty() || !right.stack[i].isStackEmpty()) {
                result->stack[i] = this->stack[i] + right.stack[i];
                result->status[i] = true;
            }
        }
        return *result;
    }

        HashTable &operator=(HashTable const &right) {
            auto *result = new HashTable(this->size);

            delete[] this->stack;
            delete[] this->status;

            initialize(this->size);
            countOfElem = 0;

            for (int i = 0; i < right.size; i++) {
                if (right.status[i]) {
                    result->stack[i].dubStack(right.stack[i]);
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
    HashTable table(30);
    Record record;

    record.key = 1009;
    record.fullName = "Kirill Mokhovichenko10";

    for (int i = 0; i < 8; i++) {

        table.add(record);
        record.key = record.key + 1000 - 1;
        record.fullName[0] += 1;

    }

    cout << table;

    for (int i = 0; i < 8; i++) {


        table.deleteRecord(record);
        record.key = record.key - 2000 + 2;
        record.fullName[0] -= 2;

    }

    cout << table;


    record.key = 1119;
    record.fullName = "Kirill Mokhovichenko10";

    for (int i = 0; i < 10; i++) {

        table.add(record);
        record.key = record.key + 1000 - 1;
        record.fullName[0] += 1;

    }

    cout << table;


    record.key = 1009;
    record.fullName = "Kirill Mokhovichenko10";

    for (int i = 0; i < 8; i++) {

        table.add(record);
        record.key = record.key + 1000 - 1;
        record.fullName[0] += 1;

    }

    cout << table;


    for (int i = 0; i < 8; i++) {
        cout << "del " << record.fullName << " " << record.key << endl;

        table.deleteRecord(record);
        record.key = record.key - 1000 + 1;
        record.fullName[0] -= 1;

    }

    cout << table;


    record.key = 1109;
    record.fullName = "Kirill";

    for (int i = 0; i < 8; i++) {

        table.add(record);
        record.key = record.key + 1000 - 1;
        record.fullName[0] += 1;

    }

    cout << table;

    record.key = 32414;
    record.fullName = "Sasha";
    table + record;
    cout << table;

    record.key = 23144;
    record.fullName = "Nikolai";
    table + record;
    cout << table;

    HashTable table1 = table;

    cout << table1;
    record.key = 24341;
    record.fullName = "Nastia";
    table1 + record;

    cout << table1;
    cout << table;

    HashTable table3(5);
    HashTable table4(6);

    for (int i = 0; i < 8; i++) {
        table3.add(record);
        record.key = record.key + 1000 - 1;
        record.fullName[0] += 1;
    }

    cout << table3;

    record.key = 10000;
    for (int i = 0; i < 8; i++) {
        table4.add(record);
        record.key = record.key + 1000 - 1;
        record.fullName[0] += 1;
    }

    cout << table4;

    HashTable table5 = table4 + table3;

    cout << table5;

}