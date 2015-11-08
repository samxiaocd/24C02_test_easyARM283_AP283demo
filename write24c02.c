#include     <stdio.h>
#include     <stdlib.h> 
#include     <unistd.h>  
#include     <sys/types.h>
#include     <sys/stat.h>
#include     <fcntl.h> 
#include     <termios.h>
#include     <errno.h>  

#include "iic.h"

#define I2C_DEVICE_ADDRESS_DEFAULT  0xA0
#define DATA_LEN 256
#define I2CBUS_SELECTED (i2cbusNo == 0) ? "/dev/i2c-0" : \
                       ((i2cbusNo == 1) ? "/dev/i2c-1" : \
                       ((i2cbusNo == 2) ? "/dev/i2c-2" : \
                        (i2cbusNo == 3) ? "/dev/i2c-3" : "/dev/i2c-4" \
                        ))

void help(void)
{
	printf("  ==========================================================\n");
	printf("  usage : \n");
	printf("      write24c  <I2Cbus number> <device address> <offset> <value1> [value2 ... valueN] \n");	
	printf("      parameter: \n");
	printf("        I2Cbus number  : 0-4.  linux device number.\n");
	printf("            0: '/dev/i2c-0' \n");
	printf("            1: '/dev/i2c-1' \n");
	printf("        device address : 0x00~0xFF(0xA0). 24c02 device address.  \n");
	printf("        offset         : 0~255(0).  start register.  \n");
	printf("        valueN         : 0~255(16). values.  \n");
	printf("  example:  \n");
	printf("      write24c  0 0xa0 0 10  \n");
	printf("      write24c  0 0xa0 2 51 98 201 \n");
	printf("  ==========================================================\n");
}
void verinfo(void)
{
	printf("  ==========================================================\n");
	printf("    prj: I2C test for 24c02\n");
	printf("      hardware: zlg.cn: easyARM-i.MX283A & AP-283Demo\n");
	printf("    ver: v1.0.0\n");
	printf("    bld: %s %s\n", __DATE__, __TIME__);
	printf("    aut: SamXIAO(139585237@qq.com)\n");
	printf("  ==========================================================\n");
	printf("  comand line input:\n");
}

int main(int   argc,   char*   argv[])
{
//    	unsigned int uiRet;
    	int uiRet;
    	int i, bytes, cntError, iOffset=0, iNum=16;

    	unsigned char tx_buf[DATA_LEN];
    	unsigned char rx_buf[DATA_LEN];
    	unsigned char addr[2];
	int i2cbusNo=0, wsv=100;
	int i2cDeviceAddress = I2C_DEVICE_ADDRESS_DEFAULT;

	verinfo();
	for(i=0;  i<argc; i++) printf("    arg%d: %s \n", i, argv[i]);
   	if(i<5) {help(); return(-1);}

	//printf("Please input the I2C bus(0-4): ");
	//scanf("%d \n", &i2cn);

	i2cbusNo = atoi(argv[1]);
	sscanf(argv[2], "0x%x", &i2cDeviceAddress);
	sscanf(argv[3], "%d", &iOffset);
	iNum = argc - 4;
	for (i = 1; i < DATA_LEN; i++) tx_buf[i] = 33;
	for (i = 0; i < iNum; i++)
	{
		sscanf(argv[4+i], "%d", &uiRet);   
		tx_buf[i+1] = uiRet;
	}
    	addr[0] = iOffset;
	tx_buf[0] = addr[0];

	printf("  %d => select  %s\n", i2cbusNo, I2CBUS_SELECTED);
	printf("    Device address(8 bit): 0x%X \n", i2cDeviceAddress);
	printf("    reg offset: %d\n",tx_buf[0]);

    	GiFd = open(I2CBUS_SELECTED, O_RDWR);   
	printf("  I2CBUS> open %s : 0x%X(%d). > %s \n",I2CBUS_SELECTED, GiFd, GiFd, GiFd < 0 ? "FAILURE" : "SUCCESS");
    	if(GiFd < 0) return -1;		


  	uiRet = ioctl(GiFd, I2C_SLAVE, i2cDeviceAddress >> 1);
	printf("  I2CBUS> set slave address to 0x%02X: 0x%X(%d). > %s \n",i2cDeviceAddress,  uiRet, uiRet, uiRet < 0 ? "FAILURE" : "SUCCESS");
    	if (uiRet < 0) return -1;


  	uiRet = ioctl(GiFd, I2C_TENBIT, 0); // 8bit SLAVE ADDRESS
	printf("  SZ.2>>set SLAVE ADDRESS is 8 bit : 0x%02x\n", uiRet);


    	bytes = write(GiFd, tx_buf, iNum + 1);
	printf("  I2C write %d bytes : 0x%X(%d) bytes writed. > %s \n",iNum, bytes, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");
	//usleep(100*1000); // delay 100ms !!!
    	usleep(10*1000);    // delay 10ms !!!

    	bytes = write(GiFd, addr, 1);
	printf("  I2C write 1 bytes : 0x%X(%d) bytes writed. > %s \n", bytes, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");
	//usleep(100*1000); // delay 100ms

	for(i=0; i<sizeof(rx_buf); i++) rx_buf[i] = 0;
    	bytes = read(GiFd, rx_buf, iNum);
	printf("  I2C read %d bytes: 0x%X(%d) bytes read back. > %s \n", iNum,  bytes, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");

	cntError = 0;
    	printf("  read from 24C02's eeprom(hex) from REG-%d: \n   ", iOffset);
    	for(i = 0; i < iNum; i++)
	{
         	printf(" %x", rx_buf[i]);
		if(rx_buf[i] != tx_buf[i+1]) cntError++;
    	}	
    	printf("\n");
	printf("    error bytes:%d. \n",  cntError);
	printf("    24c02 readback after writing : %s!\n", cntError ? " WRONG" : " OK");
	
	close(GiFd);
	return 0;
}
