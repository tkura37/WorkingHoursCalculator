#include <gtest/gtest.h>
#include <vector>
#include "WorkingHoursCalculator.h"

class WorkingHoursCalculatorTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

/* parseTime()のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_parseTime) {
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

    for (const auto& testCase : testCases_parseTime) {
        Time time = {0, 0};
        int parseResult = parseTime(testCase.timeStr, &time);

        EXPECT_EQ(parseResult, testCase.expectedParseResult)    << "  Failed for input: " << testCase.timeStr;
        EXPECT_EQ(time.hours, testCase.expectedTime.hours)      << "  Failed for input: " << testCase.timeStr;
        EXPECT_EQ(time.minutes, testCase.expectedTime.minutes)  << "  Failed for input: " << testCase.timeStr;
    }
}

/* isEarlierThan()のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_isEarlierThan) {
    struct TestCase_isEarlierThan {
        Time timeA;
        Time timeB;
        bool expectedResult;
    };
    std::vector<TestCase_isEarlierThan> testCases_isEarlierThan = {
        {{8, 30}, {9, 0}, true},    /* timeA.hours < timeB.hours */
        {{9, 0}, {8, 30}, false},   /* timeA.hours > timeB.hours */
        {{8, 30}, {8, 30}, true},   /* timeA.hours == timeB.hours && timeA.minutes = timeB.minutes */
        {{12, 0}, {12, 30}, true},  /* timeA.hours == timeB.hours && timeA.minutes < timeB.minutes */
        {{12, 30}, {12, 0}, false}, /* timeA.hours == timeB.hours && timeA.minutes > timeB.minutes */
    };

    for (const auto& testCase : testCases_isEarlierThan) {
        bool result = isEarlierThan(testCase.timeA, testCase.timeB);
        EXPECT_EQ(result, testCase.expectedResult) << "  Failed for input: " << testCase.timeA.hours << ":" << testCase.timeA.minutes << " and " << testCase.timeB.hours << ":" << testCase.timeB.minutes;
    }
}

/* subtractTime()のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_subtractTime) {
    struct TestCase_subtractTime {
        Time timeA;
        Time timeB;
        Time expectedtimeDiff;
    };
    std::vector<TestCase_subtractTime> testCases_subtractTime = {
        /* timeAは常にtimeBと同じかそれより後の時刻が入力される。
            引数の前後関係はソースコード上保証していないが、呼び出し元関数の単体テスト通過をもって前後関係が正しいことを保証する。 */
        {{9, 30}, {8, 00}, {1, 30}},    /* timeA.hours > timeB.hours timeA.minutes > timeB.minutes */
        {{9, 30}, {8, 30}, {1, 00}},    /* timeA.hours > timeB.hours timeA.minutes = timeB.minutes */
        {{9, 00}, {8, 30}, {0, 30}},    /* timeA.hours > timeB.hours timeA.minutes < timeB.minutes */
        {{9, 30}, {9, 00}, {0, 30}},    /* timeA.hours = timeB.hours timeA.minutes > timeB.minutes */
        {{9, 30}, {9, 30}, {0, 00}},    /* timeA.hours = timeB.hours timeA.minutes = timeB.minutes */
    };

    for (const auto& testCase : testCases_subtractTime) {
        Time result = subtractTime(testCase.timeA, testCase.timeB);
        EXPECT_EQ(result.hours, testCase.expectedtimeDiff.hours) << "  Failed for input: " << testCase.timeA.hours << ":" << testCase.timeA.minutes << " and " << testCase.timeB.hours << ":" << testCase.timeB.minutes;
        EXPECT_EQ(result.minutes, testCase.expectedtimeDiff.minutes) << "  Failed for input: " << testCase.timeA.hours << ":" << testCase.timeA.minutes << " and " << testCase.timeB.hours << ":" << testCase.timeB.minutes;
    }
}

/* addTime()のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_addTime) {
    struct TestCase_addTime {
        Time timeA;
        Time timeB;
        Time expectedTimeSum;
    };
    std::vector<TestCase_addTime> testCases_addTime = {
        {{0, 30}, {0, 15}, {0, 45}},    /* timeA.minutes + timeB.minutes < 60 */
        {{0, 30}, {0, 30}, {1, 00}},    /* timeA.minutes + timeB.minutes = 60 */
        {{0, 30}, {0, 45}, {1, 15}},    /* timeA.minutes + timeB.minutes > 60 */
    };

    for (const auto& testCase : testCases_addTime) {
        Time result = addTime(testCase.timeA, testCase.timeB);
        EXPECT_EQ(result.hours, testCase.expectedTimeSum.hours) << "  Failed for input: " << testCase.timeA.hours << ":" << testCase.timeA.minutes << " and " << testCase.timeB.hours << ":" << testCase.timeB.minutes;
        EXPECT_EQ(result.minutes, testCase.expectedTimeSum.minutes) << "  Failed for input: " << testCase.timeA.hours << ":" << testCase.timeA.minutes << " and " << testCase.timeB.hours << ":" << testCase.timeB.minutes;
    }
}

/* calculateBreakTime()とcalculateOverTime() のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_BreakAndOverTime) {
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

    for (const auto& testCase : testCases_BreakAndOverTime) {
        Time calculatedBreakTime = calculateBreakTime(testCase.endTime);
        Time calculatedOverTime = calculateOverTime(testCase.startTime, testCase.endTime, calculatedBreakTime);

        EXPECT_EQ(calculatedBreakTime.hours, testCase.expectedBreakTime.hours)      << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedBreakTime.minutes, testCase.expectedBreakTime.minutes)  << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedOverTime.hours, testCase.expectedOverTime.hours)        << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedOverTime.minutes, testCase.expectedOverTime.minutes)    << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
    }
}
