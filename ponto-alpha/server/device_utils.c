//
// Created by root on 09/10/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libfprint/fprint.h>

int verify(struct fp_dev *dev, struct fp_print_data *data)
{
    int r;

    do {
        sleep(1);
        printf("\nScan your finger now.\n");


        r = fp_verify_finger(dev, data);


        if (r < 0) {
            printf("verification failed with error %d :(\n", r);
            return r;
        }
        switch (r) {
            case FP_VERIFY_NO_MATCH:
                printf("NO MATCH!\n");
                return 0;
            case FP_VERIFY_MATCH:
                printf("MATCH!\n");
                return 1;
            case FP_VERIFY_RETRY:
                printf("Scan didn't quite work. Please try again.\n");
                break;
            case FP_VERIFY_RETRY_TOO_SHORT:
                printf("Swipe was too short, please try again.\n");
                break;
            case FP_VERIFY_RETRY_CENTER_FINGER:
                printf("Please center your finger on the sensor and try again.\n");
                break;
            case FP_VERIFY_RETRY_REMOVE_FINGER:
                printf("Please remove finger from the sensor and try again.\n");
                break;
        }
    } while (1);
}
