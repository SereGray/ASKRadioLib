#ifndef INTERNAL_FUNCTIONS
#define INTERNAL_FUNCTIONS

#include<inttypes.h>


#define BIT_1 1
#define BIT_0 0
#define START_READ_DATA 1
#define RESUME_READ_DATA 0

typedef struct
{
    double bit_time_; // длительность 1сек(10^9 мкс) бита 1/скорость связи
    double one_timer_tick_time_; // длительность 1сек(10^9 мкс) тика 1/частота
    uint32_t TIM_ticks_per_bit_;
    uint8_t delta_timer_ticks_per_bit_;  // допуск на длину бита +- 1/50 длительности бита
    uint16_t TIM_ticks_per_bit_min_;
    uint16_t TIM_ticks_per_bit_max_;
    uint16_t start_bit_ticks_;
    uint16_t start_bit_ticks_min_;
    uint16_t start_bit_ticks_max_;
}bit_time;

typedef struct
{
    // циклический буффер значений таймера
    uint16_t* TIM_ticks_sequence_; //{0,} // принимаемая последовательность
    uint16_t sequence_length_;
    // (не переведенная) (304 бита (19*16бит) округл до 20)
    uint16_t sequence_iterator_; // 0 TODO: нужно что бы при приеме данных этот итератор увеличивался
}timer_receive_sequence;


typedef struct
{
    uint8_t data_length_; // = 0; 
    uint16_t data_iterator_;//  0; // побитовый итератор, номера битов: 7`6`5`4`3`2`1`0  15`14`13`12`11`10`9`8   23`22`21 ...
    uint8_t* data_;//{ 0, };
    uint8_t CRC_message_[2]; // { 0, };
} data_full_msg;

typedef struct {
    uint8_t* sequence_;
    uint8_t words_;
    uint8_t length_;
}converted_sequence;


static const uint8_t symbols[] =    // спизжено с virtualwire - данная таблица переводит 4 бита в 6, при этом у 6 бит равное отношение "1" и "0" т.е. 3 "1" и 3 "0"
{
    0xd,  0xe,  0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c, // 001101, 001110, 010011,
    0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34
};

extern const uint8_t start_symbol; // 0x38 0b111000  12bit symbol <--- 2x
static uint8_t started = 0;
static uint8_t starts_from_high_lvl_bit = 0;
extern const uint32_t bitrate_[]; // bitrate

uint8_t max_timer_buffer_length;
uint8_t max_data_length;

bit_time bt; // timings



void add_bits_to_buffer(uint8_t bit, uint8_t *count, uint16_t *buff, uint8_t *iterator);
uint8_t bit_counter(bit_time* bt, timer_receive_sequence* tim_seq, uint16_t index);
converted_sequence convert_timer_sequence(bit_time* bt, timer_receive_sequence* timers_sequence, uint16_t* length, uint16_t* data_iterator);
void convert_data_to_TIM_sequence(uint8_t* send_buffer, uint8_t* data, uint8_t* data_length, uint16_t* data_iterator);
uint8_t convert_6to4(uint8_t data_6bit_in); // return converted 4 bit , just "inline" it
uint8_t convert_4to6(uint8_t data_4bit_in); // return converted 6 bit ,
bit_time init_timings_(uint32_t changed_bitrate, uint32_t timer_freq);
void read_data_from_buffer(data_full_msg* message, timer_receive_sequence* local_buffer, uint8_t first_reading);
void remove_second_start_sequence(timer_receive_sequence* local_buffer);
converted_sequence init_converted_sequence(uint8_t len);
data_full_msg* init_data_struct(uint8_t len);


#endif // !INTERNAL_FUNCTIONS
