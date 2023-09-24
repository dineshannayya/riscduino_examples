#define int_reg_dmem            (*(volatile uint32_t*)0x08000100)    // DMEM Register
#define char_reg_dmem           (*(volatile uint8_t*) 0x08000100)    // DMEM Register

#define BIST_DATA_PAT_TYPE1 0x55555555
#define BIST_DATA_PAT_TYPE2 0x33333333
#define BIST_DATA_PAT_TYPE3 0x0F0F0F0F
#define BIST_DATA_PAT_TYPE4 0x00FF00FF
#define BIST_DATA_PAT_TYPE5 0x0000FFFF
#define BIST_DATA_PAT_TYPE6 0xFFFFFFFF
#define BIST_DATA_PAT_TYPE7 0x01010101
#define BIST_DATA_PAT_TYPE8 0x00000000

bool TestStatus = true;

int hextochar(char iData){

switch(iData) {
case 0 : return '0';
case 1 : return '1';
case 2 : return '2';
case 3 : return '3';
case 4 : return '4';
case 5 : return '5';
case 6 : return '6';
case 7 : return '7';
case 8 : return '8';
case 9 : return '9';
case 0xA : return 'A';
case 0xB : return 'B';
case 0xC : return 'C';
case 0xD : return 'D';
case 0xE : return 'E';
case 0xF : return 'F';
}

return ' ';

}
void print_var(int iData) {

	   Serial.write(hextochar((iData >> 28) & 0xF));
	   Serial.write(hextochar((iData >> 24) & 0xF));
	   Serial.write(hextochar((iData >> 20) & 0xF));
	   Serial.write(hextochar((iData >> 16) & 0xF));
	   Serial.write(hextochar((iData >> 12) & 0xF));
	   Serial.write(hextochar((iData >> 8) & 0xF));
	   Serial.write(hextochar((iData >> 4) & 0xF));
	   Serial.write(hextochar((iData ) & 0xF));
  }

void run_dmem_mem_check(int iPattern,int iMemSize) {

  // put your main code here, to run repeatedly:
int *int_ptr, *int_addr;

int iSum;
int i,j;
int iTxData;
int iRxData;

int_ptr = &int_reg_dmem;

// Write 2K Location in 32 Bit Aligned write and Read
Serial.print("########### Testing DATA Memory in 32 Bit Aligned Write ###########");
Serial.println();

for(j=0; j < iMemSize; j++) {

    for(i=0; i < 32; i++){
      iTxData = iPattern;
      int_addr = int_ptr + (j*32 + i);
      *int_addr = iTxData;
      Serial.print("Writing DATA Memory Reg Addr:");
      print_var(int_addr);
      Serial.print("=> Val:");
      print_var(iTxData);
      Serial.println();
    }
}
Serial.print("########### Testing DATA Memory in 32 Bit Aligned Read Back and Verify-1 ###########");
Serial.println();
for(j=0; j < iMemSize; j++) {
    for(i=0; i < 32; i++){
      iTxData = iPattern;
      int_addr = int_ptr + (j*32 + i);
      iRxData = *int_addr;
      Serial.print("Reading DATA Memory Reg Addr:");
      print_var(int_addr);
      Serial.print("=> TxData:");
      print_var(iTxData);
      Serial.print("=> RxData:");
      print_var(iRxData);      
      if(iTxData == iRxData) Serial.print("=> Matched");
      else { TestStatus = false; Serial.print("=> FAIL"); }
      Serial.println();
    }
}

Serial.print("########### Testing DATA Memory in 32 Bit Aligned Read Back and Verify-2 ###########");
Serial.println();
for(j=0; j < iMemSize; j++) {
    for(i=0; i < 32; i++){
      iTxData = iPattern;
      int_addr = int_ptr + (j*32 + i);
      iRxData = *int_addr;
      Serial.print("Reading DATA Memory Reg Addr:");
      print_var(int_addr);
      Serial.print("=> TxData:");
      print_var(iTxData);
      Serial.print("=> RxData:");
      print_var(iRxData);      
      if(iTxData == iRxData) Serial.print("=> Matched");
      else { TestStatus = false; Serial.print("=> FAIL"); }
      Serial.println();
    }
}


}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  run_dmem_mem_check(BIST_DATA_PAT_TYPE1,4);
  run_dmem_mem_check(BIST_DATA_PAT_TYPE2,4);
  run_dmem_mem_check(BIST_DATA_PAT_TYPE3,4);
  run_dmem_mem_check(BIST_DATA_PAT_TYPE4,4);
  run_dmem_mem_check(BIST_DATA_PAT_TYPE5,4);
  run_dmem_mem_check(BIST_DATA_PAT_TYPE6,4);
  run_dmem_mem_check(BIST_DATA_PAT_TYPE7,4);
  run_dmem_mem_check(BIST_DATA_PAT_TYPE8,4);
  
  if(TestStatus==false) Serial.println("#### DATA MEMORY TEST FAILED ####");
  else Serial.println("#### DATA Memory TEST PASSED #####");
}



void loop() {

}
