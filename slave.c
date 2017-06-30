//created 6/29/17 by james roach
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pigpiod_if2.h>

//address slave responds to
#define I2C_ADDR 0x13

int pi;

//str needs to be null terminated
void sendString(char* str, bsc_xfer_t * tPtr){

	strcpy(tPtr->txBuf, str);
	transfer.txCnt = strlen(str);
	bsc_i2c(pi, I2C_ADDR, &transfer);


}

char * mockTest(char *cmd){

	

}

//method that gets called when i2c command recieved
void respond(int id, unsigned event, uint32_t tick){
	
	puts("Callback Responding");
	
	int status;
	bsc_xfer_t transfer;//struct that represents the transfer
	transfer.txCnt = 0;

	if((status=bsc_i2c(pi, I2C_ADDR, &transfer))>=0){//basic IO command. Just updates transfer buffer
		
		printf("Received %d bytes\n", transfer.rxCnt);
		if(transfer.rxCnt > 0){//if there is data to read
			
			char command[transfer.rxCnt];
			strcpy(command, transfer.rxBuf);//read it
			command[transfer.rxCnt] = '\0'
			
			char * response = mockTest(command);//get response to send back
			sendString(response, &transfer);//send it
		}
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
		
	if((pi = pigpio_start(NULL,NULL))<0){
		printf("connection failed");
		exit(0);
	}

	int event = event_callback(pi, 31u, responsd);
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
