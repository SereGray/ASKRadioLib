#include"soft_rf_internals.h"


const uint32_t bitrate_[] = { 9600, 19200, 38400, 57600, 76800, 115200 };
uint8_t started = 0;
uint8_t starts_from_high_lvl_bit = 0; // the starts_from_high_lvl_bit indicates which bit the sequence starts from hight lvl
const uint8_t start_symbol = 0x38;



void add_bits_to_buffer(uint8_t bit, uint8_t* count, uint32_t* buff, uint8_t* iterator) {
	// перевожу из "1" и "0" в буффер

	if (!bit) // if bit = BIT_0
	{
		*iterator += *count;
		*buff = (*buff << *count);
		return;
	}

	// if bit = BIT_1
	uint8_t value = 0;
	uint8_t count_local = *count;
	while (count_local > 0)
	{
		value = value << 1;
		value++;
		count_local --;
	}
	// add to buffer
	//TODO убрать смещение value смещать на count
	*buff = (*buff << *count) | value ;
	*iterator += *count;
}

// no more than 6 bits in a row
uint8_t bit_counter(bit_time* bt, timer_receive_sequence* tim_seq, uint16_t index) {
	uint8_t count_bit = 0;
	count_bit = tim_seq->TIM_ticks_sequence_[index] / bt->TIM_ticks_per_bit_;  // TODO: 16 BIT TO 8 BIT
	if (tim_seq->TIM_ticks_sequence_[index] / bt->TIM_ticks_per_bit_min_ > count_bit) ++count_bit; // checking the remainder
	return count_bit;
}

// call from  convert_timer_sequence()
void convert_from_buffer(uint32_t* buffer_, uint8_t* buffer_iterator_, converted_sequence* res, uint16_t* data_length, uint16_t* data_iterator)
{
	uint8_t word_count_ = res->words_; //
	uint16_t mask = 0b0000000000111111; // 6 bit mask 0000 0000 0011 1111
	if (*buffer_iterator_ >= 12)
	{
		while (*buffer_iterator_ > 5)
		{
			uint32_t local_buffer = *buffer_;
			// take left part of the buffer_
			if (*buffer_iterator_ > 6)
			{
				local_buffer = local_buffer >> (*buffer_iterator_ - 6);
			}
			uint8_t converted_halfword = convert_6to4((uint8_t)(local_buffer & mask)); // TODO: нужно брать левую часть и сдвигать влево !!!

			// there could be a reversal of lsb to msb
			res->word_buffer_ = (res->word_buffer_ << (res->halfword_iterator_ * 4)) + converted_halfword; // paste to right
			res->halfword_iterator_ += 1;
			//*buffer_ = *buffer_ >> 6;
			//// zeroing used data in the buffer_     NOT NECESSARY!!!
			//uint32_t zero_mask = 0xFFFFFF00;
			//if (*buffer_iterator_ > 6)
			//{
			//	local_buffer = local_buffer >> (*buffer_iterator_ - 6);
			//}
			//*buffer_ = ;
			*buffer_iterator_ -= 6;
			if (res->halfword_iterator_ > 1)
			{
				res->sequence_[word_count_] = res->word_buffer_;
				word_count_ += 1;
				res->halfword_iterator_ = 0;
				res->word_buffer_ = 0;
				*data_iterator += 1; // TODO: сделать локальную копию ?
				if (*data_iterator >= *data_length)
				{
					break; // the end of the message
				}
			}
		}
	}
	res->words_ = word_count_;
	// DEBUG bookmark
	uint32_t countt = 0;
	if (word_count_ > 2) {
		++countt;
	}
}

// Decoding
// считаю что данные приходят младшим битом вперед
// разворот битов из LSB в MSB
// предпологается что таймер принял 6 + 6 бит
// call from  read_data_from_buffer()
converted_sequence* convert_timer_sequence(bit_time* bt, timer_receive_sequence* tim_seq, uint16_t* length, uint16_t* data_iterator)
{
	uint8_t start_ = tim_seq->sequence_iterator_;
	// if the iterator points to the end
	if (start_ == max_timer_buffer_length)return init_converted_sequence(0);
	converted_sequence* res = init_converted_sequence(*length);

	// TODO: проверка на шум если длительность много меньше длительности бита - отсекаем

	uint32_t buffer_ = { 0 };  // FIFO buffer to fill 6 bits 
	uint8_t buffer_iterator_ = 0; //the buffer_iterator_ that counts the bits writen to the buffer_ 
	uint8_t count_1 = 0, count_0 = 0; // count bits with "1", "0" and count decoded WORD (8bits)


	// поиск
	for (uint8_t i = start_; i < max_timer_buffer_length; i = i + 2)
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

		count_1 = bit_counter(bt, tim_seq, index_1); // count bits with "1"

		count_0 = bit_counter(bt, tim_seq, index_0); // count bits with "0"

		// add to buffer bits
		if (starts_from_high_lvl_bit)
		{
			add_bits_to_buffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
			add_bits_to_buffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
		}
		else
		{
			add_bits_to_buffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
			add_bits_to_buffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
		}
		// TODO: if buffer_iterator < 5 and >0 ???
		// decoding from buffer to word_buffer	
		if (&buffer_iterator_ > 5) convert_from_buffer(&buffer_, &buffer_iterator_, res, length, data_iterator);
	}
	tim_seq->sequence_iterator_ = 0; // reading sequence done
	return res;
}

void convert_data_to_TIM_sequence(uint8_t* send_buffer, uint8_t* data, uint8_t* data_length, uint16_t* data_iterator)
{
	//TODO: there
	for (uint8_t i = 0; i < data_length; i++)
	{

	}
}

uint8_t convert_6to4(uint8_t data_6bit_in)
{
	for (uint8_t i = 0; i < 16; ++i) {
		if (data_6bit_in == symbols[i])return i;
	}
	return 0;
}


uint8_t convert_4to6(uint8_t data_4bit_in)
{
	return symbols[data_4bit_in];
}


bit_time init_timings_(uint32_t changed_bitrate, uint32_t timer_freq)
{
	bit_time bt;
	bt.TIM_ticks_per_bit_ = (uint32_t)round((double)timer_freq / (double)changed_bitrate);
	// bit lenght tolerance +- 1/50 of bit duration
	bt.delta_timer_ticks_per_bit_ = (uint8_t)round((double)bt.TIM_ticks_per_bit_ / 50);
	bt.TIM_ticks_per_bit_min_ = bt.TIM_ticks_per_bit_ - \
		bt.delta_timer_ticks_per_bit_;
	bt.TIM_ticks_per_bit_max_ = bt.TIM_ticks_per_bit_ + \
		bt.delta_timer_ticks_per_bit_;
	bt.start_bit_ticks_ = 3 * bt.TIM_ticks_per_bit_;
	bt.start_bit_ticks_min_ = bt.start_bit_ticks_ - \
		bt.delta_timer_ticks_per_bit_ * 3;
	bt.start_bit_ticks_max_ = bt.start_bit_ticks_ + \
		bt.delta_timer_ticks_per_bit_ * 3;
	return bt;
}

//call on on_timer_count_interrupt()
void read_data_from_buffer(bit_time* bt, data_full_msg* message, timer_receive_sequence* local_buffer, uint8_t start_read_data)
{
	if (start_read_data)
	{
		remove_second_start_sequence(bt,local_buffer); // offset sequence iterator
		//message->data_length_ = MAX_DATA_LENGTH; // only for the first conversion, the data length is taken from the message after the first decoding secuence
		message = init_data_struct(max_data_length);
	}

	converted_sequence* temp = convert_timer_sequence(bt, local_buffer, &message->data_length_, &message->data_iterator_);
	//TODO: if converted_sequence is NULL
	if (start_read_data)
	{
		message = init_data_struct(temp->sequence_[0]);
	}

	//copying data from sequence 
	uint16_t sequence_iterator = 0;
	for (uint16_t i = message->data_iterator_; i < message->data_length_; i++)
	{
		message->data_[i] = temp->sequence_[sequence_iterator];
		message->data_iterator_ = i;
		sequence_iterator++;
		if (sequence_iterator >= temp->length_) // if the current transmission has ended
		{
			return; // aborting data copying and resume in next transfer ( after next function call)
		}
	}
	started = 0;
}

void remove_second_start_sequence(bit_time* bt, timer_receive_sequence* local_buffer)
{
	// checking the second start symbol
	uint16_t hight_lvl_time = local_buffer->TIM_ticks_sequence_[local_buffer->sequence_iterator_]; // the duration of first signal is the duration of hight level
	uint16_t low_lvl_time = local_buffer->TIM_ticks_sequence_[local_buffer->sequence_iterator_ + 1];  // the duration of second signal is the duration of low level

	if (hight_lvl_time > bt->start_bit_ticks_min_ && hight_lvl_time < bt->start_bit_ticks_max_ && low_lvl_time > bt->start_bit_ticks_min_ && low_lvl_time < bt->start_bit_ticks_max_)
	{
		//the next bit after the start sequence is "1"
		starts_from_high_lvl_bit = 1;
		local_buffer->sequence_iterator_ += 2; //  remove the start symbol from input buff
	}
	else if (hight_lvl_time > bt->start_bit_ticks_min_ && hight_lvl_time < bt->start_bit_ticks_max_ && low_lvl_time > bt->start_bit_ticks_min_ + bt->TIM_ticks_per_bit_min_)
	{
		// the next bit after the start sequence is "0"
		starts_from_high_lvl_bit = 0;
		local_buffer->sequence_iterator_ += 1;               // remove higth lvl of the  start symbol
		local_buffer->TIM_ticks_sequence_[local_buffer->sequence_iterator_] -= bt->start_bit_ticks_; //  remove low lvl of the second start symbol
	}
}

converted_sequence* init_converted_sequence(uint8_t len)
{
	converted_sequence* seq = malloc(sizeof(converted_sequence));
	if (seq)
	{
		seq->sequence_ = malloc(len * sizeof(uint8_t));
		for (uint8_t i = 0; i < len; i++) seq->sequence_[i] = 0;
		if (seq->sequence_ == NULL) return NULL;
		seq->words_ = 0;
		seq->length_ = len;
		seq->word_buffer_ = 0;
		seq->halfword_iterator_ = 0;
		return seq;
	}
	return NULL;
}

void delete_converted_sequence(converted_sequence* seq)
{
	if (seq != NULL) 
	{
		free(seq->sequence_);
		free(seq);
	}
}

data_full_msg* init_data_struct(uint8_t len)
{
	// TODO: who delete var "data" ? "data" is deleted after the funcion ends ? memory leak ?
	data_full_msg* data = malloc(sizeof(data_full_msg));
	if (data)
	{
		data->data_ = malloc(sizeof(uint8_t) * len);
		data->data_length_ = len;
		data->data_iterator_ = 0;
		for (uint8_t i = 0; i < data->data_length_; i++) data->data_[i] = 0;
		data->CRC_message_[0] = 0;
		data->CRC_message_[1] = 0;
		return data;
	}
	return NULL;
}

void delete_data_struct(data_full_msg* msg)
{
	if (msg != NULL)
	{
		free(msg->data_);
		free(msg);
	}
}

timer_receive_sequence* init_receive_sequence(uint8_t len)
{
	timer_receive_sequence* seq = malloc(sizeof(timer_receive_sequence));
	if (seq)
	{
		seq->TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * len);
		seq->sequence_iterator_ = 0;
		seq->sequence_length_ = len;
		for (uint8_t i = 0; i < seq->sequence_length_; i++) seq->TIM_ticks_sequence_[i] = 0;
		return seq;
	}
	return NULL;
}

void delete_receive_sequence(timer_receive_sequence* seq)
{
	if (seq != NULL)
	{
		free(seq->TIM_ticks_sequence_);
		free(seq);
	}
}