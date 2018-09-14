//
// Created by leticia on 14/09/18.
//
#include <stdlib.h>
#include <stdio.h>
#include "cadastro.c"
#include "ponto.c"


void menu(int opcao){

    switch(opcao){
        case 1:
            do_point();
            break;
        case 2:
            cadastra_user();
            break;
        case 3:
            printf("Tchau\n");
            break;
        default:
            printf("Opção inválida\n");
    }
}

void init(){
    int opcao = 0;
    scanf("%d", &opcao);
    while(opcao !=3){
        printf("\n1 - Ponto\n2 - Cadastrar\n3-  Sair\n");
        menu(opcao);
    }
}

int main(){
    init();
}