// C program for linked list implementation of stack
// It contains the minimum methods to attend our needs
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
 
// A structure to represent a stack
struct StackNode;
 
struct StackNode* newNode(uint32_t data);
 
uint32_t isEmpty(struct StackNode* root);
 
void push(struct StackNode** root, uint32_t data);
 
uint32_t pop(struct StackNode** root);
