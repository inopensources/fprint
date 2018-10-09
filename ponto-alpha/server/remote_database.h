//
// Created by root on 05/10/18.
//

#ifndef UNTITLED_REMOTE_DATABASE_H
#define UNTITLED_REMOTE_DATABASE_H

void post_user(int id_usuario, char digital[], int tamanho_array);

int deal_with_json(char* json_str, struct user_list *list);

char *post_ponto(int id_usuario);

char *get_user_list_mini();

char *get_full_user_list();

int create_list_users();

#endif //UNTITLED_REMOTE_DATABASE_H
