// C_project_test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include<stdio.h>
#include "unity.h"
#include "soft_rf_internals.h"

void setUp(void);
void tearDown(void);
void test_FunctionUnderTest_should_ReturnFive(void);
int FunctionUnderTest();

void test_convert_6to4(void);
void test_convert_4to6(void);
void test_init_timings(void);


int main(void)
{
    char test[] = "soft_rf_internals unit testing\n";
    printf("%s",test);
    int a = 1;
    TEST_ASSERT(a == 1); //this one will pass
    //TEST_ASSERT(a == 2); //this one will fail
    test_FunctionUnderTest_should_ReturnFive();
    test_convert_6to4();
    test_convert_4to6(); 
    test_init_timings();
    getchar();
}

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
    uint16_t ticks_per_bit = round(timer_frequency / bitrate_[0]);
    uint8_t delta = ticks_per_bit / 50;
    TEST_ASSERT_EQUAL_INT(ticks_per_bit, bt.TIM_ticks_per_bit_);
    TEST_ASSERT_EQUAL_INT(bt.delta_timer_ticks_per_bit_, 150);
    TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_min_, ticks_per_bit - delta);
    TEST_ASSERT_EQUAL_INT(bt.TIM_ticks_per_bit_max_, ticks_per_bit + delta);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_, ticks_per_bit * 3);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_min_, ticks_per_bit * 3 - delta * 3);
    TEST_ASSERT_EQUAL_INT(bt.start_bit_ticks_max_, ticks_per_bit * 3 + delta * 3);
}

void setUp(void) {}
void tearDown(void) {}

int FunctionUnderTest()
{
    return 5;
}

void test_FunctionUnderTest_should_ReturnFive(void) {
    TEST_ASSERT_EQUAL_INT(5, FunctionUnderTest());
    TEST_ASSERT_EQUAL_INT(5, FunctionUnderTest()); //twice even!
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
