//
// Created by root on 09/10/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "utils.h"


//struct user_list *list_of_users;
//int number_of_users;
//int num_digitais;

/*struct user_list *list_of_users get_local_user_list(){

    return  list_of_users;
}*/

/*void update_list_of_users(){

    //free this later on
    char *json = get_full_user_list();
    get_number_of_users(json, &number_of_users);

    //Allocating structs to fill with user data
    //PS: Free this later on
    list_of_users = malloc(number_of_users * sizeof(struct user_list));
    num_digitais = deal_with_json(json, list_of_users);

    //free(json);

}*/

//
//void is_user_adm(int id, struct user_list * list_of_users, int number_of_users){
//    for(int i = 0; i < number_of_users; i++){
//        if (strcmp((list_of_users)[i].fingerprint, "") != 0){
//            if((list_of_users)[i].user_id == id){
//                printf("role: %s\n", (list_of_users)[i].role);
//                if (strcmp((list_of_users)[i].role, "ROLE_ADM") == 0){
//                    char admIdAsStr[12];
//                    sprintf(admIdAsStr, "%d", (list_of_users)[i].user_id);
//                    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "is_user_adm", "Usuário é adm", admIdAsStr);
//                }else{
//                    compose_json_answer("SCREEN_UPDATE", "ERROR", "is_user_adm", "Usuário não é adm", "-1");
//
//                }
//            }
//        }
//    }
//}
//
//
//void verify_adm(){
//
//    //lista de digitais
//    int num_digitais = 0;
//    unsigned char *ret;
//    int num_ret = 0;
//
//    //free this later on
//    char *json = get_full_user_list();
//
//    int number_of_users;
//    get_number_of_users(json, &number_of_users);
//
//    //Allocating structs to fill with user data
//    //PS: Free this later on
//    struct user_list *list_of_users = malloc(number_of_users * sizeof(struct user_list));
//    num_digitais = deal_with_json(json, list_of_users);
//
//    //criando lista de digitais
//    unsigned char digitais[num_digitais][12050];
//    int ids_list[num_digitais];
//
//    for (int i = 0; i < number_of_users; i++){
//        if (strcmp((list_of_users)[i].fingerprint, "") != 0){
////          printf("Id: %d\n", (list_of_users)[i].user_id);
////          printf("Name: %s\n", (list_of_users)[i].name);
//            ids_list[num_ret] = (list_of_users)[i].user_id;
//            string_to_fprint((list_of_users)[i].fingerprint, digitais[num_ret]);
//            num_ret++;
//        }
//    }
//
//    ///*Iniciando device*///
//
//    int r = 1;
//    struct fp_dscv_dev *ddev;
//    struct fp_dscv_dev **discovered_devs;
//    struct fp_dev *dev;
//    // struct fp_print_data *data;
//
//
//    r = fp_init();
//
//    if (r < 0) {
//        fprintf(stderr, "Failed to initialize libfprint\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao inicializar a libfprint", "");
//        return 1;
//    }
//
//    fp_set_debug(3);
//
//    discovered_devs = fp_discover_devs();
//    if (!discovered_devs) {
//        fprintf(stderr, "Could not discover devices\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao descobrir dispositivos", "");
//        return 1;
//    }
//    ddev = discover_device(discovered_devs);
//    if (!ddev) {
//        fprintf(stderr, "No devices detected.\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Nenhum dispositivo encontrado", "");
//        return 1;
//    }
//    dev = fp_dev_open(ddev);
//    fp_dscv_devs_free(discovered_devs);
//    if (!dev) {
//        fprintf(stderr, "Could not open device.\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao abrir dispositivo", "");
//        return 1;
//    }
//
//    printf("Opened device. It's now time to enroll your finger.\n");
//    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "do_point", "Dispositivo inicializado. Cadastre sua digital.", "");
//
//    ///Fim inicialização device
//
//    int result = compare_digital(dev, digitais, num_digitais, ids_list); //chamada em data.c
//
//    char userIdAsStr[12];
//    sprintf(userIdAsStr, "%d", result);
//
//
//    if(result > -1){
//        printf("id_user: %d\n", result);
//        is_user_adm(result, list_of_users, number_of_users);
//        //compose_json_answer("SCREEN_UPDATE", "SUCCESS", "do_point_final", "", compose_json_answer_user_matches(userIdAsStr,  get_name_by_id(result, list_of_users, number_of_users)));
//    }else {
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "User doesn't match", "-1");
//    }
//
//    printf("Result do_point:%d\n", result);
//    post_ponto(result);
//
//    out_close:
//    fp_dev_close(dev);
//    out:
//
//    fp_exit();
//
//
//
//}


char * get_name_by_id(int id, struct user_list * list_of_users, int number_of_users){
    for(int i = 0; i < number_of_users; i++){
        if (strcmp((list_of_users)[i].fingerprint, "") != 0){
            if((list_of_users)[i].user_id == id){
                return (list_of_users)[i].name;
            }
        }
    }
}