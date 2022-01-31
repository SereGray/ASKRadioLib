#ifndef CONVERT6TO4
#define CONVERT6TO4

#include<inttypes.h>

#include"soft_rf.h"

extern const uint8_t start_symbol; // 0x38 0b111000  12bit symbol <--- 2x

extern const uint32_t bitrate_[]; // bitrate

typedef struct
{
    double bit_time_; // ������������ 1���(10^9 ���) ���� 1/�������� �����
    double one_timer_tick_time_; // ������������ 1���(10^9 ���) ���� 1/�������
    uint32_t TIM_ticks_per_bit_;
    uint8_t delta_timer_ticks_per_bit_;  // ������ �� ����� ���� +- 1/50 ������������ ����
    uint16_t TIM_ticks_per_bit_min_;
    uint16_t TIM_ticks_per_bit_max_;
    uint16_t start_bit_ticks_;
    uint16_t start_bit_ticks_min_;
    uint16_t start_bit_ticks_max_;
}bit_time;

typedef struct
{
    // ����������� ������ �������� �������
    uint16_t * TIM_ticks_sequence_; //{0,} // ����������� ������������������
    uint16_t sequence_length_;
// (�� ������������) (304 ���� (19*16���) ������ �� 20)
    uint16_t sequence_iterator_; // 0 TODO: ����� ��� �� ��� ������ ������ ���� �������� ������������
}timer_receive_sequence;


typedef struct
{
    uint8_t data_length_; // = 0; 
    uint16_t data_iterator_;//  0; // ��������� ��������, ������ �����: 7`6`5`4`3`2`1`0  15`14`13`12`11`10`9`8   23`22`21 ...
    uint8_t * data_;//{ 0, };
    uint16_t data_length_;
    uint8_t CRC_message_[2]; // { 0, };
} data_full_msg;

typedef struct {
    uint8_t* sequence_;
    uint8_t words_;
    uint8_t length_;
}converted_sequence;


static const uint8_t symbols[] =    // �������� � virtualwire - ������ ������� ��������� 4 ���� � 6, ��� ���� � 6 ��� ������ ��������� "1" � "0" �.�. 3 "1" � 3 "0"
{
    0xd,  0xe,  0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c, // 001101, 001110, 010011,
    0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34
};

#endif