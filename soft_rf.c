#include"soft_rf.h"

const uint32_t speed_[] = { 9600, 19200, 38400, 57600, 76800, 115200 };

const uint8_t start_symbol = 0x38;

bit_time init_timings_(uint32_t changed_bitrate, uint32_t timer_freq)
{
	bit_time bt;
	// длительность 1сек(10^9 мкс) бита 1/скорость связи
	bt.bit_time_ = 1000000000.0 / changed_bitrate;
	// длительность 1сек(10^9 мкс) тика 1/частота
	bt.one_timer_tick_time_ = 1000000000.0 / timer_freq;
	bt.TIM_ticks_per_bit_ = round(bt.bit_time_ / \
		bt.one_timer_tick_time_);
	// допуск на длину бита +- 1/50 длительности бита
	bt.delta_timer_ticks_per_bit_ = bt.TIM_ticks_per_bit_ / 50;
	bt.TIM_ticks_per_bit_min_ = bt.TIM_ticks_per_bit_ - \
		bt.delta_timer_ticks_per_bit_;
	bt.timer_ticks_per_bit_max_ = bt.TIM_ticks_per_bit_ + \
		bt.delta_timer_ticks_per_bit_;
	bt.start_bit_ticks_ = 3 * bt.TIM_ticks_per_bit_;
	bt.start_bit_ticks_min_ = bt.start_bit_ticks_ - \
		bt.delta_timer_ticks_per_bit_;
	bt.start_bit_ticks_max_ = bt.start_bit_ticks_ + \
		bt.delta_timer_ticks_per_bit_;
	return bt;
}

void Add_converted_signal_to_data(uint16_t* buffer, uint16_t* sequence, uint16_t* sequence_iterator)
{

}

data_full_msg init_data_struct()
{
	data_full_msg data;
	data.data_length_ = 0;
	data.data_iterator_ = 0;
	for (unsigned i = 0; i < MAX_DATA_LENGTH; i++) data.data_[i] = i;
	data.CRC_message_[0] = 0;
	data.CRC_message_[1] = 0;
	return data;
}



// выполняется во время прерывания по запросу ПДП (DMA)
uint8_t Add_signal_to_sequence(bit_time* bt, uint16_t* buffer, timer_receive_sequence *sequence, data_full_msg * data)
{
	if (sequence->sequence_iterator_ == MAX_SEQUENCE_ITERATOR) return 1;
	sequence->TIM_ticks_sequence_[sequence->sequence_iterator_] = buffer[0];
	++sequence->sequence_iterator_;
	sequence->TIM_ticks_sequence_[sequence->sequence_iterator_] = buffer[1];
	++sequence->sequence_iterator_;
	Convert_sequence_find_data_length(bt, sequence, data);
	if (sequence->sequence_iterator_ == MAX_SEQUENCE_ITERATOR) sequence->sequence_iterator_ = 0; // обнуление последовательности
	// принимаю длину сообщения, устанавливаю sequence_iterator в 0, принимаую сообщение
	// , при достижении sequence iterator длины сообщения снимаю флаг start_bit_ вызываю прерывание?!?!?
	return 0;
}

// поиск начала сообщение и его расшифровка
// разворот битов из LSB в MSB
uint8_t Convert_sequence_find_data_length(bit_time* bt, timer_receive_sequence* tim_seq, data_full_msg* data )
{
	// предпологается что таймер принял 6 + 6 бит
	// TODO: проверка на шум если длительность много меньше длительности бита - отсекаем
	uint8_t start_ = 0;
	uint16_t buffer_ = { 0, };  // буфер для заполнения 6 битными значениями
	uint8_t buffer_iterator_ = 0;
	uint8_t word_buffer_ = 0; // заполняется по 4 бита, для учета нужен итератор
	uint8_t halfword_iterator_ = 0;
	uint8_t count_1 = 0, count_0 = 0;
	if (start_ == tim_seq->sequence_iterator_)return 0;
	// поиск
	for (uint8_t i = start_; i < tim_seq->sequence_iterator_; i = i + 2) {//TODO: что если sequence_iterator нечетный

		// разделяю на длительность бита (в отсчетах таймера)
		count_1 = tim_seq->TIM_ticks_sequence_[i] / bt->TIM_ticks_per_bit_;  // TODO: 16 BIT TO 8 BIT
		if (tim_seq->TIM_ticks_sequence_[i] / bt->TIM_ticks_per_bit_min_ >  count_1) ++count_1; // проверяю остаток длительность 
		// "1" не должна быть большой
		count_0 = tim_seq->TIM_ticks_sequence_[i + 1] / bt->TIM_ticks_per_bit_;
		if (tim_seq->TIM_ticks_sequence_[i + 1] % bt->TIM_ticks_per_bit_min_ > count_0) ++count_0;

		// перевожу из "1" и "0" в слово
		// считаю что данные приходят младшим битом вперед
		for (uint8_t y = 0; y < count_1; ++y) {

			buffer_ |= 1 << buffer_iterator_;
			++buffer_iterator_; // до этого момента buffer_iterator не должен быть больше 6

			if (buffer_iterator_ == 6) {
				if (halfword_iterator_ == 0) {
					word_buffer_ = 0;
					word_buffer_ |= Convert_6to4(buffer_);
				}
				else {
					word_buffer_ |= Convert_6to4(buffer_) << 4;
					halfword_iterator_ = 0;
					// TODO: 8 bit received
				}
				buffer_ = 0;
			}
		}

		if (buffer_iterator_ + count_0 >= 6) {

			if (halfword_iterator_ == 0) {
				word_buffer_ = Convert_6to4(buffer_);
			}
			else {
				word_buffer_ = Convert_6to4(buffer_) << 4;
				halfword_iterator_ = 0;
				// TODO: 8 bit received
			}
			buffer_iterator_ = count_0 - (6 - buffer_iterator_);
		}
		buffer_iterator_ += count_0;
	}
}

uint8_t Convert_6to4(uint8_t data_6bit_in)
{
	for (uint8_t i = 0; i < 16; ++i) {
		if (data_6bit_in == symbols[i])return i;
	}
	return 0;
}

uint8_t Convert_4to6(uint8_t data_4bit_in)
{
	return symbols[data_4bit_in];
}



