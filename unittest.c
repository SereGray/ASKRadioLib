// C_project_test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include<stdio.h>
#include "unity.h"
#include "soft_rf_internals.h"

extern uint8_t starts_from_high_lvl_bit;
//------------------------FUNCTION-DECLARATION----------------------------
void setUp(void);
void tearDown(void);
void test_FunctionUnderTest_should_ReturnFive(void);
int FunctionUnderTest();

void test_convert_6to4(void);
void test_convert_4to6(void);
void test_init_timings(void);
void test_init_timings_all_bitrate_testing(void);
void test_init_timings_manual_testing(void);
void test_init_delete_convert_sequence(void);
void test_init_delete_data_struct(void);
void test_bit_counter_count_from_0_to_7_bits(void);
void test_bit_counter_borderline_bitrate_test(void);
void test_remove_second_start_sequence_from_low_lvl_bit(void);
void test_remove_second_start_sequence_from_low_lvl_bit_hight_speed(void);
void test_remove_second_start_sequence_from_hight_lvl_bit(void);
void test_remove_second_start_sequence_from_hight_lvl_bit_hight_speed(void);
void test_add_bits_to_buffer(void);
void test_convert_from_buffer_big_buffer(void);
void test_convert_from_buffer(void);
void test_init_delete_timer_sequence(void);
void test_convert_timer_sequence_starts_from_low_lvl(void);
void test_convert_timer_sequence_starts_from_hight_lvl(void);
void test_init_symbols_to_TIM_sequence(void);
void test_get_length_of_TIM_sequence(void);


//----------------------------MAIN--------------------------------

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_convert_6to4);
    RUN_TEST(test_convert_4to6);
    RUN_TEST(test_init_timings);
    RUN_TEST(test_init_timings_all_bitrate_testing);
    RUN_TEST(test_init_timings_manual_testing);
    RUN_TEST(test_init_delete_convert_sequence);
    RUN_TEST(test_init_delete_data_struct);
    RUN_TEST(test_bit_counter_count_from_0_to_7_bits);
    RUN_TEST(test_bit_counter_borderline_bitrate_test);
    RUN_TEST(test_remove_second_start_sequence_from_low_lvl_bit);
    RUN_TEST(test_remove_second_start_sequence_from_hight_lvl_bit);
    RUN_TEST(test_remove_second_start_sequence_from_low_lvl_bit_hight_speed);
    RUN_TEST(test_remove_second_start_sequence_from_hight_lvl_bit_hight_speed);
    RUN_TEST(test_add_bits_to_buffer);
    RUN_TEST(test_convert_from_buffer_big_buffer);
    RUN_TEST(test_convert_from_buffer);
    RUN_TEST(test_init_delete_timer_sequence);
    RUN_TEST(test_convert_timer_sequence_starts_from_low_lvl);
    RUN_TEST(test_convert_timer_sequence_starts_from_hight_lvl);
    RUN_TEST(test_init_symbols_to_TIM_sequence);
    RUN_TEST(test_get_length_of_TIM_sequence);
    UNITY_END();
}


//------------------FUNCTION-DEFINITION---------------------------

void setUp(void) {}
void tearDown(void) {}


void test_convert_6to4(void) 
{
    TEST_ASSERT_EQUAL_INT(15, convert_6to4(0x34));
    TEST_ASSERT_EQUAL_INT(0, convert_6to4(0x0d));
    TEST_ASSERT_EQUAL_INT(4, convert_6to4(0x16));
}

void test_convert_4to6(void)
{
    TEST_ASSERT_EQUAL_INT(0x34, convert_4to6(15));
    TEST_ASSERT_EQUAL_INT(0x0d, convert_4to6(0));
    TEST_ASSERT_EQUAL_INT(0x16, convert_4to6(4));
}

void test_init_timings(void)
{
    uint32_t timer_frequency = 72000000;
    bt = init_timings_(bitrate_[0], timer_frequency); // bitrate 9600
    // bit time 1`000`000`000 / 9600
    // one timer tick time 1`000`000`000 / 72000000 
    // TIM_ticks_per_bit_ = round(72000000 / 9600)
    // delta_timer_ticks_per_bit_ = 150 
    uint32_t ticks_per_bit = (uint32_t)round((double)timer_frequency / (double)bitrate_[0]);
    uint8_t delta = (uint8_t)round((double)ticks_per_bit / 50);
    TEST_ASSERT_EQUAL_INT(ticks_per_bit, bt.TIM_ticks_per_bit_);
    TEST_ASSERT_EQUAL_INT(bt.delta_timer_ticks_per_bit_, delta);
    TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_min_, ticks_per_bit - delta);
    TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_max_, ticks_per_bit + delta);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_, ticks_per_bit * 3);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_min_, ticks_per_bit * 3 - delta * 3);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_max_, ticks_per_bit * 3 + delta * 3);
}

void test_init_timings_all_bitrate_testing(void) 
{
    for (int exponent = 0; exponent < 3; exponent++)
    {
        for (int i = 0; i < 6; i++)
        {
            uint32_t timer_frequency = 72000000 / 8 * 2^ exponent;
            bt = init_timings_(bitrate_[i], timer_frequency); // bitrate 9600
            // bit time 1`000`000`000 / 9600
            // one timer tick time 1`000`000`000 / 72000000 
            // TIM_ticks_per_bit_ = round(72000000 / 9600)
            // delta_timer_ticks_per_bit_ = 150 
            uint32_t ticks_per_bit = (uint32_t)round((double)timer_frequency / (double)bitrate_[i]); 
            uint8_t delta = (uint8_t)round((double)ticks_per_bit / 50);
            TEST_ASSERT_EQUAL_INT(ticks_per_bit, bt.TIM_ticks_per_bit_);
            TEST_ASSERT_EQUAL_INT(bt.delta_timer_ticks_per_bit_, delta);
            TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_min_, ticks_per_bit - delta);
            TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_max_, ticks_per_bit + delta);
            TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_, ticks_per_bit * 3);
            TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_min_, ticks_per_bit * 3 - delta * 3);
            TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_max_, ticks_per_bit * 3 + delta * 3);
        }
    }
}

void test_init_timings_manual_testing(void)
{
    uint32_t timer_frequency = 18000000 ;
    bt = init_timings_(115200, timer_frequency); // bitrate 9600
    // bit time 1`000`000`000 / 9600
    // one timer tick time 1`000`000`000 / 72000000 
    // TIM_ticks_per_bit_ = 156
    // delta_timer_ticks_per_bit_ = 3 
    TEST_ASSERT_EQUAL_INT(156, bt.TIM_ticks_per_bit_);
    TEST_ASSERT_EQUAL_INT(bt.delta_timer_ticks_per_bit_, 3);
    TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_min_, 156 - 3);
    TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_max_, 156 + 3);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_, 468);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_min_, 468 - 9);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_max_, 468 + 9);
}

void test_init_delete_convert_sequence(void)
{
    uint8_t len = 8;
    converted_sequence* seq = init_converted_sequence(len);
    for (uint8_t i = 0; i < len; i++)
    {
        TEST_ASSERT_EQUAL(seq->sequence_[0], 0);
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(seq, "seq is not null");  
    TEST_ASSERT_NOT_NULL_MESSAGE(seq->sequence_, " seq->sequence is null");
    TEST_ASSERT_EQUAL(seq->words_, 0);
    TEST_ASSERT_EQUAL(seq->length_, len);
    delete_converted_sequence(seq);
  //  TEST_ASSERT_NULL_MESSAGE(seq->sequence_, " internal_pointer is null");
  //  TEST_ASSERT_NULL_MESSAGE(seq, " seq is null");
}

void test_init_delete_data_struct(void)
{
    uint8_t len = 8;
    data_full_msg* msg = init_data_struct(len);
    for (uint8_t i = 0; i < len; i++)
    {
        TEST_ASSERT_EQUAL(msg->data_[0], 0);
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(msg, "msg is not null");
    TEST_ASSERT_NOT_NULL_MESSAGE(msg->data_, " msg->data_ is null");
    delete_data_struct(msg);
    //  TEST_ASSERT_NULL_MESSAGE(seq->sequence_, " internal_pointer is null");
    //  TEST_ASSERT_NULL_MESSAGE(seq, " seq is null");
}

void test_bit_counter_count_from_0_to_7_bits(void)
{
    bit_time test_bt = init_timings_(bitrate_[0], 72000000);
    TIM_sequence seq;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 8);
    for (uint8_t i = 0; i < 8; i++)
    {
        seq.TIM_ticks_sequence_[i] = 7500 * i; 
        TEST_ASSERT_EQUAL(bit_counter(&test_bt,&seq, i),i);
    }
    free(seq.TIM_ticks_sequence_);
}

// no more than 6 bits in a row
// borderline bitrate test
void test_bit_counter_borderline_bitrate_test(void)
{
    bit_time test_bt = init_timings_(bitrate_[5], 8000000); // one bit = 69,4444 timer ticks
    TIM_sequence seq;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 8);
    seq.TIM_ticks_sequence_[0] = 69 * 6;
    seq.TIM_ticks_sequence_[1] = 69 * 2;
    seq.TIM_ticks_sequence_[2] = 69;
    TEST_ASSERT_EQUAL(bit_counter(&test_bt, &seq, 0), 6);
    TEST_ASSERT_EQUAL(bit_counter(&test_bt, &seq, 1), 2);
    TEST_ASSERT_EQUAL(bit_counter(&test_bt, &seq, 2), 1);
    test_bt = init_timings_(bitrate_[0], 8000000); // one bit = 833,3334 timer ticks
    seq.TIM_ticks_sequence_[0] = 833 * 6;
    seq.TIM_ticks_sequence_[1] = 833 * 2;
    seq.TIM_ticks_sequence_[2] = 833;
    TEST_ASSERT_EQUAL(bit_counter(&test_bt, &seq, 0), 6);
    TEST_ASSERT_EQUAL(bit_counter(&test_bt, &seq, 1), 2);
    TEST_ASSERT_EQUAL(bit_counter(&test_bt, &seq, 2), 1);
}

void test_remove_second_start_sequence_from_low_lvl_bit(void)
{
    bit_time bt = init_timings_(bitrate_[5], 8000000); // one bit = 69,4444 timer ticks
    TIM_sequence seq;
    seq.sequence_iterator_ = 0;
    seq.sequence_length_ = 35;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 35);
    //  0x38,  0x38,   0xd,  0xe,   0x13,  0x15,         0x16,  0x19,     0x1a,     0x1c
    // 111000`111000`001101`001110`010011`010101`       010110`011001`   011010`   011100 
    //               ^ here the message starts low
    // 3,  3,  3,  5,  2,1,1,2,3, 2,1,2,2,1,1,1,1,1,1, 1,1,1,2,2,2,2,1, 1,2,1,1,1, 1,3,2
    uint16_t sequence[] = { 3, 3, 3, 5, 2, 1, 1, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 3, 2};
    for (int i = 0; i < 35; i++)
    {
        seq.TIM_ticks_sequence_[i] = sequence[i] * 69;
    }
    seq.sequence_iterator_ += 2; // offset first part start sequence
    remove_second_start_sequence(&bt, &seq); // second part start sequence
    TEST_ASSERT_EQUAL(starts_from_high_lvl_bit, 0); // global varable testing
    TEST_ASSERT_EQUAL(seq.TIM_ticks_sequence_[3], 138);
    TEST_ASSERT_EQUAL(seq.sequence_iterator_ , 3);
    free(seq.TIM_ticks_sequence_);
}

void test_remove_second_start_sequence_from_low_lvl_bit_hight_speed(void)
{
    bit_time test_bt = init_timings_(bitrate_[0], 72000000);
    uint8_t length = 35;
    uint16_t data_iterator = 0;
    TIM_sequence* test_sequence = init_TIM_sequence(length);
    uint16_t sequence[] = { 3, 3, 3, 5, 2, 1, 1, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 3, 2 };
    //                       0x38,  0x38,   0xd,  0xe,   0x13,  0x15,         0x16,  0x19,     0x1a,     0x1c
    for (int i = 0; i < 35; i++)
    {
        test_sequence->TIM_ticks_sequence_[i] = sequence[i] * test_bt.TIM_ticks_per_bit_;
    }
    // remove start sequence from test_sequence
    test_sequence->sequence_iterator_ += 2; // offset first part start sequence
    remove_second_start_sequence(&test_bt, test_sequence); // second part start sequence
    TEST_ASSERT_EQUAL(test_sequence->TIM_ticks_sequence_[3], 15000);
    TEST_ASSERT_EQUAL(test_sequence->sequence_iterator_, 3);
    delete_receive_sequence(test_sequence);
}

void test_remove_second_start_sequence_from_hight_lvl_bit(void)
{
    bit_time test_bt = init_timings_(bitrate_[5], 8000000); // one bit = 69,4444 timer ticks
    TIM_sequence seq;
    seq.sequence_iterator_ = 0;
    seq.sequence_length_ = 34;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 35);
    //  0x38,  0x38,   0x23,  0xe,   0x13,  0x15,         0x16,  0x19,     0x1a,     0x1c
    // 111000`111000`100011`001110`010011`010101`       010110`011001`   011010`   011100 
    //               ^ here the message starts hight
    // 3, 3,  3, 3,  1,3,2, 2,3, 2, 1,2,2,1,1,1,1,1,1,  1,1,1,2,2, 2,2,1,  1,2,1,1,1,  1,3,2
    uint16_t sequence[] = { 3, 3, 3, 3, 1, 3, 2, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 3, 2 };
    for (int i = 0; i < 34; i++)
    {
        seq.TIM_ticks_sequence_[i] = sequence[i] * 69;
    }
    seq.sequence_iterator_ += 2; // offset first start sequence
    remove_second_start_sequence(&test_bt, &seq);
    TEST_ASSERT_EQUAL(starts_from_high_lvl_bit, 1); // global variable testing
    TEST_ASSERT_EQUAL(seq.TIM_ticks_sequence_[3], 207);
    TEST_ASSERT_EQUAL(seq.sequence_iterator_, 4);
    free(seq.TIM_ticks_sequence_);
}

void test_remove_second_start_sequence_from_hight_lvl_bit_hight_speed(void)
{
    bit_time test_bt = init_timings_(bitrate_[0], 72000000); // one bit = 69,4444 timer ticks
    TIM_sequence seq;
    seq.sequence_iterator_ = 0;
    seq.sequence_length_ = 34;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 35);
    //  0x38,  0x38,   0x23,  0xe,   0x13,  0x15,         0x16,  0x19,     0x1a,     0x1c
    // 111000`111000`100011`001110`010011`010101`       010110`011001`   011010`   011100 
    //               ^ here the message starts hight
    // 3,  3, 3, 3,  1,3,2, 2,3, 2, 1,2,2, 1,1,1,1,1,1,  1,1,1,2,2, 2,2,1,  1,2,1,1,1,  1,3,2
    uint16_t sequence[] = { 3, 3, 3, 3, 1, 3, 2, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 3, 2 };
    for (int i = 0; i < 34; i++)
    {
        seq.TIM_ticks_sequence_[i] = sequence[i] * 7500;
    }
    seq.sequence_iterator_ += 2; // offset first start sequence
    remove_second_start_sequence(&test_bt, &seq);
    TEST_ASSERT_EQUAL(starts_from_high_lvl_bit, 1); // global variable testing
    TEST_ASSERT_EQUAL(seq.TIM_ticks_sequence_[3], 22500);
    TEST_ASSERT_EQUAL(seq.sequence_iterator_, 4);
    free(seq.TIM_ticks_sequence_);
}

void test_add_bits_to_buffer(void)
{
    uint32_t buffer_ = { 0 };  // buffer to fill 6 bits 
    uint8_t buffer_iterator_ = 0; 
    uint8_t count_0_1 = 1, count_1_1 = 3, count_0_2 = 2; // 1,3,2 - 011100 0x1c
    add_bits_to_buffer(BIT_0, &count_0_1, &buffer_, &buffer_iterator_); 
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0, " first test - buffer is zero");
    TEST_ASSERT_EQUAL(buffer_iterator_, 1);
    add_bits_to_buffer(BIT_1, &count_1_1, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x7, " second test - buffer is 0b0000`0000`0000`0111 (0x7 or 7");
    TEST_ASSERT_EQUAL(buffer_iterator_, 4);
    add_bits_to_buffer(BIT_0, &count_0_2, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x1c, " third test - buffer is 0b0000`0000`0001`1100 (0x1c or 28");
    TEST_ASSERT_EQUAL(buffer_iterator_, 6);

    // add next 6-bit word
    uint8_t count_0_3 = 1, count_1_2 = 2, count_0_4 = 1, count_1_3 = 1, count_0_5 = 1; // 1,2,1,1,1 - 011010 0x1a
    add_bits_to_buffer(BIT_0, &count_0_3, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x38, " fourth test - 0b0000`0000`0011`1000 (0x38 or 56");
    TEST_ASSERT_EQUAL(buffer_iterator_, 7);
    add_bits_to_buffer(BIT_1, &count_1_2, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0xe3, " fifth test - buffer is 0b0000`0000`1110`0011 (0xe3 or 227");
    TEST_ASSERT_EQUAL(buffer_iterator_, 9);
    add_bits_to_buffer(BIT_0, &count_0_4, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x1c6, " sixth test - buffer is 0b0000`0001`1100`0110 (0x1c6 or 456");
    TEST_ASSERT_EQUAL(buffer_iterator_, 10);
    add_bits_to_buffer(BIT_1, &count_1_3, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x38d, " seventh test - buffer is 0b0000`0011`1000`1101 (0x38d or 509");
    TEST_ASSERT_EQUAL(buffer_iterator_, 11);
    add_bits_to_buffer(BIT_0, &count_0_5, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x71a, " eight test - buffer is 0b0000`0111`0001`1010 (0x71a or 1818");
    TEST_ASSERT_EQUAL(buffer_iterator_, 12);
}

void test_convert_from_buffer_big_buffer(void)
{
    uint32_t buffer =  0x58E ;  // 0b0000`0101`1000`1110 ---- 22(0x16) 14(0x0e)
    uint8_t buffer_iterator = 12;
    uint16_t length = 1;
    converted_sequence* seq = init_converted_sequence(32);
    data_full_msg *message = init_data_struct(max_data_length);

    convert_from_buffer(&buffer, &buffer_iterator, seq, &length, &message->data_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(seq->sequence_[0], 65, " message data[0]");
//    TEST_ASSERT_EQUAL_MESSAGE(message->data_[1], 4, " message data[1]");
    delete_data_struct(message);
    delete_converted_sequence(seq);
}

// TODO word_buffer in sequence struct ненужен ?
void test_convert_from_buffer(void)
{
    max_timer_buffer_length = 35;
    starts_from_high_lvl_bit = 0;
    bit_time test_bt = init_timings_(bitrate_[0], 72000000);
    uint8_t length = 35;
    uint16_t data_iterator = 0;
    TIM_sequence* test_sequence = init_TIM_sequence(length);
    uint16_t sequence[] = {0,0, 0, 2, 2,  1, 1,  2, 3,  2, 1,  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 3, 2 };
    //                         0x0d,          0x0e,      0x13,  0x15,         0x16,  0x19,     0x1a,     0x23
    //                   3, 3, 3, 5, 2,  1, 1,  2, 3,  2, 1,  2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 3, 2
    for (int i = 0; i < 35; i++)
    {
        test_sequence->TIM_ticks_sequence_[i] = sequence[i] * test_bt.TIM_ticks_per_bit_;
    }
    uint32_t buffer_ = 0;  // 0b0000`0101`1000`1110 ---- 22(0x16) 14(0xe)
    uint8_t buffer_iterator_ = 0;
    uint8_t count_1 = 0, count_0 = 0;
    converted_sequence* seq = init_converted_sequence(32);
    data_full_msg* message = init_data_struct(max_data_length);

    uint8_t i = 3;
    count_0 = sequence[i];
    count_1 = sequence[i+1];
    add_bits_to_buffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
    add_bits_to_buffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
    convert_from_buffer(&buffer_, &buffer_iterator_, seq, &length, &message->data_iterator_);
    TEST_ASSERT_EQUAL(seq->words_, 0);
   
    i += 2;
    count_0 = sequence[i];
    count_1 = sequence[i + 1];
    add_bits_to_buffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
    add_bits_to_buffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
    convert_from_buffer(&buffer_, &buffer_iterator_, seq, &length, &message->data_iterator_);
    TEST_ASSERT_EQUAL(seq->words_, 0);

    i += 2;
    count_0 = sequence[i];
    count_1 = sequence[i + 1];
    add_bits_to_buffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
    add_bits_to_buffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
    convert_from_buffer(&buffer_, &buffer_iterator_, seq, &length, &message->data_iterator_);
    TEST_ASSERT_EQUAL(seq->words_, 0);

    i += 2;
    count_0 = sequence[i];
    count_1 = sequence[i + 1];
    add_bits_to_buffer(BIT_0, &count_0, &buffer_, &buffer_iterator_);
    add_bits_to_buffer(BIT_1, &count_1, &buffer_, &buffer_iterator_);
    convert_from_buffer(&buffer_, &buffer_iterator_, seq, &length, &message->data_iterator_);
    TEST_ASSERT_EQUAL(seq->words_, 1);
    TEST_ASSERT_EQUAL(buffer_, 3385);
    TEST_ASSERT_EQUAL(seq->sequence_[0], 1);
    //TEST_ASSERT_EQUAL_MESSAGE(seq->sequence_[0], 20, " message data[0]");
    //    TEST_ASSERT_EQUAL_MESSAGE(message->data_[1], 4, " message data[1]");
    delete_data_struct(message);
    delete_converted_sequence(seq);
    delete_receive_sequence(test_sequence);
}



void test_init_delete_timer_sequence(void)
{
    uint8_t len = 8;
    TIM_sequence* seq = init_TIM_sequence(len);
    for (uint8_t i = 0; i < len; i++)
    {
        TEST_ASSERT_EQUAL(seq->TIM_ticks_sequence_[0], 0);
    }
    TEST_ASSERT_NOT_NULL_MESSAGE(seq, "msg is not null");
    TEST_ASSERT_NOT_NULL_MESSAGE(seq->TIM_ticks_sequence_, " msg->data_ is null");
    delete_receive_sequence(seq);
    //  TEST_ASSERT_NULL_MESSAGE(seq->sequence_, " internal_pointer is null");
    //  TEST_ASSERT_NULL_MESSAGE(seq, " seq is null");
}

void test_convert_timer_sequence_starts_from_low_lvl(void)
{
    max_timer_buffer_length = 34;
    bit_time test_bt = init_timings_(bitrate_[0], 72000000);
    uint8_t length = 34;
    uint16_t data_iterator = 0;
    TIM_sequence *test_sequence = init_TIM_sequence(length);
    uint16_t sequence[] = { 3, 3, 3, 5, 2, 1, 1, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 3, 2 };
    //                       0x38,  0x38,   0xd,  0xe,   0x13,  0x15,         0x16,  0x19,     0x1a,     0x1c
    for (int i = 0; i < 34; i++)
    {
        test_sequence->TIM_ticks_sequence_[i] = sequence[i] * test_bt.TIM_ticks_per_bit_;
    }
    // remove start sequence from test_sequence
    test_sequence->sequence_iterator_ += 2; // offset first part start sequence
    remove_second_start_sequence(&test_bt, test_sequence); // second part start sequence
    uint16_t convert_sequence_length = 8;
    converted_sequence* res = convert_timer_sequence(&test_bt, test_sequence, &convert_sequence_length, &data_iterator);
    TEST_ASSERT_EQUAL(res->sequence_[0], (0<<4) + 1);
    TEST_ASSERT_EQUAL(res->sequence_[1], (2<<4) + 3);
    TEST_ASSERT_EQUAL(res->sequence_[2], (4<<4) + 5);
    TEST_ASSERT_EQUAL(res->sequence_[3], (6<<4) + 7);
    delete_receive_sequence(test_sequence);
}


void test_convert_timer_sequence_starts_from_hight_lvl(void)
{
    max_timer_buffer_length = 34;
    bit_time test_bt = init_timings_(bitrate_[0], 72000000);
    uint8_t length = 34;
    uint16_t data_iterator = 0;
    TIM_sequence* test_sequence = init_TIM_sequence(length);
    uint16_t sequence[] = { 3, 3, 3, 3, 1, 3, 2, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 3, 2 };
    //                       0x38,  0x38,   0x23,  0xe,   0x13,  0x15,         0x16,  0x19,     0x1a,     0x1c
    for (int i = 0; i < 34; i++)
    {
        test_sequence->TIM_ticks_sequence_[i] = sequence[i] * test_bt.TIM_ticks_per_bit_;
    }
    // remove start sequence from test_sequence
    test_sequence->sequence_iterator_ += 2; // offset first part start sequence
    remove_second_start_sequence(&test_bt, test_sequence); // second part start sequence
    uint16_t convert_sequence_length = 8;
    converted_sequence* res = convert_timer_sequence(&test_bt, test_sequence, &convert_sequence_length, &data_iterator);
    TEST_ASSERT_EQUAL(res->sequence_[0], (8 << 4) + 1);
    TEST_ASSERT_EQUAL(res->sequence_[1], (2 << 4) + 3);
    TEST_ASSERT_EQUAL(res->sequence_[2], (4 << 4) + 5);
    TEST_ASSERT_EQUAL(res->sequence_[3], (6 << 4) + 7);
    delete_receive_sequence(test_sequence);
}

void test_init_symbols_to_TIM_sequence(void)
{
    symbol_bit_sequence sym_bit_seq[16];
    bit_time test_bt = init_timings_(bitrate_[0], 72000000);
    init_symbols_to_TIM_sequence( sym_bit_seq, 16, &test_bt);
    TEST_ASSERT_EQUAL(sym_bit_seq[0].length, 4);
    TEST_ASSERT_EQUAL(sym_bit_seq[1].length, 3);
    TEST_ASSERT_EQUAL(sym_bit_seq[2].length, 4);
    TEST_ASSERT_EQUAL(sym_bit_seq[3].length, 6);
    TEST_ASSERT_EQUAL(sym_bit_seq[4].length, 6);
    TEST_ASSERT_EQUAL(sym_bit_seq[5].length, 4);
    TEST_ASSERT_EQUAL(sym_bit_seq[6].length, 5);
    TEST_ASSERT_EQUAL(sym_bit_seq[7].length, 3);
    TEST_ASSERT_EQUAL(sym_bit_seq[8].length, 4);
    TEST_ASSERT_EQUAL(sym_bit_seq[9].length, 6);
    TEST_ASSERT_EQUAL(sym_bit_seq[10].length, 5);
    TEST_ASSERT_EQUAL(sym_bit_seq[11].length, 5);
    TEST_ASSERT_EQUAL(sym_bit_seq[12].length, 7);
    TEST_ASSERT_EQUAL(sym_bit_seq[13].length, 5);
    TEST_ASSERT_EQUAL(sym_bit_seq[14].length, 5);
    TEST_ASSERT_EQUAL(sym_bit_seq[15].length, 5);
}

void test_get_length_of_TIM_sequence(void)
{
    uint8_t test_arr[] = { 0,1,2,3,4,5,6 }; // 00001101 00001110 00010011 00010101 00010110 00011001 00011010 00011100
    uint32_t test_length = 7;
    uint16_t res = 0;
    res = get_length_of_TIM_sequence(&test_arr, test_length);
    TEST_ASSERT_EQUAL(res, 31);
}