#include <gmock/gmock.h>
#include "ring_buffer.h"


int main(int argc, char** argv)
{
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}

typedef ring_buffer<1024, 4> TestRingBuffer;

TEST(ring_buffer_test, constructor_initializes_correctly)
{
	TestRingBuffer trb;
	ASSERT_EQ(1024, trb.free);
}

TEST(ring_buffer_test, alloc_simple)
{
	ring_buffer<16, 4> rb;
	void* ptr = rb.alloc(3);
	ASSERT_NE(nullptr, ptr);
	ASSERT_EQ(rb.free, 16 - 4);

	rb.clear(3);
	ASSERT_EQ(rb.free, 16);
}

TEST(ring_buffer_test, two_stage_alloc)
{
	ring_buffer<16, 4> rb;
	void* ptr0 = rb.alloc(8);
	void* ptr1 = rb.alloc(8);

	ASSERT_EQ(0, rb.free);

	void* ptr2 = rb.alloc(8);
	ASSERT_EQ(nullptr, ptr2);
	rb.clear(4);
	void* ptr3 = rb.alloc(2);
	ASSERT_NE(nullptr, ptr3);
	rb.clear(6);
	void* ptr4 = rb.alloc(4);
	ASSERT_NE(nullptr, ptr4);
}

TEST(ring_buffer_test, three_stage_alloc)
{
}