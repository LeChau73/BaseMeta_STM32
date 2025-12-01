#ifndef __MAIN
#define __MAIN

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SEGGER_RTT.h"



typedef struct Node Node;

typedef struct Node {
    int data;
    struct Node* left_Node;
    struct Node* right_Node;
} Node;


void insert(Node** root, int value);
void initNode(Node** root, int value);
bool contain(Node* root, int value);
//trả về mảng các giá trị theo thứ tự tăng dần
//TODO:
void inOrderTraverse(Node* root);

#endif