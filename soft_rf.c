#include"soft_rf.h"

#define BIT_1 1
#define BIT_0 0
#define START_READ_DATA 1
#define RESUME_READ_DATA 0

typedef struct {
	uint8_t* sequence_;
	uint8_t length_;
}converted_sequence;

converted_sequence ConvertSequence(bit_time* bt, timer_receive_sequence* timers_sequence);
converted_sequence Init_converted_sequence(uint8_t len);
void Read_data_from_buffer(data_full_msg* message, timer_receive_sequence* local_buffer, uint8_t first_reading); 
void Remove_second_start_sequence(timer_receive_sequence* local_buffer);
void Next_read_data_from_local_buffer(data_full_msg*  message, timer_receive_sequence* local_buffer);

const uint32_t bitrate_[] = { 9600, 19200, 38400, 57600, 76800, 115200 };

const uint8_t start_symbol = 0x38;


//static uint16_t transmitted_count = 0; not used
// "1" when transmition started 
static uint8_t started = 0;
// the starts_from_bit indicates which bit the sequence starts from
static uint8_t starts_from_high_lvl_bit = 0; 

timer_receive_sequence input_timer_buff_one; // buffer for timer
timer_receive_sequence input_timer_buff_two; // buffer for timer to write input while input_timer_buff_one is decoding and vice versa
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
	bt.TIM_ticks_per_bit_max_ = bt.TIM_ticks_per_bit_ + \
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
	ConvertSequence(bt, sequence, data);
	if (sequence->sequence_iterator_ == MAX_TIMER_BUFFER_LENGTH) sequence->sequence_iterator_ = 0; // reset sequence
	
																								  
	// get message lenght, set sequence_iterator in 0, receiving message
	
	// , when the message length of sequence iterator is reached  - set start_bit_ flag false and start interrupt?!?!?
	return 0;
}


// Decoding
// считаю что данные приходят младшим битом вперед
// разворот битов из LSB в MSB

// sequence can start with "0" and "1"
converted_sequence ConvertSequence(bit_time* bt, timer_receive_sequence* tim_seq)
{
	converted_sequence res;
	// предпологается что таймер принял 6 + 6 бит
	// TODO: проверка на шум если длительность много меньше длительности бита - отсекаем
	// TODO: длина сообщения
	uint8_t start_ = tim_seq->sequence_iterator_;
	uint16_t buffer_ = { 0 };  // buffer to fill 6 bits 
	uint8_t buffer_iterator_ = 0;
	uint8_t word_buffer_ = 0; // buffer padding by 4 bits, (each 6 bits convert to 4 bits )
	uint8_t halfword_iterator_ = 0; // iterator need for checking 4 bits overflow 
	uint8_t count_1 = 0, count_0 = 0, word_count_ = 0;
	// if the iterator points to the end
	if (start_ == MAX_TIMER_BUFFER_LENGTH)return Init_converted_sequence(0);

	// поиск
	for (uint8_t i = start_; i < MAX_TIMER_BUFFER_LENGTH; i = i + 2)
	{ //TODO: что если sequence_iterator нечетный
		uint8_t index_0 = 0, index_1 = 0; // index shows where bits are high (index_1) or low (index_0) on this iteration

		if (starts_from_high_lvl_bit) // if starts from "1"
		{
			index_1 = i;
			index_0 = i + 1;
		}
		else
		{
			index_0 = i;
			index_1 = i + 1;
		}

		count_1 = BitCounter(bt, tim_seq, index_1); // count bits with "1"

		count_0 = BitCounter(bt, tim_seq, index_0); // count bits with "0"

		// add to buffer bits
		if (starts_from_high_lvl_bit)
		{
			AddBitsToBuffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
			AddBitsToBuffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
		}
		else
		{
			AddBitsToBuffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
			AddBitsToBuffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
		}
	}

	// TODO: decoding from buffer to word_buffer
	uint16_t mask = 0b0000000000111111; // 6 bit mask 1111 1100 0000 0000

	// TODO broken code
	for (uint8_t y = 0; y < count_1 + count_0; ++y) {

		if (count_1 > 0)
		{
			buffer_ |= 1 << buffer_iterator_;
			++buffer_iterator_; // до этого момента buffer_iterator не должен быть больше 6
			count_1 -= 1;
		}
		else if (buffer_iterator_ < 6!!!&& count_0 > 0)
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
				halfword_iterator_ = 1;
			}
			else {
				word_buffer_ |= Convert_6to4(buffer_) << 4;
				halfword_iterator_ = 0;

				// TODO: 8 bit received ?????
				word_count_ += 1;

			}
			buffer_ = 0;
		}
	}
	tim_seq->sequence_iterator_ = 0; // reading sequence done
}

void AddBitsToBuffer(uint8_t bit, uint8_t* count, uint16_t* buff, uint8_t* iterator) {
	// перевожу из "1" и "0" в буффер

	if (!bit) // if bit = BIT_0
	{
		*iterator += *count;
		return;
	}

	// if bit = BIT_1

	// add to buffer
	*buff = *buff | ((2 * (*count) + 1) << *iterator);
	*iterator += *count;
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


// sequence can start with "0" and "1"
void On_timer_count_interrupt() {


	// copying the input buffer to local copy, the input buffer can be overwritten
	timer_receive_sequence local_buffer;  
	local_buffer = input_timer_buff_one;
	//TODO: replace DMA address timer to another input buffer after copying
	
	//search start sequence
	if (!started) {
		for (unsigned i = 0; i < MAX_TIMER_BUFFER_LENGTH; i = i + 1) {
			uint16_t hight_lvl_time = local_buffer.TIM_ticks_sequence_[i]; // the duration of first signal is the duration of hight level
			uint16_t low_lvl_time = local_buffer.TIM_ticks_sequence_[i + 1];  // the duration of second signal is the duration of low level
			local_buffer.sequence_iterator_ = i ;
			// start sequence is two start symbol, start symbol is 0x38 or 0b111000 (or 3*tick_time hight level and 3*tick_time low level) 

			
			// finding the initial condition
			if (hight_lvl_time > bt.start_bit_ticks_min_ && hight_lvl_time < bt.start_bit_ticks_max_ && low_lvl_time > bt.start_bit_ticks_min_ && low_lvl_time < bt.start_bit_ticks_max_) {
				message.data_length_= 0;
				started = 1;

				// if find start sequence reading data lenght and decoding
				// the rest of the message will be decoded after a new interrupt
				Read_data_from_buffer(&message, &local_buffer,START_READ_DATA); // with reading & decoding data
				return; // 
			}

		}
	}
	// decryption of the remaining message
	Read_data_from_buffer(&message, &local_buffer, RESUME_READ_DATA); // if full message read - set started = 0
}


converted_sequence Init_converted_sequence(uint8_t len)
{
	converted_sequence seq;
	seq.sequence_ = malloc(len * sizeof(uint8_t));
	seq.length_ = len;
	return seq;
}


void Read_data_from_buffer(data_full_msg* message, timer_receive_sequence* local_buffer, uint8_t first_reading)
{
	if (first_reading)
	{
		Remove_second_start_sequence(local_buffer); // offset sequence iterator
	}

	converted_sequence temp = ConvertSequence(&bt, local_buffer);

	if (first_reading)
	{
		message->data_length_ = temp.sequence_[0];
	}
	
	//copying data from sequence 
	uint16_t sequence_iterator = 0;
	for (uint16_t i = message->data_iterator_; i < message->data_length_; i++)
	{
		message->data_[i] = temp.sequence_[sequence_iterator];
		message->data_iterator_ = i;
		sequence_iterator++;
		if (sequence_iterator >= temp.length_) // if the current transmission has ended
		{
			return; // aborting data copying and resume in next transfer ( after next function call)
		}
	}
	started = 0;
}

void Remove_second_start_sequence(timer_receive_sequence* local_buffer)
{
	// checking the second start symbol
	uint16_t hight_lvl_time = local_buffer->TIM_ticks_sequence_[local_buffer->sequence_iterator_]; // the duration of first signal is the duration of hight level
	uint16_t low_lvl_time = local_buffer->TIM_ticks_sequence_[local_buffer->sequence_iterator_ + 1];  // the duration of second signal is the duration of low level

	if (hight_lvl_time > bt.start_bit_ticks_min_ && hight_lvl_time < bt.start_bit_ticks_max_ && low_lvl_time > bt.start_bit_ticks_min_ && low_lvl_time < bt.start_bit_ticks_max_)
	{
		//the next bit after the start sequence is "1"
		starts_from_high_lvl_bit = 1;
		local_buffer->sequence_iterator_ += 2; //  remove the start symbol from input buff
	}
	else if(hight_lvl_time > bt.start_bit_ticks_min_ && hight_lvl_time < bt.start_bit_ticks_max_&& low_lvl_time > bt.start_bit_ticks_min_ + bt.TIM_ticks_per_bit_min_)
	{
		// the next bit after the start sequence is "0"
		starts_from_high_lvl_bit = 0;
		local_buffer->sequence_iterator_ += 1;               // remove higth lvl of the  start symbol
		local_buffer->TIM_ticks_sequence_[local_buffer->sequence_iterator_] -= bt.start_bit_ticks_; //  remove low lvl of the second start symbol
	}
}

