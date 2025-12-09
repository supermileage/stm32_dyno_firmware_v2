#ifndef INC_USB_USB_H_
#define INC_USB_USB_H_

#include <stdint.h>
#include "cmsis_os2.h"

#include "main.h"

#include "usbd_cdc_if.h"

#ifdef __cplusplus
extern "C" {
#endif

void usbcontroller_main(osMessageQueueId_t sessionControllerToBpmHandle);

#ifdef __cplusplus
}
#endif



#endif /* INC_USB_USB_H_ */
