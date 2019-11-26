#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/***************************************************************
文件名	: ledApp.c
作者	  	: Mr.chen
版本	   	: V1.0
描述	   	: 应用测试例程
其他	   	: 无
博客 	   	: https://me.csdn.net/u014133923
GitHub  : https://github.com/pupilpro
日志	   	: [create][2019.11.6]
***************************************************************/

#define MAX_LEDS				4

static const char *gpio_dir[MAX_LEDS] = {
	"/sys/class/gpio/gpio960/direction",
	"/sys/class/gpio/gpio961/direction",
	"/sys/class/gpio/gpio962/direction",
	"/sys/class/gpio/gpio963/direction"
};

static const char *gpio_val[MAX_LEDS] = {
	"/sys/class/gpio/gpio960/value",
	"/sys/class/gpio/gpio961/value",
	"/sys/class/gpio/gpio962/value",
	"/sys/class/gpio/gpio963/value"
};

enum ledctl{
	LED_ON,
	LED_OFF
};

enum leds{
	LED_RUNNING = 960,
	LED_FAULT,
	LED_COMMUNICATION,
	LED_MOITON_INS
};

int main()
{
	int i = 0;
    int exportfd, valuefd[MAX_LEDS] = {0}, directionfd[MAX_LEDS] = {0};
 
    printf("GPIO test running...\n");
 
    // The GPIO has to be exported to be able to see it
    // in sysfs
 
    exportfd = open("/sys/class/gpio/export", O_WRONLY);
    if (exportfd < 0)
    {
        printf("Cannot open GPIO to export it\n");
        exit(1);
    }
 
	write(exportfd, "960", 4);
	write(exportfd, "961", 4);
	write(exportfd, "962", 4);
	write(exportfd, "963", 4);
	
	close(exportfd);
 
    printf("GPIO exported successfully\n");
 
    // Update the direction of the GPIO to be an output
 
	for(i = 0; i < MAX_LEDS; i++)
	{
		directionfd[i] = open(gpio_dir[i], O_RDWR);
		if (directionfd[i] < 0)
		{
			printf("Cannot open GPIO direction[%d] it\n", i);
			exit(1);
		}
		write(directionfd[i], "out", 4);
		close(directionfd[i]);
	}
 
    printf("GPIO direction set as output successfully\n");
 
    // Get the GPIO value ready to be toggled
 
	for(i = 0; i < MAX_LEDS; i++)
	{
		valuefd[i] = open(gpio_val[i], O_RDWR);
		if (valuefd[i] < 0)
		{
			printf("Cannot open GPIO%d value\n", i);
			exit(1);
		}
	}
    printf("GPIO value opened, now toggling...\n");
 
    // toggle the GPIO as fast a possible forever, a control c is needed
    // to stop it
 
    while (1)
    {
		for(i = 0; i < MAX_LEDS; i++)
		{
			write(valuefd[i],"1", 2);
			sleep(1);
			write(valuefd[i],"0", 2);
			sleep(1);
		}
    }
	
}
