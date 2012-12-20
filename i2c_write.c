/*
* Description:
*
* An I2C master write application
*
* License:
*
* Copyright 2012 Crystalfontz America, Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*/
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
