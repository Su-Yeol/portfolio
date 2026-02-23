/***********************************************************************
*	Hand Control 
***********************************************************************/

#include "usart.c"
#include "printfd.c"
#include <avr/sleep.h>
#include <util/delay.h>

//INC(PORTC1) and STBY(PORTC0)
#define INC		PORTC3
#define STBY	PORTC2

/*************************************************************************
 * Function prototype sector
 ************************************************************************/
void init_os_tick( void );		// 타이머 0 = 1msec, 타이머 2 = 10 msec 설정
void init_adc( void ); 			// ADC 세마포어 = 1
void init_pwm( void ); 			// 타이머 1을 PWM로 설정
void init_interrupt_mask( void ); 	// 타이머 0, 타이머 2 인터럽트 인에이블
void init_uart( INT32U ulbrate );
void sleep( void );

void uart_tx(INT8U data);		// UART 송신 Polling 방식
void uart_tx_string(INT8U* str);	// UART 문자열 송신 Polling 방식
INT8U uart_rx(void);			// UART 수신 Polling 방식

void stop_control( void );
void close_control( void );
void open_control( void );

/*************************************************************************
 * Sleep mode
 ************************************************************************/
volatile INT16U Sleep_on_cnt = 1900;
volatile INT16U sleep_cnt = 0;

/*************************************************************************
 * ADC
 ************************************************************************/
#define	_ADC_CH_NUM	3

volatile INT8U	adc_channel = 0;
volatile INT8U 	sem_adc = 0;

volatile INT8U	EMG1 = 0;
volatile INT8U	EMG2 = 0;
volatile INT8U	MCUR_F_CH2 = 0;		// Motor Load Current

#define	_CURRENT_BUF_MAX	8
volatile INT16U	current_val = 0;
volatile INT16U	current_sum = 0;
volatile INT16U	current_buf[_CURRENT_BUF_MAX];
volatile INT16U	current_cnt =	0;
volatile INT16U	current_sensor = 0;

/*************************************************************************
 * PWM
 ************************************************************************/
/* set the PWM top value	*/
#define	_TOP_PWM	255

#define	OPEN_MAX_PWM	210	//210
#define	CLOSE_MAX_PWM	210	//210

#define _STOP	0
#define _CLOSE	1
#define _OPEN	2

volatile INT8U	PWM_duty = 0;
volatile INT8U  Current_PWM_duty = 0;		// Close 동작 duty
volatile INT8U  Current_OPEN_PWM_duty = 0;	// Open 동작 duty
volatile INT8U	direction = _STOP;			// 현재 상태

/*************************************************************************
 * Control
 ************************************************************************/
/* threshold values for EMG Sensor */
volatile int E1_flag = 0;
volatile int E2_flag = 0;

volatile INT8U Lock_cnt = 0;
volatile INT8U Lock_flag = 0;
volatile INT8U	diff_sensor;
// 환자 신호 특성에 따라 변경
#define	_PRI_TH	50	// Close
#define	_AUX_TH	30	// Open

#define _STOP_STATE		0
#define _CLOSE_STATE	1
#define _OPEN_STATE		2

volatile INT8U	state = _STOP_STATE;
volatile INT16U	currentTick = 0;
volatile INT16U	Tick_mul = 4;

// PWM Duty //
volatile INT8U   MIN_PWM_Normal = 120;	//120
volatile INT8U   MIN_PWM_Normal2 = 100;	
volatile INT8U   MIN_PWM_Normal3 = 150;	//150;
volatile INT8U   MIN_PWM_Normal4 = 100;	
volatile INT8U   MID_PWM_Normal = 150;	//150;
volatile INT8U   MID_PWM_Normal2 = 150;	//150;
volatile INT8U   MAX_PWM_Normal = 180;	//180;
volatile INT8U   MAX_PWM_Normal2 = 180;	//180;
volatile INT8U   MAX_PWM_Normal3 = 180;	//180;

volatile INT8U   MIN_OPENPWM_Normal = 120;	//120
volatile INT8U   MIN_OPENPWM_Normal2 = 100;	
volatile INT8U   MIN_OPENPWM_Normal3 = 150;	//150
volatile INT8U   MIN_OPENPWM_Normal4 = 100;	
volatile INT8U   MID_OPENPWM_Normal = 150;	//150
volatile INT8U   MID_OPENPWM_Normal2 = 150;	//150
volatile INT8U   MAX_OPENPWM_Normal = 180;	//180
volatile INT8U   MAX_OPENPWM_Normal2 = 180;	//180
volatile INT8U   MAX_OPENPWM_Normal3 = 180;	//180

//PWM_duty = PWM_MIN+(currentTick*PWM_Offset); tick<7
volatile INT8U 	 PWM_MIN = 70;
volatile INT8U 	 PWM_OPEN_MIN = 70;
volatile INT8U   PWM_Offset = 2;
volatile INT8U   PWM_OPEN_Offset = 3;

/*************************************************************************
 * USART
 ************************************************************************/
extern void init_usart0(INT32U ulbrate);
extern void init_usart_queue(void);

volatile INT8U bUART=0;
INT8U WAKEUP_STATE = 0;

/*************************************************************************
 * USART
 ************************************************************************/
#define USART0_BRATE		9600
#define CPU_CLOCK_HZ 		8000000
#define USART0_BRATE_SLT(x) ((CPU_CLOCK_HZ/(x*16L)-1))

/*----------------------------------------------------------------------
*
* Function Name  : main
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
int main (void){
	PWM_duty = PWM_MIN;
	Current_PWM_duty = PWM_MIN;
	Current_OPEN_PWM_duty = PWM_OPEN_MIN;

	// Wake up Interrupt Initialize //
	GICR |= (1<<INT2);		// INT2 enable
	MCUCSR |= (1<<ISC2); 	// Rising edge

	/////////////////////////////////////////
	init_os_tick();
	init_adc();
	init_pwm();
	init_interrupt_mask();
	//init_uart(USART0_BRATE);
	init_usart0(USART0_BRATE);
	init_usart_queue();	

	sei();					//SEI(Set Global Interrupt Flag) - SREG의 I비트로 인터럽트 활성화 비트

	// Sleep mode on //
	sleep();

	while(1){
		if(WAKEUP_STATE == 1){
			GICR &= 0b11000011;
			WAKEUP_STATE = 0;
		}
	}

	cli();					// CLI: 인터럽트 중 인터럽트 발생을 방지하기 위한 처리 ~ 없어도 댐

	return 0;
}

/*----------------------------------------------------------------------
*
* Function Name  : INT2_vect
* Returned Value : void
* Comments       : Sleep mode wake up interrupt(EMG)
*
*---------------------------------------------------------------------*/
ISR(INT2_vect){
	WAKEUP_STATE = 1;
}

/*----------------------------------------------------------------------
*
* Function Name  : SLEEP()
* Returned Value : void
* Comments       : MCUCR(MCU Control Register)에서 sleep mode 관리(SE set, SM, etc)
*				   총 6가지 Sleep mode 중 Power-Down mode를 사용
*   		 	#define _MODE_IDLE         (0)
*    			#define SLEEP_MODE_ADC          _BV(SM0)
*   		 	#define SLEEP_MODE_PWR_DOWN     _BV(SM1)
*  			 	#define SLEEP_MODE_PWR_SAVE     (_BV(SM0) | _BV(SM1))
*  				#define SLEEP_MODE_STANDBY      (_BV(SM1) | _BV(SM2))
*  				#define SLEEP_MODE_EXT_STANDBY  (_BV(SM0) | _BV(SM1) | _BV(SM2))
*---------------------------------------------------------------------*/
/* sleep mode variable, 16비트 타이머/카운터 사용 */
void sleep(){
  	set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
 	sleep_mode(); 
}

/*----------------------------------------------------------------------
*
* Function Name  : SIGNAL(SIG_UART_RECV)
* Returned Value :
* Comments       : 
*
*---------------------------------------------------------------------*/
SIGNAL(SIG_UART_RECV)
{	
  INT8U uch;

  uch = UDR;

#if 0
  in_queue(&usart0_rx_q, uch);			
#else
  if( usart0_rx_q.cnt < USART0_RXQ_LEN ) {
    in_queue(&usart0_rx_q, uch);
    usart0_rx_q.cnt++;
  }
#endif
}

/*----------------------------------------------------------------------
*
* Function Name  : SIGNAL(SIG_UART0_TRANS)
* Returned Value :
* Comments       : 
*
*---------------------------------------------------------------------*/	
SIGNAL(SIG_UART_TRANS)
{	
  INT8U uch;
        
  usart0_tx_q.cnt--;
  if(usart0_tx_q.cnt) {
    uch = out_queue(&usart0_tx_q);
    UDR = uch;
  } 
  else {
    UCSRB |=(1<<RXCIE|1<<RXEN);
    UCSRB &=~(INT8U)(1<<TXCIE|1<<TXEN);	
  }	
  //OSSemPost(usart0_tx_q.sem);
}

/*----------------------------------------------------------------------
*
* Function Name  : init_os_tick
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
void init_os_tick( void ){
	/*----------------------------------------------------------*/
	/* time 0 is used for ADC with duration 5msec				*/
	/*----------------------------------------------------------*/
	/*	Clock Select Bit Description
	CS02	CS01	CS00	Description
	0		0		0		No clock source (Timer/Counter stopped).
	0		0		1		clkI/O / 1 (No prescaling)
	0		1		0		clkI/O / 8 (From prescaler)
	0		1		1		clkI/O / 64 (From prescaler)
	1		0		0		clkI/O / 256 (From prescaler)
	1		0		1		clkI/O / 1024 (From prescaler)
	1		1		0		External clock source on T1 pin. Clock on falling edge.
	1		1		1		External clock source on T1 pin. Clock on rising edge. */

	TCCR0 |= (1<<CS00|1<<CS02);		//	8MHz / 1024 = 8KHz = 125usec
	TCNT0 = 255-8;					//	125usec * 8 = 1msec

	/*----------------------------------------------------------*/
	/* time 2 is used for control task with duration 1msec		*/
	/*----------------------------------------------------------*/
	/*	Clock Select Bit Description
	CS22	CS21	CS20	Description
	0		0		0		No clock source (Timer/Counter stopped).
	0		0		1		clkT2S/(No prescaling)
	0		1		0		clkT2S/8 (From prescaler)
	0		1		1		clkT2S/32 (From prescaler)
	1		0		0		clkT2S/64 (From prescaler)
	1		0		1		clkT2S/128 (From prescaler)
	1		1		0		clkT2S/256 (From prescaler)
	1		1		1		clkT2S/1024 (From prescaler)*/

	TCCR2 |= (1<<CS20|1<<CS21|1<<CS22);		//	8MHz / 1024 = 8KHz = 125usec
	TCNT2 = 255-80;							// 125usec * 80 = 10 msec
}

/*----------------------------------------------------------------------
*
* Function Name  : init_adc
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
void init_adc( void ){
	outp( 0x00, DDRA );

	sem_adc = 1;
}

/*----------------------------------------------------------------------
*
* Function Name  : init_pwm
* Returned Value : void
* Comments       :
*	output compare port: OC1A (PB5), OC1B(PB6)
*	1. TCCR1A : COM1A1,COM1A0,COM1B1,COM1B0,COM1C1,COM1C0,WGM11,WGM10
*	2. TCCR1B : ICNC1 ,ICES1 , ---- ,WGM13 ,WGM12 ,CS12  ,CS11 ,CS10
*	3. Timer/Counter1: TCNT1H/TCNT1L
*	4. ouput compare reg. 1A: OCR1AH/OCR1AL
*	5. Time/Counter Int. Mask Reg (TIMSK)
*		   OCIE2, TOIE2, TICIE1,OCIE1A,OCIE1B, TOIE1,OCIE0,TOIE0
*	6. Time/Counter Int. Flag Reg (TIFR)
*		   OCF2, TOV2, ICF1, OCF1A ,OCF1B , TOV1 ,OCF0 ,TOV0
*---------------------------------------------------------------------*/
void init_pwm( void ){
	outp( 0xA0, TCCR1A );	// 0xA0=10100000
		                    // [xxxxxx00] PWM, Phase and Frequency Correct mode (TCCR1B와 함께)
							// [10xxxxxx] match->high while upcounting, match->clear while downcounting (P & F correct mode )
							// [xx10xxxx] match->high, TOP->clear OC1B port (Fast PWM일 때)
							// [xxxx00xx] OC1C is disconnected
	outp( 0x51, TCCR1B );	// 0x51=01010001
							// [xxx10xxx] PWM, Phase and Frequency Correct mode (TCCR1A와 함께)
							// [0xxxxxxx] Input Capture Noise Canceler 동작안함
							//            동작하면 시스템 클럭 4주기만큼 지연
							// [x1xxxxxx] Input Capture Edge Select
							//          1 = 캡쳐신호가 rising edge일 때 캡쳐
							//          0 = 캡쳐신호가 falling edge일 때 캡쳐
							// [xxxxx001] Prescaler의 Clock Select
							//       001 = clk / 1
							//       010 = clk / 8
							//       011 = clk / 64
							//       100 = clk / 256
							//       101 = clk / 1024

	outp( 0x00, TCNT1H );
	outp( 0x00, TCNT1L );

	// set compare register value to bottom
	outp( 0x00, OCR1AH );
	outp( 0x00, OCR1AL );

	// set compare register value to bottom
	outp( 0x00, OCR1BH );
	outp( 0x00, OCR1BL );

	// set TOP_PWM to the top value
	// carrier freq for PWM is 15.686KHz = 8MHz/(2*TOP_PWM(255))
	outp( 0, ICR1H );
	outp( _TOP_PWM, ICR1L );

	// init PORTB I/O direction and value
	DDRD = (1<<PORTD5)|(1<<PORTD4);

	/* INC and STBY are always high */
	/* lever sensor ON/OFF */
	DDRC = (1<<INC)|(1<<STBY);
	PORTC = (0<<INC)|(1<<STBY);	// 제어기
	//PORTC = (1<<INC)|(0<<STBY);	// 진단보드
}

/*----------------------------------------------------------------------
*
* Function Name  : init_interrupt_mask
* Returned Value : void
* Comments       :	TIMSK: OCIE2, TOIE2, TICIE1, OCIE1A, OCIE1B, TOIE1, OCIE0, TOIE0
*
*---------------------------------------------------------------------*/
void init_interrupt_mask( void ){
	/* Enable Timer0 & Timer2 overflow interrupt */
	outp((1<<TOIE0)|(1<<TOIE2), TIMSK);	
}

/*----------------------------------------------------------------------
*
* Function Name  : stop_control
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
void stop_control( void ){
	state = _STOP_STATE;
	direction = _STOP;

	/* PWM & direction setting */
	Current_PWM_duty = PWM_MIN;			// 70
	Current_OPEN_PWM_duty = PWM_MIN;	// 70
	PWM_duty = 0;

	currentTick = 0;
	sleep_cnt++;

	if(sleep_cnt > Sleep_on_cnt){
		sleep_cnt = 0;
		GICR |= (1<<INT2);
		sei();
		sleep();
	}
}

/*----------------------------------------------------------------------
*
* Function Name  : close_control
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
void close_control( void ){
	if(state != _CLOSE_STATE) currentTick = 0;
	sleep_cnt = 0;

	state = _CLOSE_STATE;
	direction = _CLOSE;
	
	if(currentTick > 45 * Tick_mul) PWM_duty = MAX_PWM_Normal3;
	else if(currentTick > 40 * Tick_mul) PWM_duty = MAX_PWM_Normal2;
	else if(currentTick > 32 * Tick_mul)  PWM_duty = MAX_PWM_Normal;
	else if(currentTick > 28 * Tick_mul)  PWM_duty = MID_PWM_Normal2;
	else if(currentTick > 20 * Tick_mul)  PWM_duty = MID_PWM_Normal;
	else if(currentTick > 14 * Tick_mul)  PWM_duty = MIN_PWM_Normal4;
	else if(currentTick > 8 * Tick_mul)  PWM_duty = MIN_PWM_Normal3;
	else if(currentTick > 4 * Tick_mul)  PWM_duty = MIN_PWM_Normal2;
	else if(currentTick > 2 * Tick_mul)  PWM_duty = MIN_PWM_Normal;
	else{
		if( PWM_duty > MIN_PWM_Normal) PWM_duty = MIN_PWM_Normal;
		else PWM_duty = PWM_MIN + (currentTick * PWM_Offset);
	}

	currentTick++;

	/* PWM duty limit check	*/
	if( PWM_duty > CLOSE_MAX_PWM )	PWM_duty = CLOSE_MAX_PWM;
}

/*----------------------------------------------------------------------
*
* Function Name  : open_control
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
void open_control( void ){
	if(state != _OPEN_STATE) currentTick = 0;
	sleep_cnt = 0;

	state = _OPEN_STATE;
	direction = _OPEN;

	if(currentTick > 60 * Tick_mul) PWM_duty = MAX_OPENPWM_Normal3;
	else if(currentTick > 40 * Tick_mul) PWM_duty = MAX_OPENPWM_Normal2;
	else if(currentTick > 32 * Tick_mul)  PWM_duty = MAX_OPENPWM_Normal;
	else if(currentTick > 28 * Tick_mul)  PWM_duty = MID_OPENPWM_Normal2;
	else if(currentTick > 20 * Tick_mul)  PWM_duty = MID_OPENPWM_Normal;
	else if(currentTick > 14 * Tick_mul)  PWM_duty = MIN_OPENPWM_Normal4;
	else if(currentTick > 8 * Tick_mul)  PWM_duty = MIN_OPENPWM_Normal3;
	else if(currentTick > 4 * Tick_mul)  PWM_duty = MIN_OPENPWM_Normal2;
	else if(currentTick > 2 * Tick_mul)  PWM_duty = MIN_OPENPWM_Normal;
	else{
		if( PWM_duty > MIN_OPENPWM_Normal) PWM_duty = MIN_OPENPWM_Normal;
		else PWM_duty = PWM_OPEN_MIN + (currentTick * PWM_OPEN_Offset);
	}

	currentTick++;

	/* PWM duty limit check	*/
	if( PWM_duty > OPEN_MAX_PWM )	PWM_duty = OPEN_MAX_PWM;
}

/*----------------------------------------------------------------------
*
* Function Name  : SIGNAL(SIG_ADC)
* Returned Value :
* Comments       : signal handler for ADC end interrupt
*
*---------------------------------------------------------------------*/
SIGNAL(SIG_ADC){
	INT8U	adc_data;
	adc_data = ADCH;

	if( ADMUX == 0x20 ){			// ADC0 = EMG2
		EMG2 = adc_data;
		adc_channel++;
	}
	
	else if( ADMUX == 0x21 ){		// ADC1 = EMG1
		EMG1 = adc_data;
		adc_channel++;
	}

	else if( ADMUX == 0x23 ){		// ADC3 = MCUR_F
		MCUR_F_CH2 = adc_data;

		current_sum = current_sum - current_buf[current_cnt] + (MCUR_F_CH2);
		current_buf[current_cnt++] = (MCUR_F_CH2);
		if( current_cnt == _CURRENT_BUF_MAX )	current_cnt = 0;
		current_val = current_sum >> 3;
		
		adc_channel++;
	}

	else if ( adc_channel > _ADC_CH_NUM ){
		adc_channel = 0;
	}

	else{							// adc_channel == 2
		adc_data = adc_data;
		adc_channel++;
	}
	
	current_sensor = current_val;
	sem_adc++;
}

/*----------------------------------------------------------------------
*
* Function Name  : tsk_ADC
* Returned Value : void
* Comments       : signal handler for tcnt0 overflow interrupt, 1 msec
*
*---------------------------------------------------------------------*/
SIGNAL(SIG_OVERFLOW0){
	if( direction == _STOP )
	{
		// high gate
		outp( 0, OCR1AL );
		outp( 0, OCR1AH );
		// low gate
		outp( 0, OCR1BL );
		outp( 0, OCR1BH );

		//PORTC = (1<<INC)|(0<<STBY);	// 진단보드
	}

	else if( direction == _OPEN){
		//micro control per 1ms	
		if( Current_OPEN_PWM_duty < PWM_duty) Current_OPEN_PWM_duty += 2;
		else Current_OPEN_PWM_duty = PWM_duty;

		// high gate
        outp( 0, OCR1AL );  
		outp( 0, OCR1AH );
		// low gate
    	outp( Current_OPEN_PWM_duty, OCR1BL );
		outp( 0, OCR1BH );

		//PORTC = (1<<INC)|(1<<STBY);	// 진단보드
	}

	else if( direction == _CLOSE ){
		if( Current_PWM_duty < PWM_duty) Current_PWM_duty += 5;
		else Current_PWM_duty = PWM_duty;
		
		//high gate
		outp( Current_PWM_duty, OCR1AL );
		outp( 0, OCR1AH );
		// low gate
		outp( 0, OCR1BL );
		outp( 0, OCR1BH );

		//PORTC = (1<<INC)|(1<<STBY);	// 진단보드
	}

	else{
		// high gate
		outp( 0, OCR1AL );
		outp( 0, OCR1AH );
		// low gate
		outp( 0, OCR1BL );
		outp( 0, OCR1BH );

		//PORTC = (1<<INC)|(0<<STBY);	// 진단보드
	}
	
	if(sem_adc){
		sem_adc--;

		outp(0x20|adc_channel, ADMUX);
		ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	}
	
	TCNT0 = 255-8;					//	125usec * 8 = 1msec
}

/*----------------------------------------------------------------------
*
* Function Name  : tsk_control
* Returned Value : void
* Comments       : signal handler for tcnt2 overflow interrupt, 10 msec
*
*---------------------------------------------------------------------*/

// Default 제어기
SIGNAL(SIG_OVERFLOW2){
	diff_sensor = abs(EMG1 - EMG2);

	if( diff_sensor > 40) {  //aux value problem solution
		if(EMG1 < EMG2) open_control();
		
		else close_control();
	}

	else{
		if( EMG1 < _AUX_TH && EMG2 < _AUX_TH ) stop_control();

		else if( EMG1 > _PRI_TH && EMG2 < _AUX_TH )	close_control();
	
		else if( EMG1 < _AUX_TH && EMG2 > _PRI_TH )	open_control();
				
		else if( EMG1 > _PRI_TH && EMG2 > _PRI_TH )	close_control();
		
		else{
			//NOP;
		}
	}
	TCNT2 = 255-80;					// 125usec * 20 = 2.5msec
}


/*
// Ottobock Switch System
SIGNAL(SIG_OVERFLOW2){
	diff_sensor = abs(EMG1 - EMG2);

	if( diff_sensor > 40){  	//Envelope EMG value, aux value problem solution
		if(EMG1 > EMG2){
			E1_flag = 1; E2_flag = 0;
			stop_control();
		}
	
		else if(EMG1 < EMG2){
			E1_flag = 0; E2_flag = 1;
			stop_control();			
		}

		else{
			stop_control();
		}
	}

	else{
		// 2 channel proportional sensor feedback control //
		if( EMG1 > _PRI_TH && EMG2 > _PRI_TH ){			// H & H Co-Contraction
			if ( E1_flag == 1 && E2_flag == 0 ){ 		// 이전 flag 1 0
				close_control(); // close_control
			}
	
			else if ( E1_flag == 0 && E2_flag == 1 ){	// 이전 flag 0 1
				open_control();
			}

			else{
				stop_control();
			}
		}

		else{
			stop_control();
			//NOP;
		}
	}
	TCNT2 = 255-80;					// 125usec * 80 = 10msec
}
*/