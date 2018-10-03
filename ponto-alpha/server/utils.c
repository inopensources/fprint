//
// Created by leticia on 14/09/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char * compose_json_answer(char status[], char method_name[], char message[]){
    int length_of_parameters = strlen(status) + strlen(method_name) + strlen(message);

    char json_0[] = "{\n";
    char json_1[] = " \"status\" : ";
    char json_2[] = " \"method_name\" : ";
    char json_3[] = " \"message\" : ";
    char json_4[] = "}\n";

    int json_size = strlen(json_0) + strlen(json_1) + strlen(status) + strlen(json_2) + strlen(method_name) + strlen(json_3)+ strlen(message) + strlen(json_4);

    //+10 to take each \0 into consideration
    char * json = calloc(json_size + 10, sizeof(char));

    sprintf(json, "{\n \"status\" : \"%s\",\n \"method_name\" : \"%s\",\n \"message\": \"%s\"\n}", status, method_name, message);

    return json;

}