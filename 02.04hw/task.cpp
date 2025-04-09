#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

const int MAX_CONTACTS = 100;
const int MAX_LEN = 100; 

class Contact {
private:
    char* fullName;
    char homePhone[MAX_LEN];
    char workPhone[MAX_LEN];
    char mobilePhone[MAX_LEN];
    char extraInfo[MAX_LEN];

public:
    Contact(const char* name = "No Name", const char* home = "", const char* work = "",
        const char* mobile = "", const char* extra = "") {
        fullName = new char[strlen(name) + 1];
        strcpy(fullName, name);
        strncpy(homePhone, home, MAX_LEN);
        strncpy(workPhone, work, MAX_LEN);
        strncpy(mobilePhone, mobile, MAX_LEN);
        strncpy(extraInfo, extra, MAX_LEN);
    }

    Contact(const Contact& other) {
        fullName = new char[strlen(other.fullName) + 1];
        strcpy(fullName, other.fullName);
        strcpy(homePhone, other.homePhone);
        strcpy(workPhone, other.workPhone);
        strcpy(mobilePhone, other.mobilePhone);
        strcpy(extraInfo, other.extraInfo);
    }

    ~Contact() {
        delete[] fullName;
    }

    inline const char* getName() const { return fullName; }

    void show() const {
        cout << "ПІБ: " << fullName << "\nДомашній: " << homePhone
            << "\nРобочий: " << workPhone << "\nМобільний: " << mobilePhone
            << "\nДодатково: " << extraInfo << "\n" << endl;
    }

    void saveToFile(ofstream& out) const {
        size_t nameLen = strlen(fullName);
        out.write((char*)&nameLen, sizeof(nameLen));
        out.write(fullName, nameLen);
        out.write(homePhone, MAX_LEN);
        out.write(workPhone, MAX_LEN);
        out.write(mobilePhone, MAX_LEN);
        out.write(extraInfo, MAX_LEN);
    }

    void loadFromFile(ifstream& in) {
        delete[] fullName;
        size_t nameLen;
        in.read((char*)&nameLen, sizeof(nameLen));
        fullName = new char[nameLen + 1];
        in.read(fullName, nameLen);
        fullName[nameLen] = '\0';
        in.read(homePhone, MAX_LEN);
        in.read(workPhone, MAX_LEN);
        in.read(mobilePhone, MAX_LEN);
        in.read(extraInfo, MAX_LEN);
    }

    bool matchName(const char* name) const {
        return strcmp(fullName, name) == 0;
    }
};

class PhoneBook {
private:
    Contact* contacts[MAX_CONTACTS];
    int size;

public:
    PhoneBook() : size(0) {}

    ~PhoneBook() {
        for (int i = 0; i < size; ++i) {
            delete contacts[i];
        }
    }

    void addContact(Contact* contact) {
        if (size < MAX_CONTACTS) {
            contacts[size++] = contact;
            cout << "Контакт додано.\n";
        }
        else {
            cout << "Телефонна книга заповнена!\n";
            delete contact;
        }
    }

    void deleteContact(const char* name) {
        for (int i = 0; i < size; ++i) {
            if (contacts[i]->matchName(name)) {
                delete contacts[i];
                for (int j = i; j < size - 1; ++j)
                    contacts[j] = contacts[j + 1];
                --size;
                cout << "Контакт видалено.\n";
                return;
            }
        }
        cout << "Контакт не знайдено.\n";
    }

    void searchContact(const char* name) const {
        for (int i = 0; i < size; ++i) {
            if (contacts[i]->matchName(name)) {
                contacts[i]->show();
                return;
            }
        }
        cout << "Контакт не знайдено.\n";
    }

    void showAll() const {
        for (int i = 0; i < size; ++i)
            contacts[i]->show();
    }

    void saveToFile(const char* filename) const {
        ofstream out(filename, ios::binary);
        out.write((char*)&size, sizeof(size));
        for (int i = 0; i < size; ++i)
            contacts[i]->saveToFile(out);
        out.close();
        cout << "Збережено у файл.\n";
    }

    void loadFromFile(const char* filename) {
        ifstream in(filename, ios::binary);
        if (!in) {
            cout << "Не вдалося відкрити файл.\n";
            return;
        }

        for (int i = 0; i < size; ++i)
            delete contacts[i];
        size = 0;

        int count;
        in.read((char*)&count, sizeof(count));
        for (int i = 0; i < count && i < MAX_CONTACTS; ++i) {
            Contact* c = new Contact();
            c->loadFromFile(in);
            contacts[size++] = c;
        }
        in.close();
        cout << "Завантажено з файлу.\n";
    }
};

int main() {
    PhoneBook book;

    book.addContact(new Contact("Іван Петренко", "123-45-67", "234-56-78", "0671234567", "Друг дитинства"));
    book.addContact(new Contact("Олена Коваль", "321-65-43", "432-76-54", "0509876543", "Колега"));

    cout << "Всі контакти:\n";
    book.showAll();

    cout << "\nПошук:\n";
    book.searchContact("Олена Коваль");

    book.saveToFile("contacts.dat");

    PhoneBook loaded;
    loaded.loadFromFile("contacts.dat");

    cout << "\nКонтакти з файлу:\n";
    loaded.showAll();

    return 0;
}
