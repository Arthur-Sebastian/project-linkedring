#include <iostream>
#include <sstream>
#include <string>
#include <gtest/gtest.h>
#include "bi_ring.hpp"

#define loop_up(startpoint, endpoint) for(int i = startpoint; i < endpoint; i++)
#define loop_dn(startpoint, endpoint) for(int i = startpoint; i > endpoint; i--)

class RingTests : public ::testing::Test {
public:
	bi_ring<int, int>* t0;
	bi_ring<int, int>* t1;
	bi_ring<int, int>* t2;
	bi_ring<int, int>::const_iterator* itr_v;
	bi_ring<int, int>::const_iterator* itr_e;
	void SetUp() {
		t0 = new bi_ring<int, int>;
		t1 = new bi_ring<int, int>(0, 1);
		loop_up(0, 9) {
			t1 -> push(i+1, i+2);
		}
		t2 = new bi_ring<int, int>(*t1);
		bi_ring<int, int>::iterator itr1(*t2, 9);
		bi_ring<int, int>::iterator itr2(*t2, 1);
		t2 -> swap(itr1, itr2);
		t2 -> push(2, 5);
		itr_v = new bi_ring<int, int>::const_iterator(*t1);
		itr_e = new bi_ring<int, int>::const_iterator(*t2);
	}
	void TearDown() {
		delete t0;
		delete t1;
		delete t2;
		delete itr_v;
		delete itr_e;
	}
};

TEST_F(RingTests, Constructor) {
	EXPECT_EQ(0, t0 -> size());
}

TEST_F(RingTests, InitConstructor) {
	EXPECT_EQ(9, t1 -> get_info(8));
}

TEST_F(RingTests, CopyConstructor) {
	EXPECT_EQ(11, t2 -> size());
}

TEST_F(RingTests, OperatorEquality) {
	bi_ring<int, int> newRing(*t1);
	bi_ring<int, int> emptyRing;
	EXPECT_EQ(newRing == *t1, true);
	EXPECT_EQ(newRing == *t2, false);
	EXPECT_EQ(*t2 == *t2, true);
	EXPECT_EQ(emptyRing == *t0, true);
}

TEST_F(RingTests, OperatorInequality) {
	bi_ring<int, int> newRing(*t1);
	bi_ring<int, int> emptyRing;
	EXPECT_EQ(newRing != *t1, false);
	EXPECT_EQ(newRing != *t2, true);
	EXPECT_EQ(*t2 != *t2, false);
	EXPECT_EQ(emptyRing != *t0, false);
}

TEST_F(RingTests, OperatorAssignment) {
	bi_ring<int, int> newRing;
	newRing = *t1;
	EXPECT_EQ(newRing, *t1);
	newRing = newRing;
	EXPECT_NE(0, newRing.size());
}

TEST_F(RingTests, OperatorAddition) {
	bi_ring<int, int> newRing;
	loop_up(10, 19) {
		newRing.push(i, i+1);
	}
	newRing = *t1 + newRing;
	bi_ring<int, int>::const_iterator itr(newRing);
	loop_up(0, 19) {
		EXPECT_EQ(itr.key(), i);
		EXPECT_EQ(itr.info(), i+1);
		itr++;
	}
}

TEST_F(RingTests, OperatorAdditionAssignment) {
	//create a new ring
	bi_ring<int, int> newRing;
	loop_up(10, 19) {
		newRing.push(i, i+1);
	}
	//sum up ring from fixture with new
	*t1 += newRing;
	//check every element of the newly modified one
	bi_ring<int, int>::const_iterator itr(*t1);
	loop_up(0, 19) {
		EXPECT_EQ(itr.key(), i);
		EXPECT_EQ(itr.info(), i+1);
		itr++;
	}
}

TEST_F(RingTests, OperatorInsertion) {
	//insert into the custom stream and compare output
	std::stringstream str;
	str << *t1;
	std::string output = str.str();
	EXPECT_EQ(output, "[0] 1\n[1] 2\n[2] 3\n[3] 4\n[4] 5\n[5] 6\n[6] 7\n[7] 8\n[8] 9\n[9] 10\n");
}

TEST_F(RingTests, Push) {
	//push and check if successful
	t0 -> push(1, 5);
	EXPECT_EQ(5, t0 -> get_info(1));
}

TEST_F(RingTests, InsertAfter) {
	//one-element case
	bi_ring<int, int> oneElement(1, 9);
	bi_ring<int, int>::iterator itr1(oneElement, 1);
	itr1 = oneElement.insert_after(5, 6, itr1);
	EXPECT_EQ(5, itr1.key());
	EXPECT_EQ(6, itr1.info());
	itr1--;
	EXPECT_EQ(1, itr1.key());
	EXPECT_EQ(9, itr1.info());
	//multiple elements
	bi_ring<int, int>::iterator itr2(*t1, 1);
	itr2 = t1 -> insert_after(5, 6, itr2);
	EXPECT_EQ(5, itr2.key());
	EXPECT_EQ(6, itr2.info());
	itr2--;
	EXPECT_EQ(1, itr2.key());
	EXPECT_EQ(2, itr2.info());
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		t1 -> insert_after(5, 6, empty);
	}, std::domain_error);
}

TEST_F(RingTests, InsertBefore) {
	//insert and check next element after new
	bi_ring<int, int>::iterator itr(*t1, 1);
	itr = t1 -> insert_before(54, 49, itr);
	EXPECT_EQ(54, itr.key());
	EXPECT_EQ(49, itr.info());
	itr++;
	EXPECT_EQ(1, itr.key());
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		t1 -> insert_before(5, 6, empty);
	}, std::domain_error);
}

TEST_F(RingTests, Replace) {
	//check if replace was successful
	bi_ring<int, int>::iterator itr(*t1, 5);
	t1 -> replace(60, 70, itr);
	EXPECT_EQ(60, itr.key());
	EXPECT_EQ(70, itr.info());
	//check return
	bi_ring<int, int>::const_iterator empty;
	EXPECT_FALSE(t1 -> replace(5, 6, empty));
}

TEST_F(RingTests, Purge) {
	//clean the whole list
	t1 -> purge();
	EXPECT_EQ(0, t1 -> size());
	EXPECT_EQ(false, t0 -> purge());
}

TEST_F(RingTests, RemoveAfter) {
	//ordinary element in multi-element ring
	bi_ring<int, int>::iterator itr(*t1, 0);
	itr = t1 -> remove_after(itr);
	EXPECT_EQ(itr.key(), 2);
	//one element case
	t0 -> push(5, 7);
	bi_ring<int, int>::iterator itr2(*t0, 5);
	itr2 = t0 -> remove_after(itr2);
	EXPECT_EQ(itr2.valid(), false);
	EXPECT_EQ(t0 -> empty(), true);
	//remove any in multi-element ring
	itr = bi_ring<int, int>::iterator(*t1, 9);
	itr = t1 -> remove_after(itr);
	EXPECT_EQ(itr.valid(), true);
	std::stringstream str;
	str << *t1;
	std::string output = str.str();
	EXPECT_EQ(output, "[2] 3\n[3] 4\n[4] 5\n[5] 6\n[6] 7\n[7] 8\n[8] 9\n[9] 10\n");
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		t1 -> remove_after(empty);
	}, std::domain_error);
}

TEST_F(RingTests, RemoveBefore) {
	//ordinary element in multi-element ring
	bi_ring<int, int>::iterator itr(*t1, 6);
	itr = t1 -> remove_before(itr);
	EXPECT_EQ(itr.key(), 6);
	//one element case
	t0 -> push(5, 7);
	bi_ring<int, int>::iterator itr2(*t0, 5);
	itr2 = t0 -> remove_before(itr2);
	EXPECT_EQ(itr2.valid(), false);
	EXPECT_EQ(t0 -> empty(), true);
	//remove any in multi-element ring
	itr = bi_ring<int, int>::iterator(*t1, 1);
	itr = t1 -> remove_before(itr);
	EXPECT_EQ(itr.valid(), true);
	std::stringstream str;
	str << *t1;
	std::string output = str.str();
	EXPECT_EQ(output, "[1] 2\n[2] 3\n[3] 4\n[4] 5\n[6] 7\n[7] 8\n[8] 9\n[9] 10\n");
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		t1 -> remove_before(empty);
	}, std::domain_error);
}

TEST_F(RingTests, Remove) {
	//ordinary element in multi-element ring
	bi_ring<int, int>::iterator itr(*t1, 6);
	itr = t1 -> remove(itr);
	EXPECT_EQ(itr.key(), 7);
	//one element case
	t0 -> push(5, 7);
	bi_ring<int, int>::iterator itr2(*t0, 5);
	itr2 = t0 -> remove(itr2);
	EXPECT_EQ(itr2.valid(), false);
	EXPECT_EQ(t0 -> empty(), true);
	//remove any in multi-element ring
	itr = bi_ring<int, int>::iterator(*t1, 0);
	itr = t1 -> remove(itr);
	EXPECT_EQ(itr.valid(), true);
	std::stringstream str;
	str << *t1;
	std::string output = str.str();
	EXPECT_EQ(output, 
	"[1] 2\n[2] 3\n[3] 4\n[4] 5\n[5] 6\n[7] 8\n[8] 9\n[9] 10\n");
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		t1 -> remove(empty);
	}, std::domain_error);
}

TEST_F(RingTests, Empty) {
	EXPECT_EQ(t0 -> empty(), true);
	EXPECT_EQ(t1 -> empty(), false);
}

TEST_F(RingTests, Size) {
	EXPECT_EQ(t0 -> size(), 0);
	EXPECT_EQ(t1 -> size(), 10);
	EXPECT_EQ(t2 -> size(), 11);
}

TEST_F(RingTests, Print) {
	testing::internal::CaptureStdout();
	t1 -> print();
	t0 -> print();
	std::string output = testing::internal::GetCapturedStdout();
	EXPECT_EQ(output, 
	"Key 0: 1\nKey 1: 2\nKey 2: 3\nKey 3: 4\nKey 4: 5\nKey 5: 6"\
	"\nKey 6: 7\nKey 7: 8\nKey 8: 9\nKey 9: 10\nRing empty!\n");
}

TEST_F(RingTests, GetInfo) {
	//try with non-existent element
	EXPECT_THROW({
		t0 -> get_info(0);
	}, std::invalid_argument);
	//try with existing elements
	EXPECT_EQ(t1 -> get_info(9), 10);
	EXPECT_EQ(t2 -> get_info(7), 8);
}

TEST_F(RingTests, BeginEnd) {
	//use a range based loop to test
	std::stringstream str;
	for(int info : *t1) {
		str << info;
	}
	//see if output matches
	std::string output = str.str();
	EXPECT_EQ(output, "123456789");
}

TEST_F(RingTests, ClearInfo) {
	//check if every element got replaced
	t1 -> clear_info(0);
	do{
		EXPECT_EQ(**itr_v, 0);
	} while(*itr_v != t1 -> begin());
}

TEST_F(RingTests, Swap) {
	bi_ring<int, int>::iterator empty;
	EXPECT_FALSE(t1 -> swap(*itr_v, empty));
	bi_ring<int, int>::iterator itr(*itr_v);
	itr++;
	EXPECT_TRUE(t1 -> swap(*itr_v, itr));	
	std::stringstream str;
	str << *t1;
	std::string output = str.str();
	EXPECT_EQ(output, 
	"[1] 2\n[0] 1\n[2] 3\n[3] 4\n[4] 5\n[5] 6\n[6] 7\n[7] 8\n[8] 9\n[9] 10\n");
}

TEST_F(RingTests, ConstIteratorDefaultConstructor) {
	//try dereferencing invalid iterator to nothing
	bi_ring<int, int>::const_iterator itr;
	EXPECT_THROW({
		*itr;
	}, std::domain_error);
}

TEST_F(RingTests, ConstIteratorCopyConstructor) {
	//build an initialised iterator
	bi_ring<int, int>::const_iterator itr(*itr_v);
	EXPECT_EQ(itr.key(), 0);
}

TEST_F(RingTests, ConstIteratorInitConstructorRing) {
	//build an initialised iterator
	bi_ring<int, int>::const_iterator itr(*t1);
	EXPECT_EQ(itr.key(), 0);
}

TEST_F(RingTests, ConstIteratorInitConstructorKey) {
	//build an initialised iterator
	bi_ring<int, int>::const_iterator itr(*t1, 5);
	EXPECT_EQ(itr.key(), 5);
}

TEST_F(RingTests, ConstIteratorAssignmentOperator) {
	//build an initialised iterator
	bi_ring<int, int>::const_iterator itr;
	itr = (*itr_v);
	EXPECT_EQ(itr.key(), 0);
}

TEST_F(RingTests, ConstIteratorIncrement) {
	//compare the workings of operators
	EXPECT_EQ(*itr_v, ++(*itr_v));
	EXPECT_NE(*itr_v, (*itr_v)++);
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		empty++;
	}, std::domain_error);
}

TEST_F(RingTests, ConstIteratorDecrement) {
	//compare the workings of operators
	EXPECT_EQ(*itr_v, --(*itr_v));
	EXPECT_NE(*itr_v, (*itr_v)--);
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		empty--;
	}, std::domain_error);
}

TEST_F(RingTests, ConstIteratorDereference) {
	int keyProto = 0;
	while(*itr_v != t1 -> begin()) {
		EXPECT_EQ(**itr_v, keyProto);
		keyProto++;
		(*itr_v)--;
	}
	//check exception
	bi_ring<int, int>::const_iterator empty;
	EXPECT_THROW({
		*empty;
	}, std::domain_error);
}

TEST_F(RingTests, ConstIteratorEqualityOperator) {
	//build a copy iterator to compare
	bi_ring<int, int>::const_iterator itr(*itr_v);
	EXPECT_EQ(itr == *itr_v, true);
	EXPECT_EQ(itr == *itr_e, false);
}

TEST_F(RingTests, ConstIteratorInequalityOperator) {
	//build a copy iterator to compare
	bi_ring<int, int>::const_iterator itr(*itr_v);
	EXPECT_EQ(itr != *itr_v, false);
	EXPECT_EQ(itr != *itr_e, true);
}

TEST_F(RingTests, ConstIteratorKeyGetter) {
	//build an initialised iterator
	bi_ring<int, int>::const_iterator itr(*t1, 5);
	EXPECT_EQ(itr.key(), 5);
}

TEST_F(RingTests, ConstIteratorInfoGetter) {
	//build an initialised iterator
	bi_ring<int, int>::const_iterator itr(*t1, 5);
	EXPECT_EQ(itr.info(), 6);
}

TEST_F(RingTests, ConstIteratorValid) {
	bi_ring<int, int>::const_iterator empty;
	//compare initialised against new
	EXPECT_EQ(empty.valid(), false);
	EXPECT_EQ(itr_v -> valid(), true);
}

TEST_F(RingTests, IteratorCastConstructor) {
	bi_ring<int, int>::iterator itr(*itr_v);
	//compare initialised against source
	EXPECT_EQ(itr.key(), 0);
}

TEST_F(RingTests, IteratorDereferenceOperator) {
	bi_ring<int, int>::iterator itr;
	EXPECT_THROW({
		*itr;
	}, std::domain_error);
	itr = t1 -> begin();
	//compare initialised against source
	EXPECT_EQ(*itr, 1);
}

TEST_F(RingTests, IteratorKeyAccessor) {
	//build an initialised iterator
	bi_ring<int, int>::iterator itr(*t1, 5);
	EXPECT_EQ(itr.key(), 5);
	itr.key() = 10;
	EXPECT_EQ(itr.key(), 10);
}

TEST_F(RingTests, IteratorInfoAccessor) {
	//build an initialised iterator
	bi_ring<int, int>::iterator itr(*t1, 5);
	EXPECT_EQ(itr.info(), 6);
	itr.info() = 20;
	EXPECT_EQ(itr.info(), 20);
}

TEST_F(RingTests, Shuffle) {
	//test exceptions first
	bi_ring<int, int> result;
	try {
		result = shuffle<int, int>(*t1, 0, *t2, 5, 2);
	}
	catch(const std::exception& e) {
		EXPECT_TRUE(true);
	}
	result = shuffle<int, int>(*t1, 2, *t2, 3, 3);
	std::stringstream str;
	str << result;
	std::string output = str.str();
	EXPECT_EQ(output, 
	"[0] 1\n[1] 2\n[0] 1\n[9] 10\n[2] 3\n[2] 3\n[3] 4\n"\
	"[3] 4\n[4] 5\n[5] 6\n[4] 5\n[5] 6\n[6] 7\n[7] 8\n[8] 9\n");
}

int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
