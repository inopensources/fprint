//
// Created by leticia on 18/09/18.
//

#ifndef FPRINT_INFARMA_DEVICE_UTILS_H
#define FPRINT_INFARMA_DEVICE_UTILS_H

struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs);
struct fp_print_data *enroll(struct fp_dev *dev);

#endif //FPRINT_INFARMA_DEVICE_UTILS_H
