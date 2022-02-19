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
void test_remove_second_start_sequence_from_hight_lvl_bit(void);
void test_add_bits_to_buffer(void);


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
    RUN_TEST(test_add_bits_to_buffer);
    UNITY_END();
}


//------------------FUNCTION-DEFINITION---------------------------

void setUp(void) {}
void tearDown(void) {}


void test_convert_6to4(void) 
{
    TEST_ASSERT_EQUAL_INT(15, convert_6to4(0x34));
    TEST_ASSERT_EQUAL_INT(0, convert_6to4(0xd));
    TEST_ASSERT_EQUAL_INT(4, convert_6to4(0x16));
}

void test_convert_4to6(void)
{
    TEST_ASSERT_EQUAL_INT(0x34, convert_4to6(15));
    TEST_ASSERT_EQUAL_INT(0xd, convert_4to6(0));
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
    bit_time bt = init_timings_(bitrate_[0], 72000000);
    timer_receive_sequence seq;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 8);
    for (uint8_t i = 0; i < 8; i++)
    {
        seq.TIM_ticks_sequence_[i] = 7500 * i; 
        TEST_ASSERT_EQUAL(bit_counter(&bt,&seq, i),i);
    }
    free(seq.TIM_ticks_sequence_);
}

// no more than 6 bits in a row
// borderline bitrate test
void test_bit_counter_borderline_bitrate_test(void)
{
    bit_time bt = init_timings_(bitrate_[5], 8000000); // one bit = 69,4444 timer ticks
    timer_receive_sequence seq;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 8);
    seq.TIM_ticks_sequence_[0] = 69 * 6;
    seq.TIM_ticks_sequence_[1] = 69 * 2;
    seq.TIM_ticks_sequence_[2] = 69;
    TEST_ASSERT_EQUAL(bit_counter(&bt, &seq, 0), 6);
    TEST_ASSERT_EQUAL(bit_counter(&bt, &seq, 1), 2);
    TEST_ASSERT_EQUAL(bit_counter(&bt, &seq, 2), 1);
    bt = init_timings_(bitrate_[0], 8000000); // one bit = 833,3334 timer ticks
    seq.TIM_ticks_sequence_[0] = 833 * 6;
    seq.TIM_ticks_sequence_[1] = 833 * 2;
    seq.TIM_ticks_sequence_[2] = 833;
    TEST_ASSERT_EQUAL(bit_counter(&bt, &seq, 0), 6);
    TEST_ASSERT_EQUAL(bit_counter(&bt, &seq, 1), 2);
    TEST_ASSERT_EQUAL(bit_counter(&bt, &seq, 2), 1);
}

void test_remove_second_start_sequence_from_low_lvl_bit(void)
{
    bit_time bt = init_timings_(bitrate_[5], 8000000); // one bit = 69,4444 timer ticks
    timer_receive_sequence seq;
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
    seq.sequence_iterator_ += 2; // offset first start sequence
    remove_second_start_sequence(&bt, &seq);
    TEST_ASSERT_EQUAL(starts_from_high_lvl_bit, 0); // global varable testing
    TEST_ASSERT_EQUAL(seq.TIM_ticks_sequence_[3], 138);
    TEST_ASSERT_EQUAL(seq.sequence_iterator_ , 3);
}

void test_remove_second_start_sequence_from_hight_lvl_bit(void)
{
    bit_time bt = init_timings_(bitrate_[5], 8000000); // one bit = 69,4444 timer ticks
    timer_receive_sequence seq;
    seq.sequence_iterator_ = 0;
    seq.sequence_length_ = 35;
    seq.TIM_ticks_sequence_ = malloc(sizeof(uint16_t) * 35);
    //  0x38,  0x38,   0x23,  0xe,   0x13,  0x15,         0x16,  0x19,     0x1a,     0x1c
    // 111000`111000`100011`001110`010011`010101`       010110`011001`   011010`   011100 
    //               ^ here the message starts hight
    // 3,  3, 3, 3,  1,3,2, 2,3, 2, 1,2,2, 1,1,1,1,1,1,  1,1,1,2,2, 2,2,1,  1,2,1,1,1,  1,3,2
    uint16_t sequence[] = { 3, 3, 3, 3, 1, 3, 2, 2, 3, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 3, 2 };
    for (int i = 0; i < 35; i++)
    {
        seq.TIM_ticks_sequence_[i] = sequence[i] * 69;
    }
    seq.sequence_iterator_ += 2; // offset first start sequence
    remove_second_start_sequence(&bt, &seq);
    TEST_ASSERT_EQUAL(starts_from_high_lvl_bit, 1); // global variable testing
    TEST_ASSERT_EQUAL(seq.TIM_ticks_sequence_[3], 207);
    TEST_ASSERT_EQUAL(seq.sequence_iterator_, 4);
}

void test_add_bits_to_buffer(void)
{
    uint16_t buffer_ = { 0 };  // buffer to fill 6 bits 
    uint8_t buffer_iterator_ = 0; 
    uint8_t count_0_1 = 1, count_1_1 = 3, count_0_2 = 2; // 1,3,2 - 011100 0x1c
    add_bits_to_buffer(BIT_0, &count_0_1, &buffer_, &buffer_iterator_); 
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0, " first test - buffer is zero");
    TEST_ASSERT_EQUAL(buffer_iterator_, 1);
    add_bits_to_buffer(BIT_1, &count_1_1, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0xE, " second test - buffer is 0b0000`0000`0000`1110 (0xE or 14");
    TEST_ASSERT_EQUAL(buffer_iterator_, 4);
    add_bits_to_buffer(BIT_0, &count_0_2, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0xE, " third test - buffer is 0b0000`0000`0000`1110 (0xE or 14");
    TEST_ASSERT_EQUAL(buffer_iterator_, 6);

    // add next 6-bit word
    uint8_t count_0_3 = 1, count_1_2 = 2, count_0_4 = 1, count_1_3 = 1, count_0_5 = 1; // 1,2,1,1,1 - 011010 0x1a
    add_bits_to_buffer(BIT_0, &count_0_3, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0xE, " fourth test - 0b0000`0000`0000`1110 (0xE or 14");
    TEST_ASSERT_EQUAL(buffer_iterator_, 7);
    add_bits_to_buffer(BIT_1, &count_1_2, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x18E, " fifth test - buffer is 0b0000`0001`1000`1110 (0x18E or 398");
    TEST_ASSERT_EQUAL(buffer_iterator_, 9);
    add_bits_to_buffer(BIT_0, &count_0_4, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x18E, " sixth test - buffer is 0b0000`0001`1000`1110 (0x18E or 398");
    TEST_ASSERT_EQUAL(buffer_iterator_, 10);
    add_bits_to_buffer(BIT_1, &count_1_3, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x58E, " seventh test - buffer is 0b0000`0101`1000`1110 (0x58E or 1422");
    TEST_ASSERT_EQUAL(buffer_iterator_, 11);
    add_bits_to_buffer(BIT_0, &count_0_5, &buffer_, &buffer_iterator_);
    TEST_ASSERT_EQUAL_MESSAGE(buffer_, 0x58E, " eight test - buffer is 0b0000`0101`1000`1110 (0x58E or 1422");
    TEST_ASSERT_EQUAL(buffer_iterator_, 12);
}