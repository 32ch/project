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



/**�ʱ�ȭ �Լ�.
- Example
@code
   dot_matrix dot_show;			//��Ʈ ��Ʈ���� ����ü ����
//#1
   dot_show.row = Dot_portSet(&PORTB, 0);		//�� ��Ʈ, ���� �ɹ�ȣ ����
   dot_show.col = Dot_portSet(&PORTC, 0);		//�� ��Ʈ, ���� �ɹ�ȣ ����

//#2
	dot_show.row.port = &PORTB;
	dot_show.row.portNum = 0;
	dot_show.col.port = &PORTC;
	dot_show.col.portNum = 0;

   dot_show.greenPtr.all = green;				//�ʷϻ� ����� �迭 ������ ����
   dot_show.redPtr.all = red;					//������ ����� �迭 ������ ����
   Dot_init(&dot_show);							//��Ʈ��Ʈ���� �ʱ�ȭ
@endcode
*/
void Dot_init(dot_matrix * init)
{
	_dotP = init;
}

/**��� �Լ�.
@param state ǥ���� ���� �ɼ� ����
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
	
/**������ �Լ�, ���� �ϳ�, ���ڹ迭 �� �޾ƿ�.
@return ����� �迭�� xy��ǥ�� ��Ʈ��
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

/**��Ʈ ���� �Լ�.
@param port ��Ʈ 
@param pNum ù��° �� ����

- Example : 
@code
   dot_matrix dot_show;			//��Ʈ ��Ʈ���� ����ü ����
   dot_show.row = Dot_portSet(&PORTB, 0);		//�� ��Ʈ, ���� �ɹ�ȣ ����
   dot_show.col = Dot_portSet(&PORTC, 0);		//�� ��Ʈ, ���� �ɹ�ȣ ����
@endcode
*/
dot_port Dot_portSet(uint8_t * port, unsigned char pNum)
{
	dot_port dp;
	dp.port = port;
	dp.portNum = pNum;
	return dp;
}
