/***********************************************************************
*	Hand Control
***********************************************************************/
#include "includes.h"
#include <avr/sleep.h>

#define MOTOR_PS	PORTC2
#define MOTOR_PWM	PORTC3
#define MOTOR_INB	PORTD4
#define MOTOR_INA	PORTD5

/*************************************************************************
 * Function prototype sector
 ************************************************************************/
void init_os_tick( void );		// 타이머 0 = 1msec, 타이머 2 = 10 msec 설정
void init_pwm( void );			// 타이머 1을 PWM로 설정
void init_interrupt_mask( void );	// 타이머 0, 타이머 2 인터럽트 인에이블
void init_adc_gpio( void );
void init_uart( INT32U ulbrate );
void sleep( void );

void uart_tx(INT8U data);		// UART 송신 Polling 방식
//void uart_tx_string(char *str);	// UART 문자열 송신 Polling 방식
INT8U uart_rx(void);			// UART 수신 Polling 방식

void stop_control( void );
void close_control( void );
void open_control( void );
void lock_control( void );
void offset_control( void );
void init_offset( void );

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
volatile INT8U	MOTOR_LOAD = 0;		// Motor Load Current
volatile INT8U	MOTION_CONTROL = 0;

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
volatile INT8U  Current_CLOSE_PWM_duty = 0;	// Close 동작 duty
volatile INT8U  Current_OPEN_PWM_duty = 0;	// Open 동작 duty
volatile INT8U	direction = _STOP;		// 현재 상태

// PWM Duty //
volatile INT8U	MIN_PWM_Normal = 150;			//120
volatile INT8U	MIN_PWM_Normal2 = 100;	
volatile INT8U	MIN_PWM_Normal3 = 180;			//150
volatile INT8U	MIN_PWM_Normal4 = 100;	
volatile INT8U	MID_PWM_Normal = 180;			//150
volatile INT8U	MID_PWM_Normal2 = 180;			//150
volatile INT8U	MAX_PWM_Normal = 210;			//180
volatile INT8U	MAX_PWM_Normal2 = 210;			//180
volatile INT8U	MAX_PWM_Normal3 = 210;			//180

volatile INT8U	MIN_OPENPWM_Normal = 150;		//120
volatile INT8U	MIN_OPENPWM_Normal2 = 100;	
volatile INT8U	MIN_OPENPWM_Normal3 = 180;		//150v
volatile INT8U	MIN_OPENPWM_Normal4 = 100;	
volatile INT8U	MID_OPENPWM_Normal = 180;		//150
volatile INT8U	MID_OPENPWM_Normal2 = 180;		//150
volatile INT8U	MAX_OPENPWM_Normal = 210;		//180
volatile INT8U	MAX_OPENPWM_Normal2 = 210;		//180
volatile INT8U	MAX_OPENPWM_Normal3 = 210;		//180

//PWM_duty = PWM_MIN+(currentTick*PWM_Offset); tick<7
volatile INT8U	PWM_CLOSE_MIN = 70;
volatile INT8U	PWM_OPEN_MIN = 70;
volatile INT8U	PWM_CLOSE_Offset = 2;
volatile INT8U	PWM_OPEN_Offset = 3;

/*************************************************************************
 * Control
 ************************************************************************/
// Push Lock SW
volatile int MC_cnt = 0;

volatile INT8U diff_sensor;

// Close - Open, Open - Close 오작동 방지
volatile INT8U close_flag = 0;
volatile INT8U open_flag = 0;

//Init interrupt
volatile INT8U adc_flag = 0;
volatile INT8U tc0_flag = 0;
volatile INT8U tc2_flag = 0;

/*************************************************************************
 * Locking Algorithm
 ************************************************************************/
#define _LOCK	3
#define _LOCK_OFF	4

#define _LCK_TH1	30	// Lock on = 김재웅 35
#define _LCK_TH2	30	// Lock off

#define _LOCK_STATE		3
#define _LOCK_OFF_STATE	4

// 일반모드 & 락 모드
volatile INT8U lock_mode = 0;

// 락 모드 전환용 변수
volatile INT8U close_time = 0;	// 2~3초 이후 락 변환 변수
volatile INT8U lock_change_time = 0;	// 락 변환 전 정지동작 유지 변수
volatile INT8U standard_change_time = 0;// 락 변환 후 30초 후 일반모드 
volatile INT8U standard_change_cnt = 0; // 30초 시간 카운트( 0~255 )

// Dobule Lock check
volatile INT8U state_cnt = 0;
volatile INT8U state_time = 0;

// Dobule Lock off check
volatile INT8U lock_off_cnt = 0;
volatile INT8U lock_off_time = 0;
volatile INT8U LCK_OFF_TIME = 30; // 더블락 stop 시간 0.3s ~ (0.3+0.5)s

volatile INT8U Lock_cnt = 0;
volatile INT8U Lock_time = 0;
volatile INT8U Lock_flag = 0;
volatile INT8U stop_time = 0;

/*************************************************************************
 * Offset
 ************************************************************************/
#define WINDOW_MAX	50

volatile INT8U	EMG1_out = 0;
volatile INT8U	EMG2_out = 0;
volatile INT8U	old_EMG1[WINDOW_MAX] = {0}; // 이전 EMG1 데이터 저장
volatile INT8U	old_EMG2[WINDOW_MAX] = {0}; // 이전 EMG2 데이터 저장

volatile INT8U	EMG1_index = 0; // EMG1 배열 index
volatile INT8U	EMG2_index = 0; // EMG2 배열 index
volatile INT16U	EMG1_sum = 0; // WINDOW_MAXms EMG1 데이터 총합
volatile INT16U	EMG2_sum = 0; // WINDOW_MAXms EMG2 데이터 총합
volatile INT8U	EMG1_avg = 0; // WINDOW_MAXms EMG1 데이터 평균(offset)
volatile INT8U	EMG2_avg = 0; // WINDOW_MAXms EMG2 데이터 평균(offset)

volatile INT8U	EMG1_motion_avg = 0; // 동작에 적용할 EMG1 데이터 평균(offset)
volatile INT8U	EMG2_motion_avg = 0; // 동작에 적용할 EMG2 데이터 평균(offset)
volatile int	EMG1_motion_cnt = 0; // EMG1 동작 판별 카운트(offset)
volatile int	EMG2_motion_cnt = 0; // EMG2 동작 판별 카운트(offset)

volatile INT16U	EMG1_motion_time = 0; // EMG1 오동작 판단 시간
volatile INT16U	EMG2_motion_time = 0; // EMG1 오동작 판단 시간

volatile INT8U	EMG1_avg_flag = 0; // 처음 사이클 후 다음 EMG1 데이터 평균 체크용
volatile INT8U	EMG2_avg_flag = 0; // 처음 사이클 후 다음 EMG2 데이터 평균 체크용

/*************************************************************************
 * Init Value
 ************************************************************************/
// 환자 신호 특성에 따라 변경
#define	_PRI_TH	50	// Close
#define	_AUX_TH	30	// Open

#define _STOP_STATE	0
#define _CLOSE_STATE	1
#define _OPEN_STATE	2

volatile INT8U	state = _STOP_STATE;
volatile INT16U	currentTick = 0;
volatile INT16U	Tick_mul = 4;

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
	PWM_duty = PWM_CLOSE_MIN;
	Current_CLOSE_PWM_duty = PWM_CLOSE_MIN;
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

			if( ADMUX == 0x20 ){
				MOTOR_LOAD = adc_data;

				current_sum = current_sum - current_buf[current_cnt] + (MOTOR_LOAD);
				current_buf[current_cnt++] = (MOTOR_LOAD);
				if( current_cnt == _CURRENT_BUF_MAX )	current_cnt = 0;
				current_val = current_sum >> 3;
				
				adc_channel++;
			}
	
			else if( ADMUX == 0x21 ){
				MOTION_CONTROL = adc_data;
				adc_channel++;
			}
	
			else if( ADMUX == 0x22 ){
				EMG1 = adc_data;
				
				// Offsetㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
				if( EMG1_avg_flag == 1 ){ // 첫 100ms 동작 X
					EMG1_sum -= old_EMG1[EMG1_index]; // 다음 데이터의 Moving Avg를 위한 코드
					
					if( EMG1_index >= 0 ){ // 2개 이상의 데이터가 들어왔을 시
						if( old_EMG1[EMG1_index-1] < EMG1 ){ // 이전 데이터와 현재 데이터 크기 비교 new_EMG1[EMG1_cnt-1] < new_EMG1[EMG1_cnt]
							EMG1_motion_cnt++; // 현재 데이터가 클 시 카운트

							if( EMG1_motion_cnt == 1 ){ // 첫 증가때의 평균값을 동작구간에서의 offset으로 사용
								EMG1_motion_avg = EMG1_avg; // 첫 증가때의 평균값 저장
								EMG1_motion_flag = 1;
							}
						}
					
						else if( old_EMG1[EMG1_index-1] >= EMG1 ){
							EMG1_motion_cnt--;

 							if( EMG1_motion_cnt <= 0 ){
								EMG1_motion_cnt = 0;
								EMG1_motion_flag = 0;
							}
						}
					}
				}
				
				old_EMG1[EMG1_index] = EMG1; // 이전 데이터 제거를 위한 old 변수 new_EMG1[EMG1_cnt]
				EMG1_sum += EMG1; // 배열에 추가되는 데이터 Moving Avg를 위한 합 new_EMG1[EMG1_cnt]
				EMG1_index++; // 다음 배열을 위해 index값 증가
				
				if( EMG1_avg_flag == 0 ){
					if( EMG1_index == WINDOW_MAX ){
						EMG1_index = 0;

						EMG1_avg = (int)(EMG1_sum/WINDOW_MAX);
						if( EMG1_avg > 180 )	EMG1_avg = 0;

						EMG1_avg_flag = 1;
					}
				}

				else{
					EMG1_avg = (int)(EMG1_sum/WINDOW_MAX);
					if( EMG1_avg > 180 )	EMG1_avg = 0;

					if( EMG1_index == WINDOW_MAX ) EMG1_index = 0;
				}
				//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
				
				adc_channel++;
			}
			
			else if( ADMUX == 0x23 ){
				EMG2 = adc_data;
				
				// Offsetㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
				if( EMG2_avg_flag == 1){
					EMG2_sum -= old_EMG2[EMG2_index];
					
					if( EMG2_index >= 0 ){
						if( old_EMG2[EMG2_index-1] < EMG2 ){
							EMG2_motion_cnt++;

							if( EMG2_motion_cnt == 1 ){
								EMG2_motion_avg = EMG2_avg;
								EMG2_motion_flag = 1;
							}
						}

						else if( old_EMG2[EMG2_index-1] >= EMG2 ){
							EMG2_motion_cnt--;

							if( EMG2_motion_cnt == 0 ){
								EMG2_motion_cnt = 0;
								EMG2_motion_flag = 0;
							}
						}
					}
				}
				
				old_EMG2[EMG2_index] = EMG2;	// 기존 데이터 옮기기
				EMG2_sum += EMG2;		// 현재 100ms 근전도 데이터 합치기
				EMG2_index++;
					
				if( EMG2_avg_flag == 0 ){
					if( EMG2_index == WINDOW_MAX ){
						EMG2_index = 0; 

						EMG2_avg = (int)(EMG2_sum/WINDOW_MAX);
						if( EMG2_avg > 180 )	EMG2_avg = 0;

						EMG2_avg_flag = 1;
					}
				}

				else{
					EMG2_avg = (int)(EMG2_sum/WINDOW_MAX);
					if( EMG2_avg >= 180 )	EMG2_avg = 0;

					if( EMG2_index == WINDOW_MAX )	EMG2_index = 0;
				}
				//ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ
				adc_channel++;
			}

			else if ( adc_channel > _ADC_CH_NUM ){
				adc_channel = 0;
			}

			else{
				adc_data = adc_data;
				adc_channel++;
			}
	
			current_sensor = current_val;
			sem_adc++;
			adc_flag = 0;
		}
		
		// T/C 0
		if( tc0_flag == 1 ){
			
			if( direction == _STOP ){
				// high gate
				outp( 0, OCR1AL );
				outp( 0, OCR1AH );
				// low gate
				outp( 0, OCR1BL );
				outp( 0, OCR1BH );
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
			}

			else if( direction == _CLOSE && close_flag == 1 ){
				if( Current_CLOSE_PWM_duty < PWM_duty) Current_CLOSE_PWM_duty += 5;
				else Current_CLOSE_PWM_duty = PWM_duty;
		
				//high gate
				outp( Current_CLOSE_PWM_duty, OCR1AL );
				outp( 0, OCR1AH );
				// low gate
				outp( 0, OCR1BL );
				outp( 0, OCR1BH );
			}

			else{
				// high gate
				outp( 0, OCR1AL );
				outp( 0, OCR1AH );
				// low gate
				outp( 0, OCR1BL );
				outp( 0, OCR1BH );
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
			
			else{						// E1 E2 SW
				MC_cnt = 0;
				offset_control();
			}

			TCNT2 = 255-80;					// 125usec * 80 = 15msec
			tc2_flag = 0;
		}
		
		//uart_tx(EMG1_index);
		uart_tx(EMG1_motion_cnt);
		uart_tx(EMG1);
		uart_tx(EMG1_avg);
		uart_tx(EMG1_motion_avg);
		uart_tx(abs(EMG1-EMG1_avg));
		uart_tx(abs(EMG1-EMG1_motion_avg));
		uart_tx('\n');
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
/*
void uart_tx_string(char* str){
	while(*str){
		while(!(UCSRA & (1<<UDRE)));
    	UDR = *str;
		str++;
	}
}
*/
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
	// PA0: MOTOR_LOAD, PA1: MOTION_CONTROL, PA2: EMG1, PA3: EMG2, PA4: COM_SLC
	DDRA = (0<<PORTA0)|(0<<PORTA1)|(0<<PORTA2)|(0<<PORTA3)|(1<<PORTA4);
	PORTA = (0<<PORTA4);							// 1: RS232, 0: BLE

	/* INC and STBY are always high */
	/* lever sensor ON/OFF */
	DDRC = (1<<MOTOR_PWM)|(1<<MOTOR_PS);	// (1<<MOTOR_PWM)|(1<<MOTOR_PS) 0x0C
	PORTC = (0<<MOTOR_PWM)|(1<<MOTOR_PS);	// 진단보드 V2 - BD65496
	
	DDRD = (1<<MOTOR_INB)|(1<<MOTOR_INA);
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
	/* 모터 제어용으로 좌우대칭형인 PC PWM을 주로 사용
	   T/C 1(16bit)를 상위 8bit, 하위 8bit로 나누어 Close, Open 동작 PWM Duty cycle을 통한 평균전압 제어 */

	outp( 0xA0, TCCR1A );	
	/* 0xA0=10100000
	   [xxxxxx00] PWM, Phase and Frequency Correct mode (TCCR1B와 함께)
	   [10xxxxxx] match->clear while upcounting, match->set while downcounting (P & F correct mode )
	   [xx10xxxx] match->high, TOP->clear OC1B port (Fast PWM일 때)
	   [xxxx00xx] OC1C is disconnected */
	
	outp( 0x51, TCCR1B );
	/* 0x51=01010001
	   [xxx10xxx] PWM, Phase and Frequency Correct mode (TCCR1A와 함께)
	   [0xxxxxxx] Input Capture Noise Canceler 동작안함
				  동작하면 시스템 클럭 4주기만큼 지연
	   [x1xxxxxx] Input Capture Edge Select
	     1 = 캡쳐신호가 rising edge일 때 캡쳐
	     0 = 캡쳐신호가 falling edge일 때 캡쳐
	   [xxxxx001] Prescaler의 Clock Select
	         001 = clk / 1
	         010 = clk / 8
	         011 = clk / 64
	         100 = clk / 256
	         101 = clk / 1024 */

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

// Offset 초기화
void init_offset( void ){
	EMG1_index = 0; EMG2_index = 0; 
	EMG1_sum = 0; EMG2_sum = 0; 
	EMG1_avg = 0; EMG2_avg = 0; 

	EMG1_motion_avg = 0; EMG2_motion_avg = 0; 
	EMG1_motion_cnt = 0; EMG2_motion_cnt = 0; 

	EMG1_avg_flag = 0; EMG2_avg_flag = 0;
	EMG1_base_flag = 0;	EMG2_base_flag = 0;
	EMG1_motion_flag = 0; EMG2_motion_flag = 0;
}

/*----------------------------------------------------------------------
*			 < Control >
* Function Name  : stop_control, close_control, open_control, lock_control, lock_off_control
* Returned Value : void
* Comments       :
*
*---------------------------------------------------------------------*/
// Stop
void stop_control( void ){
	state = _STOP_STATE;
	direction = _STOP;
	
	// close - open 오작동 방지
	close_flag = 0;
	open_flag = 0;

	Current_CLOSE_PWM_duty = PWM_CLOSE_MIN;	// 70
	Current_OPEN_PWM_duty = PWM_OPEN_MIN;	// 70
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

// Close
void close_control( void ){
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
		else PWM_duty = PWM_CLOSE_MIN + (currentTick * PWM_CLOSE_Offset);
	}

	currentTick++;
	if( PWM_duty > CLOSE_MAX_PWM )	PWM_duty = CLOSE_MAX_PWM;
}

// Open
void open_control( void ){
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
	if( PWM_duty > OPEN_MAX_PWM )	PWM_duty = OPEN_MAX_PWM;
}
/*----------------------------------------------------------------------
*			   < Offset >
* Function Name  : Offset 동작
* Returned Value : void
* Comments       : 
*  EMG1_base_flag, EMG1_motion_flag, EMG2_base_flag, EMG2_motion_flag
*
*---------------------------------------------------------------------*/
void offset_control( void ){
	// 0 0
	if( EMG1_motion_flag == 0 && EMG2_motion_flag == 0 ){
		if( EMG1 < EMG1_avg ) EMG1_out = 0;
		else EMG1_out = abs(EMG1-EMG1_avg);

		if( EMG2 < EMG2_avg ) EMG2_out = 0;
		else EMG2_out = abs(EMG2-EMG2_avg);
	}

	// 1 0
	else if( EMG1_motion_flag == 1 && EMG2_motion_flag == 0 ){
		if( EMG1 < EMG1_motion_avg ) EMG1_out = 0;
		else EMG1_out = abs(EMG1-EMG1_motion_avg);

		if( EMG2 < EMG2_avg ) EMG2_out = 0;
		else EMG2_out = abs(EMG2-EMG2_avg);
	}

	// 0 1
	else if( EMG1_motion_flag == 0 && EMG2_motion_flag == 1 ){
		if( EMG1 < EMG1_avg ) EMG1_out = 0;
		else EMG1_out = abs(EMG1-EMG1_avg);

		if( EMG2 < EMG2_motion_avg ) EMG2_out = 0;
		else EMG2_out = abs(EMG2-EMG2_motion_avg);
	}

	// 1 1
	else if( EMG1_motion_flag == 1 && EMG2_motion_flag == 1 ){
		if( EMG1 < EMG1_motion_avg ) EMG1_out = 0;
		else EMG1_out = abs(EMG1-EMG1_motion_avg);

		if( EMG2 < EMG2_motion_avg ) EMG2_out = 0;
		else EMG2_out = abs(EMG2-EMG2_motion_avg);
	}

	/*
	// 0 0 0 0
	if( EMG1_base_flag == 0 && EMG1_motion_flag == 0 && EMG2_base_flag == 0 && EMG2_motion_flag == 0 ){
		EMG1 = new_EMG1[EMG1_index]; EMG2 = new_EMG2[EMG2_index];
	}	
	// 0 0 1 0
	else if( EMG1_base_flag == 0 && EMG1_motion_flag == 0 && EMG2_base_flag == 1 && EMG2_motion_flag == 0 ){
		if( new_EMG2[EMG2_index]-EMG2_avg < 0 ) EMG2 = 0;
		else EMG2 = abs(new_EMG2[EMG2_index]-EMG2_avg);

		EMG1 = new_EMG1[EMG1_index];
	}
	// 0 0 0 1
	else if( EMG1_base_flag == 0 && EMG1_motion_flag == 0 && EMG2_base_flag == 0 && EMG2_motion_flag == 1 ){
		if( new_EMG2[EMG2_index]-EMG2_motion_avg < 0 ) EMG2 = 0;
		else EMG2 = abs(new_EMG2[EMG2_index]-EMG2_motion_avg);

		EMG1 = new_EMG1[EMG1_index];
	}
	// 1 0 0 0
	else if( EMG1_base_flag == 1 && EMG1_motion_flag == 0 && EMG2_base_flag == 0 && EMG2_motion_flag == 0 ){
		if( new_EMG1[EMG1_index]-EMG1_avg < 0 ) EMG1 = 0;
		else EMG1 = abs(new_EMG1[EMG1_index]-EMG1_avg);

		EMG2 = new_EMG2[EMG2_index];
	}
	// 1 0 1 0
	else if( EMG1_base_flag == 1 && EMG1_motion_flag == 0 && EMG2_base_flag == 1 && EMG2_motion_flag == 0 ){
		if( new_EMG1[EMG1_index]-EMG1_avg < 0 ) EMG1 = 0;
		else EMG1 = abs(new_EMG1[EMG1_index]-EMG1_avg);

		if( new_EMG2[EMG2_index]-EMG2_avg < 0 ) EMG2 = 0;
		else EMG2 = abs(new_EMG2[EMG2_index]-EMG2_avg);
	}
	// 1 0 0 1
	else if( EMG1_base_flag == 1 && EMG1_motion_flag == 0 && EMG2_base_flag == 0 && EMG2_motion_flag == 1 ){
		if( new_EMG1[EMG1_index]-EMG1_avg < 0 ) EMG1 = 0;
		else EMG1 = abs(new_EMG1[EMG1_index]-EMG1_avg);
		
		if( new_EMG2[EMG2_index]-EMG2_motion_avg < 0 ) EMG2 = 0;
		else EMG2 = abs(new_EMG2[EMG2_index]-EMG2_motion_avg);
	}
	// 0 1 0 0
	else if( EMG1_base_flag == 0 && EMG1_motion_flag == 1 && EMG2_base_flag == 0 && EMG2_motion_flag == 0 ){
		if( new_EMG1[EMG1_index]-EMG1_motion_avg < 0 ) EMG1 = 0;
		else EMG1 = abs(new_EMG1[EMG1_index]-EMG1_motion_avg);

		EMG2 = new_EMG2[EMG2_index];
	}
	// 0 1 1 0
	else if( EMG1_base_flag == 0 && EMG1_motion_flag == 1 && EMG2_base_flag == 1 && EMG2_motion_flag == 0 ){
		if( new_EMG1[EMG1_index]-EMG1_motion_avg < 0 ) EMG1 = 0;
		else EMG1 = abs(new_EMG1[EMG1_index]-EMG1_motion_avg);
		
		if( new_EMG2[EMG2_index]-EMG2_avg < 0 ) EMG2 = 0;
		else EMG2 = abs(new_EMG2[EMG2_index]-EMG2_avg);
	}
	// 0 1 0 1
	else if( EMG1_base_flag == 0 && EMG1_motion_flag == 1 && EMG2_base_flag == 0 && EMG2_motion_flag == 1 ){
		if( new_EMG1[EMG1_index]-EMG1_motion_avg < 0 ) EMG1 = 0;
		else EMG1 = abs(new_EMG1[EMG1_index]-EMG1_motion_avg);

		if( new_EMG2[EMG2_index]-EMG2_motion_avg < 0 ) EMG2 = 0;
		else EMG2 = abs(new_EMG2[EMG2_index]-EMG2_motion_avg);
	}

	diff_sensor = abs(EMG1-EMG2);*/
	
	diff_sensor = abs(EMG1_out-EMG2_out);

	if( diff_sensor > 40) {
		if( EMG1_out < EMG2_out ) open_control();
		else close_control();
	}

	else{
		if( EMG1_out < _AUX_TH && EMG2_out < _AUX_TH ) stop_control();

		else if( EMG1_out > _PRI_TH && EMG2_out < _AUX_TH )	close_control();
	
		else if( EMG1_out < _AUX_TH && EMG2_out > _PRI_TH )	open_control();
				
		else if( EMG1_out > _PRI_TH && EMG2_out > _PRI_TH )	close_control();
		
		else{
			//NOP;
		}
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