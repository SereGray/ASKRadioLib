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
 * прием-расшифровка начало OK
   //TODO: Проблема чтения конца сообщения когда 2ой таймер не отсчитает до конца и сообщение зависнет в ожидании заполнения 2ого таймера
 * прием-расшифровка конец OK
 * прием CRC -- пока оставим
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

//internals headers
#include"soft_rf_internals.h"

#define BIT_PER_SECOND_TRANSFER_SPEED 4 // speed rate in bts speed_[] bellow

// uncomment this if use radio receiver module
//#define RECEIVER 

 //const uint16_t MAX_TIMER_BUFFER_LENGTH = 24; // 12 бит Х 2
#define MAX_TIMER_BUFFER_LENGTH 24
//const uint8_t max_data_length_ = 27; // 27 byte data + 1 byte data lenght + 2 byte CRC
#define MAX_DATA_LENGTH 27

#define TIMER_CLOCK_FREQ 72000000;


//static uint16_t transmitted_count = 0; not used
// "1" when transmition started 

// the starts_from_bit indicates which bit the sequence starts from

static timer_receive_sequence input_timer_buff_one; // buffer for timer
static timer_receive_sequence input_timer_buff_two; // buffer for timer to write input while input_timer_buff_one is decoding and vice versa

extern data_full_msg* received_message;


// оцифровывает добавляет сигнал в виде двух длительностей
//  ( длительность выского уровня и длительность низкого уровня) к
// последовательности данных итератор указывает на позицию вставки следующего значения

//PUBLIC FUNCTIONS
void on_timer_count_interrupt();
void send_data(uint8_t* data, uint8_t data_length);
//void send(uint8_t * msg);

#ifdef RECEIVER
    void on_receive(uint8_t * msg, uint16_t len); // after receive MUST BE IMPLEMENTED BY USER
#endif // RECEIVER

void init_rf(); // init radio 
// PRIVATE FUNCTIONS ( will be send to .c file)



void set_timer_to_start();

#endif /* INC_SOFT_RF_H_ */
