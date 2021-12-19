/*
	Authored by 
	Artur Sebastian Miller
	Nov, 2021
*/

#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

//dependencies
#include <iostream>
#include <stdexcept>

const char* nulldef_exc = "Invalid iterator dereferencing attempt.";
const char* itrinvl_exc = "Operation forbidden for invalid iterator.";

template <typename Key, typename Info>
class bi_ring;
template <typename Key, typename Info>
std::ostream& operator<<(std::ostream& str, const bi_ring<Key,Info>& seq);

template <typename Key, typename Info>
class bi_ring {
public:
	//(de)constructors
	bi_ring(); //DONE
	bi_ring(const Key& key, const Info& inf); //DONE
	bi_ring(const bi_ring<Key, Info>& src);   //DONE
	bi_ring(bi_ring&& src);
	~bi_ring(); //DONE
	
	//operators
	bool operator==(const bi_ring<Key, Info>& cmp) const; //DONE
	bool operator!=(const bi_ring<Key, Info>& cmp) const; //DONE
	bi_ring<Key, Info>& operator=(const bi_ring<Key, Info>& src); //DONE
	bi_ring<Key, Info>& operator=(bi_ring<Key, Info>&& src);
	bi_ring<Key, Info> operator+(const bi_ring<Key, Info>& src) const; //DONE
	bi_ring<Key, Info>& operator+=(const bi_ring<Key, Info>& src); //DONE
	friend std::ostream& operator<< <Key, Info>(std::ostream& str,
					 	    const bi_ring<Key, Info>& seq); //DONE
					 	     	    
	//iterators
	class const_iterator; //DONE
	class iterator; //DONE
	
	//insertion methods
	iterator push(const Key& key, const Info& inf); //DONE
	iterator insert_after(const Key& key,  const Info& inf, 
			      iterator what); //DONE
	iterator insert_before(const Key& key,  const Info& inf, 
			       iterator what); //DONE
	bool replace(const Key& key,   const Info& inf, 
		     iterator what); //DONE
	
	//removal methods
	bool purge(); //DONE
	iterator remove_after(iterator what); //DONE
	iterator remove_before(iterator what); //DONE
	iterator remove(iterator what); //DONE
	
	//getter methods
	bool empty() const; //DONE
	unsigned int size() const; //DONE
	void print() const; //DONE
	Info get_info(const Key& key, int n_key = 1) const; //DONE
	const_iterator begin() const; //DONE
	const_iterator end() const; //DONE
	
	//utility methods
	bool clear_info(const Info& filler); //DONE
	bool swap(iterator what,
		  iterator dest); //DONE
	
private:
	//storage members
	unsigned int length;
	struct Element {
		Key key;
		Info info;
		Element* next;
		Element* prev;
		bool operator==(const Element& cmp) const; //DONE
		bool operator!=(const Element& cmp) const; //DONE
	};
	Element* any;
	//helper methods
	Element* _find(const Key& key, int n_key = 1) const; //DONE
	bool _clone(const bi_ring<Key, Info>& src); //DONE
};

template <typename Key, typename Info>
class bi_ring<Key, Info>::const_iterator {

friend bi_ring<Key, Info>;

public:
	const_iterator(); //DONE
	const_iterator(const bi_ring<Key, Info>& of); //DONE
	const_iterator(const const_iterator& src); //DONE
	const_iterator(const bi_ring<Key, Info>& of,
		       int key, int n_key = 1); //DONE
	
	const_iterator& operator=(const const_iterator& src); //DONE
	const_iterator& operator++();   //DONE
	const_iterator operator++(int ops); //DONE
	const_iterator& operator--();   //DONE
	const_iterator operator--(int ops); //DONE
	Info operator*() const;   //DONE
	bool operator==(const const_iterator& itr) const; //DONE
	bool operator!=(const const_iterator& itr) const; //DONE
	
	//custom getters
	Key key() const; //DONE
	Info info() const; //DONE
	bool valid() const; //DONE
private:
	Element* current;
	const_iterator(Element* at); //DONE
};

template <typename Key, typename Info>
class bi_ring<Key, Info>::iterator : public bi_ring<Key, Info>::const_iterator {

friend bi_ring<Key, Info>;

public:
	iterator(const const_iterator& src); //DONE
	using const_iterator::const_iterator;
	
	Info& operator*(); //DONE
	
	//custom getters
	Key& key(); //DONE
	Info& info(); //DONE
	
private:
	//insertion methods
	iterator insert_after(const Key& key, const Info& inf); //DONE
	iterator insert_before(const Key& key, const Info& inf); //DONE
	//deletion methods
	iterator remove(bi_ring<Key, Info>& parent); //DONE
};

template <typename Key, typename Info>
bi_ring<Key, Info> shuffle(const bi_ring<Key, Info>& first, unsigned int fcnt,
			   const bi_ring<Key, Info>& secnd, unsigned int scnt,
			   unsigned int reps); //DONE

#include "bi_ring_impl.hpp"

#endif
