/**
@file dot_matrix.c
@author	Seung-Jin Choi(ROLAB 32nd, Kwangwoon University)
@date	2017.02.12
@version 	1.0.0
*/

#include <avr/io.h>
#include <util/delay.h>
#include "dot_matrix.h"



void Dot_init(dot_matrix * init);
void Dot_disp(disp_state state);
unsigned char Dot_data(unsigned char y, unsigned char x);
//void Dot_clear(void);
dot_port Dot_portSet(uint8_t * port, unsigned char pNum);



/**초기화 함수.
- Example
@code
   dot_matrix dot_show;			//도트 매트릭스 구조체 선언
//#1
   dot_show.row = Dot_portSet(&PORTB, 0);		//행 포트, 시작 핀번호 설정
   dot_show.col = Dot_portSet(&PORTC, 0);		//열 포트, 시작 핀번호 설정

//#2
	dot_show.row.port = &PORTB;
	dot_show.row.portNum = 0;
	dot_show.col.port = &PORTC;
	dot_show.col.portNum = 0;

   dot_show.greenPtr.all = green;				//초록색 출력할 배열 데이터 설정
   dot_show.redPtr.all = red;					//빨간색 출력할 배열 데이터 설정
   Dot_init(&dot_show);							//도트매트릭스 초기화
@endcode
*/
void Dot_init(dot_matrix * init)
{
	_dotP = init;
}

/**출력 함수.
@param state 표시할 색깔 옵션 선택
@see disp_state
*/
void Dot_disp(disp_state state)
{
	unsigned char i,j;
	for(i=0;i<DOT_COL_SIZE;i++)
	{
		for(j=0;j<DOT_ROW_SIZE;j++)
		{
			*(_dotP->row.port) = ((Dot_data(i,j)&state)<<(_dotP->row.portNum));
			SCK_PULSE
		}
		_delay_ms(5);
		RCK_PULSE
		*(_dotP->col.port) = (i<<(_dotP->col.portNum));
	}
}
	
/**데이터 함수, 숫자 하나, 숫자배열 값 받아옴.
@return 출력할 배열의 xy좌표의 비트값
*/
unsigned char Dot_data(unsigned char y, unsigned char x)
{
	return (( *((_dotP->greenPtr).all+y) >> x) & 0x01 ) | (( (*((_dotP->redPtr).all+y) >> x) & 0x01 ) << 1);
}

/*
void Dot_Clear(void)
{
	int i;
	for(i=0;i<8;i++)
	{
		__DOT_PORT_ROW__ = 0x00;
		SCK_PULSE;
	}
}
*/

/**포트 설정 함수.
@param port 포트 
@param pNum 첫번째 핀 숫자

- Example : 
@code
   dot_matrix dot_show;			//도트 매트릭스 구조체 선언
   dot_show.row = Dot_portSet(&PORTB, 0);		//행 포트, 시작 핀번호 설정
   dot_show.col = Dot_portSet(&PORTC, 0);		//열 포트, 시작 핀번호 설정
@endcode
*/
dot_port Dot_portSet(uint8_t * port, unsigned char pNum)
{
	dot_port dp;
	dp.port = port;
	dp.portNum = pNum;
	return dp;
}
