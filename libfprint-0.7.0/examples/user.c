//
// Created by leticia on 14/09/18.
//
///*controlling the user*///

#include <libfprint/fprint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node_user * create_list_users(){

//    printf("Creating list of users..\n");
//
//    unsigned char *digital;
//    struct node_user * head = createList(0, digital);
//    /*struct node_user * current = head;
//    for(int id = 1; id<10; id++){
//        current->next = malloc(sizeof(struct node_user));
//        current->next->id = id;
//        memcpy(current->next->digital ,(const unsigned char*)&digital,sizeof(digital));
//        current->next->next = NULL;
//        current= current->next;
//    }*/
//    return head;
}

struct node_user * createList(int id, unsigned char *digital){

//
//    struct node_user * head = NULL;
//    //head = malloc(struct node_user);
//    if (head == NULL) {
//        return head;
//    }
//
//    //memcpy(head->digital,(const unsigned char*)&digital,sizeof(digital));
//    //strcpy(head->digital, digital);
//    //int lengthDigital = sizeof(head->digital)/sizeof(char);
//
//    head->id = id;
//    head->next = NULL;
//    return head;
}

void iterOverList(struct node_user * head){

    struct node_user * current = head;
    int lengthDigital = 0;

    while (current != NULL) {

        printf("id %d\n", current->id);

        lengthDigital = sizeof(current->digital)/sizeof(char);
        printf("length digital %d\n", lengthDigital);

        for(int i = 0; i<= lengthDigital; i++){

            printf("%c\n", current->digital[i]);
        }

        current = current->next;
    }
}

void append(struct node_user * head, int id) {

//	struct node_user *current = head;
//	while (current->next != NULL) {
//		current = current->next;
//	}
//
//	*//* now we can add a new variable *//*
//	current->next = malloc(sizeof(struct node_user));
//	current->next->id = id;
//	current->next->next = NULL;

}
