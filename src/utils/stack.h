#ifndef CANARY_STACK_H
#define CANARY_STACK_H

#include <stdio.h>
#include "definitions.h"


// stack operations

/**
 * @brief Get the top dir element in stack
 *
 * @param current_directory_ptr : stack head pointer
 * @param size : current size of the stack before insertion
 * @return char* : the string present in top of the stack
 */
char* get_top_dir(char** current_directory_ptr, int* size){
    if((*size)==0){
        printf("err: stack is empty, and top isn't defined. exiting..\n");
        exit(1);
    }
    return current_directory_ptr[(*size)-1];
}


/**
 * @brief function to push a directory to the stack
 *
 * @param current_directory_ptr : stack head pointer
 * @param size : current size of the stack before insertion
 * @param label : string which needs to be pushed. Ensure that it's dynamically allocated
 */
void push_dir(char** current_directory_ptr, int* size, char* label){
    if((*size) == MAX_DIR_DEPTH){
        printf("err: stack is full. please increase the max dir depth and try again. exiting..\n");
        exit(1);
    }
    // push the element on top and update the size
    current_directory_ptr[(*size)++]=label;
}

/**
 * @brief function to pop one element from top of the stack
 *
 * @param current_directory_ptr : stack head pointer
 * @param size : current size of the stack before insertion
 */
void pop_dir(char** current_directory_ptr, int* size){
    if((*size)==0){
        printf("err: stack is empty, and pop isn't defined. exiting..\n");
        exit(1);
    }
    printf("Current sze: %d\n", (*size));
    // free the memory
    // note: it will be --size, as we need to free the a[size-1] elem
    free(current_directory_ptr[--(*size)]);
    printf("Current sze: %d\n", (*size));
}

// Stack operations end



#endif
