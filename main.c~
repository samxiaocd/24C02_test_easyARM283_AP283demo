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
#define DATA_LEN 17
#define I2CBUS_SELECTED (i2cbusNo == 0) ? "/dev/i2c-0" : \
                       ((i2cbusNo == 1) ? "/dev/i2c-1" : \
                       ((i2cbusNo == 2) ? "/dev/i2c-2" : \
                        (i2cbusNo == 3) ? "/dev/i2c-3" : "/dev/i2c-4" \
                        ))

void help(void)
{
	printf("  ==========================================================\n");
	printf("  usage : \n");
	printf("      i2c_24c02_test  <I2Cbus number> [device address] [wsv] \n");	
	printf("      parameter: \n");
	printf("        I2Cbus number  : 0-4.  linux device number.\n");
	printf("            0: '/dev/i2c-0' \n");
	printf("            1: '/dev/i2c-1' \n");
	printf("        device address : 0x00~0xFF. 24c02 device address.  \n");
	printf("        wsv            : 0~255. write start value of 16 regs.  \n");
	printf("  example:  \n");
	printf("      i2c_24c02_test  0 \n");
	printf("      i2c_24c02_test  0 0xa0  \n");
	printf("      i2c_24c02_test  0 0xa0 66  \n");
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
    	int i, bytes, cntError;

    	unsigned char tx_buf[DATA_LEN];
    	unsigned char rx_buf[DATA_LEN];
    	unsigned char addr[2];
	int i2cbusNo=0, wsv=100;
	int i2cDeviceAddress = I2C_DEVICE_ADDRESS_DEFAULT;

	verinfo();
	for(i=0;  i<argc; i++) printf("    arg%d: %s \n", i, argv[i]);
   	if(i<2) {help(); return(-1);}

	//printf("Please input the I2C bus(0-4): ");
	//scanf("%d \n", &i2cn);
    	addr[0] = 0x00;

	i2cbusNo = atoi(argv[1]);
	if(argc >=3) sscanf(argv[2], "0x%x", &i2cDeviceAddress);
	if(argc >=4) sscanf(argv[3], "%d", &wsv);

	printf("  %d => select  %s,  Device address(8 bit): 0x%X \n", i2cbusNo, I2CBUS_SELECTED, i2cDeviceAddress);
    	GiFd = open(I2CBUS_SELECTED, O_RDWR);   
	printf("  I2CBUS> open %s : 0x%X(%d). > %s \n",I2CBUS_SELECTED, GiFd, GiFd, GiFd < 0 ? "FAILURE" : "SUCCESS");
    	if(GiFd < 0) return -1;		

  	uiRet = ioctl(GiFd, I2C_SLAVE, i2cDeviceAddress >> 1);
	printf("  I2CBUS> set slave address to 0x%02X: 0x%X(%d). > %s \n",i2cDeviceAddress,  uiRet, uiRet, uiRet < 0 ? "FAILURE" : "SUCCESS");
    	if (uiRet < 0) return -1;

  	uiRet = ioctl(GiFd, I2C_TENBIT, 0); // 8bit SLAVE ADDRESS
	printf("  SZ.2>>set SLAVE ADDRESS is 8 bit : 0x%02x\n", uiRet);


	tx_buf[0] = addr[0];
	for (i = 1; i < DATA_LEN; i++) tx_buf[i] = wsv+i-1;

    	bytes = write(GiFd, tx_buf, DATA_LEN);
	printf("  I2C write %d bytes : 0x%X(%d) bytes writed. > %s \n",DATA_LEN, bytes, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");
	//usleep(100*1000); // delay 100ms !!!
    	usleep(10*1000);    // delay 10ms !!!
    	bytes = write(GiFd, addr, 1);
	printf("  I2C write 1 bytes : 0x%X(%d) bytes writed. > %s \n", bytes, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");
	//usleep(100*1000); // delay 100ms
	for(i=0; i<sizeof(rx_buf); i++) rx_buf[i] = 0;
    	bytes = read(GiFd, rx_buf, DATA_LEN - 1);
	printf("  I2C read %d bytes: 0x%X(%d) bytes read back. > %s \n",DATA_LEN,  bytes, bytes, bytes < 0 ? "FAILURE" : "SUCCESS");

	cntError = 0;
    	printf("  read from 24C02's eeprom(hex) 0x: \n   ");
    	for(i = 0; i < DATA_LEN - 1; i++)
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
