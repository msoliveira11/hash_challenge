#include "stack.h"

struct StackNode {
    uint32_t data;
    struct StackNode* next;
};
 
struct StackNode* newNode(uint32_t data)
{
    struct StackNode* stackNode =
      (struct StackNode*)
      malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->next = NULL;
    return stackNode;
};
 
uint32_t isEmpty(struct StackNode* root)
{
    return !root;
}
 
void push(struct StackNode** root, uint32_t data)
{
    struct StackNode* stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;
}
 
uint32_t pop(struct StackNode** root)
{
    if (isEmpty(*root))
        return INT_MIN;
    struct StackNode* temp = *root;
    *root = (*root)->next;
    uint32_t popped = temp->data;
    free(temp);
 
    return popped;
}
