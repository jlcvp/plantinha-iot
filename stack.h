class Stack
{
    int top;
    int size;
    int *array;

public:
    Stack(int size);
    ~Stack();
    void push(int value);
    int pop();
    bool isEmpty();
    bool isFull();
};