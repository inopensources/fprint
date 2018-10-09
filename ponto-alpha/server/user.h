//
// Created by root on 09/10/18.
//

#ifndef PONTO_INF_USER_H
#define PONTO_INF_USER_H

//struct user_list *list_of_users get_local_user_list();

void is_user_adm(int id, struct user_list * list_of_users, int number_of_users);

char * get_name_by_id(int id, struct user_list * list_of_users, int number_of_users);
#endif //PONTO_INF_USER_H
