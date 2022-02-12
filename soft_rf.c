#include"soft_rf.h"
#include"soft_rf_internals.h"


extern uint8_t max_timer_buffer_length;
extern uint8_t max_data_length;

data_full_msg* received_message = NULL;
timer_receive_sequence input_timer_buff_one; // buffer for timer
timer_receive_sequence input_timer_buff_two; // buffer for timer to write input while input_timer_buff_one is decoding and vice versa



void init_rf() {
	const uint32_t timer_freq = TIMER_CLOCK_FREQ;
	bt = init_timings_(bitrate_[BIT_PER_SECOND_TRANSFER_SPEED], timer_freq);
	//const uint16_t MAX_TIMER_BUFFER_LENGTH = 24; // 12 бит Х 2
	max_timer_buffer_length = MAX_TIMER_BUFFER_LENGTH;
		//const uint8_t max_data_length_ = 27; // 27 byte data + 1 byte data lenght + 2 byte CRC
	max_data_length = MAX_DATA_LENGTH;
}



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
				started = 1;

				// if find start sequence reading data lenght and decoding
				// the rest of the message will be decoded after a new interrupt
				read_data_from_buffer(&received_message, &local_buffer,START_READ_DATA); // with reading & decoding data
				return; // 
			}

		}
	}
	// decryption of the remaining message
	read_data_from_buffer(&received_message, &local_buffer, RESUME_READ_DATA); // if full message read - set started = 0
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
	set_timer_to_start();
}





void set_timer_to_start()
{
	//TODO : set TIM_CR register to start 
}

