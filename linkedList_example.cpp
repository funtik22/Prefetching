#include <iostream>
#include <cstdlib>
#include <ctime>

#define LIST_SIZE 10  // Размер связанного списка
#define NUM_PASSES 100     // Количество проходов по списку

// Узел связанного списка
struct Node {
    int value;
    Node* next;
};

// Создание связанного списка
Node* createLinkedList(size_t size) {
    if (size == 0) return nullptr;

    Node* head = new Node{std::rand() % 100, nullptr};
    Node* current = head;

    for (size_t i = 1; i < size; ++i) {
        current->next = new Node{std::rand() % 100, nullptr};
        current = current->next;
    }

    return head;
}

// Просто обходит список, ничего не делая
int traverseList(Node* head) {
    int count = 0;
    Node* current = head;
    while (current) {
        volatile int val = current->value;  // Теперь доступ будет виден как L <heap addr>
        ++count;
        current = current->next;
    }
    return count;
}

// Освобождение памяти
void deleteList(Node* head) {
    while (head) {
        Node* next = head->next;
        delete head;
        head = next;
    }
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Node* head = createLinkedList(LIST_SIZE);

    std::cout<<"LINKED_START"<<std::flush;
    for (int i = 1; i <= NUM_PASSES; ++i) {
        std::cout<<traverseList(head);
    }
    std::cout<<"LINKED_END"<<std::flush;

    deleteList(head);
    return 0;
}
