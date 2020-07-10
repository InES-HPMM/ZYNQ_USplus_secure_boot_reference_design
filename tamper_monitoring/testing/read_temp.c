/*
 * Read out temperature from the internal sensor
 *
 * Author: Tobias Vögeli, Thierry Delafontaine
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define TEMP_RAW        "/sys/bus/iio/devices/iio:device0/in_temp0_ps_temp_raw"
#define TEMP_OFFSET     "/sys/bus/iio/devices/iio:device0/in_temp0_ps_temp_offset"
#define TEMP_SCALE      "/sys/bus/iio/devices/iio:device0/in_temp0_ps_temp_scale"

#define KELVIN          273.15

/*  Global variables */

/*  Local functions */

/*
** Read float value from sysfs file
 */
static int read_file(const char *path, float *retval) {
    FILE *fp;
    fp = fopen(path, "r");
    if(fp == NULL) {
        printf("ERROR: %s\n", strerror(errno));
        return 1; 
    }
    fscanf(fp, "%f", retval);
    fclose(fp);
    return 0;
}
/*
** Read temperature and give it back in celsius
 */ 
static float read_temp(void) {
    float temp_raw, temp;

    /*  Read all values */
    if(read_file(TEMP_RAW, &temp_raw)) {
        return 0;
    }
    
    /*  Convert to value in Celsius */
    temp = (temp_raw * 0.007771514990234375) - 280.23087870; //Conversion found in UG580

    return temp;
}

/*
** Read temperature and give it back in celsius
** The way described in the AMS driver description is used
 */ 
static float read_temp_ams(void) {
    float temp_raw, temp_offset, temp_scale, temp;

    /*  Read all values */
    if(read_file(TEMP_RAW, &temp_raw)) {
        return 0;
    }
    if(read_file(TEMP_OFFSET, &temp_offset)) {
        return 0;
    }
    if(read_file(TEMP_SCALE, &temp_scale)) {
        return 0;
    }

    /*  Convert to value in Celsius */
    temp = temp_scale * (temp_raw + temp_offset) / 1000;

    return temp;
}

/*
** Print help
 */
static void help_function (void) {
	printf("USAGE:\n");
	printf("./read_temp -<k/c> \n");
	printf("\tDifferent modes:\n");
	printf("\t\t-c\t-\tTemperature in Celsius\n");
	printf("\t\t-k\t-\tTemperature in Kelvin\n");
    printf("\t\t-ct\t-\tTemperature in Celsius (AMS driver)\n\n");
}

int main(int argc, char **argv) {

    float value;
    printf("\n");
	/* Read temperature in celsius */
	if ((argc == 2) && !strcmp(argv[1], "-c")) {
        while(1) {
            printf("\rTemperature: %3.3f\tC", read_temp());
            fflush(stdout);
            usleep(100000);
        }
    } else if ((argc == 2) && !strcmp(argv[1], "-k")) {
        while(1) {
            printf("\rTemperature: %3.3f\tK", read_temp() + 273.15);
            fflush(stdout);
            usleep(100000);
        }
    } else if ((argc == 2) && !strcmp(argv[1], "-ct")) {
        while(1) {
            printf("\rTemperature: %3.3f\tC", read_temp_ams());
            fflush(stdout);
            usleep(100000);
        }
    } else {
	/* write help on any other case */
		help_function();
	}

    return 0;
}
