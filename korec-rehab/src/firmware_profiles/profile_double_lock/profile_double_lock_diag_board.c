/***********************************************************************
*	Hand Control
***********************************************************************/

#include "includes.h"
#include <avr/sleep.h>

//INC(PORTC1) and STBY(PORTC0)
#define INC		PORTC3
#define STBY	PORTC2

/*************************************************************************
 * Function prototype sector
 ************************************************************************/
void init_os_tick( void );		// 타이머 0 = 1msec, 타이머 2 = 10 msec 설정
void init_adc_gpio( void ); 		// ADC 세마포어 = 1
void init_pwm( void ); 		// 타이머 1을 PWM로 설정
void init_interrupt_mask( void ); 	// 타이머 0, 타이머 2 인터럽트 인에이블
void init_uart( INT32U ulbrate );
void sleep( void );

void uart_tx(INT8U data);		// UART 송신 Polling 방식
void uart_tx_string(INT8U* str);	// UART 문자열 송신 Polling 방식
INT8U uart_rx(void);		// UART 수신 Polling 방식

void stop_control( void );
void stop_control_2( void );
void close_control( void );
void close_control_2( void );
void open_control( void );
void open_control_2( void );
void lock_control( void );
void lock_off_control( void );

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
volatile INT8U	MOTOR_LOAD = 0;		// Motor Load Current
volatile INT8U	MOTION_CONTROL = 0;		// Motor Load Current

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
#define _LOCK	3
#define _LOCK_OFF	4

volatile INT8U	PWM_duty = 0;
volatile INT8U  Current_PWM_duty = 0;	// Close 동작 duty
volatile INT8U  Current_OPEN_PWM_duty = 0;	// Open 동작 duty
volatile INT8U	direction = _STOP;		// 현재 상태

/*************************************************************************
 * Control
 ************************************************************************/
 // Interrupt check
volatile int adc_flag = 0;
volatile int tc0_flag = 0;
volatile int tc2_flag = 0;

// Push Lock SW
volatile int MC_cnt = 0;

// Close - Open, Open - Close 오작동 방지
volatile INT8U close_flag = 0;
volatile INT8U open_flag = 0;

// Dobule Lock check
volatile INT8U state_cnt = 0;
volatile INT8U state_time = 0;

// Dobule Lock off check
volatile INT8U lock_off_cnt = 0;
volatile INT8U lock_off_time = 0;

// Lock
volatile INT8U Lock_cnt = 0;
volatile INT8U Lock_time = 0;
volatile INT8U Lock_flag = 0;
volatile INT8U stop_time = 0;

volatile INT8U diff_sensor;

// 환자 신호 특성에 따라 변경
#define	_PRI_TH	50	// Close
#define	_AUX_TH	30	// Open
#define _LCK_TH	30	// Lock

#define _STOP_STATE		0
#define _CLOSE_STATE	1
#define _OPEN_STATE		2
#define _LOCK_STATE		3
#define _LOCK_OFF_STATE	4

volatile INT8U	state = _STOP_STATE;
volatile INT16U	currentTick = 0;
volatile INT16U	Tick_mul = 4;

// PWM Duty //
volatile INT8U   MIN_PWM_Normal = 150;		//120
volatile INT8U   MIN_PWM_Normal2 = 100;	
volatile INT8U   MIN_PWM_Normal3 = 180;		//150;
volatile INT8U   MIN_PWM_Normal4 = 100;	
volatile INT8U   MID_PWM_Normal = 180;		//150;
volatile INT8U   MID_PWM_Normal2 = 180;		//150;
volatile INT8U   MAX_PWM_Normal = 210;		//180;
volatile INT8U   MAX_PWM_Normal2 = 210;		//180;
volatile INT8U   MAX_PWM_Normal3 = 210;		//180;

volatile INT8U   MIN_OPENPWM_Normal = 150;	//120
volatile INT8U   MIN_OPENPWM_Normal2 = 100;	
volatile INT8U   MIN_OPENPWM_Normal3 = 180;	//150
volatile INT8U   MIN_OPENPWM_Normal4 = 100;	
volatile INT8U   MID_OPENPWM_Normal = 180;	//150
volatile INT8U   MID_OPENPWM_Normal2 = 180;	//150
volatile INT8U   MAX_OPENPWM_Normal = 210;	//180
volatile INT8U   MAX_OPENPWM_Normal2 = 210;	//180
volatile INT8U   MAX_OPENPWM_Normal3 = 210;	//180

//PWM_duty = PWM_MIN+(currentTick*PWM_Offset); tick<7
volatile INT8U 	 PWM_MIN = 70;
volatile INT8U 	 PWM_OPEN_MIN = 70;
volatile INT8U   PWM_Offset = 2;
volatile INT8U   PWM_OPEN_Offset = 3;

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

	// Function Init
	init_os_tick();
	init_adc_gpio();
	init_pwm();
	init_interrupt_mask();
	init_uart(USART0_BRATE);

	sei();		//SEI(Set Global Interrupt Flag) - SREG의 I비트로 인터럽트 활성화 비트

	// Sleep mode on //
	sleep();
	
	while(1){
		if( WAKEUP_STATE == 1 ){
			GICR &= 0b11000011;
			WAKEUP_STATE = 0;
		}

		// ADC
		if( adc_flag == 1 ){
			INT8U	adc_data;
			adc_data = ADCH;

			if( ADMUX == 0x20 ){		// ADC0 = EMG2
				MOTOR_LOAD = adc_data;

				current_sum = current_sum - current_buf[current_cnt] + (MOTOR_LOAD);
				current_buf[current_cnt++] = (MOTOR_LOAD);
				if( current_cnt == _CURRENT_BUF_MAX )	current_cnt = 0;
				current_val = current_sum >> 3;
				
				adc_channel++;
			}
	
			else if( ADMUX == 0x21 ){		// ADC1 = EMG1
				MOTION_CONTROL = adc_data;
				adc_channel++;
			}

			else if( ADMUX == 0x22 ){		// ADC3 = MCUR_F
				EMG1 = adc_data;
				adc_channel++;
			}
			
			else if( ADMUX == 0x23 ){		// ADC3 = MCUR_F
				EMG2 = adc_data;
				adc_channel++;
			}			

			else if ( adc_channel > _ADC_CH_NUM ){
				adc_channel = 0;
			}

			else{				// adc_channel == 2
				adc_data = adc_data;
				adc_channel++;
			}
	
			current_sensor = current_val;
			sem_adc++;
			adc_flag = 0;
		}
		
		// T/C 0
		if( tc0_flag == 1 ){
			
			if( direction == _STOP || direction == _LOCK || direction == _LOCK_OFF){
				// high gate
				outp( 0, OCR1AL );
				outp( 0, OCR1AH );
				// low gate
				outp( 0, OCR1BL );
				outp( 0, OCR1BH );

				//PORTC = (1<<INC)|(0<<STBY);	// 진단보드
			}

			else if( direction == _OPEN && open_flag == 1 ){
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

			else if( direction == _CLOSE && close_flag == 1 ){
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
	
			TCNT0 = 255-8;				// 125usec * 8 = 1msec
			tc0_flag = 0;
		}
		
		// T/C 2
		if( tc2_flag == 1 ){
			if( MOTION_CONTROL ){
				MC_cnt++;

				if( MC_cnt < 70 )	stop_control();

				else if( MC_cnt >= 70 && MC_cnt < 140 )	open_control();

				else if( MC_cnt >= 140 && MC_cnt < 180 ) stop_control();

				else if( MC_cnt >= 180 && MC_cnt < 250 ) close_control();

				else MC_cnt = 0;
			}
			
			else{
				MC_cnt = 0;

			diff_sensor = abs(EMG1 - EMG2);
	
			// 락 풀기
			if( Lock_flag == 1){
				if( EMG1 > _LCK_TH && EMG2 > _LCK_TH ){
					// Lock 걸리고 1초동안 수행X
					if( Lock_time == 101 ){
						if( lock_off_cnt <= 1 )	stop_control_2();
					
						else if( lock_off_cnt == 2 )	lock_off_control();
					}
					
					else lock_control();		//stop_control()	
				}

				else	lock_control();
			}
			
			// 락 걸기
			else{
				if( diff_sensor > 60){ 
					if( EMG1 > EMG2 ){
					 	if( EMG1 > _LCK_TH && EMG2 > _LCK_TH ){
							if( state_cnt <= 1)	close_control_2();

							else if( state_cnt == 2 )	lock_control();	
						}

						else	close_control();
					}

					else if( EMG1 < EMG2 ){
						if( EMG1 > _LCK_TH && EMG2 > _LCK_TH ){
							if( state_cnt <= 1)	open_control_2();
	
							else if( state_cnt == 2 )	lock_control();	
						}

						else	open_control();
					}
				}
	
				else{
					if( EMG1 < _AUX_TH && EMG2 < _AUX_TH ) 		stop_control();	// L & L
									
					else if( EMG1 > _PRI_TH && EMG2 < _AUX_TH )	close_control();	// H & L
				
					else if( EMG1 < _AUX_TH && EMG2 > _PRI_TH )	open_control();	// L & H
				
					else if( EMG1 > _LCK_TH && EMG2 > _LCK_TH ){			// H & H: Co-Contraction
						if( state_cnt <= 1)	close_control_2();

						else if( state_cnt == 2 )	lock_control();	
					}

					else stop_control();
				} 
			}
			}

			TCNT2 = 255-80;					// 125usec * 80 = 15msec
			tc2_flag = 0;
		}

		uart_tx(EMG1);
		uart_tx(EMG2);
		//uart_tx(lock_off_cnt);
		//uart_tx(lock_off_time);
	}

	cli();		// CLI: 인터럽트 중 인터럽트 발생을 방지하기 위한 처리 ~ 없어도 댐

	return 0;
}

/*----------------------------------------------------------------------
*
* Function Name  : SLEEP()
* Returned Value : void
* Comments       : MCUCR(MCU Control Register)에서 sleep mode 관리(SE set, SM, etc)
*		총 6가지 Sleep mode 중 Power-Down mode를 사용
*   		#define _MODE_IDLE         (0)
*    		#define SLEEP_MODE_ADC          _BV(SM0)
*   		#define SLEEP_MODE_PWR_DOWN     _BV(SM1)
*  		#define SLEEP_MODE_PWR_SAVE     (_BV(SM0) | _BV(SM1))
*  		#define SLEEP_MODE_STANDBY      (_BV(SM1) | _BV(SM2))
*  		#define SLEEP_MODE_EXT_STANDBY  (_BV(SM0) | _BV(SM1) | _BV(SM2))
*---------------------------------------------------------------------*/
/* sleep mode variable, 16비트 타이머/카운터 사용 */
void sleep(){
  	set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
 	sleep_mode(); 
}

/*----------------------------------------------------------------------
* 			< UART 통신 >
* Function Name  : init_uart(INT32U ulbrate), uart_tx(INT8U data), 
		uart_tx_string(INT8U* str), uart_rx(void)
* Returned Value :
* Comments       : 
*
*---------------------------------------------------------------------*/
void init_uart(INT32U ulbrate){
  	UCSRB = (1<<RXEN|1<<TXEN);			// RXEN, TXEN: 송수신 허용
  	UCSRC = (1<<URSEL|1<<UCSZ1|1<<UCSZ0);		// URSEL = 1(동기), UCSZ2, 1, 0 = 0 1 1(8bit)
	UBRRH = (INT8U)(USART0_BRATE_SLT(ulbrate) >> 8);	// UBRR 상위 4비트 Write
  	UBRRL = (INT8U)(USART0_BRATE_SLT(ulbrate));		// UBRR 하위 8비트 Write
}

void uart_tx(INT8U data){
    while(!(UCSRA & (1<<UDRE)));		// 이전 송신이 끝나기를 기다림
    UDR = data;
}

void uart_tx_string(INT8U* str){
	while(*str){
		while(!(UCSRA & (1<<UDRE)));
    	UDR = *str;
		str++;
	}
}

INT8U uart_rx(void){
    while(!(UCSRA & (1<<RXC)));		// 이전 수신이 끝나기를 기다림
    return UDR;
}



/*----------------------------------------------------------------------
*
* Function Name  : init_adc
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
void init_adc_gpio( void ){
	sem_adc = 1;
	
	/* DDRx = 1(출력), 0(입력) */
	DDRA = (0<<PORTA0)|(0<<PORTA1)|(0<<PORTA2)|(0<<PORTA3)|(1<<PORTA4);
	PORTA = (0<<PORTA4);

	

	/* INC and STBY are always high
	   lever sensor ON/OFF */
	DDRC = (1<<INC)|(1<<STBY);
	PORTC = (0<<INC)|(1<<STBY);	// 제어기
	//PORTC = (1<<INC)|(0<<STBY);	// 진단보드

	DDRD = (1<<PORTD5)|(1<<PORTD4);
}

/*----------------------------------------------------------------------
*						< PWM >
* Function Name  : init_os_tick, init_pwm, init_interrupt_mask
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
*
*   TIMSK: OCIE2, TOIE2, TICIE1, OCIE1A, OCIE1B, TOIE1, OCIE0, TOIE0
*---------------------------------------------------------------------*/
void init_os_tick( void ){
	TCCR0 |= (1<<CS00|1<<CS02);			// 8MHz / 1024 = 8KHz = 125usec
	TCNT0 = 255-8;					// 125usec * 8 = 1msec
	/*----------------------------------------------------------*/
	/* time 0 is used for ADC with duration 5msec	          */
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

	TCCR2 |= (1<<CS20|1<<CS21|1<<CS22);		// 8MHz / 1024 = 8KHz = 125usec
	TCNT2 = 255-80;					// 125usec * 80 = 10 msec
	/*----------------------------------------------------------*/
	/* time 2 is used for control task with duration 1msec   */
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
}

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
}


void init_interrupt_mask( void ){
	/* Enable Timer0 & Timer2 overflow interrupt */
	outp((1<<TOIE0)|(1<<TOIE2), TIMSK);	
}

/*----------------------------------------------------------------------
*			 < Control >
* Function Name  : stop_control, close_control, open_control, lock_control, lock_off_control
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
/* Stop */
void stop_control( void ){
	if(state == _LOCK_STATE){
		lock_control();
	}
	
	else if(state == _LOCK_OFF_STATE){
		if(Lock_cnt == 2){
			stop_time++;
		
			if(stop_time > 100){
				state = _STOP_STATE;
				direction = _STOP;
				stop_time = 101;
				Lock_flag = 0;

				state_time = 0;
				state_cnt = 0;
				lock_off_time = 0;
				lock_off_cnt = 0;
			}

			else{
				state = _LOCK_OFF_STATE;
			}
		}
	}

	else{
		state = _STOP_STATE;
		direction = _STOP;
	
		// close - open 오작동 방지
		close_flag = 0;
		open_flag = 0;
		Lock_flag = 0;
		Lock_cnt = 0;
		Lock_time = 0;
		stop_time = 0;
		
		// Lock on
		if( state_cnt >= 1 ){
			state_time++;

			if ( state_time >= 40 && state_time <= 80 )	state_cnt = 2;		
			
			else if( state_time > 80 ){
				state_time = 0;
				state_cnt = 0;
			}		
		}
		
		// PWM & direction setting
		Current_PWM_duty = PWM_MIN;
		Current_OPEN_PWM_duty = PWM_MIN;
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
}

// C.C Lock off
void stop_control_2( void ){
	lock_off_cnt = 1;
}

/* Close */
void close_control( void ){
	if(state == _LOCK_STATE || state == _LOCK_OFF_STATE){
		lock_control();
	}

	else{
		if(state != _CLOSE_STATE) currentTick = 0;
		sleep_cnt = 0;

		// close - open 오작동 방지
		close_flag = 1;
		//open_flag = 0;

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

		// PWM duty limit check
		if( PWM_duty > CLOSE_MAX_PWM )	PWM_duty = CLOSE_MAX_PWM;
	}
}

/* C.C Close */
void close_control_2( void ){
	state_cnt = 1;
	close_control();
}

/* Open */
void open_control( void ){
	if(state == _LOCK_STATE || state == _LOCK_OFF_STATE){
		lock_control();
	}

	else{
		if(state != _OPEN_STATE) currentTick = 0;
		
		sleep_cnt = 0;

		// close - open 오작동 방지
		//close_flag = 0;
		open_flag = 1;

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

		// PWM duty limit check
		if( PWM_duty > OPEN_MAX_PWM )	PWM_duty = OPEN_MAX_PWM;
	}
}

/* C.C Open */
void open_control_2( void ){
	state_cnt = 1;
	open_control();
}

/* Lock */
void lock_control( void ){
	if(state == _LOCK_OFF_STATE){
		// Lock off
		if( lock_off_cnt >= 1 ){
			lock_off_time++;
		
			if ( lock_off_time >= 40 && lock_off_time <= 80 )	lock_off_cnt = 2;		
		
			else if( lock_off_time > 80 ){
				lock_off_time = 0;
				lock_off_cnt = 0;
			}		
		}
		
		stop_control();
	}
	
	else{
		Lock_cnt = 1;
		Lock_flag = 1;
		Lock_time++;

		if( Lock_time > 100){
			state = _LOCK_OFF_STATE;
			direction = _LOCK;
			Lock_time = 101;
		}

		else{
			state = _LOCK_STATE;
			direction = _LOCK;
		}
	
		Current_PWM_duty = PWM_MIN;
		Current_OPEN_PWM_duty = PWM_MIN;
		PWM_duty = 0;
	
		currentTick = 0;
	}
}

/* Lock Off */
void lock_off_control( void ){
	if(state == _LOCK_OFF_STATE){
		direction = _LOCK_OFF;
		Lock_cnt = 2;
		stop_control();
	}
	
	else{
		Current_PWM_duty = PWM_MIN;
		Current_OPEN_PWM_duty = PWM_MIN;
		PWM_duty = 0;

		currentTick = 0;
	}
}

/*----------------------------------------------------------------------
*			   < Interrupt >
* Function Name  : INT2_vect, SIGNAL(SIG_ADC), SIGNAL(SIG_OVERFLOW0), SIGNAL(SIG_OVERFLOW2)
* Returned Value : void
* Comments       : 
* 1. INT2_vect: 		  Sleep mode wake up interrupt(EMG)
* 2. SIGNAL(SIG_ADC): 	  Signal handler for ADC end interrupt
* 3. SIGNAL(SIG_OVERFLOW0): Signal handler for tcnt0 overflow interrupt, 1 msec
* 			   ADC 값이 계속들어 오는 경우: ADC 인터럽트를 계속 사용하는 것 보다 TC를 통해 특정 시간마다 ADC 변환하도록 설정하는 것이 좋다 - 자원 줄이기 위함
* 			   특정 값 이상의 ADC가 들어올 경우는 ADC 인터럽트를 사용가능
* 			   T/C 0(Timer Count Update)을 사용하여 특정시간마다 T/C 1(PWM 제어)를 동작 - 일반적으로 T/C 1개는 T/C update로 사용, 다른 T/C는 PWM 제어에 사용
* 4. SIGNAL(SIG_OVERFLOW2): Signal handler for tcnt2 overflow interrupt, 10 msec
*
*---------------------------------------------------------------------*/
/* 외부 인터럽트 2의 서비스 루틴 */
ISR(INT2_vect){
	WAKEUP_STATE = 1;
}

/* ADC */
SIGNAL(SIG_ADC){
	adc_flag = 1;
}

/* Timer/Counter 0 */
SIGNAL(SIG_OVERFLOW0){
	tc0_flag = 1;
}

/* Timer/Counter 2 */
SIGNAL(SIG_OVERFLOW2){
	tc2_flag = 1;
}