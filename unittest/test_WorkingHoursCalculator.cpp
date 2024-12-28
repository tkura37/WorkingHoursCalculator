#include <gtest/gtest.h>
#include "WorkingHoursCalculator.h"

class WorkingHoursCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

/* テストケース1(通常のマクロ定義を使用) */
/* parseTime() のテスト */
TEST_F(WorkingHoursCalculatorTest, ParseTime_Valid) {
    Time time;
    EXPECT_EQ(parseTime("12:00", &time), 2);
    EXPECT_EQ(time.hours, 12);
    EXPECT_EQ(time.minutes, 00);
}

TEST_F(WorkingHoursCalculatorTest, ParseTime_Invalid) {
    Time time;
    EXPECT_EQ(parseTime("25:00", &time), 1);  // 無効な時間
    EXPECT_EQ(parseTime("12:60", &time), 1);  // 無効な分
    EXPECT_EQ(parseTime("abcd", &time), 1);   // 不正なフォーマット
}

/* calculateBreakTime() のテスト */
TEST_F(WorkingHoursCalculatorTest, BreakTime_BeforeLunch)
{
    /* 昼休み開始前に退勤 */
    Time end = {11, 15};
    Time breakTime = calculateBreakTime(end);
    EXPECT_EQ(breakTime.hours, 0);
    EXPECT_EQ(breakTime.minutes, 0);
}

TEST_F(WorkingHoursCalculatorTest, BreakTime_StartOfLunch)
{
    /* 昼休み開始時刻に退勤 */
    Time end = {12, 15};
    Time breakTime = calculateBreakTime(end);
    EXPECT_EQ(breakTime.hours, 0);
    EXPECT_EQ(breakTime.minutes, 0);
}

TEST_F(WorkingHoursCalculatorTest, BreakTime_EndOfLunch)
{
    /* 昼休み終了時刻に退勤 */
    Time end = {13, 00};
    Time breakTime = calculateBreakTime(end);
    EXPECT_EQ(breakTime.hours, 0);
    EXPECT_EQ(breakTime.minutes, 0);
}

TEST_F(WorkingHoursCalculatorTest, BreakTime_Normal)
{
    /* 定時に退勤 */
    Time end = {17, 00};
    Time breakTime = calculateBreakTime(end);
    EXPECT_EQ(breakTime.hours, 0);
    EXPECT_EQ(breakTime.minutes, 45);
}

TEST_F(WorkingHoursCalculatorTest, BreakTime_StartOvertime)
{
    /* 残業開始時刻に退勤 */
    Time end = {17, 15};
    Time breakTime = calculateBreakTime(end);
    EXPECT_EQ(breakTime.hours, 0);
    EXPECT_EQ(breakTime.minutes, 45);
}

TEST_F(WorkingHoursCalculatorTest, BreakTime_Overtime)
{
    /* 残業して退勤 */
    Time end = {18, 00};
    Time breakTime = calculateBreakTime(end);
    EXPECT_EQ(breakTime.hours, 1);
    EXPECT_EQ(breakTime.minutes, 00);
}

/* calculateOverTime() のテスト */
TEST_F(WorkingHoursCalculatorTest, OverTime_BeforeLunch)
{
    /* 昼休み開始前に退勤 */
    Time start = {8, 30};
    Time end = {11, 15};
    Time breakTime = calculateBreakTime(end);
    Time overTime = calculateOverTime(start, end, breakTime);
    EXPECT_EQ(overTime.hours, -5);
    EXPECT_EQ(overTime.minutes, 00);
}

TEST_F(WorkingHoursCalculatorTest, OverTime_StartOfLunch)
{
    /* 昼休み開始時刻に退勤 */
    Time start = {8, 30};
    Time end = {12, 15};
    Time breakTime = calculateBreakTime(end);
    Time overTime = calculateOverTime(start, end, breakTime);
    EXPECT_EQ(overTime.hours, -4);
    EXPECT_EQ(overTime.minutes, 00);
}

TEST_F(WorkingHoursCalculatorTest, OverTime_EndOfLunch)
{
    /* 昼休み終了時刻に退勤 */
    Time start = {8, 30};
    Time end = {13, 00};
    Time breakTime = calculateBreakTime(end);
    Time overTime = calculateOverTime(start, end, breakTime);
    EXPECT_EQ(overTime.hours, -4);
    EXPECT_EQ(overTime.minutes, 0);
}

TEST_F(WorkingHoursCalculatorTest, OverTime_Normal)
{
    /* 定時に退勤 */
    Time start = {8, 30};
    Time end = {17, 00};
    Time breakTime = calculateBreakTime(end);
    Time overTime = calculateOverTime(start, end, breakTime);
    EXPECT_EQ(overTime.hours, 0);
    EXPECT_EQ(overTime.minutes, 00);
}

TEST_F(WorkingHoursCalculatorTest, OverTime_StartOvertime)
{
    /* 残業開始時刻に退勤 */
    Time start = {8, 30};
    Time end = {17, 15};
    Time breakTime = calculateBreakTime(end);
    Time overTime = calculateOverTime(start, end, breakTime);
    EXPECT_EQ(overTime.hours, 0);
    EXPECT_EQ(overTime.minutes, 00);
}

TEST_F(WorkingHoursCalculatorTest, OverTime_Overtime)
{
    /* 残業して退勤 */
    Time start = {8, 30};
    Time end = {18, 15};
    Time breakTime = calculateBreakTime(end);
    Time overTime = calculateOverTime(start, end, breakTime);
    EXPECT_EQ(overTime.hours, 1);
    EXPECT_EQ(overTime.minutes, 00);
}

/* テストケース2(単体テスト用マクロ定義を使用) */
// /* parseTime() のテスト */
// TEST_F(WorkingHoursCalculatorTest, ParseTime_Valid) {
//     Time time;
//     EXPECT_EQ(parseTime("12:00", &time), 2);
//     EXPECT_EQ(time.hours, 12);
//     EXPECT_EQ(time.minutes, 00);
// }

// TEST_F(WorkingHoursCalculatorTest, ParseTime_Invalid) {
//     Time time;
//     EXPECT_EQ(parseTime("25:00", &time), 1);  // 無効な時間
//     EXPECT_EQ(parseTime("12:60", &time), 1);  // 無効な分
//     EXPECT_EQ(parseTime("abcd", &time), 1);   // 不正なフォーマット
// }

// /* calculateBreakTime() のテスト */
// TEST_F(WorkingHoursCalculatorTest, BreakTime_BeforeLunch)
// {
//     /* 昼休み開始前に退勤 */
//     Time end = {11, 15};
//     Time breakTime = calculateBreakTime(end);
//     EXPECT_EQ(breakTime.hours, 0);
//     EXPECT_EQ(breakTime.minutes, 0);
// }

// TEST_F(WorkingHoursCalculatorTest, BreakTime_StartOfLunch)
// {
//     /* 昼休み開始時刻に退勤 */
//     Time end = {12, 15};
//     Time breakTime = calculateBreakTime(end);
//     EXPECT_EQ(breakTime.hours, 0);
//     EXPECT_EQ(breakTime.minutes, 0);
// }

// TEST_F(WorkingHoursCalculatorTest, BreakTime_EndOfLunch)
// {
//     /* 昼休み終了時刻に退勤 */
//     Time end = {13, 00};
//     Time breakTime = calculateBreakTime(end);
//     EXPECT_EQ(breakTime.hours, 0);
//     EXPECT_EQ(breakTime.minutes, 0);
// }

// TEST_F(WorkingHoursCalculatorTest, BreakTime_Normal)
// {
//     /* 定時に退勤 */
//     Time end = {17, 45};
//     Time breakTime = calculateBreakTime(end);
//     EXPECT_EQ(breakTime.hours, 0);
//     EXPECT_EQ(breakTime.minutes, 45);
// }

// TEST_F(WorkingHoursCalculatorTest, BreakTime_StartOvertime)
// {
//     /* 残業開始時刻に退勤 */
//     Time end = {18, 00};
//     Time breakTime = calculateBreakTime(end);
//     EXPECT_EQ(breakTime.hours, 0);
//     EXPECT_EQ(breakTime.minutes, 45);
// }

// TEST_F(WorkingHoursCalculatorTest, BreakTime_Overtime)
// {
//     /* 残業して退勤 */
//     Time end = {19, 00};
//     Time breakTime = calculateBreakTime(end);
//     EXPECT_EQ(breakTime.hours, 1);
//     EXPECT_EQ(breakTime.minutes, 00);
// }

// /* calculateOverTime() のテスト */
// TEST_F(WorkingHoursCalculatorTest, OverTime_BeforeLunch)
// {
//     /* 昼休み開始前に退勤 */
//     Time start = {9, 00};
//     Time end = {11, 00};
//     Time breakTime = calculateBreakTime(end);
//     Time overTime = calculateOverTime(start, end, breakTime);
//     EXPECT_EQ(overTime.hours, -6);
//     EXPECT_EQ(overTime.minutes, 00);
// }

// TEST_F(WorkingHoursCalculatorTest, OverTime_StartOfLunch)
// {
//     /* 昼休み開始時刻に退勤 */
//     Time start = {9, 00};
//     Time end = {12, 15};
//     Time breakTime = calculateBreakTime(end);
//     Time overTime = calculateOverTime(start, end, breakTime);
//     EXPECT_EQ(overTime.hours, -4);
//     EXPECT_EQ(overTime.minutes, 45);
// }

// TEST_F(WorkingHoursCalculatorTest, OverTime_EndOfLunch)
// {
//     /* 昼休み終了時刻に退勤 */
//     Time start = {9, 00};
//     Time end = {13, 00};
//     Time breakTime = calculateBreakTime(end);
//     Time overTime = calculateOverTime(start, end, breakTime);
//     EXPECT_EQ(overTime.hours, -4);
//     EXPECT_EQ(overTime.minutes, 45);
// }

// TEST_F(WorkingHoursCalculatorTest, OverTime_Normal)
// {
//     /* 定時に退勤 */
//     Time start = {9, 00};
//     Time end = {17, 45};
//     Time breakTime = calculateBreakTime(end);
//     Time overTime = calculateOverTime(start, end, breakTime);
//     EXPECT_EQ(overTime.hours, 0);
//     EXPECT_EQ(overTime.minutes, 00);
// }

// TEST_F(WorkingHoursCalculatorTest, OverTime_StartOvertime)
// {
//     /* 残業開始時刻に退勤 */
//     Time start = {9, 00};
//     Time end = {18, 00};
//     Time breakTime = calculateBreakTime(end);
//     Time overTime = calculateOverTime(start, end, breakTime);
//     EXPECT_EQ(overTime.hours, 0);
//     EXPECT_EQ(overTime.minutes, 00);
// }

// TEST_F(WorkingHoursCalculatorTest, OverTime_Overtime)
// {
//     /* 残業して退勤 */
//     Time start = {9, 00};
//     Time end = {19, 00};
//     Time breakTime = calculateBreakTime(end);
//     Time overTime = calculateOverTime(start, end, breakTime);
//     EXPECT_EQ(overTime.hours, 1);
//     EXPECT_EQ(overTime.minutes, 00);
// }
