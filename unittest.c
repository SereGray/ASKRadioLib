// C_project_test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include<stdio.h>
#include "unity.h"
#include "soft_rf_internals.h"

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
void test_bit_counter_cycle(void);


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
    RUN_TEST(test_bit_counter_cycle);
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

void test_bit_counter_cycle(void)
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
void test_bit_counter_more_(void)
{
    //TODO: this
}