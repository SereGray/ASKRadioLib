#ifndef INTERNAL_FUNCTIONS
#define INTERNAL_FUNCTIONS

#include<inttypes.h>
#include<math.h>
#include<stdlib.h>


#define BIT_1 1
#define BIT_0 0
#define START_READ_DATA 1
#define RESUME_READ_DATA 0

typedef struct
{
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
}TIM_sequence;


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
    uint8_t word_buffer_;  // buffer padding by 4 bits, (each 6 bits convert to 4 bits)
    uint8_t halfword_iterator_; 	 // iterator need for checking 4 bits overflow 
}converted_sequence;

typedef struct {
    // TODO: проверить рузультат 0%2 и проверить логику start_end_lvl_
    uint8_t start_end_lvl_; //  =1 if start high, =2 if end high, =3 if start & end hight, 0 if start & end low
    uint8_t length; // number of last element + 1
    //TODO: maybe uint16_t data ?
    uint16_t data[8]; //TIM ticks count micro sequences
}symbol_bit_sequence;

static const uint8_t symbols[] =    // спизжено с virtualwire - данная таблица переводит 4 бита в 6, при этом у 6 бит равное отношение "1" и "0" т.е. 3 "1" и 3 "0"
{
    0x0d,  0x0e,  0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c, // 001101, 001110, 010011, 010101, 010110, 011001, 011010, 011100, 
    0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34    // 100011, 100101, 100110, 101001, 101010, 101100, 110010, 110100
};

extern symbol_bit_sequence sym_to_TIM[];
extern const uint8_t start_symbol; // 0x38 0b111000  12bit symbol <--- 2x
uint8_t started;
uint8_t transmiting;
uint8_t starts_from_high_lvl_bit ; // the starts_from_high_lvl_bit indicates which bit the sequence starts from hight lvl
extern const uint32_t bitrate_[]; // bitrate

uint8_t max_timer_buffer_length;
uint8_t max_data_length;

bit_time bt; // timings



void add_bits_to_buffer(uint8_t bit, uint8_t *count, uint32_t *buff, uint8_t *iterator); //ok
uint8_t bit_counter(bit_time* bt, TIM_sequence* tim_seq, uint16_t index); // ok
void convert_from_buffer(uint32_t *buffer_, uint8_t *buffer_iterator_, converted_sequence* res, uint16_t* data_length, uint16_t* data_iterator); // ok
converted_sequence* convert_timer_sequence(bit_time* bt, TIM_sequence* timers_sequence, uint16_t* length, uint16_t* data_iterator); // TODO
TIM_sequence* convert_data_to_TIM_sequence(uint8_t* data, uint8_t* data_length, uint8_t* data_iterator, bit_time* bt);   //-------------- ok
uint8_t convert_6to4(uint8_t data_6bit_in); // return converted 4 bit , just "inline" it  // ok
uint8_t convert_4to6(uint8_t data_4bit_in); // return converted 6 bit ,  // ok
void copy_intro_start_seq_to_transmit_buffer(uint16_t *output_timer_buffer, bit_time *bt);  //------------------------
void copy_to_transmit_buffer(uint16_t* output_timer_buffer,uint8_t offset,TIM_sequence *message_TIM_seq); //------------
bit_time init_timings_(uint32_t changed_bitrate, uint32_t timer_freq);  // ok
uint16_t get_length_of_TIM_sequence(uint8_t *numbers_of_symbols_arr, uint32_t len); // ok
void read_data_from_buffer(bit_time* bt, data_full_msg* message, TIM_sequence* local_buffer, uint8_t first_reading);   // --------------
void remove_second_start_sequence(bit_time* bt, TIM_sequence* local_buffer);  // ok
void init_symbols_to_TIM_sequence(symbol_bit_sequence* sym_bit_seq, uint8_t length, bit_time* bt);   // ---------- ok
converted_sequence* init_converted_sequence(uint8_t len); // ok
void delete_converted_sequence(converted_sequence* seq);  // ok
data_full_msg* init_data_struct(uint8_t len); // ok
void delete_data_struct(data_full_msg* msg); // ok
TIM_sequence* init_receive_sequence(uint8_t len);  //------------ ok
void delete_receive_sequence(TIM_sequence * seq);  //------------- ok


#endif // !INTERNAL_FUNCTIONS
