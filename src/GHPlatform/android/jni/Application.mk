APP_PROJECT_PATH := $(call my-dir)
APP_BUILD_SCRIPT := $(call my-dir)/Android.mk
APP_MODULES      := GHPlatform
APP_OPTIM	 := release
APP_STL := c++_static
APP_PLATFORM := android-19
//APP_ABI := armeabi-v7a arm64-v8a
APP_ABI := all
APP_CPPFLAGS += -std=c++11

