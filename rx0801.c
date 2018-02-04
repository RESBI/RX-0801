

//#include "avr/pgmspace.h"
#include <stdio.h>
#include <unistd.h>

long RAM[0xFF];
long ROM[0xFF] = {
  /* A LITTLE TEST PROGRAM.
00   15 04    MAIN: MIC LOOP
02   14 F0          MCR F
04   15 0F    LOOP: MIC F
06   14 A0          MCR A
08   15 0B          MIC B
0A   14 B0          MCR B
0C   02 AB          MIN A B
0E   14 D0          MCR D
10   15 02          MIC 2
12   14 10          MCR 1
14   15 02          MIC 2
16   14 20          MCR 2
18   03 12          MUL 1 2
1A   14 90          MCR 9
1C   15 02          MIC 2
1E   14 80          MCR 8
20   01 D9          ADD D 9
22   14 70          MCR 7
24   04 78          DIV 7 8
26   14 F0          MCR F
   */
  0x15, 0x04, 0x14, 0xF0,
  0x15, 0x0F, 0x14, 0xA0,
  0x15, 0x0B, 0x14, 0xB0,
  0x02, 0xAB, 0x14, 0xD0,
  0x15, 0x02, 0x14, 0x10,
  0x15, 0x02, 0x14, 0x20,
  0x03, 0x12, 0x14, 0x90,
  0x15, 0x02, 0x14, 0x80,
  0x01, 0xD9, 0x14, 0x70,
  0x04, 0x78, 0x14, 0xF0

};

long registers[0xF];
long cache;

long NOW = 0xA;
long TEMPH;
long TEMPL;

long CTREG(long R, long W, long Address, long Data){
    if(R){
        long Res = registers[Address];
        return Res;
    }else if(W){
        registers[Address] = Data;
        return 0;
    }
}

long CTCACHE(long R, long W, long Data){
    if(R){
        long Res = cache;
        return Res;
    }else if(W){
        cache = Data;
        return 0;
    }
}

long CTROM(long R, long W, long Address, long Data){
    if(R){
        long Res = ROM[Address];
        return Res;
    }else if(W){
        ROM[Address] = Data;
        return 0;
    }
}

long CTRAM(long R, long W, long Address, long Data){
    if(R){
        long Res = RAM[Address];
        return Res;
    }else if(W){
        RAM[Address] = Data;
        return 0;
    }
}

void CTR(long Rl){
    long CData = CTCACHE(1, 0, 0x00);
    CTREG(0, 1, Rl, CData);
}

void RTO(long Rl, long Ol){
    long OAddress = CTREG(1, 0, Ol, 0x00);
    long RData = CTREG(1, 0, Rl, 0x00);
    CTROM(0, 1, OAddress, RData);
}

void OTR(long Rl, long Ol){
    long OAddress = CTREG(1, 0, Ol, 0x00);
    long OData = CTROM(1, 0, OAddress, 0x00);
    CTREG(0, 1, Rl, OData);
}

void RTA(long Rl, long Al){
    long AAddress = CTREG(1, 0, Al, 0x00);
    long RData = CTREG(1, 0, Rl, 0x00);
    CTRAM(0, 1, AAddress, RData);
}

void ATR(long Rl, long Al){
    long AAddress = CTREG(1, 0, Al, 0x00);
    long AData = CTROM(1, 0, AAddress, 0x00);
    CTREG(0, 1, Rl, AData);
}

void CMP(long rA1, long rA2){
    long RA1 = CTREG(1, 0, rA1, 0x00);
    long RA2 = CTREG(1, 0, rA2, 0x00);
    if(RA1 != RA2) TIMER();
}

void OTHER(long cmd, long Data){
    long reg1 = Data >> 4;
    long reg2 = Data - (reg1 << 4);
    
    if(cmd == 0) CMP(reg1, reg2);
}

void CTL(long cmd, long Data){
    long reg1 = Data >> 4;
    long reg2 = Data - (reg1 << 4);
    
    if(cmd == 0) ATR(reg1, reg2);
    else if(cmd == 1) RTA(reg1, reg2);
    else if(cmd == 2) OTR(reg1, reg2);
    else if(cmd == 3) RTO(reg1, reg2);
    else if(cmd == 4) CTR(reg1);
    else if(cmd == 5) CTCACHE(0, 1, (reg1 << 2) + reg2);
}

void ALU(long cmd, long Data){
    long reg1 = Data >> 4;
    long reg2 = Data - (reg1 << 4);
    
                      //Math
    if(cmd == 1) CTCACHE(0, 1, registers[reg1] + registers[reg2]);
    else if(cmd == 2) CTCACHE(0, 1, registers[reg1] - registers[reg2]);
    else if(cmd == 3) CTCACHE(0, 1, registers[reg1] * registers[reg2]);
    else if(cmd == 4) CTCACHE(0, 1, registers[reg1] / registers[reg2]);                  //Logic
    else if(cmd == 5) CTCACHE(0, 1, registers[reg1] << registers[reg2]);
    else if(cmd == 6) CTCACHE(0, 1, registers[reg1] >> registers[reg2]);
    else if(cmd == 7) CTCACHE(0, 1, registers[reg1] || registers[reg2]);
    else if(cmd == 8) CTCACHE(0, 1, !registers[reg1]);
}


void bus(long H, long L){
    long CT = H >> 4;
    long ACT = H - (CT << 4);

    printf("Command: %x\n", (H << 8) + L);
    printf("CMD: %x\n", H);
    printf("Data: %x\n\n", L);

    if(H == 0x00);
    else if(CT == 0) ALU(ACT, L);
    else if(CT == 1) CTL(ACT, L);
    else if(CT == 2) OTHER(ACT, L);
    else{
        NOW = 0xB;
    }
}
/*
void light(){
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
}
*/

void TIMER(){
    //delay(500);
    long Addr = CTREG(1, 0, 0xF, 0x00) + 1;
    if(Addr > 0xFF) Addr = 0;
    CTREG(0, 1, 0xF, Addr);
}

void setup(){
//    Serial.begin(115200);
    registers[0] = 0;
    registers[0xF] = 0;
    printf("RX-0801!\n");
    //Serial.println("RX-0000!");
    //pinMode(13, OUTPUT);
}

void loop(){
    sleep(1);
//    light();
//    Serial.println(String(" ") + registers[3]);
    TEMPH = CTROM(1, 0, CTREG(1, 0, 0xF, 0x00), 0x00);
    TIMER();
    TEMPL = CTROM(1, 0, CTREG(1, 0, 0xF, 0x00), 0x00);
    TIMER();
    bus(TEMPH, TEMPL);
}

void main(){
    setup();
    while(1){
        printf("Address: %x\n", CTREG(1, 0, 0xF, 0x00));
        if(NOW == 0xA){
            loop();
        }else{
    	    printf("ERROR! CODE: %x\n",NOW);
	}
    }
}





