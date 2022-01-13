/**
@mainpage 도트 매트릭스 테트리스
@section Information 정보
@author
- 30th 류도현
- 32nd 최승진
- 33rd 박건희
@version 1.0
@date 2017.02.12

@section FILE  파일 목록
@ref dot_matrix.h \n
@ref dot_matrix.c \n
@ref tetris.c
*/

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
#include <stdlib.h>
#include "dot_matrix.h"

unsigned char _timer_flag;
unsigned char count, pad_count;
dot_matrix * dP;


#define LEFT 75 //좌로 이동    //키보드값들 
#define RIGHT 77 //우로 이동 
#define UP 72 //회전 
#define DOWN 80 //soft drop


#define PAD_SENSITIVITY		30
#define PAD_X				 0
#define PAD_Y				 1
/** @def PAD_SENSITIVITY
센서 민감도
*/
/** @def PAD_X
패드 X축 핀번호
*/
/** @def PAD_Y
패드 Y축 핀번호
*/

unsigned short tetriminos[7][4] =
{
   { 0x6600, 0x6600, 0x6600, 0x6600 },    ///< O
   { 0xF000, 0x4444, 0xF000, 0x4444 },    ///< I
   { 0x6C00, 0x8C40, 0x6C00, 0x8C40 },    ///< S
   { 0xC600, 0x4C80, 0xC600, 0x4C80 },    ///< Z
   { 0xE400, 0x8C80, 0x4E00, 0x4C40 },    ///< T
   { 0x2E00, 0xC440, 0xE800, 0x88C0 },    ///< L
   { 0x8E00, 0x44C0, 0xE200, 0xC880 },    ///< J

};

unsigned char shape;                  ///< 테트리미노스의 7가지 모양
unsigned char pattern;                 ///< 테트리미노스의 4가지 패턴
unsigned char cur_line;               ///< 테트리니노스의 현재 라인
unsigned char cur_col;                ///< 테트리니노스의 현재 칸 
unsigned short temp_line[4];
unsigned short game_board[17];
unsigned char red[16];
unsigned char green[16];

unsigned char level = 1;
unsigned char Collision_flag = 1;

unsigned int adc[2];               ///<adc 변환값 저장하는 배열
unsigned char mux = 0;             ///<adc 포트
unsigned char delay = 250;
unsigned char remove_count = 0;
unsigned char end_flag = 0;

/**충돌 판단*/
signed char Collision(void)
{
   if (((game_board[cur_line] & temp_line[0]) != 0) | ((game_board[cur_line + 1] & temp_line[1]) != 0) |
      ((game_board[cur_line + 2] & temp_line[2]) != 0) | ((game_board[cur_line + 3] & temp_line[3]) != 0))
      return -1;         // 충돌
   else
      return 0;  // 충돌 없음
}

void drop()
{
   cur_line++;

   if (Collision() == -1)
   {
      cur_line--;
      game_board[cur_line] = game_board[cur_line] | temp_line[0];
      game_board[cur_line + 1] = game_board[cur_line + 1] | temp_line[1];
      game_board[cur_line + 2] = game_board[cur_line + 2] | temp_line[2];
      game_board[cur_line + 3] = game_board[cur_line + 3] | temp_line[3];

      Collision_flag = 1;
   }
}

void draw()
{
   int i;
   for (i = 0; i < 16; i++)
   {
      red[i] = 0x00;
      green[i] = game_board[i] >> 4;
   }

   red[cur_line] = red[cur_line] | (temp_line[0] >> 4);
   red[cur_line + 1] = red[cur_line + 1] | (temp_line[1] >> 4);
   red[cur_line + 2] = red[cur_line + 2] | (temp_line[2] >> 4);
   red[cur_line + 3] = red[cur_line + 3] | (temp_line[3] >> 4);
}


void NewTetriminos(void)
{

   shape = rand() % 7;
   pattern = 0;

   temp_line[0] = (tetriminos[shape][pattern] & 0xF000) >> 6;
   temp_line[1] = (tetriminos[shape][pattern] & 0x0F00) >> 2;
   temp_line[2] = (tetriminos[shape][pattern] & 0x00F0) << 2;
   temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << 6;

   cur_line = 0;                 // 테트리미노스 현재 라인 (최상위 라인)
   cur_col = 9;
   Collision_flag = 0;

   if (Collision() == -1)
      end_flag = 1;
}

void spin()
{
   pattern++;
   if (pattern == 4)
      pattern = 0;

   temp_line[0] = ((tetriminos[shape][pattern] & 0xF000) >> 12) << (cur_col - 3);
   temp_line[1] = ((tetriminos[shape][pattern] & 0x0F00) >> 8) << (cur_col - 3);
   temp_line[2] = ((tetriminos[shape][pattern] & 0x00F0) >> 4) << (cur_col - 3);
   temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << (cur_col - 3);

   if (Collision() == -1)
   {
      pattern--;
      temp_line[0] = ((tetriminos[shape][pattern] & 0xF000) >> 12) << (cur_col - 3);
      temp_line[1] = ((tetriminos[shape][pattern] & 0x0F00) >> 8) << (cur_col - 3);
      temp_line[2] = ((tetriminos[shape][pattern] & 0x00F0) >> 4) << (cur_col - 3);
      temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << (cur_col - 3);
   }
}


void move(int n)
{
   switch (n) {
   case 0:
      break;
   case LEFT: //왼쪽키 눌렀을때 

      cur_col++;
      temp_line[0] = ((tetriminos[shape][pattern] & 0xF000) >> 12) << (cur_col - 3);
      temp_line[1] = ((tetriminos[shape][pattern] & 0x0F00) >> 8) << (cur_col - 3);
      temp_line[2] = ((tetriminos[shape][pattern] & 0x00F0) >> 4) << (cur_col - 3);
      temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << (cur_col - 3);

      if (Collision() == -1)
      {
         cur_col--;
         temp_line[0] = ((tetriminos[shape][pattern] & 0xF000) >> 12) << (cur_col - 3);
         temp_line[1] = ((tetriminos[shape][pattern] & 0x0F00) >> 8) << (cur_col - 3);
         temp_line[2] = ((tetriminos[shape][pattern] & 0x00F0) >> 4) << (cur_col - 3);
         temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << (cur_col - 3);
      }

      break;                     //왼쪽으로 갈 수 있는지 체크 후 가능하면 이동
   case RIGHT: //오른쪽 방향키 눌렀을때- 위와 동일하게 처리됨 
      cur_col--;
      temp_line[0] = ((tetriminos[shape][pattern] & 0xF000) >> 12) << (cur_col - 3);
      temp_line[1] = ((tetriminos[shape][pattern] & 0x0F00) >> 8) << (cur_col - 3);
      temp_line[2] = ((tetriminos[shape][pattern] & 0x00F0) >> 4) << (cur_col - 3);
      temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << (cur_col - 3);

      if (Collision() == -1)
      {
         cur_col++;
         temp_line[0] = ((tetriminos[shape][pattern] & 0xF000) >> 12) << (cur_col - 3);
         temp_line[1] = ((tetriminos[shape][pattern] & 0x0F00) >> 8) << (cur_col - 3);
         temp_line[2] = ((tetriminos[shape][pattern] & 0x00F0) >> 4) << (cur_col - 3);
         temp_line[3] = (tetriminos[shape][pattern] & 0x000F) << (cur_col - 3);
      }
      break;
   case DOWN: //아래쪽 방향키 눌렀을때-위와 동일하게 처리됨 
            //      delay = 0;
      break;
   case UP: //위쪽 방향키 눌렀을때 
      spin();
      break;
      //회전할 수 있는지 체크 후 가능하면 회전
   }

}

void remove()
{
   int i, j = 0;
   for (i = 0; i < 16; i++)
   {
      if (game_board[i] == 0x7FFE)
      {
         remove_count++;
         for (j = i; j > 0; j--)
         {
            game_board[j] = game_board[j - 1];
            
         }
      }
   }
   if (remove_count == 5){
      level++;
	  speed(level);
	  remove_count = 0;}
}

void speed(unsigned char level)
{
   switch (level)
   {
   case 1:
      delay = 250;
      break;
   case 2:
      delay = 200;
      break;
   case 3:
      delay = 150;
      break;
   case 4:
      delay = 100;
      break;
   case 5:
      delay = 50;
      break;
   case 6:
      end_flag = 1;
   }
}

int main()
{
   DDRB = 0xff;
   DDRC = 0xff;
   PORTB = 0x00;
   PORTC = 0x00;
   int i = 0;

   for (i = 0; i < 20; i++)
      game_board[i] = 0x700E;   // 0111000000001110

   game_board[16] = 0xFFFF;
   dot_matrix dot_show;			//도트 매트릭스 구조체 선언

   dot_show.row = Dot_portSet(&PORTB, 0);		//행 포트, 시작 핀번호 설정
   dot_show.col = Dot_portSet(&PORTC, 0);		//열 포트, 시작 핀번호 설정
   dot_show.greenPtr.all = green;				//초록색 출력할 배열 데이터 설정
   dot_show.redPtr.all = red;					//빨간색 출력할 배열 데이터 설정
   Dot_init(&dot_show);							//도트매트릭스 초기화

	//인터럽트 셋팅
   EIMSK = 0x03;
   EICRA = 0b0000001010;

	//ADC(Pad) 포트 초기화
   ADMUX = 0x40;
   ADCSRA = 0b11101111;

	//타이머 초기화
   TIMSK = 1;
   TCCR0 = 6;
   TCNT0 = 6;
   SREG = 0x80;

	//랜덤 시드 생성
   _delay_ms(5);
   srand(ADC);
   while (1)
   {
      if (Collision_flag == 1)
         NewTetriminos();

      if (end_flag == 1)
      {
         Dot_disp(OFF);
		 break;
      }


      draw();
      Dot_disp(ALL);
      remove();
   }
}

ISR(INT0_vect)
{
   spin();
}

ISR(TIMER0_OVF_vect) {
   count++;
   pad_count++;
   TCNT0 = 6;
   if (pad_count == PAD_SENSITIVITY)
   {
      if (adc[0]>500)	//왼쪽
      {
         move(LEFT);
      }
      else if (adc[0]<300)	//오른쪽
      {
         move(RIGHT);
      }
      if (adc[1]<300)		//아래
         delay = 30;
      else
         speed(level);
      pad_count = 0;
   }
   if (count == delay) {
      drop();
      count = 0;
   }
}


ISR(ADC_vect)
{
   if (mux == PAD_X)
   {
      adc[PAD_X] = ADC;
      ADMUX = 0x40 | PAD_Y;
      mux = PAD_Y;
   }
   else
   {
      adc[PAD_Y] = ADC;
      ADMUX = 0x40 | PAD_X;
      mux = PAD_X;
   }

}
