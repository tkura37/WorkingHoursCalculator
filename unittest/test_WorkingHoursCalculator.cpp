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
        /* timeA.hours > timeB.hours */
        {{9, 30}, {8, 00}, {1, 30}},    /* timeA.minutes > timeB.minutes */
        {{9, 30}, {8, 30}, {1, 00}},    /* timeA.minutes = timeB.minutes */
        {{9, 00}, {8, 30}, {0, 30}},    /* timeA.minutes < timeB.minutes */
        /* timeA.hours = timeB.hours */
        {{9, 30}, {9, 00}, {0, 30}},    /* timeA.minutes > timeB.minutes */
        {{9, 30}, {9, 30}, {0, 00}},    /* timeA.minutes = timeB.minutes */
        {{9, 00}, {9, 30}, {0, -30}},   /* timeA.minutes < timeB.minutes */
        /* timeA.hours < timeB.hours */
        {{8, 30}, {9, 00}, {0, -30}},   /* timeA.minutes > timeB.minutes */
        {{8, 30}, {9, 30}, {-1, 00}},   /* timeA.minutes = timeB.minutes */
        {{8, 00}, {9, 30}, {-1, 30}},   /* timeA.minutes < timeB.minutes */
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
        /* 昼休み開始時刻までに出勤した場合 */
        {{8, 30}, {11, 15}, {0, 00}, {-5, 00}},     /* 昼休み開始時刻前に退勤 */
        {{8, 30}, {12, 15}, {0, 00}, {-4, 00}},     /* 昼休み開始時刻に退勤 */
        {{8, 30}, {13, 00}, {0, 00}, {-4, 00}},     /* 昼休み終了時刻に退勤 */
        {{8, 30}, {16, 00}, {0, 45}, {-1, 00}},     /* 定時前に退勤 */
        {{8, 30}, {17, 00}, {0, 45}, {0, 00}},      /* 定時に退勤 */
        {{8, 30}, {17, 15}, {0, 45}, {0, 00}},      /* 残業開始時刻に退勤 */
        {{8, 30}, {18, 15}, {1, 00}, {1, 00}},      /* 残業して退勤 */
        /* 昼休み開始時刻までに出勤した場合(フレックス) */
        {{8, 45}, {11, 15}, {0, 00}, {-5, 15}},     /* 昼休み開始時刻前に退勤 */
        {{8, 45}, {12, 15}, {0, 00}, {-4, 15}},     /* 昼休み開始時刻に退勤 */
        {{8, 45}, {13, 00}, {0, 00}, {-4, 15}},     /* 昼休み終了時刻に退勤 */
        {{8, 45}, {16, 00}, {0, 45}, {-1, 15}},     /* 定時前に退勤 */
        {{8, 45}, {17, 00}, {0, 45}, {0, -15}},     /* 定時に退勤 */
        {{8, 45}, {17, 15}, {0, 45}, {0, -15}},     /* 残業開始時刻に退勤 */
        {{8, 45}, {18, 30}, {1, 00}, {1, 00}},      /* 残業して退勤 */
        /* 昼休み開始時刻～昼休み終了時刻までに出勤した場合(午前休) */
        {{12, 30}, {13, 00}, {0, 00}, {0, 00}},     /* 昼休み終了時刻に退勤 */
        {{12, 30}, {16, 00}, {0, 00}, {-4, 45}},    /* 定時前に退勤 */
        {{12, 30}, {17, 00}, {0, 00}, {-3, 45}},    /* 定時に退勤 */
        {{12, 30}, {17, 15}, {0, 00}, {-3, 45}},    /* 残業開始時刻に退勤 */
        {{12, 30}, {20, 45}, {0, 00}, {0, 00}},     /* 標準勤務時間分勤務して退勤 */
        {{12, 30}, {22, 45}, {1, 00}, {1, 00}},     /* 標準勤務時間分+1時間残業して退勤 */
        /* 昼休み終了時刻～定時までに出勤した場合 */
        {{14, 00}, {16, 00}, {0, 00}, {-5, 45}},    /* 定時前に退勤 */
        {{14, 00}, {17, 00}, {0, 00}, {-4, 45}},    /* 定時に退勤 */
        {{14, 00}, {17, 15}, {0, 00}, {-4, 45}},    /* 残業開始時刻に退勤 */
        {{14, 00}, {21, 45}, {0, 00}, {0, 00}},     /* 標準勤務時間分勤務して退勤 */
        {{14, 00}, {23, 45}, {1, 00}, {1, 00}},     /* 標準勤務時間分+残業勤務して退勤 */
    };

    for (const auto& testCase : testCases_BreakAndOverTime) {
        Time calculatedBreakTime = calculateBreakTime(testCase.startTime, testCase.endTime);
        Time calculatedOverTime = calculateOverTime(testCase.startTime, testCase.endTime, calculatedBreakTime);

        EXPECT_EQ(calculatedBreakTime.hours, testCase.expectedBreakTime.hours)      << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedBreakTime.minutes, testCase.expectedBreakTime.minutes)  << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedOverTime.hours, testCase.expectedOverTime.hours)        << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
        EXPECT_EQ(calculatedOverTime.minutes, testCase.expectedOverTime.minutes)    << "  Failed for input: " << testCase.startTime.hours << ":" << testCase.startTime.minutes << " - " << testCase.endTime.hours << ":" << testCase.endTime.minutes;
    }
}
/* printResult()のテスト */
TEST_F(WorkingHoursCalculatorTest, Test_printResult) {
    struct TestCase_printResult {
        const char *outputLabel;
        Time time;
        float expectedResult;
    };
    std::vector<TestCase_printResult> testCases_printResult = {
        /* 休憩時間のテストケース */
        {"休憩時間: ", {0, 00}, 0.00},
        {"休憩時間: ", {0, 45}, 0.75},
        {"休憩時間: ", {1, 00}, 1.00},
        /* 残業時間のテストケース */
        {"残業時間: ", {0, 00}, 0.00},      /* hours = 0, minutes = 0 */
        {"残業時間: ", {0, 30}, 0.50},      /* hours = 0, minutes > 0 */
        {"残業時間: ", {0, -30}, -0.50},    /* hours = 0, minutes < 0 */
        {"残業時間: ", {1, 00}, 1.00},      /* hours > 0, minutes = 0 */
        {"残業時間: ", {1, 30}, 1.50},      /* hours > 0, minutes > 0 */
        {"残業時間: ", {-1, 00}, -1.00},    /* hours < 0, minutes = 0 */
        {"残業時間: ", {-1, 30}, -1.50},    /* hours < 0, minutes > 0 */
    };

    for (const auto& testCase : testCases_printResult) {
        float result = printResult(testCase.outputLabel, testCase.time);
        EXPECT_EQ(result, testCase.expectedResult) << "  Failed for input: " << testCase.outputLabel << testCase.time.hours << ", " << testCase.time.minutes;
    }
}