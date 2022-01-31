#include"soft_rf.h"






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


void init_rf() {
	const uint32_t timer_freq = TIMER_CLOCK_FREQ;
	bt = init_timings_(bitrate_[BIT_PER_SECOND_TRANSFER_SPEED], timer_freq);
}


// sequence can start with "0" and "1"
void on_timer_count_interrupt() {


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
				read_data_from_buffer(&message, &local_buffer,START_READ_DATA); // with reading & decoding data
				return; // 
			}

		}
	}
	// decryption of the remaining message
	read_data_from_buffer(&message, &local_buffer, RESUME_READ_DATA); // if full message read - set started = 0
	//TODO: Receive start on_Receive() funct
}

void send_data(uint8_t* data, uint8_t data_length)
{
	uint16_t data_iterator = 0;
	uint8_t send_buffer[MAX_TIMER_BUFFER_LENGTH] = { 0 };
	// add start sequence
	send_buffer[0] = 0x38;
	send_buffer[1] = 0x38;
	//  convert data from 8 bit to 6+6 bit (4to6)
	convert_data_to_TIM_sequence(&send_buffer, data, data_length, &data_iterator);
	// TODO: add CRC
	// TODO: start transmitting
	SetTimerToStart();
}





void SetTimerToStart()
{
	//TODO : set TIM_CR register to start 
}

