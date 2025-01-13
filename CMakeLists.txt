# CMakeの最小バージョン
cmake_minimum_required(VERSION 3.10)

# プロジェクト名
project(WorkingHoursCalculator)

# C++のバージョン指定
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# カバレッジ計測用オプションの追加
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

# GoogleTestのダウンロードと設定
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# CTestの有効化
enable_testing()

# ソースファイルの相対パスを指定
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/WorkingHoursCalculator.c
)

# テスト用ソースファイルの相対パスを指定
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/unittest/test_WorkingHoursCalculator.cpp
)

# メインソースコードをライブラリとしてビルド
add_library(workinghours_calculator_lib
    ${SOURCES}
)

# ビルド時のプリプロセッサ定義追加
target_compile_definitions(workinghours_calculator_lib PRIVATE TESTING)

# テスト実行ファイル
add_executable(workinghours_calculator_test
    ${TEST_SOURCES}
)

# インクルードディレクトリの設定
target_include_directories(workinghours_calculator_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_include_directories(workinghours_calculator_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
)

# リンク設定
target_link_libraries(workinghours_calculator_test
    PRIVATE
    GTest::gtest_main
    workinghours_calculator_lib
)

include(GoogleTest)
gtest_discover_tests(workinghours_calculator_test)
