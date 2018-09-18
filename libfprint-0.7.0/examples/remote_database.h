//
// Created by leticia on 18/09/18.
//

#ifndef FPRINT_INFARMA_REMOTE_DATABASE_H
#define FPRINT_INFARMA_REMOTE_DATABASE_H

struct node_user * get_users(void);
void post_user(int id_usuario, char* digital, int tamanho_array);

#endif //FPRINT_INFARMA_REMOTE_DATABASE_H
