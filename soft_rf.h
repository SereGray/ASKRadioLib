/*
 * soft_rf.h
 *
 *  Created on: 14 апр. 2021 г.
 *      Author: sergey
 * 
 * Specification
 * 
 * Неблокирующий прием и переда данных (преемущественно аппаратно)
 * Входящой сигнал - последовательность количества тиков таймера
 *      считаем что длительность первого сигнала - это длительность "1" (нормальное состояние входа соответствует "0")
 * 
 * Выходной сигнал - ШИМ (изменение значение регистра CCR таймера для посредством DMA)
 * 
 * Условно считаем что входной фильтр таймера полностью справляется с шумом и в принимаемой последовательности только длительности 1 и 0
 * 
 * 
 * преобразование 4 to 6 и обратно необходимо для баланса 1 и 0 в сигнале (пока по таблице ~30% скорости)
 * CRC аппаратное
 * DMA + PWM для передачи
 * DMA + TIM для приема + TIM input filter для отсечения случайных всплесков
 * 
 * Далее:
 * Подобрать параметры фильтра (и возможно ли использовать несколько каналов таймера для лучшей фильтрации ?)
 * 
 * hardware part
 * one TIM for capture signals
 * one TIM for counting the number of captured signals - when the threshold value (MAX_TIMER_BUFFER_LENGTH) is reached, the message search function called (by interrupt) 
 * DMA - to move values from capture/compare reg to timer_buff 
 * 
 * 
 * bit_time - struct for calculate one bit time ( in timer ticks to one bit) 
 * speed_ - array of standart baud rates (bitrate bit/s)
 * symbols - table for decoding 6to4 and 4to6
 * timer_receive_sequence - программный циклический буфер для приема сигнала ( в этот буфер шлет данные таймер ) ( содержит длительности 1 и 0 вместе с шумом )
 * 
 * buffers:
 * timer_buff - raw ticks data - there we search intro(...1010101....) and start_symbol(0x38)
 * data - not decrypted data
 * message - decrypted data 
 * 
 *  План:
 * поиск вступление "забор" 101010...101010 (ищет 2 таймер и функция on_timer_count_interrupt) OK
 * (при срабатывании прерывания по количеству обработанных сообщений - во 2 таймере меняю буфер у первого, что бы он продолжил сохранение данных) NO
 * поиск начала сигнала - поиск во входящей последовательности двух стартовых последовательностей 0х38 ( в функции on_timer_count_interrupt ) OK
 * если начало найдено расшифровка с начала ( пока расшифровка всей последовательности) OK
 * получение длины сообщения (в функции on_timer_count_interrupt ) OK
 * прием начало OK
   //TODO: Проблема чтения конца сообщения когда 2ой таймер не отсчитает до конца и сообщение зависнет в ожидании заполнения 2ого таймера
 * прием конец OK
 * прием CRC -- пока оставим
 * расшифровка 
 * 
 * 
 *      Преобразование входного сигнала в сообщение:
 *          1) перевод количество тиков таймера в 1 и 0
 *          2) перевод 6to4 
 *          3) соединение 4 + 4  или другие
 *          5) проверка CRC
 * 
 *      Преобразование сообщения в данные:
 *          1) расчет CRC
 *          2) разбиение на 4 бита
 *          3) перевод 4to6
 *          4) перевод в длительности ШИМ 
 *
 * 
 */

#ifndef INC_SOFT_RF_H_
#define INC_SOFT_RF_H_

#include<math.h>
#include<inttypes.h>
#include<malloc.h>

#define BIT_PER_SECOND_TRANSFER_SPEED 4 // speed rate in bts speed_[] bellow

// uncomment this if use radio receiver module
//#define RECEIVER 

 //const uint16_t MAX_TIMER_BUFFER_LENGTH = 24; // 12 бит Х 2
#define MAX_TIMER_BUFFER_LENGTH 24
//const uint8_t max_data_length_ = 27; // 27 byte data + 1 byte data lenght + 2 byte CRC
#define MAX_DATA_LENGTH 27

#define TIMER_CLOCK_FREQ 72000000;

typedef struct
{
     double bit_time_; // длительность 1сек(10^9 мкс) бита 1/скорость связи
     double one_timer_tick_time_ ; // длительность 1сек(10^9 мкс) тика 1/частота
     uint32_t TIM_ticks_per_bit_;
     uint8_t delta_timer_ticks_per_bit_ ;  // допуск на длину бита +- 1/50 длительности бита
     uint16_t TIM_ticks_per_bit_min_;
     uint16_t TIM_ticks_per_bit_max_;
     uint16_t start_bit_ticks_;
     uint16_t start_bit_ticks_min_;
     uint16_t start_bit_ticks_max_;
 }bit_time;

typedef struct
{
    // циклический буффер значений таймера
    uint16_t TIM_ticks_sequence_[MAX_TIMER_BUFFER_LENGTH]; //{0,} // принимаемая последовательность
// (не переведенная) (304 бита (19*16бит) округл до 20)
    uint16_t sequence_iterator_; // 0 TODO: нужно что бы при приеме данных этот итератор увеличивался
}timer_receive_sequence; 


typedef struct
{
    uint8_t data_length_; // = 0; 
    uint16_t data_iterator_;//  0; // побитовый итератор, номера битов: 7`6`5`4`3`2`1`0  15`14`13`12`11`10`9`8   23`22`21 ...
    uint8_t data_[MAX_DATA_LENGTH];//{ 0, };
    uint8_t CRC_message_[2]; // { 0, };
} data_full_msg;

extern const uint32_t bitrate_[]; // bitrate

extern const uint8_t start_symbol; // 0x38 0b111000  12bit symbol <--- 2x

static const uint8_t symbols[] =    // спизжено с virtualwire - данная таблица переводит 4 бита в 6, при этом у 6 бит равное отношение "1" и "0" т.е. 3 "1" и 3 "0"
{
    0xd,  0xe,  0x13, 0x15, 0x16, 0x19, 0x1a, 0x1c, // 001101, 001110, 010011,
    0x23, 0x25, 0x26, 0x29, 0x2a, 0x2c, 0x32, 0x34
};

// оцифровывает добавляет сигнал в виде двух длительностей
//  ( длительность выского уровня и длительность низкого уровня) к
// последовательности данных итератор указывает на позицию вставки следующего значения

//PUBLIC FUNCTIONS
// uint8_t may cast to any type
void send(uint8_t * msg);

#ifdef RECEIVER
    void on_receive(uint8_t * msg, uint16_t len); // after receive MUST BE IMPLEMENTED BY USER
#endif // RECEIVER

void init_rf(); // init radio 

// PRIVATE FUNCTIONS ( will be send to .c file)
bit_time init_timings_(uint32_t changed_bitrate, uint32_t timer_freq);
data_full_msg init_data_struct();

void On_timer_count_interrupt(); 
void Add_converted_signal_to_data(uint16_t *buffer, timer_receive_sequence *sequence, data_full_msg* data); // buffer - place where are dma send data_, sequence -... this function
uint8_t Add_signal_to_sequence(bit_time* bt, uint16_t* buffer, timer_receive_sequence* sequence, data_full_msg* data);


uint8_t Convert_6to4(uint8_t data_6bit_in); // return converted 4 bit , just "inline" it
uint8_t Convert_4to6(uint8_t data_4bit_in); // return converted 6 bit ,

#endif /* INC_SOFT_RF_H_ */
