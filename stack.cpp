#include <cstdlib>
#include <Arduino.h>
#include "stack.h"

Stack::Stack(int size) {
    top = -1;
    this->size = size;
    this->array = (int*)malloc(size * sizeof(int));
}

Stack::~Stack() {
    free(array);
}

void Stack::push(int value) {
    if (top == size - 1) {
        Serial.println("Stack is full");
        return;
    }
    top++;
    array[top] = value;
}

int Stack::pop() {
    if (top == -1) {
        Serial.println("Stack is empty");
        return -1;
    }
    int value = array[top];
    top--;
    return value;
}

bool Stack::isEmpty() {
    return top == -1;
}

bool Stack::isFull() {
    return top == size - 1;
}