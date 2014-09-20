 /*
 * romerom_project_file1.c Created: 5/11/2012 8:07:22 AM
 *    Name & E-mail: Michael Romero mrome007@student.ucr.edu
 *    CS Login: romerom
 *    Partner(s) Name & E-mail: Andrew Nguyen anguy080@gmail.com
 *    Lab Section: 21
 *    Assignment: Final Project
 *	  Description: Tamagotchi
 *   
 *    I acknowledge all content contained herein is my own original work.
 */ 
 #include <avr/io.h>
#include <avr/interrupt.h>

unsigned char alive = 0;
unsigned char born = 0;
unsigned char evolve = 0;
unsigned char death = 0;
//happiness - changes as tamagotchi evolves
unsigned char happiness = 10;
unsigned char unhappy = 0;
unsigned char content = 5;
unsigned char happy = 10;
//food - changes as tamagotchi evolves
unsigned char hunger = 10;
unsigned char starved = 0;
unsigned char hungry = 5;
unsigned char full = 10;
//sleep - changes as tamagotchi evolves
unsigned char sleep = 10;
unsigned char exhausted = 0;
unsigned char tired = 5;
unsigned char fresh = 10; 
// Health
unsigned char health =15;
unsigned char dead = 0; 
unsigned char sick = 5;
unsigned char well = 10;
unsigned char healthy = 15;
unsigned char nap = 0;
unsigned char sleepy = 0;
unsigned char want_2_eat = 0;
unsigned char sleepy_hungry = 0;
unsigned char feeding = 0;
unsigned char evolving = 0;
unsigned char pace = 0;
unsigned char cry = 0;
unsigned char play_around = 0;
unsigned char overfed = 0;
unsigned char annoyed = 0;
unsigned char cnt = 0;
unsigned char angry = 0;
unsigned char spin_around = 0;
unsigned char food = 3;
unsigned char rando = 0;
unsigned char sad = 0;
unsigned char joy = 0;
unsigned char level = 0;

unsigned char isHungry()
{
if(hunger <= hungry)
return 1;
else
return 0;
}

unsigned char isSleepy()
{
if(sleep <= tired)
return 1;
else
return 0;
}

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
   return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
   return ((x & (0x01 << k)) != 0);
}
void Write7Seg(unsigned char x){
	if(x == 3){
		PORTB = (PORTB & 0x1F) | 0xE0;
	}
	else if(x == 2){
		PORTB = (PORTB & 0x1F) | 0xC0;
	}
	else if(x == 1){
		PORTB = (PORTB & 0x1F) | 0x80;
	}
	else{
		PORTB = PORTB & 0x1F;
	}
}
volatile unsigned char TimerFlag=0;
unsigned long _avr_timer_M=1; 
unsigned long _avr_timer_cntcurr=0; 

void TimerOn() {
   TCCR0 = 0x0B;
   OCR0 = 125;
   TIMSK = 0x02;
   TCNT0=0;
    _avr_timer_cntcurr = _avr_timer_M;
   SREG |= 0x80;
}

void TimerOff() {
    TCCR0 = 0x00; 
}

void TimerISR(){
    TimerFlag = 1;
}

ISR(TIMER0_COMP_vect)
{
   _avr_timer_cntcurr--;
   if (_avr_timer_cntcurr == 0) { 
       TimerISR();
       _avr_timer_cntcurr = _avr_timer_M;
   }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

	
unsigned char *LCD_Data = &PORTD;
unsigned char *LCD_Ctrl = &PORTB;
const unsigned char LCD_RS=3; 
const unsigned char LCD_E=4;  

unsigned char temp = 0;
unsigned char LCD_rdy_g=0; 
unsigned char LCD_go_g=0;  
unsigned char LCD_string_g[17]; 

void LCD_WriteCmdStart(unsigned char cmd) {
   *LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_RS, 0);  
   *LCD_Data = cmd; 
   *LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 1); 
}
void LCD_WriteCmdEnd() { 
   *LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 0); 
}
void LCD_WriteDataStart(unsigned char Data) {
   *LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_RS,1);  
   *LCD_Data = Data;  
   *LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 1);
}
void LCD_WriteDataEnd() {
   *LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 0);
}

enum LI_States { LI_Init1, LI_Init2, LI_Init3, LI_Init4, LI_Init5, LI_Init6, 
   LI_WaitDisplayString, LI_Clr, LI_PositionCursor, LI_DisplayChar, LI_WaitGo0 } LI_State;

void LI_Tick() {
   static unsigned char i;
   switch(LI_State) { // Transitions
      case -1:
         LI_State = LI_Init1;
         break;
      case LI_Init1:
         LI_State = LI_Init2;
         i=0;
         break;
      case LI_Init2:
         if (i<10) {
            LI_State = LI_Init2;
         }
         else {
            LI_State = LI_Init3;
         }
         break;
      case LI_Init3:
         LI_State = LI_Init4;
         LCD_WriteCmdEnd();
         break;
      case LI_Init4:
         LI_State = LI_Init5;
         LCD_WriteCmdEnd();
         break;
      case LI_Init5:
         LI_State = LI_Init6;
         LCD_WriteCmdEnd();
         break;
      case LI_Init6:
         LI_State = LI_WaitDisplayString;
         LCD_WriteCmdEnd();
         break;
      case LI_WaitDisplayString:
         if (!LCD_go_g) {
            LI_State = LI_WaitDisplayString;
         }
         else if (LCD_go_g) {
		    LCD_rdy_g = 0; 
            LI_State = LI_Clr;
         }
         break;
      case LI_Clr:
        LI_State = LI_PositionCursor;
         LCD_WriteCmdEnd();
		i=0;
         break;
      case LI_PositionCursor:
            LI_State = LI_DisplayChar;
            LCD_WriteCmdEnd();
         break;
      case LI_DisplayChar:
         if (i<16) {
            LI_State = LI_PositionCursor;
            LCD_WriteDataEnd();
			i++;
         }
         else {
            LI_State = LI_WaitGo0;
            LCD_WriteDataEnd();
         }
         break;
      case LI_WaitGo0:
         if (!LCD_go_g) {
            LI_State = LI_WaitDisplayString;
         }
         else if (LCD_go_g) {
            LI_State = LI_WaitGo0;
         }
         break;
      default:
         LI_State = LI_Init1;
      } 

   switch(LI_State) {
      case LI_Init1:
	     LCD_rdy_g = 0; 
         break;
      case LI_Init2:
         i++;
         break;
      case LI_Init3:
         LCD_WriteCmdStart(0x38);
         break;
      case LI_Init4:
         LCD_WriteCmdStart(0x06);
         break;
      case LI_Init5:
         LCD_WriteCmdStart(0x0F);
         break;
      case LI_Init6:
         LCD_WriteCmdStart(0x01); // Clear
         break;
      case LI_WaitDisplayString:
	     LCD_rdy_g = 1; 
         break;
      case LI_Clr:
         LCD_WriteCmdStart(0x01); 
         break;
      case LI_PositionCursor:
	     if ( i < 8 ) {
			 LCD_WriteCmdStart(0x80+i);
		 } 
		 else {
			 LCD_WriteCmdStart(0xB8+i);
		 }			 
         break;
      case LI_DisplayChar:
         LCD_WriteDataStart(LCD_string_g[i]);
         break;
      case LI_WaitGo0:
         break;
      default: 
         break;
   } 
}
unsigned char messages = 0;
unsigned char m = 0;
unsigned char l = 0;
unsigned char bound = 0;
enum MSG_States { MSG_notborn, MSG_notBzero, MSG_notborn1,  MSG_zero, MSG_StatusMessages, MSG_StatusLevels } MSG_State;
void MSG_Tick() {
unsigned char i = 0;
switch(MSG_State) {
      case -1:
         MSG_State = MSG_notborn;
         break;
      case MSG_notborn:
         if (1) {
            MSG_State = MSG_notBzero;
         }
         break;
	  case MSG_notBzero:
	  	 if (1) {
            MSG_State = MSG_notborn1;
         }
         break;
	  case MSG_notborn1:
	  	 if (born) {
            MSG_State = MSG_zero;
         }
         break;
      case MSG_zero:
         if (!born) {
            MSG_State = MSG_notborn;
			l = 0;
			m = 0;
			messages = 0;
         }
         else if (messages % 2 == 0) {
            MSG_State = MSG_StatusMessages;
            messages++;
         }
         else {
            MSG_State = MSG_StatusLevels;
            messages++;
         }
         break;
      case MSG_StatusMessages:
         MSG_State = MSG_zero;
         break;
      case MSG_StatusLevels:
         MSG_State = MSG_zero;
         break;
      default:
         MSG_State = MSG_notborn;
      }
   switch(MSG_State) {
         case MSG_notborn:
         strcpy(LCD_string_g,"Welcome 2 Pets  ");
LCD_go_g = 1;
         break;
		 
		 case MSG_notBzero:
		 LCD_go_g = 0;
		 break;
		 
		 case MSG_notborn1:
		 strcpy(LCD_string_g,"Press B0 2 Start ");
		 LCD_go_g = 1;
		 break;
      case MSG_zero:
         LCD_go_g = 0;
         break;
      case MSG_StatusMessages:
         if(m == 0)
{ 
strcpy(LCD_string_g,"health level:    ");
m++;
}
else if(m == 1)
{ 
strcpy(LCD_string_g,"happiness level: ");
m++;
}
else if(m == 2)
{ 
strcpy(LCD_string_g,"sleep level:     ");
m++;
}
else if(m == 3)
{ 
strcpy(LCD_string_g,"hunger level:    ");
m++;
}
else if(m == 4)
{ 	if(nap)
	strcpy(LCD_string_g,"status: sleeping ");
	else if(sleepy)
	strcpy(LCD_string_g,"status: sleepy   ");
	else if(want_2_eat)
	strcpy(LCD_string_g,"status: hungry   ");
	else if(feeding)
	strcpy(LCD_string_g,"status: eating   ");
	else if(sleepy_hungry)
	strcpy(LCD_string_g,"status: tired    ");
	else if(evolving)
	strcpy(LCD_string_g,"status: evolving ");
	else if(death)
	strcpy(LCD_string_g,"status: dead     ");
	else if(pace)
	strcpy(LCD_string_g,"status: content  ");
	else if(play_around)
	strcpy(LCD_string_g,"status: happy    ");
	else if(spin_around)
	strcpy(LCD_string_g,"status: in bliss ");
	else if(cry)
	strcpy(LCD_string_g,"status: crying   ");
	else if(angry)
	strcpy(LCD_string_g,"status: angry    ");
	else
	strcpy(LCD_string_g,"status:          ");
m = 0;
messages = 0;
}
LCD_go_g = 1;
         break;
      case MSG_StatusLevels:
         if(l == 0)
{
bound = health; l++;
}
else if(l == 1)
{
bound = happiness; l++;
}
else if(l == 2)
{
bound = sleep; l++;
}
else if(l == 3)
{
bound = hunger;l = 0;
}
for(i = 0; i < bound; i++)
{
LCD_string_g[i] = 219;
}
for(i = bound; i < 16; i++)
{
LCD_string_g[i] = ' ';
}
LCD_string_g[i] = '\0';
LCD_go_g = 1;
         break;
      default: 
         break;
   }
}
enum TBP_States { TBP_pregame, TBP_pregame1, TBP_startgame, TBP_reset, TBP_pet, TBP_feed, TBP_go2sleep, TBP_ignore, TBP_scold, TBP_annoyed, TBP_overfeeding } TBP_State;
void TBP_Tick() {
switch(TBP_State) {
      case -1:
         TBP_State = TBP_pregame;
         break;
      case TBP_pregame:
         if (GetBit(PINB,0) == 0) {
            TBP_State = TBP_pregame1;
            happiness = rand()%10;
hunger = rand()%10;
sleep = rand()%10;
health = rand()%10;
alive = 1;
level = 1;
evolve = 0;
         }
         break;
      case TBP_pregame1:
		 if(!born){
			 TBP_State = TBP_pregame;
		 }			 
         if (GetBit(PINB,0) == 1) {
            TBP_State = TBP_startgame;
            born = 1;
         }
         break;
      case TBP_startgame:
		 //if(!alive){
			// TBP_State = TBP_pregame;
			 //happiness = 0;hunger = 0;sleep = 0;health = 0;alive = 0;content = 5;tired = 5;hungry = 5;evolve=0;food = 3;
		 //}
		 if(GetBit(PINB,0) == 0){
			 TBP_State = TBP_reset;
			 born = 0;
		 }			 
		 PORTC = 0xFF; PORTC = SetBit(PORTC,7,0);
         if (alive && GetBit(PINC,0) == 0 && happiness < happy) {
            TBP_State = TBP_pet;
            happiness++;
         }
         if (alive && GetBit(PINC,1) == 0 && hunger < full && food > 0) {
            TBP_State = TBP_feed;
            feeding = 1;
			food--;
			cnt = 0;
			//hunger++;
         }
         if (alive && GetBit(PINC,2) == 0 && sleep <= tired) {
            TBP_State = TBP_go2sleep;
            nap =1;
         }
         if (alive && GetBit(PINC,3) == 0 && happiness > 0) {
            TBP_State = TBP_ignore;
            happiness--;
         }
         if (alive && GetBit(PINC,0) == 0 && happiness == happy) {
            TBP_State = TBP_annoyed;
            annoyed++;
         }
         if (alive && GetBit(PINC,1) == 0 && hunger == full && food > 0) {
            TBP_State = TBP_overfeeding;
            overfed++;
			if(happiness > 0)
			happiness--;
         }
		 PORTC = 0xFF; PORTC = SetBit(PORTC,6,0);		 
		 if (alive && GetBit(PINC,3) == 0 && happiness > 1) {
            TBP_State = TBP_scold;
            happiness-=2;
         }
         break;
      case TBP_reset:
         if (GetBit(PINB,0) == 1) {
            TBP_State = TBP_pregame;
         }
         break;
      case TBP_pet:
		PORTC = 0xFF; PORTC = SetBit(PORTC,7,0);
         if (GetBit(PINC,0) == 0) {
            TBP_State = TBP_pet;
         }
         else if (GetBit(PINC,0) == 1) {
            TBP_State = TBP_startgame;
         }
         break;
      case TBP_feed:
         if (cnt < 40) {
            TBP_State = TBP_feed;
            cnt++;
         }
         else {
            TBP_State = TBP_startgame;
            feeding = 0;
			cnt = 0;
         }
         break;
      case TBP_go2sleep:
         if (sleep < fresh) {
            TBP_State = TBP_go2sleep;
         }
         else {
            TBP_State = TBP_startgame;
            nap = 0;
         }
         break;
      case TBP_ignore:
		 PORTC = 0xFF; PORTC = SetBit(PORTC,7,0);	  
         if (GetBit(PINC,3) == 0) {
            TBP_State = TBP_ignore;
         }
         else if (GetBit(PINC,3) == 1) {
            TBP_State = TBP_startgame;
         }
         break;
      case TBP_scold:
	  	PORTC = 0xFF; PORTC = SetBit(PORTC,6,0);
         if (GetBit(PINC,3) == 0) {
            TBP_State = TBP_scold;
         }
         else if (GetBit(PINC,3) == 1) {
            TBP_State = TBP_startgame;
         }
         break;
      case TBP_annoyed:
	  	PORTC = 0xFF; PORTC = SetBit(PORTC,7,0);
         if (GetBit(PINC,0) == 0) {
            TBP_State = TBP_annoyed;
         }
         else if (GetBit(PINC,0) == 1) {
            TBP_State = TBP_startgame;
         }
         break;
      case TBP_overfeeding:
	  	PORTC = 0xFF; PORTC = SetBit(PORTC,7,0);
         if (GetBit(PINC,1) == 0) {
            TBP_State = TBP_overfeeding;
         }
         else if (GetBit(PINC,1) == 1) {
            TBP_State = TBP_startgame;
         }
         break;
      default:
         TBP_State = TBP_pregame;
      }

   switch(TBP_State) {
      case TBP_reset:
         happiness = 0;hunger = 0;sleep = 0;health = 0;alive = 0;content = 5;tired = 5;hungry = 5;evolve=0;food = 3;
         break;
      case TBP_annoyed:
         if(annoyed > 2)
{
happiness = 5;
annoyed = 0;
}
         break;
      case TBP_overfeeding:
         if(overfed > 3)
{
	if(health > 0){
	health--; 	
	}
overfed = 0; hunger = 5;
}
         break;
      default:
         break;
   } 
   switch(TBP_State)
   {   case TBP_pregame:
	   PORTA = 0xFF;
	   break;	
	   case TBP_startgame:
	   Write7Seg(food);
	   break;
	   default:
	   break;
   }
}
enum R1A_States { R1A_notBorn, R1A_R1acts, R1A_dead, R1A_evolve, R1A_isHungry, R1A_both, R1A_isSleepy } R1A_State;
void R1A_Tick() {
switch(R1A_State) {
      case -1:
         R1A_State = R1A_notBorn;
         break;
      case R1A_notBorn:
         if (born) {
            R1A_State = R1A_R1acts;
         }
         break;
      case R1A_R1acts:
         if (!born) {
            R1A_State = R1A_notBorn;
         }
         else if (!alive) {
            R1A_State = R1A_dead;
            death = 1;
         }
         else if (alive && evolve && !nap && !feeding) {
            R1A_State = R1A_evolve;
            evolving = 1;
         }
         else if (alive && isHungry() && !isSleepy() && !evolve && !nap && !feeding) {
            R1A_State = R1A_isHungry;
            want_2_eat = 1;
         }
         else if (alive && isHungry() && isSleepy() && !evolve && !nap && !feeding) {
            R1A_State = R1A_both;
            sleepy_hungry = 1;
         }
         else if (alive  && isSleepy() && !isHungry() && !evolve && !nap && !feeding) {
            R1A_State = R1A_isSleepy;
            sleepy = 1;
         }
         break;
      case R1A_dead:
         if (!alive) {
            R1A_State = R1A_dead;
         }
         else if (!born) {
            R1A_State = R1A_R1acts;
            death = 0;
         }
         break;
      case R1A_evolve:
         if (alive && evolve) {
            R1A_State = R1A_evolve;
         }
         else {
            R1A_State = R1A_R1acts;
            evolving = 0;
         }
         break;
      case R1A_isHungry:
         if (alive && isHungry() && !isSleepy() && !evolve && !nap && !feeding) {
            R1A_State = R1A_isHungry;
         }
         else {
            R1A_State = R1A_R1acts;
            want_2_eat = 0;
         }
         break;
      case R1A_both:
         if (alive && isHungry() && isSleepy() && !evolve && !nap && !feeding) {
            R1A_State = R1A_both;
         }
         else {
            R1A_State = R1A_R1acts;
            sleepy_hungry = 0;
         }
         break;
      case R1A_isSleepy:
         if (alive && !isHungry() && isSleepy() && !evolve && !nap && !feeding) {
            R1A_State = R1A_isSleepy;
         }
         else {
            R1A_State = R1A_R1acts;
            sleepy = 0;
         }
         break;
      default:
         R1A_State = R1A_notBorn;
      }
}
enum R2A_States { R2A_notBorn, R2A_R2Acts, R2A_sad, R2A_angry, R2A_content, R2A_play, R2A_happy } R2A_State;
void R2A_Tick() {
switch(R2A_State) {
      case -1:
         R2A_State = R2A_notBorn;
         break;
      case R2A_notBorn:
         if (born) {
            R2A_State = R2A_R2Acts;
         }
         break;
      case R2A_R2Acts:
         if (!born) {
            R2A_State = R2A_notBorn;
         }
         else if (happiness == unhappy && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_sad;
            cry = 1;
         }
         else if (happiness > unhappy && happiness < content && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_angry;
            angry = 1;
sad = rand()%3;
         }
         else if (happiness == content && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_content;
            pace = 1;
         }
         else if (happiness > content && happiness < happy && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_play;
            play_around = 1;
joy = rand()%3;
         }
         else if (happiness == happy && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_happy;
            spin_around = 1;
         }
         break;
      case R2A_sad:
         if (happiness == unhappy && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_sad;
         }
         else {
            R2A_State = R2A_R2Acts;
            cry = 0;
         }
         break;
      case R2A_angry:
         if (happiness > unhappy && happiness < content && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_angry;
         }
         else {
            R2A_State = R2A_R2Acts;
            angry = 0;
sad = 0;
         }
         break;
      case R2A_content:
         if (happiness == content && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_content;
         }
         else {
            R2A_State = R2A_R2Acts;
            pace = 0;
         }
         break;
      case R2A_play:
         if (happiness > content && happiness < happy && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_play;
         }
         else {
            R2A_State = R2A_R2Acts;
            play_around = 0;
joy = 0;
         }
         break;
      case R2A_happy:
         if (happiness == happy && alive && !evolve && !nap && !feeding && !isHungry() && !isSleepy()) {
            R2A_State = R2A_happy;
         }
         else {
            R2A_State = R2A_R2Acts;
            spin_around = 0;
         }
         break;
      default:
         R2A_State = R2A_notBorn;
      }
}
unsigned char i = 0;
unsigned char tama = 1;
enum FA_States { FA_notBorn, FA_s1 } FA_State;

void FA_Tick() {
unsigned char fdng1[] = {0x01, 0x09, 0x05, 0x03, 0x01};
unsigned char fdng2[] = {0x03, 0x0B, 0X25, 0X23, 0X03};
switch(FA_State) {
      case -1:
         FA_State = FA_notBorn;
         break;
      case FA_notBorn:
         if (born && alive && feeding) {
            FA_State = FA_s1;
            i = 0;
			hunger++;
         }
         else {
            FA_State = FA_notBorn;
         }
         break;
      case FA_s1:
         if (born && alive && feeding) {
            FA_State = FA_s1;
         }
         else if (!born || !feeding || !alive) {
            FA_State = FA_notBorn;
			i = 0;
         }
         break;
      default:
         FA_State = FA_notBorn;
      } 
   switch(FA_State) {
         case FA_notBorn:
         break;
      case FA_s1:
         if(level == 1)
PORTA = fdng1[i];
else if(level == 2)
PORTA = fdng2[i];
i++;
if(i > 4)
i = 0;
         break;
      default:
         break;
   }
}
unsigned char sleepCounter = 0;
enum SA_States { SA_notBorn, SA_s1 } SA_State;

void SA_Tick() {
unsigned char slpng1[] = {0x10, 0x0D, 0x0E, 0x4C, 0x8C};
unsigned char slpng2[] = {0x30, 0x1D, 0x0F, 0x4E, 0xCC};
switch(SA_State) {
      case -1:
         SA_State = SA_notBorn;
         break;
      case SA_notBorn:
         if (born && alive && nap) {
            SA_State = SA_s1;
            i = 0;
         }
         else {
            SA_State = SA_notBorn;
         }
         break;
      case SA_s1:
         if (born && alive && nap) {
            SA_State = SA_s1;
         }
         else if (!born || !nap || !alive) {
            SA_State = SA_notBorn;
            i = 0;
         }
         break;
      default:
         SA_State = SA_notBorn;
      }
   switch(SA_State) {
         case SA_notBorn:
         break;
      case SA_s1:
         if(level == 1)
PORTA = slpng1[i];
else if(level == 2)
PORTA = slpng2[i];
i++;
if(i > 4)
i = 4;
if(i == 4)
{
	sleepCounter++;
	if(sleepCounter == 2)
	{
		if(sleep < fresh)
		{
		sleep++;	
		}
	sleepCounter = 0;	
	}
	
}

         break;
      default:
         break;
   }
}
unsigned long isHungryBound = 1000;
enum IHA_States { IHA_notBorn, IHA_s1 } IHA_State;
void IHA_Tick() {
unsigned char isHngry1[] = {0x02, 0x04, 0x40, 0x20};
unsigned char isHngry2[] = {0x06, 0x88, 0x60, 0x11};
switch(IHA_State) { // Transitions
      case -1:
         IHA_State = IHA_notBorn;
         break;
      case IHA_notBorn:
         if (born && alive && want_2_eat) {
            IHA_State = IHA_s1;
            i = 0;
         }
         else {
            IHA_State = IHA_notBorn;
         }
         break;
      case IHA_s1:
         if (born && alive && want_2_eat) {
            IHA_State = IHA_s1;
         }
         else if (!born || !want_2_eat || !alive) {
            IHA_State = IHA_notBorn;
            i = 0;
			isHungryBound = 1000;
         }
         break;
      default:
         IHA_State = IHA_notBorn;
      } // Transitions

   switch(IHA_State) { // State actions
         case IHA_notBorn:
         break;
      case IHA_s1:
         if(level == 1)
PORTA = isHngry1[i];
else if(level == 2)
PORTA = isHngry2[i];
i++;
isHungryBound+=500;
if(i > 3)
i = 0;
else if(isHungryBound >= 5000)
isHungryBound = 5000;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

unsigned long isSleepyBound = 1000;
enum ISA_States { ISA_notBorn, ISA_s1 } ISA_State;
void ISA_Tick() {
unsigned char isSlpy1[] = {0x01, 0x20, 0x40, 0x80, 0x80, 0x80, 0x04, 0x02,  0x01, 0x01};
unsigned char isSlpy2[] = {0x03, 0x22, 0x60, 0xC0, 0xC0, 0xC0, 0x88, 0x06, 0x03, 0x03};
switch(ISA_State) { // Transitions
      case -1:
         ISA_State = ISA_notBorn;
         break;
      case ISA_notBorn:
         if (born && alive && sleepy) {
            ISA_State = ISA_s1;
            i = 0;
         }
         else {
            ISA_State = ISA_notBorn;
         }
         break;
      case ISA_s1:
         if (born && alive && sleepy) {
            ISA_State = ISA_s1;
         }
         else if (!born || !sleepy || !alive) {
            ISA_State = ISA_notBorn;
            i = 0;
			isSleepyBound = 1000;
         }
         break;
      default:
         ISA_State = ISA_notBorn;
      } // Transitions

   switch(ISA_State) { // State actions
         case ISA_notBorn:
         break;
      case ISA_s1:
         if(level == 1)
PORTA = isSlpy1[i];
else if(level == 2)
PORTA = isSlpy2[i];
i++;
isSleepyBound+=200;
if(i > 9)
i = 0;
else if(isSleepyBound >= 5000)
isSleepyBound = 1000;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

unsigned long isBothBound = 500;
enum IBSHA_States { IBSHA_notBorn, IBSHA_s1 } IBSHA_State;
void IBSHA_Tick() {
unsigned char isBoth1[] = {0x4D, 0x2D};
unsigned char isBoth2[] = {0x6D, 0x9D};
switch(IBSHA_State) { // Transitions
      case -1:
         IBSHA_State = IBSHA_notBorn;
         break;
      case IBSHA_notBorn:
         if (born && alive && sleepy_hungry) {
            IBSHA_State = IBSHA_s1;
            i = 0;
         }
         else {
            IBSHA_State = IBSHA_notBorn;
         }
         break;
      case IBSHA_s1:
         if (born && alive && sleepy_hungry) {
            IBSHA_State = IBSHA_s1;
         }
         else if (!born || !sleepy_hungry || !alive) {
            IBSHA_State = IBSHA_notBorn;
            i = 0;
			isBothBound = 500;
         }
         break;
      default:
         IBSHA_State = IBSHA_notBorn;
      } // Transitions
   switch(IBSHA_State) { // State actions
         case IBSHA_notBorn:
         break;
      case IBSHA_s1:
         if(level == 1)
PORTA = isBoth1[i];
else if(level == 2)
PORTA = isBoth2[i];
i++;
isBothBound+=300;
if(i > 1)
i = 0;
else if(isBothBound >= 3000)
isBothBound = 500;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum CA_States { CA_notBorn, CA_s1 } CA_State;
void CA_Tick() {
unsigned char cry1[] = {0x02, 0x52, 0x0A};
unsigned char cry2[] = {0x06, 0x66, 0x0F};
switch(CA_State) { // Transitions
      case -1:
         CA_State = CA_notBorn;
         break;
      case CA_notBorn:
         if (born && alive && cry) {
            CA_State = CA_s1;
            i = 0;
         }
         else {
            CA_State = CA_notBorn;
         }
         break;
      case CA_s1:
         if (born && alive && cry) {
            CA_State = CA_s1;
         }
         else if (!born || !cry || !alive) {
            CA_State = CA_notBorn;
            i = 0;
         }
         break;
      default:
         CA_State = CA_notBorn;
      } // Transitions
  switch(CA_State) { // State actions
         case CA_notBorn:
         break;
      case CA_s1:
         if(level == 1)
PORTA = cry1[i];
else if(level == 2)
PORTA = cry2[i];
i++;
if(i > 2)
i = 0;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum PA_States { PA_notBorn, PA_s1 } PA_State;
void PA_Tick() {
unsigned char pace1[] = {0x01, 0x02, 0x04, 0x08, 0x04, 0x02};
unsigned char pace2[] = {0x11, 0x03, 0x06, 0x0C, 0x06, 0x03};
switch(PA_State) { // Transitions
      case -1:
         PA_State = PA_notBorn;
         break;
      case PA_notBorn:
         if (born && alive && pace) {
            PA_State = PA_s1;
            i = 0;
         }
         else {
            PA_State = PA_notBorn;
         }
         break;
      case PA_s1:
         if (born && alive && pace) {
            PA_State = PA_s1;
         }
         else if (!born || !pace || !alive) {
            PA_State = PA_notBorn;
            i = 0;
         }
         break;
      default:
         PA_State = PA_notBorn;
      } // Transitions
   switch(PA_State) { // State actions
         case PA_notBorn:
         break;
      case PA_s1:
         if(level == 1)
PORTA = pace1[i];
else if(level == 2)
PORTA = pace2[i];
i++;
if(i > 5)
i = 0;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum IBA_States { IBA_notBorn, IBA_s1 } IBA_State;
void IBA_Tick() {
switch(IBA_State) { // Transitions
      case -1:
         IBA_State = IBA_notBorn;
         break;
      case IBA_notBorn:
         if (born && alive && spin_around) {
            IBA_State = IBA_s1;
            i = 0;
         }
         else {
            IBA_State = IBA_notBorn;
         }
         break;
      case IBA_s1:
         if (born && alive && spin_around) {
            IBA_State = IBA_s1;
         }
         else if (!born || !spin_around || !alive) {
            IBA_State = IBA_notBorn;
            i = 0;
         }
         break;
      default:
         IBA_State = IBA_notBorn;
      } // Transitions
   switch(IBA_State) { // State actions
         case IBA_notBorn:
         break;
      case IBA_s1:
         if(level == 1)
{
tama = 1;
PORTA = tama << rand()%8;
}
else if(level == 2)
{
tama = 3;
PORTA = tama << rand()%7;
}
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
unsigned long isAngryBound = 1000;
enum AA_States { AA_notBorn, AA_playPrank, AA_angry, AA_ignore } AA_State;

void AA_Tick() {
unsigned char angry1[]={0x01, 0x02, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x40, 0x20, 0x02, 0x04, 0x40, 0x20, 0x02};
unsigned char angry2[]={0x03, 0x06, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x88, 0xC0, 0x44, 0x0C, 0x88, 0xC0, 0x44};

unsigned char angry3[]={0x02, 0x00, 0x02, 0x00, 0x02, 0x40, 0x00, 0x40, 0x00, 0x40, 0x04, 0x00, 0x04, 0x00, 0x04, 0x20, 0x00, 0x20, 0x00, 0x20};
unsigned char angry4[]={0x03, 0x00, 0x03, 0x00, 0x03, 0xC0, 0x00, 0xC0, 0x00, 0xC0, 0x0C, 0x00, 0x0C, 0x00, 0x0C, 0x30, 0x00, 0x30, 0x00, 0x30};
	
unsigned char angry5[]={0x40, 0x08, 0x04, 0x80};
unsigned char angry6[]={0x60, 0x0C, 0x06,0x30};		
switch(AA_State) { // Transitions
      case -1:
         AA_State = AA_notBorn;
         break;
      case AA_notBorn:
         if (born && alive && angry && sad == 0) {
            AA_State = AA_playPrank;
            i = 0;
			isAngryBound = 1000;
         }
         else if (born && alive && angry && sad == 1) {
            AA_State = AA_angry;
            i = 0;
			isAngryBound = 200;
         }
		 else if(born && alive && angry && sad == 2){
			 AA_State = AA_ignore;
			 i = 0;
			 isAngryBound =1000;
		 }			 
         break;
      case AA_playPrank:
         if (!born || !angry || !alive) {
            AA_State = AA_notBorn;
            i = 0;
			isAngryBound = 1000;
         }
         else if (born && alive && angry && sad == 0) {
            AA_State = AA_playPrank;
         }
         break;
      case AA_angry:
         if (!born || !angry || !alive) {
            AA_State = AA_notBorn;
            i = 0;
			isAngryBound = 1000;
         }
         else if (born && alive && angry && sad == 1) {
            AA_State = AA_angry;
         }
	  case AA_ignore:
		  if(!born || !angry || !alive){
			  AA_State = AA_notBorn;
			  i = 0;
			  isAngryBound = 1000;
		  }
		  else if(born && alive && angry && sad == 2){
			  AA_State = AA_ignore;
		  }			  			  	 
         break;
      default:
         AA_State = AA_notBorn;
      } // Transitions

   switch(AA_State) { // State actions
         case AA_notBorn:
         break;
      case AA_playPrank:
         if(level == 1)
PORTA = angry1[i];
else if(level == 2)
PORTA = angry2[i];
i++;
if(i > 11)
isAngryBound = 100;
else if(i > 1 && i < 12)
isAngryBound-=10;
else
isAngryBound = 1000;
if(i > 18)
i = 0;
         break;
      case AA_angry:
         if(level == 1)
PORTA = angry3[i];
else if(level == 2)
PORTA = angry4[i];
i++;
if(i == 5 || i == 10 || i ==15 || i == 20)
isAngryBound = 1000;
else
isAngryBound = 200;
if(i > 19)
i = 0; 
         break;
	  case AA_ignore:
	  if(level == 1)
PORTA = angry5[i];
else if(level == 2)
PORTA = angry6[i];
i++;
if(i == 2)
isAngryBound = 10000;
else
isAngryBound = 1000;
if(i > 3)
i = 0; 
		break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum PLAY_States { PLAY_notBorn, PLAY_s1, PLAY_s2, PLAY_s3 } PLAY_State;
void PLAY_Tick() {
unsigned char par1[]={0x01, 0x09, 0x06, 0x42, 0x0A};
unsigned char par2[]={0x30, 0x38, 0x26, 0x62, 0x38};
unsigned char par3[]={0x01, 0x20, 0x04, 0x80, 0x08, 0x40, 0x02, 0x10};
unsigned char par4[]={0x03, 0x21, 0x60, 0x48, 0xC0, 0x84, 0x06, 0x12};
unsigned char par5[]={0x02, 0x20, 0x04, 0x40};
unsigned char par6[]={0x03, 0x30, 0x0C, 0xC0};				
	
switch(PLAY_State) { // Transitions
      case -1:
         PLAY_State = PLAY_notBorn;
         break;
      case PLAY_notBorn:
         if (born && alive && play_around && joy == 0) {
            PLAY_State = PLAY_s1;
            i = 0;
         }
		 else if(born && alive && alive && play_around && joy == 1){
			 PLAY_State = PLAY_s2;
			 i = 0;
		 }
		 else if(born && play_around && joy == 2){
			 PLAY_State = PLAY_s3;
			 i = 0;
		 }			 			 
         else {
            PLAY_State = PLAY_notBorn;
         }
         break;
      case PLAY_s1:
         if (born && alive && play_around && joy == 0) {
            PLAY_State = PLAY_s1;
         }
         else if (!born || !play_around || !alive) {
            PLAY_State = PLAY_notBorn;
            i = 0;
         }
         break;
	  case PLAY_s2:
         if (born && alive && play_around && joy == 1) {
            PLAY_State = PLAY_s2;
         }
         else if (!born || !play_around || !alive) {
            PLAY_State = PLAY_notBorn;
            i = 0;
         }
		 break;
	  case PLAY_s3:
         if (born && alive && play_around && joy == 2) {
            PLAY_State = PLAY_s3;
         }
         else if (!born || !play_around || !alive) {
            PLAY_State = PLAY_notBorn;
            i = 0;
         }
		 break;	 	 
      default:
         PLAY_State = PLAY_notBorn;
      } // Transitions
   switch(PLAY_State) { // State actions
         case PLAY_notBorn:
         break;
      case PLAY_s1:
         if(level == 1)
PORTA = par1[i];
else if(level == 2)
PORTA = par2[i];
i++;
if(i > 4)
i = 0;
         break;
		 case PLAY_s2:
         if(level == 1)
PORTA = par3[i];
else if(level == 2)
PORTA = par4[i];
i++;
if(i > 7)
i = 0;
		break;
		case PLAY_s3:
         if(level == 1)
PORTA = par5[i];
else if(level == 2)
PORTA = par6[i];
i++;
if(i > 3)
i = 0;
		break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum DA_States { DA_notBorn, DA_s1 } DA_State;
void DA_Tick() {
unsigned char death1[]={0x08, 0x40, 0x04, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0xF2, 0x2F, 0xFF};
unsigned char death2[]={0x88, 0x0C, 0x44, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0x00, 0x06, 0xF6, 0x6F, 0xFF};
switch(DA_State) { // Transitions
      case -1:
         DA_State = DA_notBorn;
         break;
      case DA_notBorn:
         if (born && death) {
            DA_State = DA_s1;
            i = 0;
         }
         else {
            DA_State = DA_notBorn;
         }
         break;
      case DA_s1:
         if (born && death) {
            DA_State = DA_s1;
         }
         else if (!born || !death) {
            DA_State = DA_notBorn;
            i = 0;
         }
         break;
      default:
         DA_State = DA_notBorn;
      } // Transitions

   switch(DA_State) { // State actions
         case DA_notBorn:
         break;
      case DA_s1:
         if(level == 1)
PORTA = death1[i];
else if(level == 2)
PORTA = death2[i];
i++;
if(i > 14)
{
i = 14;
born = 0;
}
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum EA_States { EA_notBorn, EA_s1 } EA_State;

void EA_Tick() {
unsigned char evolve1[]={0x02, 0x04, 0x02, 0x06, 0x02, 0x06, 0x02, 0x06, 0x02, 0x06, 0x44, 0x60, 0x22, 0x06};
switch(EA_State) { // Transitions
      case -1:
         EA_State = EA_notBorn;
         break;
      case EA_notBorn:
         if (born && alive && evolving) {
            EA_State = EA_s1;
            i = 0;
level = 2;
         }
         else {
            EA_State = EA_notBorn;
         }
         break;
      case EA_s1:
         if (born && alive && evolving) {
            EA_State = EA_s1;
         }
         else if (!born || !evolving || !alive) {
            EA_State = EA_notBorn;
            i = 0;
         }
         break;
      default:
         EA_State = EA_notBorn;
      } // Transitions
   switch(EA_State) { // State actions
         case EA_notBorn:
         break;
      case EA_s1:
         PORTA = evolve1[i];
i++;
if(i > 13)
{
i = 0;
evolve = 0;
}
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
unsigned char GettinSleepyBound = 0;
unsigned char GettinHungryBound = 0;
unsigned char EvolveBound = 0;
unsigned char GScnt = 0;
unsigned char GHcnt = 0;
unsigned char Ecnt = 0;
enum GSA_States { GSA_notBorn, GSA_s1, GSA_s2 } GSA_State;
void GSA_Tick() {
switch(GSA_State) { // Transitions
      case -1:
         GSA_State = GSA_notBorn;
         break;
      case GSA_notBorn:
         if (born) {
            GSA_State = GSA_s1;
         }
         break;
      case GSA_s1:
         if (!born) {
            GSA_State = GSA_notBorn;
         }
         else if (GScnt < GettinSleepyBound) {
            GSA_State = GSA_s1;
            GScnt++;
         }
         else {
            GSA_State = GSA_s2;
            GScnt = 0;
         }
         break;
      case GSA_s2:
         if (1) {
            GSA_State = GSA_s1;
            GScnt = 0;
         }
         break;
      default:
         GSA_State = GSA_notBorn;
      } // Transitions
   switch(GSA_State) { // State actions
         case GSA_notBorn:
         break;
      case GSA_s1:
         if(level == 1)
GettinSleepyBound = 5;
else if(level == 2)
GettinSleepyBound = 7;
         break;
      case GSA_s2:
         if(sleep > 0)
sleep--;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum GHA_States { GHA_notBorn, GHA_s1, GHA_s2 } GHA_State;
void GHA_Tick() {
switch(GHA_State) { // Transitions
      case -1:
         GHA_State = GHA_notBorn;
         break;
      case GHA_notBorn:
         if (born) {
            GHA_State = GHA_s1;
         }
         break;
      case GHA_s1:
         if (!born) {
            GHA_State = GHA_notBorn;
         }
         else if (GHcnt < GettinHungryBound) {
            GHA_State = GHA_s1;
            GHcnt++;
         }
         else {
            GHA_State = GHA_s2;
            GHcnt = 0;
         }
         break;
      case GHA_s2:
         if (1) {
            GHA_State = GHA_s1;
            GHcnt = 0;
         }
         break;
      default:
         GHA_State = GHA_notBorn;
      } // Transitions
   switch(GHA_State) { // State actions
         case GHA_notBorn:
         break;
      case GHA_s1:
         if(level == 1)
GettinHungryBound = 7;
else if(level == 2)
GettinHungryBound = 5;
         break;
      case GHA_s2:
         if(hunger > 0)
hunger--;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum DIGIVOLVE_States { DIGIVOLVE_notBorn, DIGIVOLVE_s1, DIGIVOLVE_s2, DIGIVOLVE_s3 } DIGIVOLVE_State;
void DIGIVOLVE_Tick() {
switch(DIGIVOLVE_State) { // Transitions
      case -1:
         DIGIVOLVE_State = DIGIVOLVE_notBorn;
         break;
      case DIGIVOLVE_notBorn:
         if (born) {
            DIGIVOLVE_State = DIGIVOLVE_s1;
            Ecnt = 0;
         }
         break;
      case DIGIVOLVE_s1:
         if (!born) {
            DIGIVOLVE_State = DIGIVOLVE_notBorn;
         }
         else if (Ecnt < 0) {
            DIGIVOLVE_State = DIGIVOLVE_s1;
            Ecnt++;
         }
         else {
            DIGIVOLVE_State = DIGIVOLVE_s2;
         }
         break;
      case DIGIVOLVE_s2:
         if (alive && !nap && !feeding) {
            DIGIVOLVE_State = DIGIVOLVE_s3;
			evolve = 1;
level = 2;
content = 7;
hungry = 7;
tired = 4;
         }
		 else if(!born)
		 {
			 DIGIVOLVE_State = DIGIVOLVE_notBorn;
		 }
		 else
		 {
			 DIGIVOLVE_State = DIGIVOLVE_s2;
		 }			 
         break;
      case DIGIVOLVE_s3:
         break;
      default:
         DIGIVOLVE_State = DIGIVOLVE_notBorn;
      } // Transitions
   switch(DIGIVOLVE_State) { // State actions
         case DIGIVOLVE_notBorn:
         break;
      case DIGIVOLVE_s1:
         break;
      case DIGIVOLVE_s2:
         
         break;
      case DIGIVOLVE_s3:
	  
         //evolve = 0;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
unsigned char Fcnt = 0;
enum FR_States { FR_notBorn, FR_s1, FR_s2 } FR_State;
void FR_Tick() {
switch(FR_State) { // Transitions
      case -1:
         FR_State = FR_notBorn;
         break;
      case FR_notBorn:
         if (born) {
            FR_State = FR_s1;
            Fcnt = 0;
         }
         break;
      case FR_s1:
         if (!born) {
            FR_State = FR_notBorn;
         }
         else if (Fcnt < 0) {
            FR_State = FR_s1;
            Fcnt++;
         }
         else {
            FR_State = FR_s2;
            Fcnt = 0;
         }
         break;
      case FR_s2:
         if (1) {
            FR_State = FR_s1;
         }
         break;
      default:
         FR_State = FR_notBorn;
      } // Transitions
   switch(FR_State) { // State actions
         case FR_notBorn:
         break;
      case FR_s1:
         break;
      case FR_s2:
         food = 3;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
unsigned char losingCnt = 0;
enum LHAH_States { LHAH_notBorn, LHAH_s1, LHAH_s2 } LHAH_State;
void LHAH_Tick() {
switch(LHAH_State) { // Transitions
      case -1:
         LHAH_State = LHAH_notBorn;
         break;
      case LHAH_notBorn:
         if (born && sleepy_hungry) {
            LHAH_State = LHAH_s1;
            losingCnt = 0;
         }
         break;
      case LHAH_s1:
         if (!born || !sleepy_hungry) {
            LHAH_State = LHAH_notBorn;
            losingCnt = 0;
         }
         else if (born && sleepy_hungry && losingCnt < 0) {
            LHAH_State = LHAH_s1;
            losingCnt++;
         }
         else {
            LHAH_State = LHAH_s2;
            losingCnt = 0;
         }
         break;
      case LHAH_s2:
         if (1) {
            LHAH_State = LHAH_s1;
            losingCnt = 0;
         }
         break;
      default:
         LHAH_State = LHAH_notBorn;
      } // Transitions
   switch(LHAH_State) { // State actions
         case LHAH_notBorn:
         break;
      case LHAH_s1:
         break;
      case LHAH_s2:
	  if(health > 0)
	  {
		  health--;
	  }		
	  else if(happiness > 0)
	  {  
         happiness--;
	  }
	  else if(happiness < 2 && health > 0)
	  {
		  health--;
	  }		  	
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}
enum CheckIfDead_States {CID_notBorn, CID_check} CheckIfDead_State;
void checkDead(){
	switch(CheckIfDead_State){
		case -1:
		CheckIfDead_State = CID_notBorn;
		break;
		case CID_notBorn:
		if(born){
			CheckIfDead_State = CID_check;
		}
		else{
			CheckIfDead_State = CID_notBorn;
		}
		break;
		case CID_check:
		if(!born){
			CheckIfDead_State = CID_notBorn;
		}
		else{
			CheckIfDead_State  = CID_check;
		}
		break;
		default:
		CheckIfDead_State = CID_notBorn;
		break;
	}
	switch(CheckIfDead_State){
		case CID_notBorn:
		break;
		case CID_check:
		if(health == 0){
			alive = 0;
		}
		break;
		default:
		break;
	}
};	
int main() {
   DDRB  = 0xFE; PORTB = 0x01;
   DDRD  = 0xFF; PORTD = 0x00;
   DDRC = 0xF0; PORTC = 0x0F;
   DDRA  = 0xFF; PORTA = 0x00;
   unsigned long LI_ElapsedTime = 0;
   unsigned long SM_ElapsedTime = 0;
   unsigned long TBP_ElapsedTime = 0;
   unsigned long R1A_ElapsedTime = 0;
   unsigned long R2A_ElapsedTime = 0; unsigned long FA_ElapsedTime = 0; unsigned long SA_ElapsedTime = 0; unsigned long IHA_ElapsedTime = 0;
   unsigned long ISA_ElapsedTime = 0; unsigned long IBSHA_ElapsedTime = 0;unsigned long CA_ElapsedTime = 0;unsigned long IBA_ElapsedTime = 0;
   unsigned long PA_ElapsedTime = 0; unsigned long AA_ElapsedTime = 0;unsigned long PLAY_ElapsedTime = 0;unsigned long DA_ElapsedTime = 0;
   unsigned long EA_ElapsedTime = 0; unsigned long GSA_ElapsedTime = 0;unsigned long GHA_ElapsedTime = 0;unsigned long DIGIVOLVE_ElapsedTime = 0;
   unsigned long FR_ElapsedTime = 0; unsigned long LHAH_ElapsedTime = 0;unsigned long CID_ElapsedTime = 0;
   unsigned long timerPeriod = 10;
   TimerSet(timerPeriod);
   TimerOn();
   LI_State = -1;
   MSG_State = -1;
   TBP_State = -1;
   R1A_State = -1;
   R2A_State = -1;
   FA_State = -1;
   SA_State = -1;
   IHA_State = -1; 
   ISA_State = -1; IBSHA_State = -1; CA_State = -1;IBA_State = -1;AA_State = -1;PLAY_State = -1;DA_State = -1;EA_State = -1;
   GSA_State = -1; GHA_State = -1; DIGIVOLVE_State = -1;FR_State = -1;LHAH_State = -1;CheckIfDead_State = -1;
   while(1) {
	  if(DIGIVOLVE_ElapsedTime == 30000)
	  {
		DIGIVOLVE_Tick();
		DIGIVOLVE_ElapsedTime = 0;  
	  } 
	  if(FR_ElapsedTime == 15000)
	  {
		FR_Tick();
		FR_ElapsedTime = 0;  
	  }   
	  if(GSA_ElapsedTime == 2000)
	  {
		GSA_Tick();
		GSA_ElapsedTime = 0;  
	  }
	  if(GHA_ElapsedTime == 2000)
	  {
		GHA_Tick();
		GHA_ElapsedTime = 0;  
	  }  
	  if(LHAH_ElapsedTime == 1000)
	  {
		LHAH_Tick();
		LHAH_ElapsedTime = 0;  
	  }      
	  if(SA_ElapsedTime == 2000)
	  {
		SA_Tick();
		SA_ElapsedTime = 0;  
	  } 
	  if(ISA_ElapsedTime == isSleepyBound)
	  {
		  ISA_Tick();
		  ISA_ElapsedTime = 0;
	  } 
	  if(IHA_ElapsedTime == isHungryBound)
	  {
		  IHA_Tick();
		  IHA_ElapsedTime = 0;
	  }
	  if(SM_ElapsedTime == 1000)
	  {
		MSG_Tick();
		SM_ElapsedTime = 0;  
	  }
	  if(AA_ElapsedTime == isAngryBound)
	  {
		AA_Tick();
		AA_ElapsedTime = 0;  
	  }
	  if(IBSHA_ElapsedTime == isBothBound)
	  {
		IBSHA_Tick();
		IBSHA_ElapsedTime = 0;  
	  }
	  if(DA_ElapsedTime == 1000)
	  {
		  DA_Tick();
		  DA_ElapsedTime = 0;
	  } 
	  if(PA_ElapsedTime == 700)
	  {
		  PA_Tick();
		  PA_ElapsedTime = 0;
	  } 
	  
	  if(IBA_ElapsedTime == 500)
	  {
		  IBA_Tick();
		  IBA_ElapsedTime = 0;
	  } 
	  if(PLAY_ElapsedTime == 400)
	  {
		  PLAY_Tick();
		  PLAY_ElapsedTime = 0;
	  } 
	  if(EA_ElapsedTime == 400)
	  {
		  EA_Tick();
		  EA_ElapsedTime = 0;
	  } 
	  if(FA_ElapsedTime == 400)
	  {
		  FA_Tick();
		  FA_ElapsedTime = 0;
	  } 
	  if(CA_ElapsedTime == 400)
	  {
		  CA_Tick();
		  CA_ElapsedTime = 0;
	  } 
	  if(CID_ElapsedTime == 50)
	  {
		  checkDead();
		  CID_ElapsedTime = 0;
	  }
	  if(TBP_ElapsedTime == 50)
	  {
		  TBP_Tick();
		  TBP_ElapsedTime = 0;
	  }
	  if(R1A_ElapsedTime == 50)
	  {
		  R1A_Tick();
		  R1A_ElapsedTime = 0;
	  }
	  if(R2A_ElapsedTime == 50)
	  {
		  R2A_Tick();
		  R2A_ElapsedTime = 0;
	  }
	  if(LI_ElapsedTime == 10)
	  {		   
		LI_Tick();
		LI_ElapsedTime = 0;
	  }	
	  
      while (!TimerFlag);
      TimerFlag = 0;
	  IHA_ElapsedTime += timerPeriod;ISA_ElapsedTime += timerPeriod;IBSHA_ElapsedTime += timerPeriod;CA_ElapsedTime+=timerPeriod;PA_ElapsedTime+=timerPeriod;
	  FA_ElapsedTime += timerPeriod;IBA_ElapsedTime +=timerPeriod;AA_ElapsedTime+=timerPeriod;PLAY_ElapsedTime+=timerPeriod;DA_ElapsedTime+=timerPeriod;
	  SA_ElapsedTime += timerPeriod;EA_ElapsedTime+=timerPeriod;GSA_ElapsedTime+=timerPeriod;GHA_ElapsedTime+=timerPeriod;DIGIVOLVE_ElapsedTime+=timerPeriod;
	  SM_ElapsedTime +=timerPeriod;FR_ElapsedTime+=timerPeriod;LHAH_ElapsedTime+=timerPeriod;CID_ElapsedTime+=timerPeriod;
	  LI_ElapsedTime +=timerPeriod;
	  TBP_ElapsedTime +=timerPeriod;
	  R1A_ElapsedTime +=timerPeriod;
	  R2A_ElapsedTime +=timerPeriod;
   }
}
