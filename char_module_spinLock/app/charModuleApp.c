#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"

/***************************************************************
文件名	: charModuleApp.c
作者	  	: Mr.chen
版本	   	: V1.0
描述	   	: 应用测试例程
其他	   	: 无
博客 	   	: https://me.csdn.net/u014133923
GitHub  : https://github.com/pupilpro
日志	   	: [create][2019.11.26]
***************************************************************/
char *filename = "/dev/chardevbase";

/*
 * @description : main 主程序
 * @param - argc : argv 数组元素个数
 * @param - argv : 具体参数
 * @return : 0 成功;其他 失败
*/
int main()
{
	int fd,err;
	char data[1];

	/*打开设备*/
	fd = open(filename, O_RDWR);
	if(fd < 0){
		printf("file %s open failed!\r\n",filename);
		return -1;
	}
	printf("file %s open success!\r\n",filename);
	
	/*读操作*/
	err = read(fd,data,sizeof(data));
	if(err < 0){
		printf("read failed!\r\n");
		return -1;
	}
	
	/*写操作*/
	err = write(fd, data, sizeof(data));
	if(err < 0){
		printf("write failed!\r\n");
		return -1;
	}
	
	while(1)
	{
		printf("test...\r\n");
		sleep(1);
	}
	
	/*关闭设备*/
	err = close(fd);
	if(err < 0){
		printf("close failed!\r\n");
		return -1;
	}
	
	printf("Process Done\r\n");
	return 0;
}



