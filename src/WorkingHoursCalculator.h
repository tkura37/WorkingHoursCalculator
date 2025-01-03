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

/* マクロ定義(単体テスト用) */
// #define STANDARD_WORK_HOURS     "8:00"  // 標準の勤務時間

// #define LUNCHBREAK_START_TIME   "12:15" // 昼休みの開始時刻
// #define LUNCHBREAK_END_TIME     "13:00" // 昼休みの終了時刻

// #define STANDARD_END_TIME       "17:45" // 定時の退勤時刻
// #define OVERTIME_START_TIME     "18:00" // 残業の開始時刻

/* 構造体宣言 */
typedef struct {
    int hours;
    int minutes;
} Time;

/* プロトタイプ宣言 */
int parseTime(const char *timeStr, Time *time);
Time calculateBreakTime(Time end);
Time calculateOverTime(Time start, Time end, Time breakTime);
void printResult(const char *outputLabel, const Time *time);

#ifdef __cplusplus
}
#endif

#endif /* WORKINGHOURS_CALCULATOR_H */
