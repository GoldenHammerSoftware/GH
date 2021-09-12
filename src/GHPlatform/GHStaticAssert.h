// Copyright Golden Hammer Software
#pragma once

//Implementation taken from http://www.pixelbeat.org/programming/gcc/static_assert.html

#pragma warning(disable:4804)

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#define GHSTATICASSERT(e) enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }
