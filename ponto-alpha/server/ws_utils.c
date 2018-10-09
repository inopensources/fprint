//
// Created by caws on 30/09/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>

void write_back(struct lws *wsi, char message[]){
    int size_of_message = strlen(message);

    unsigned char *buf = (unsigned char*) calloc(LWS_SEND_BUFFER_PRE_PADDING + size_of_message +
                                                 LWS_SEND_BUFFER_POST_PADDING, sizeof(char));

    for (int i=0; i < size_of_message; i++) {
        buf[LWS_SEND_BUFFER_PRE_PADDING + i ] = message[i];
    }

    lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], size_of_message, LWS_WRITE_TEXT);
    free(buf);
}