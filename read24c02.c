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
	printf("      read24c  <I2Cbus number> [device address] [offset] [len] \n");	
	printf("      parameter: \n");
	printf("        I2Cbus number  : 0-4.  linux device number.\n");
	printf("            0: '/dev/i2c-0' \n");
	printf("            1: '/dev/i2c-1' \n");
	printf("        device address : 0x00~0xFF(0xA0). 24c02 device address.  \n");
	printf("        offset         : 0~255(0).  start register.  \n");
	printf("        len            : 0~255(16). read number.  \n");
	printf("  example:  \n");
	printf("      read24c  0 \n");
	printf("      read24c  0 0xa0  \n");
	printf("      read24c  0 0xa0 0 10  \n");
	printf("      read24c  0 0xa0 2 5  \n");
	printf("  ==========================================================\n");
}
void verinfo(void)
{
	printf("  ==========================================================\n");
	printf("    prj: read 24c02\n");
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
    	int i, bytes, iOffset=0, iNum=16;

    	unsigned char rx_buf[DATA_LEN];
    	unsigned char addr[2];
	int i2cbusNo=0;
	int i2cDeviceAddress = I2C_DEVICE_ADDRESS_DEFAULT;

	verinfo();
	for(i=0;  i<argc; i++) printf("    arg%d: %s \n", i, argv[i]);
   	if(i<2) {help(); return(-1);}

	//printf("Please input the I2C bus(0-4): ");
	//scanf("%d \n", &i2cn);

	i2cbusNo = atoi(argv[1]);
	if(argc >=3) sscanf(argv[2], "0x%x", &i2cDeviceAddress);
	if(argc >=4) sscanf(argv[3], "%d", &iOffset);
	if(argc >=5) sscanf(argv[4], "%d", &iNum);

	printf("  %d => select  %s,  Device address(8 bit): 0x%X \n", i2cbusNo, I2CBUS_SELECTED, i2cDeviceAddress);
    	GiFd = open(I2CBUS_SELECTED, O_RDWR);   
	printf("  I2CBUS> open %s : 0x%X(%d). > %s \n",I2CBUS_SELECTED, GiFd, GiFd, GiFd < 0 ? "FAILURE" : "SUCCESS");
    	if(GiFd < 0) return -1;		

  	uiRet = ioctl(GiFd, I2C_SLAVE, i2cDeviceAddress >> 1);
	printf("  I2CBUS> set slave address to 0x%02X: 0x%X(%d). > %s \n",i2cDeviceAddress,  uiRet, uiRet, uiRet < 0 ? "FAILURE" : "SUCCESS");
    	if (uiRet < 0) return -1;

  	uiRet = ioctl(GiFd, I2C_TENBIT, 0); // 8bit SLAVE ADDRESS
	printf("  SZ.2>>set SLAVE ADDRESS is 8 bit : 0x%02x\n", uiRet);

    	addr[0] = iOffset;
    	bytes = write(GiFd, addr, 1);
	printf("  I2C write 1 bytes : 0x%X(%d) bytes writed. > %s \n", bytes, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");
	//usleep(100*1000); // delay 100ms
	for(i=0; i<sizeof(rx_buf); i++) rx_buf[i] = 0;
    	bytes = read(GiFd, rx_buf, iNum - 1);
	printf("  I2C read expect %d bytes :  %d bytes got . > %s \n",iNum, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");

    	printf("  read from 24C02's eeprom(hex) from REG-%d:\n   ", iOffset);
    	for(i = 0; i < iNum - 1; i++)
	{
         	printf(" %x", rx_buf[i]);
    	}	
    	printf("\n");
	
	close(GiFd);
	return 0;
}
