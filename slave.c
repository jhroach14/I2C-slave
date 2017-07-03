//created 6/29/17 by james roach
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pigpiod_if2.h>

//address slave responds to 0x2A = battery default
#define I2C_ADDR 0x2A

int pi;

//send 2 byte response. clyde protocol only
void sendResponse(char* str, bsc_xfer_t * tPtr){
	
	printf("Sending response %x , %x \n", str[0], str[1]);
	strncpy(tPtr->txBuf, str, 2);
	tPtr->txCnt = 2;
	bsc_i2c(pi, I2C_ADDR, tPtr);
}

//take in cmd array that has command code and parameter. modify response pointer to point to data you want to send back
void mockTest(char *cmd, char* rsp){
	
	printf("testing command %x, %x \n",cmd[0],cmd[1]);
	rsp[0] = 0x01; //just test values
	rsp[1] = 0x02;

}

//method that gets called on I2c write activity
void respond(int id, unsigned event, uint32_t tick){ 
	
	puts("Callback Responding");
	
	int status;
	bsc_xfer_t transfer;//struct that represents the transfer
	transfer.txCnt = 0;// 0 bytes to send. just want to read for now

	if((status=bsc_i2c(pi, I2C_ADDR, &transfer))>=0){//basic IO command. Just updates transfer buffer
		
		printf("Received %d bytes\n", transfer.rxCnt);
		if(transfer.rxCnt > 0){//if there is data to read
			
			char command[2];
			strncpy(command, transfer.rxBuf,2);//read it
			
			char response[2];
			mockTest(command, response);//get response to send back
			sendResponse(response, &transfer);//send it
		}
	}
	else{// below 0 = error
		puts(pigpio_error(status));
		
	}

	puts("callback exiting"); 
}

int main(){

	bsc_xfer_t transfer; 
	transfer.txCnt = 0;

	puts("Slave starting... ");
	
	if((pi = pigpio_start(NULL,NULL))<0){//null params use default vals
		printf("connection failed");
		exit(0);
	}
	
	puts("Connection obtained... ");
	int event = event_callback(pi, 31u, respond);
	bsc_i2c(pi, I2C_ADDR, &transfer);
	
	size_t size;
	char *input = NULL;
	int flag = 1;

	while(flag){ 
		
		getline(&input, &size, stdin);
		
		if(strcmp(input, "exit")==0){
			flag=0;
		} 
	}
	  
	
	event_callback_cancel(event);

	bsc_i2c(pi,0,&transfer);
	pigpio_stop(pi);
}
