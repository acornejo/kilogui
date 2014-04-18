#include <stdio.h>
#include <ftdi.h>

int main() {
    char buf[1024];
    int num;
    struct ftdi_context *ftdic;

    ftdic = ftdi_new();
    ftdic->usb_read_timeout = 5000;
    ftdic->usb_write_timeout = 1000;
    if (ftdi_usb_open(ftdic, 0x0403, 0x6001) != 0) {
        printf("Error: %s\n", ftdic->error_str);
        ftdi_free(ftdic);
        ftdic = NULL;
        return -1;
    } else if (ftdi_set_baudrate(ftdic, 38400) != 0) {
        printf("Error: %s\n", ftdic->error_str);
        ftdi_usb_close(ftdic);
        ftdi_free(ftdic);
        ftdic = NULL;
        return -1;
    } else if (ftdi_setflowctrl(ftdic, SIO_DISABLE_FLOW_CTRL) != 0) {
        printf("Error: %s\n", ftdic->error_str);
        ftdi_usb_close(ftdic);
        ftdi_free(ftdic);
        ftdic = NULL;
        return -1;
    } else if (ftdi_set_line_property(ftdic, BITS_8, STOP_BIT_1, NONE) != 0) {
        printf("Error: %s\n", ftdic->error_str);
        ftdi_usb_close(ftdic);
        ftdi_free(ftdic);
        ftdic = NULL;
        return -1;
    }


    while(1) {
        num = ftdi_read_data(ftdic, buf, 1024);
        if (num > 0) {
            buf[num] = '\0';
            printf("%s", buf);
            fflush(stdout);
        }
    }

    return 0;
}
