long LEDS[] = {PB1, PB11, PB14, PB15};
#include "avr/pgmspace.h"

long RAM[0xFF];
long ROM[0xFF] = {
  /* A LITTLE TEST PROGRAM.
00
01
02 MIC E
03 MCR 1
04 MIC 1
05 MCR 2
06 ADD 1 2
07 MCR 3
08
09
0A
0B MCR 1
0C MIC 2
0D MCR 2
0E MUL 1 2
0F MCR 3
10 MCR 1
11 MIC 5
12 MCR 2
13 MIN 1 2
14 MCR 3
15 MCR 1
16 MIC 5
17 MCR 2
18 DIV 1 2
19 MCR 3
   */
};
//const static unsigned

long registers[0x0F];
long cache;

long NOW = 0xA;
void KEYBOARD(){
    if(Serial.available())
        CTREG(0, 1, 0x0E, toascii(Serial.read()));
}

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
    
    Serial.print("Command: ");
    Serial.println((H << 8) + L, HEX);  
    Serial.print("CMD: ");  
    Serial.println(H, HEX);
    Serial.print("Data: ");  
    Serial.println(L, HEX);

    if(H == 0x00);
    else if(CT == 0) ALU(ACT, L);
    else if(CT == 1) CTL(ACT, L);
    else if(CT == 2) OTHER(ACT, L);
    else{
        NOW = 0xB;
    }
}

void TIMER(){
    //sleep(1);
    delay(500);
    long Addr = CTREG(1, 0, 0x0F, 0x00) + 1;
    if(Addr > 0xFF) Addr = 0;
    CTREG(0, 1, 0x0F, Addr);
}

void light(long HEXS){
    long temp=HEXS;
    for(int pinnum=0;pinnum<4;pinnum++){
        if(temp%2 == 1){digitalWrite(LEDS[pinnum],HIGH);}
        else{digitalWrite(LEDS[pinnum],LOW);}
        temp=temp/2;
    } 
}

void setup(){
    Serial.begin(115200);
    registers[0] = 0;
    registers[0x0F] = 0x00;
    Serial.println("RX-0801!");
    for(int pinnum=0;pinnum<4;pinnum++)
        pinMode(LEDS[pinnum],OUTPUT);
}

void DO(){
    long TEMPH = CTROM(1, 0, CTREG(1, 0, 0x0F, 0x00), 0x00);
    TIMER();
    long TEMPL = CTROM(1, 0, CTREG(1, 0, 0x0F, 0x00), 0x00);
    TIMER();
    bus(TEMPH, TEMPL);
}

void loop(){
    light(NOW);
    delay(500);
    Serial.println();
    Serial.println(CTREG(1, 0, 0x0F, 0x00), HEX);
    if(NOW == 0xA){
        DO();
    }else{
        Serial.print("Some error! CODE: ");
        Serial.println(NOW, HEX);
    }
}


