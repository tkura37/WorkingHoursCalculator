#include <gtest/gtest.h>
#include <vector>
#include "WorkingHoursCalculator.h"

class WorkingHoursCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

    /* parseTime()のテストINPUT・OUTPUT */
    struct TestCase_parseTime {
        const char *timeStr;
        int expectedParseResult;
        Time expectedTime;
    };
    std::vector<TestCase_parseTime> testCases_parseTime = {
        {"12:00", 2, {12, 0}},      /* 正常系 */
        {"25:00", 1, {25, 0}},      /* 無効な時間 */
        {"12:60", 1, {12, 60}},     /* 無効な分 */
        {"abcd", 0, {0, 0}},        /* 不正なフォーマット */
    };

    /* calculateBreakTime()とcalculateOverTime()のテストINPUT・OUTPUT */
    struct TestCase_BreakAndOverTime {
        Time startTime;
        Time endTime;
        Time expectedBreakTime;
        Time expectedOverTime;
    };
    std::vector<TestCase_BreakAndOverTime> testCases_BreakAndOverTime = {
        {{8, 30}, {11, 15}, {0, 0}, {-5, 0}},   /* 昼休み開始時刻前に退勤 */
        {{8, 30}, {12, 15}, {0, 0}, {-4, 0}},   /* 昼休み開始時刻に退勤 */
        {{8, 30}, {13, 0}, {0, 0}, {-4, 0}},    /* 昼休み終了時刻に退勤 */
        {{8, 30}, {16, 0}, {0, 45}, {-1, 0}},   /* 定時前に退勤 */
        {{8, 30}, {17, 0}, {0, 45}, {0, 0}},    /* 定時に退勤 */
        {{8, 30}, {17, 15}, {0, 45}, {0, 0}},   /* 残業開始時刻に退勤 */
        {{8, 30}, {18, 15}, {1, 0}, {1, 0}},    /* 残業して退勤 */
    };
};

/* parseTime()のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_parseTime) {
    for (const auto& testCase : testCases_parseTime) {
        Time time;
        int parseResult = parseTime(testCase.timeStr, &time);

        EXPECT_EQ(parseResult, testCase.expectedParseResult)    << "  Failed for input: " << testCase.timeStr;
        EXPECT_EQ(time.hours, testCase.expectedTime.hours)      << "  Failed for input: " << testCase.timeStr;
        EXPECT_EQ(time.minutes, testCase.expectedTime.minutes)  << "  Failed for input: " << testCase.timeStr;
    }
}

/* calculateBreakTime()とcalculateOverTime() のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_BreakAndOverTime) {
    for (const auto& testCase : testCases_BreakAndOverTime) {
        Time calculatedBreakTime = calculateBreakTime(testCase.endTime);
        Time calculatedOverTime = calculateOverTime(testCase.startTime, testCase.endTime, calculatedBreakTime);

        EXPECT_EQ(calculatedBreakTime.hours, testCase.expectedBreakTime.hours)      << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedBreakTime.minutes, testCase.expectedBreakTime.minutes)  << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedOverTime.hours, testCase.expectedOverTime.hours)        << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedOverTime.minutes, testCase.expectedOverTime.minutes)    << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
    }
}
