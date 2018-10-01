//
// Created by leticia on 14/09/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}
char * read_digital(){

    ///*get a string and return the array of char*///
    char digital[42682];
    unsigned char *ret_returned = alloca(12050); // todo: generalizar length
    //unsigned char *ret_returned = (unsigned char *)malloc(7088*sizeof(unsigned char));
    int pos = 0;
    FILE *fp = fopen("digital.txt", "r");
    fgets(digital, 42682, fp);
    fclose(fp);
    char** tokens;
    //printf("digital lida = %s\n\n", digital);
    //tokens é uma array de char
    tokens = str_split(digital, ',');
    if (tokens)
    {
        int  i;
        int num;
        for (i = 0; *(tokens + i); i++)
        {
            //num é um int com valor inteiro equivalente ao token
            num = atoi(*(tokens + i));
            //printf("%d ",num);
            //printf("%c ", num);
            *(ret_returned + i) = num;
            //printf("%c ", *(ret_returned + i));
            free(*(tokens + i));
        }

        printf("\nlength digital from file: %d\n", i);
        free(tokens);
    }

    return ret_returned;

    //unsigned char *digital = (unsigned char *)malloc(41048*sizeof(unsigned char));
    //char *digital = (unsigned char *)malloc(20000*sizeof(char));
    //int indice = 1;
    //FILE *fp = fopen("digital.txt", "r");
    //char c;
    //char temp;

    /*do
	{

		//faz a leitura do caracter no arquivo apontado por pont_arq
		c = getc(fp);
		if(c != '[' && c != ']' && c != ' '){

			if (c == ','){
				digital[indice] = temp;
				temp = ' ';
				indice++;
			}else{
				temp = temp + c;
				printf("%c\n" , temp);
			}


		}

	}while (c != EOF);*/
}


char * fprint_to_string(char * ret, int length){


    int i;
    char *digital = alloca(4*length);
    for (i = 0; i<length; i++)
    {
        unsigned char digital2[12];
        if (i == 0){
            sprintf(digital2, "[%d, ", (int)(*(ret+i)));
        }else if(i == length-1 ){
            sprintf(digital2, "%d] ", (int)(*(ret+i)));

        }else{
            sprintf(digital2, "%d, ", (int)(*(ret+i)));
        }
        ;
        strcat(digital, digital2);
    }

    return digital;

}