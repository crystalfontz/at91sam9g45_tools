#include <stdio.h>
#include <stdlib.h>
#include <linux-i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char **argv, char **env)
{

    int file;
    file = open(argv[1], O_RDWR);
    if (file < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Failed to open i2c port\n");
        exit(1);
    }

    int addr = strtol(argv[2], NULL, 16); /* The I2C address */

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("Failed to set slave adress on i2c port handle\n");
        exit(1);
    }

    int ret;

    ret = i2c_smbus_read_byte(file);
    if (ret >= 0) {
        printf("Read: 0x%02X\n", ret);
    }
    else {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Failed to do the full read on to the i2c port handle (read %d, errno %d, %s)\n", ret, errno, strerror(errno));
    }

    if (ret >= 0)
        return 0;
    else
        return 1;
}
