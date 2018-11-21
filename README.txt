#Compilar projeto
gcc device_utils.c remote_database.c utils.h decode.c encode.c verify_live.c -lfprint -lwebsockets -l json-c -lcurl 
