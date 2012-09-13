/**
 * Usage: kbsend {command}
 *
 * Uses libftdi to send {command} to the KiloBot overhead controller.
 *
 * @author Alex Cornejo
 * @version 1.0
 */

#include <ftdi.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char *name;
    char *cmd;
} kilo_cmd_t;

static kilo_cmd_t KILO_COMMANDS[] = {{"Bootload", "a"}, {"Sleep", "b"}, {"Wakeup", "c"}, {"Pause", "d"}, {"Voltage", "e"}, {"Run", "f"}, {"Charge", "g"}, {"Stop", "h"}, {"LedsToggle", "i"}, {"BootloadMsg", "j"}, {"Reset", "z"}};

static NUM_KILO_COMMANDS = sizeof(KILO_COMMANDS)/sizeof(kilo_cmd_t);

void error(struct ftdi_context *ftdic)
{
    printf("Error: %s\n", ftdic->error_str);
    ftdi_usb_close(ftdic);
    exit(-1);
}

int main(int argc, char *argv[])
{
    int i=0;
    if (argc > 1) {
        char *cmd='\0';
        for (i=0; i<NUM_KILO_COMMANDS; i++)
            if (!strcasecmp(KILO_COMMANDS[i].name, argv[1])) {
                cmd = KILO_COMMANDS[i].cmd;
                break;
            }
        if (cmd != '\0') {
            struct ftdi_context *ftdic = ftdi_new();
            ftdic->usb_read_timeout = 5000;
            ftdic->usb_write_timeout = 1000;
            if (ftdi_usb_open(ftdic, 0x0403, 0x6001) != 0) {
                printf("Error: Unable to open usb device\n");
                return -1;
            }
            if (ftdi_set_baudrate(ftdic, 19200) != 0)
                error(ftdic);
            if (ftdi_setflowctrl(ftdic, SIO_DISABLE_FLOW_CTRL) != 0)
                error(ftdic);
            if (ftdi_set_line_property(ftdic, BITS_8, STOP_BIT_1, NONE) != 0)
                error(ftdic);
            if (ftdi_write_data(ftdic, cmd, strlen(cmd)) != strlen(cmd))
                error(ftdic);
            ftdi_usb_close(ftdic);
            printf("Command \"%s\" send successfuly!\n", cmd);
        } else {
            printf("Error: Command \"%s\" not found!\n", argv[1]);
            return -1;
        }
    }
    else {
        printf("Usage: kbsend {command}\n");
        printf("  The following commands are available:\n");
        for (i=0; i<NUM_KILO_COMMANDS; i++) {
            printf("       %s\n", KILO_COMMANDS[i].name);
        }
    }

    return 0;
}
