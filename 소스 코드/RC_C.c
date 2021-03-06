#include <mega128.h>
#include <mega128.h>
#include <delay.h>
#include <stdio.h>

unsigned int dis_wall;   //echo 신호 도착 시간
int distance;   //계산된 거리
unsigned char rx_data;  //블루투스 데이터 저장


// External Interrupt 4 service routine
interrupt [EXT_INT4] void ext_int4_isr(void)
{
    TCCR0=0x00; //인터럽트가 발생하면 타이머0을 정지
    distance=dis_wall*0.017;    //0.034  
    distance=distance-20;       //거리 보정
    dis_wall=0; //다음 신호를 위한 초기화
}

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    dis_wall++;     //1us마다 카운터 증가
    TCNT0=256-2;   //1us

}

void main(void)
{
DDRE=0x80;
PORTE=0x7F;
PORTB=0x00;
DDRB=0xFF;
// Clock value: 2000.000 kHz Mode: Normal top=0xFF Timer Period: 0.128 ms
ASSR=0<<AS0;
TCCR0=0x00;
TCNT0=256-2;  //1us 마다 타이머 0 인터럽트가 발생
OCR0=0x00;
// Clock value: 16000.000 kHz Mode: Ph. correct PWM top=0x00FF Timer Period: 0.031875 ms
TCCR1A=(1<<COM1A1) | (0<<COM1A0) | (1<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (0<<WGM11) | (1<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (1<<TOIE0);
ETIMSK=(0<<TICIE3) | (0<<OCIE3A) | (0<<OCIE3B) | (0<<TOIE3) | (0<<OCIE3C) | (0<<OCIE1C);
EICRA=(0<<ISC31) | (0<<ISC30) | (0<<ISC21) | (0<<ISC20) | (0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
EICRB=(0<<ISC71) | (0<<ISC70) | (0<<ISC61) | (0<<ISC60) | (0<<ISC51) | (0<<ISC50) | (1<<ISC41) | (0<<ISC40);
EIMSK=(0<<INT7) | (0<<INT6) | (0<<INT5) | (1<<INT4) | (0<<INT3) | (0<<INT2) | (0<<INT1) | (0<<INT0);
EIFR=(0<<INTF7) | (0<<INTF6) | (0<<INTF5) | (1<<INTF4) | (0<<INTF3) | (0<<INTF2) | (0<<INTF1) | (0<<INTF0);
UCSR0A=(0<<RXC0) | (0<<TXC0) | (0<<UDRE0) | (0<<FE0) | (0<<DOR0) | (0<<UPE0) | (0<<U2X0) | (0<<MPCM0);
UCSR0B=(0<<RXCIE0) | (0<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
UCSR0C=(0<<UMSEL0) | (0<<UPM01) | (0<<UPM00) | (0<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
UBRR0H=0x00;
UBRR0L=0x67;

#asm("sei")

while (1)
      {
        rx_data=getchar();  //블루투스 모듈은 통해 받은 값 저장
        
        PORTE.7=0x01;      //트리거 펄스를 10us동안 보냄
        TCCR0=0x02;        //티이머 카운터를 시작시킴
        delay_us(10);      
        PORTE.7=0x00;
        delay_ms(1);       //1ms에 한번 측정
          
        if(distance>0&&distance<10)    //초음파 센서사 장애물 감지
        {                             //정지
            PORTB.0=0;
            PORTB.1=1;
            PORTB.2=1;
            PORTB.3=0;
                
            OCR1AL=0x8F;
            OCR1BL=0x8F;     
        }   
        else
        {
          switch(rx_data)
          {
            case 'S':               //정지
                
                PORTB.0=0;
                PORTB.1=0;
                PORTB.2=0;
                PORTB.3=0;
                
                OCR1AL=0x00;
                OCR1BL=0x00;
            break;
            
            case 'G':               //직진
               
                PORTB.0=1;
                PORTB.1=0;
                PORTB.2=0;
                PORTB.3=1;
                
                OCR1AL=0xFF;
                OCR1BL=0xFF;
            break;
            
            case 'L':               //좌회전
               
                PORTB.0=1;
                PORTB.1=0;
                PORTB.2=0;
                PORTB.3=1;
                
                OCR1AL=0xFF;
                OCR1BL=0x00;
                
            break;
            
            case 'R':               //우회전
                
                PORTB.0=1;
                PORTB.1=0;
                PORTB.2=0;
                PORTB.3=1;
                
                OCR1AL=0x00;
                OCR1BL=0xFF;

            break;
            
            case 'B':                //후진
                
                PORTB.0=0;
                PORTB.1=1;
                PORTB.2=1;
                PORTB.3=0;
                
                OCR1AL=0xFF;
                OCR1BL=0xFF;
                
            break;
            
            case 'l':               //후진중 좌회전
                
                
                PORTB.0=0;
                PORTB.1=1;
                PORTB.2=1;
                PORTB.3=0;
                
                OCR1AL=0x00;
                OCR1BL=0xFF;

            break;
            
            case 'r':            //후진중 우회전
                
                PORTB.0=0;
                PORTB.1=1;
                PORTB.2=1;
                PORTB.3=0;
                
                OCR1AL=0xFF;
                OCR1BL=0x00;

            break;   
          }
        }
      }
}