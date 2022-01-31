#ifndef DATA_HANDLER
#define DATA_HANDLER

#include<inttypes.h>

#include"soft_rf.h"

#define BIT_1 1
#define BIT_0 0
#define START_READ_DATA 1
#define RESUME_READ_DATA 0


void add_bits_to_buffer(uint8_t bit, uint8_t* count, uint16_t* buff, uint8_t* iterator);
uint8_t bit_counter(bit_time* bt, timer_receive_sequence* tim_seq, uint16_t index);
converted_sequence convert_timer_sequence(bit_time* bt, timer_receive_sequence* timers_sequence, uint16_t* length, uint16_t* data_iterator);
void convert_data_to_TIM_sequence(uint8_t* send_buffer, uint8_t* data, uint8_t* data_length, uint16_t* data_iterator);
uint8_t convert_6to4(uint8_t data_6bit_in); // return converted 4 bit , just "inline" it
uint8_t convert_4to6(uint8_t data_4bit_in); // return converted 6 bit ,
converted_sequence Init_converted_sequence(uint8_t len);
void read_data_from_buffer(data_full_msg* message, timer_receive_sequence* local_buffer, uint8_t first_reading);
void remove_second_start_sequence(timer_receive_sequence* local_buffer);

#endif // !DATA_HANDLER
