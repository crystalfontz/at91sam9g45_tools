#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
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

    char buf[10];
    int ret;

    /* Using I2C Write, equivalent of 
       i2c_smbus_write_word_data(file, register, 0x6543) */
    buf[0] = strtol(argv[3], NULL, 16);
    buf[1] = strtol(argv[4], NULL, 16);

    ret = write(file, buf, 2);
    if (ret != 2) {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Failed to do the full write on to the i2c port handle (wrote %d, errno %d, %s)\n", ret, errno, strerror(errno));
    }

    if (ret >= 0)
        return 0;
    else
        return 1;
}
