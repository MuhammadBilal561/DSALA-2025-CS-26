//2025(S)-CS-26
#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
};

void insertAtEnd(Node*& head, int val) {
    Node* newNode = new Node();
    newNode->data = val;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        return;
    }

    Node* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

void deleteNode(Node*& head, int key) {
    Node* temp = head;
    Node* prev = NULL;

    // if head holds the key
    if (temp != NULL && temp->data == key) {
        head = temp->next;
        delete temp;
        return;
    }

    // searching for key
    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }

    // if key not present
    if (temp == NULL) {
        cout << "Value not found in list." << endl;
        return;
    }

    // unlinking of node
    prev->next = temp->next;
    delete temp;
}

void display(Node* head) {
    Node* temp = head;
    while (temp != NULL) {
        cout << temp->data << " -> ";
        temp = temp->next;
    }
    cout << "NULL" << endl;
}

int main() {
    Node* head = NULL;

    insertAtEnd(head, 10);
    insertAtEnd(head, 20);
    insertAtEnd(head, 30);
    insertAtEnd(head, 40);

    cout << "List before deletion: ";
    display(head);

    int key;
    cout << "Enter value to delete: ";
    cin >> key;

    deleteNode(head, key);

    cout << "List after deletion: ";
    display(head);

    return 0;
}
