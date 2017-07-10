//created 6/29/17 by james roach
//The pigpio deamon MUST be started before running this program
//start with: sudo pigpiod
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pigpiod_if2.h>

//address slave responds to 0x2A = battery default
#define BATTERY_ADDR 0x2A


int pi;

//send 2 byte response. clyde protocol only
void sendResponse(unsigned char* str, bsc_xfer_t * tPtr){
	
	printf("Sending response 0x%02x , 0x%02x \n", str[0], str[1]);
	strncpy(tPtr->txBuf, str, 2);
	tPtr->txCnt = 2;
	bsc_i2c(pi, BATTERY_ADDR, tPtr);
}

void fillResponse(unsigned char msb, unsigned char lsb, unsigned char * response){
	
	printf("Setting response to 0x%02x , 0x%02x \n", msb, lsb);
	response[0] = msb;
	response[1] = lsb;

}

//take in cmd array that has command code and parameter. modify response pointer to point to data you want to send back
void mockTest(unsigned char *cmd, unsigned char* rsp){
	
	printf("testing command 0x%02x, 0x%02x \n",cmd[0],cmd[1]);
	
	switch(*cmd){//simple response mock up for battery

		case 0x01:
			if(cmd[1]==0x00){
				fillResponse(0x00,0x00, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;
		case 0x03:
			if(cmd[1]==0x00){
				fillResponse(0x20,0x00, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;
		case 0x04:
			if(cmd[1]==0x00){
				fillResponse(0x34,0x27, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;	
		case 0x05:
			if(cmd[1]==0x00){
				fillResponse(0xC3,0xF5, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;
		case 0x80:
			fillResponse(0x00,0x00, rsp);
			break;
		case 0x10:
			fillResponse(0xE2, 0x80, rsp);
			break;
		case 0x31:
			if(cmd[1]==0x00){
				fillResponse(0x00,0x00, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;	
		case 0x32:
			if(cmd[1]==0x00){
				fillResponse(0x00,0x00, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;
		case 0x33:
			if(cmd[1]==0x00){
				fillResponse(0x00,0x00, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;
		case 0x90:
			if(cmd[1]==0x00){
				fillResponse(0x01,0x00, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;
		case 0x91:
			if(cmd[1]==0x00 || cmd[1] == 0x01){
				fillResponse(0x00,0x00, rsp);
			}else{
				fillResponse(0xFF,0xFF, rsp);
			}
			break;
		default:
			fillResponse(0xFF,0xFF,rsp);
			break;
	}//switch

}

//method that gets called on I2c write activity
void respond(int id, unsigned event, uint32_t tick){ 
	
	puts("Callback Responding");
		
	int status;
	bsc_xfer_t transfer;//struct that represents the transfer
	transfer.txCnt = 0;// 0 bytes to send. just want to read for now
	if((status=bsc_i2c(pi, BATTERY_ADDR, &transfer))>=0){//basic IO command. Just updates transfer buffer
		
		printf("Received %d bytes\n", transfer.rxCnt);
		if(transfer.rxCnt > 0){//if there is data to read
			
			unsigned char command[2];
			strncpy(command, transfer.rxBuf,2);//read it
			
			unsigned char response[2];
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
	int event = event_callback(pi, 31u, respond);//31u = value for i2c activity, respond = methode to call on activity
	bsc_i2c(pi, BATTERY_ADDR, &transfer);//initiate slave connection
	
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
