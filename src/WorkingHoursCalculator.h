#ifndef WORKINGHOURS_CALCULATOR_H
#define WORKINGHOURS_CALCULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* マクロ定義 */
#define STANDARD_WORK_HOURS     "7:45"  // 標準の勤務時間

#define LUNCHBREAK_START_TIME   "12:15" // 昼休みの開始時刻
#define LUNCHBREAK_END_TIME     "13:00" // 昼休みの終了時刻

#define STANDARD_END_TIME       "17:00" // 定時の退勤時刻
#define OVERTIME_START_TIME     "17:15" // 残業の開始時刻

/* 構造体宣言 */
typedef struct {
    int hours;
    int minutes;
} Time;

typedef struct {
    Time lunchbreakStartTime;
    Time lunchbreakEndTime;
    Time standardEndTime;
    Time overtimeStartTime;
    Time standardWorkTime;
} USER_SETTING;

typedef struct {
    Time startTime;
    Time endTime;
} USER_INPUT;

/* プロトタイプ宣言 */
int parseTime(const char *timeStr, Time *time);
bool isEarlierThan(const Time *timeA, const Time *timeB);
bool validateSettingTimeOrder(const USER_SETTING *userSetting);
Time subtractTime(const Time *timeA, const Time *timeB);
Time addTime(const Time *timeA, const Time *timeB);
void calculateBreakAndOverTime(const USER_INPUT *userInput, const USER_SETTING *userSetting, Time *breakTime, Time *overTime);
float printResult(const char *outputLabel, Time time);

#ifdef __cplusplus
}
#endif

#endif /* WORKINGHOURS_CALCULATOR_H */
