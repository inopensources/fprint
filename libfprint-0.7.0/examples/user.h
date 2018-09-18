//
// Created by leticia on 18/09/18.
//

#ifndef FPRINT_INFARMA_USER_H
#define FPRINT_INFARMA_USER_H

//create a list of users
struct node_user * create_list_users();
//create a list with the content of users
struct node_user * createList(int id, unsigned char *digital);
//do a iter over the list of users
void iterOverList(struct node_user * head);
//add a user to the end og the list
void append(struct node_user * head, int id);


#endif //FPRINT_INFARMA_USER_H
