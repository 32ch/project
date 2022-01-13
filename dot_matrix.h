//dot_matrix.h

/*
�� ���� : 	|****|			|****|		|**|
			74hc238			74hc595		Vcc,GND

238 : �ڸ��� ǥ��. 2^(0,	1,	2,	3)
595 : SER_G,	SER_R,		SH_CP(shift)	ST_CP(ratch)
*/

/**
@brif
@file dot_matrix.h
@author	Seung-Jin Choi(ROLAB 32nd, Kwangwoon University)
@date	2017.02.12
@version 	1.0
*/

#ifndef	__DOT_MATRIX_H__
#define __DOT_MATRIX_H__

#include<avr/io.h>
#include<util/delay.h>

#define bit_set(reg,bit) (reg) |=  (1 << (bit))
#define bit_clr(reg,bit) (reg) &= ~(1 << (bit))

#define SCK_PULSE {bit_set(*(_dotP->row.port),(_dotP->row.portNum+2)); \
					bit_clr(*(_dotP->row.port),(_dotP->row.portNum+2));}
#define RCK_PULSE {bit_set(*(_dotP->row.port),(_dotP->row.portNum+3)); \
					bit_clr(*(_dotP->row.port),(_dotP->row.portNum+3));}

#ifndef DOT_ROW_SIZE
#define DOT_ROW_SIZE	8
#endif

#ifndef DOT_COL_SIZE
#define DOT_COL_SIZE	16
#endif


typedef struct _dot_port
{
	unsigned char portNum;
	volatile uint8_t * port;
}dot_port;

typedef union _matrix_pointer{
#if DOT_ROW_SIZE <=8
	uint8_t * all;
	uint8_t (*updown)[8];
#elif DOT_ROW_SIZE <=16
	uint16_t * all;
	uint16_t (*updown)[8];
#elif DOT_ROW_SIZE <=32
	uint32_t * all;
	uint32_t (*updown)[8];
#elif DOT_ROW_SIZE <=64
	uint64_t * all;
	uint64)t (*updown)[8];
#endif
}matrix_pointer;

/** ��Ʈ ��Ʈ���� ����ü*/
typedef struct _dot_matrix
{
	dot_port row;					/**< �� ����(��Ʈ, ��) */
	dot_port col;					/**< �� ����(��Ʈ, ��) */
	matrix_pointer redPtr;			/**< ���������� ����� �迭�� ����Ű�� ������ */
	matrix_pointer greenPtr;		/**< �ʷϻ����� ����� �迭�� ����Ű�� ������ */
}dot_matrix;

/**
@brif ��� �ɼ�
@todo ���� �ɼ� �߰�
*/
typedef enum _disp_state
{
	OFF=0b000,
	GREEN = 0b001,
	RED = 0b010,
	ALL = 0b011
	//REVERSE = 0b100,
}disp_state;

static dot_matrix * _dotP;

void Dot_init(dot_matrix * init);
void Dot_disp(disp_state state);
unsigned char Dot_data(unsigned char y, unsigned char x);
//void Dot_clear(void);
dot_port Dot_portSet(uint8_t * port, unsigned char pNum);

#endif

/** @def DOT_ROW_SIZE
��Ʈ ��Ʈ���� ��(����) ũ��(bit).
- �⺻�� : 8.
- �����Ϸ��� #define ���� �����ϸ� ��.
- Ex : 16���� �ٲٷ��� -> #define DOT_ROW_SIZE	16	
*/
/** @def DOT_COL_SIZE
��Ʈ ��Ʈ���� ��(����) ũ��(bit).
- �⺻�� : 16.
- ���� ����� DOT_ROW_SIZE�� ����.
*/

/** @typedef matrix_pointer
@brif ��� �迭 ������
*/
