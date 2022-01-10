#include"soft_rf.h"

typedef struct {
	uint8_t* sequence_;
	uint8_t length_;
}converted_sequence;

converted_sequence Convert_sequence(bit_time* bt, timer_receive_sequence* timers_sequence);
converted_sequence Init_converted_sequence(uint8_t len);
void Read_data_lenght_from_buffer(data_full_msg* message, timer_receive_sequence* local_buffer);
void Append_data_from_local_buffer(data_full_msg*  message, timer_receive_sequence* local_buffer);

const uint32_t bitrate_[] = { 9600, 19200, 38400, 57600, 76800, 115200 };

const uint8_t start_symbol = 0x38;


static uint16_t transmitted_count = 0;
// "1" when transmition started 
static uint8_t started = 0;

timer_receive_sequence timer_buff; // buffer for timer
bit_time bt; // timings
data_full_msg message;



bit_time init_timings_(uint32_t changed_bitrate, uint32_t timer_freq)
{
	bit_time bt;
	// duration 1sec(10^9 мкс) one bit/changed bitrate
	bt.bit_time_ = 1000000000.0 / changed_bitrate;
	// duration 1sec(10^9 мкс) one tick /frecuency
	bt.one_timer_tick_time_ = 1000000000.0 / timer_freq;
	bt.TIM_ticks_per_bit_ = round(bt.bit_time_ / \
		bt.one_timer_tick_time_);
	// bit lenght tolerance +- 1/50 of bit duration
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


//executed during a DMA request interrupt
uint8_t Add_signal_to_sequence(bit_time* bt, uint16_t* buffer, timer_receive_sequence *sequence, data_full_msg * data)
{
	if (sequence->sequence_iterator_ == MAX_TIMER_BUFFER_LENGTH) return 1;
	sequence->TIM_ticks_sequence_[sequence->sequence_iterator_] = buffer[0];
	++sequence->sequence_iterator_;
	sequence->TIM_ticks_sequence_[sequence->sequence_iterator_] = buffer[1];
	++sequence->sequence_iterator_;
	Convert_sequence(bt, sequence, data);
	if (sequence->sequence_iterator_ == MAX_TIMER_BUFFER_LENGTH) sequence->sequence_iterator_ = 0; // reset sequence
	// get message lenght, set sequence_iterator in 0, receiving message
	// , when the message length of sequence iterator is reached  - set start_bit_ flag false and start interrupt?!?!?
	return 0;
}


// Decoding
// разворот битов из LSB в MSB
converted_sequence Convert_sequence(bit_time* bt, timer_receive_sequence* tim_seq)
{
	converted_sequence res;
	// предпологается что таймер принял 6 + 6 бит
	// TODO: проверка на шум если длительность много меньше длительности бита - отсекаем
	uint8_t start_ = tim_seq->sequence_iterator_;
	uint16_t buffer_ = { 0, };  // буфер для заполнения 6 битными значениями
	uint8_t buffer_iterator_ = 0;
	uint8_t word_buffer_ = 0; // заполняется по 4 бита, для учета нужен итератор
	uint8_t halfword_iterator_ = 0;
	uint8_t count_1 = 0, count_0 = 0;
	if (start_ == MAX_TIMER_BUFFER_LENGTH)return Init_converted_sequence(0);
	// поиск
	for (uint8_t i = start_; i < MAX_TIMER_BUFFER_LENGTH; i = i + 2) {//TODO: что если sequence_iterator нечетный

		// разделяю на длительность бита (в отсчетах таймера)
		count_1 = BitCounter(bt, tim_seq, i);
		// "1" не должна быть большой
		count_0 = BitCounter(bt, tim_seq, i+1);

		// перевожу из "1" и "0" в слово
		// считаю что данные приходят младшим битом вперед
		for (uint8_t y = 0; y < count_1 + count_0; ++y) {

			if (count_1 > 0)
			{
				buffer_ |= 1 << buffer_iterator_;
				++buffer_iterator_; // до этого момента buffer_iterator не должен быть больше 6
				count_1 -= 1;
			}
			else if(buffer_iterator_ < 6!!! && count_0 > 0)
			{
				
				while (buffer_iterator_ < 6 || count_0 > 0) {
					buffer_iterator_ += 1;
					count_0 -= 1;
				}
				// TODO: EDIT THERE
			}
			if (buffer_iterator_ == 6!!!) {
				if (halfword_iterator_ == 0) {
					word_buffer_ = 0;
					word_buffer_ |= Convert_6to4(buffer_);
				}
				else {
					word_buffer_ |= Convert_6to4(buffer_) << 4;
					halfword_iterator_ = 0;
					// TODO: 8 bit received ?????
				}
				buffer_ = 0;
			}
			tim_seq->sequence_iterator_ = 0; // reading sequence done
		}

		//if (buffer_iterator_ + count_0 >= 6) {

		//	if (halfword_iterator_ == 0) {
		//		word_buffer_ = Convert_6to4(buffer_);
		//	}
		//	else {
		//		word_buffer_ = Convert_6to4(buffer_) << 4;
		//		halfword_iterator_ = 0;
		//		// TODO: 8 bit received
		//	}
		//	buffer_iterator_ = count_0 - (6 - buffer_iterator_);
		//}
		//buffer_iterator_ += count_0;
	}
}

uint8_t BitCounter(bit_time* bt, timer_receive_sequence* tim_seq, uint16_t index) {
	uint8_t count_bit = 0;
	count_bit = tim_seq->TIM_ticks_sequence_[index] / bt->TIM_ticks_per_bit_;  // TODO: 16 BIT TO 8 BIT
	if (tim_seq->TIM_ticks_sequence_[index] / bt->TIM_ticks_per_bit_min_ > count_bit) ++count_bit; // checking the remainder
	return count_bit;
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

void init_rf() {
	const uint32_t timer_freq = TIMER_CLOCK_FREQ;
	bt = init_timings_(bitrate_[BIT_PER_SECOND_TRANSFER_SPEED], timer_freq);
}

void On_timer_count_interrupt() {
	// copying the input buffer to local copy, the input buffer can be overwritten
	timer_receive_sequence local_buffer;  //  TODO: check for nulling of sequence iterator
	local_buffer = timer_buff;
	//TODO: find start sequence
	if (!started) {
		for (unsigned i = 0; i < MAX_TIMER_BUFFER_LENGTH; i = i + 2) {
			uint16_t hight_lvl_time = local_buffer.TIM_ticks_sequence_[i]; // the duration of first signal is the duration of hight level
			uint16_t low_lvl_time = local_buffer.TIM_ticks_sequence_[i + 1];  // the duration of second signal is the duration of low level
			local_buffer.sequence_iterator_ = i + 1;
			// start symbol is 0x38 or 0b111000 or 3*tick_time hight level and 3*tick_time low level

			// initial condition
			if (hight_lvl_time > bt.start_bit_ticks_min_ && hight_lvl_time < bt.start_bit_ticks_max_ && low_lvl_time > bt.start_bit_ticks_min_ && low_lvl_time < bt.start_bit_ticks_max_) {
				message.data_length_= 0;
				started = 1;
			}

		}
	}
	// if find start sequence reading data lenght
	Read_data_lenght_from_buffer(&message,&local_buffer);
	// start decoding
	// the rest of the message will be decoded after a new interrupt
	Append_data_from_local_buffer(&message, &local_buffer);
	// TODO: check transmitted_count decrement
	// TODO: disable started
	// started = 0;
}

converted_sequence Init_converted_sequence(uint8_t len)
{
	converted_sequence seq;
	seq.sequence_ = malloc(len * sizeof(uint8_t));
	seq.length_ = len;
	return seq;
}

void Read_data_lenght_from_buffer(data_full_msg* message, timer_receive_sequence* local_buffer)
{

}

void Append_data_from_local_buffer(data_full_msg* message, timer_receive_sequence* local_buffer)
{

}
