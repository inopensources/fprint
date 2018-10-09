//
// Created by root on 09/10/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "utils.h"


//todo: 'globalizar' lista de usuários, número de usuários e número de digitais e criar métodos de acesso para eles

//struct user_list *list_of_users;
//int number_of_users;
//int num_digitais;

/*struct user_list *list_of_users get_local_user_list(){

    return  list_of_users;
}*/

char * get_name_by_id(int id, struct user_list * list_of_users, int number_of_users){
    for(int i = 0; i < number_of_users; i++){
        if (strcmp((list_of_users)[i].fingerprint, "") != 0){
            if((list_of_users)[i].user_id == id){
                return (list_of_users)[i].name;
            }
        }
    }
}