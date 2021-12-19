/*
	Authored by 
	Artur Sebastian Miller
	Dec, 2021
*/

/*
	ELEMENT OPERATOR OVERLOADS
*/

template <typename Key, typename Info>
bool bi_ring<Key, Info>::Element::operator==(const Element& cmp) const {
	return (key == cmp.key && info == cmp.info);
}

template <typename Key, typename Info>
bool bi_ring<Key, Info>::Element::operator!=(const Element& cmp) const {
	return !(*this == cmp);
}

/*
	(DE)CONSTRUCTORS
*/

template<typename Key, typename Info>
bi_ring<Key, Info>::bi_ring() {
	any = nullptr;
	length = 0;
}

template<typename Key, typename Info>
bi_ring<Key, Info>::bi_ring(const Key& key, const Info& inf) {
	any = nullptr;
	length = 0;
	//push initial first element
	push(key, inf);
}

template<typename Key, typename Info>
bi_ring<Key, Info>::bi_ring(const bi_ring<Key, Info>& src) {
	any = nullptr;
	length = 0;
	//run the clone helper on this object
	_clone(src);
}

template<typename Key, typename Info>
bi_ring<Key, Info>::bi_ring(bi_ring<Key, Info>&& src) {
	//move src content ownership
	any = src.any;
	length = src.length;
	//disconnect source from its content
	src.any = nullptr;
	src.length = 0;
}

template<typename Key, typename Info>
bi_ring<Key, Info>::~bi_ring() {
	purge();
}

/*
	OPERATORS
*/

template<typename Key, typename Info>
bool bi_ring<Key, Info>::operator==(const bi_ring<Key, Info>& cmp) const {
	//check for comparison to self
	if(this == &cmp) {
		return true;
	}
	//check lengths first
	if(size() != cmp.size()) {
		return false;
	}
	//check emptiness
	if(!size()) {
		//both are empty, so equivalent
		return true;
	}
	//then start comparing elements
	const_iterator itr_t(*this);
	const_iterator itr_c(cmp);
	do {
		if(*itr_t.current != *itr_c.current) {
		   return false;
		}
		itr_t++;
		itr_c++;
	} while(itr_t != begin());
	//loop exited without mismatches
	return true;
}

template<typename Key, typename Info>
bool bi_ring<Key, Info>::operator!=(const bi_ring<Key, Info>& cmp) const {
	return !(*this == cmp);
}

template<typename Key, typename Info>
bi_ring<Key, Info>& bi_ring<Key, Info>::operator=(const bi_ring<Key, Info>& src) {
	//clone helper checks every condition
	_clone(src);
	return *this;
}

template<typename Key, typename Info>
bi_ring<Key, Info>& bi_ring<Key, Info>::operator=(bi_ring<Key, Info>&& src) {
	//check for self assign
	if(this != &src) {
		//clear itself
		purge();
		//move ownership of src contents
		any = src.any;
		length = src.length;
		//disconnect source from its content
		src.any = nullptr;
		src.length = 0;
	}
	return *this;
}

template<typename Key, typename Info>
bi_ring<Key, Info> bi_ring<Key, Info>::operator+(const bi_ring<Key, Info>& src) const {
	Element* current = src.any;
	//if target list is empty return unchanged
	if(current == nullptr) return *this;
	//otherwise create new to return combined
	bi_ring<Key, Info> newRing(*this);
	do {
		newRing.push(current -> key, current -> info);
		current = current -> next;
	} while(current != src.any);
	return newRing;
}

template<typename Key, typename Info>
bi_ring<Key, Info>& bi_ring<Key, Info>::operator+=(const bi_ring<Key, Info>& src) {
	//uses + operator to append to self
	*this = *this + src;
	return *this;
}

template<typename Key, typename Info>
std::ostream& operator<< (std::ostream& str, const bi_ring<Key, Info>& seq) {
	typename bi_ring<Key, Info>::Element* current = seq.any;
	//if ring is empty, return
	if(current == nullptr) return str;
	//non empty ring
	typename bi_ring<Key, Info>::const_iterator itr(seq);
	do {
		str << '[' << itr.key()
		<< "] " << itr.info() << "\n";
		itr++;
	} while(itr != seq.begin());
	return str;
}

/*
	INSERTION METHODS
*/

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator
bi_ring<Key, Info>::push(const Key& key, const Info& inf) {
	//add new element to the front
	Element* temp = new Element({key, inf, nullptr, nullptr});
	//non empty list
	if(!empty()) {
		//emplace new element
		temp -> next = any;
		temp -> prev = any -> prev;
		//stitch back tail
		any -> prev -> next = temp;
		any -> prev = temp;
	}
	//first ring element case
	else {
		temp -> next = temp;
		temp -> prev = temp;
		any = temp;
	}
	length++;
	return iterator(temp);
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator
bi_ring<Key, Info>::insert_after(const Key& key,  const Info& inf, 
		  		 iterator what) {
	what = what.insert_after(key, inf);
	length++;
	return what;
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator 
bi_ring<Key, Info>::insert_before(const Key& key,  const Info& inf, 
		  		  iterator what) {
	what = what.insert_before(key, inf);
	length++;
	return what;
}

template<typename Key, typename Info>
bool bi_ring<Key, Info>::replace(const Key& key,   const Info& inf, 
		     		 iterator what) {
	if(!what.valid()) {
		return false;
	}
	what.key() = key;
	what.info() = inf;
	return true;
}

/*
	REMOVAL METHODS
*/

template<typename Key, typename Info>
bool bi_ring<Key, Info>::purge() {
	//check if sequence is empty
	if(empty()) {
		return false;
	}
	//iterate through all elements and deallocate
	Element* current = any;
	Element* next;
	do {
		next = current -> next;
		delete current;
		current = next;
	} while(current != any);
	//mark sequence as empty again
	any = nullptr;
	length = 0;
	return true;
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator
bi_ring<Key, Info>::remove_after(iterator what) {
	//check if we even can do this
	if(!what.valid()) {
		throw std::domain_error(itrinvl_exc);
	}
	//remove using iterator's private method
	iterator elementAfter(what.current -> next);
	length--;
	return elementAfter.remove(*this);
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator 
bi_ring<Key, Info>::remove_before(iterator what) {
	//check if iterator is valid
	if(!what.valid()) {
		throw std::domain_error(itrinvl_exc);
	}
	//remove using iterator's private method
	iterator elementBefore(what.current->prev);
	length--;
	return elementBefore.remove(*this);
}


template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator
bi_ring<Key, Info>::remove(iterator what) {
	//check if iterator is valid
	if(!what.valid()) {
		throw std::domain_error(itrinvl_exc);
	}
	//update length	
	length--;
	return what.remove(*this);
}

/*
	GETTER METHODS
*/

template<typename Key, typename Info>
bool bi_ring<Key, Info>::empty() const {
	return (any == nullptr && !length);
}

template<typename Key, typename Info>
unsigned int bi_ring<Key, Info>::size() const {
	return length;
}

template<typename Key, typename Info>
Info bi_ring<Key, Info>::get_info(const Key& key, int n_key) const {
	//otherwise proceed to search
	Element* result = _find(key, n_key);
	if(result != nullptr) {
		return result -> info;
	}
	throw std::invalid_argument("Specified key not found");
}

template<typename Key, typename Info>
void bi_ring<Key, Info>::print() const {
	//check if sequence is empty	
	if(empty()) {
		std::cout << "Ring empty!\n";
		return;
	}
	//iterate through all elements printing
	const_iterator itr(*this);
	do {
		std::cout << "Key ";
		std::cout << itr.key() << ": ";
		std::cout << itr.info() << std::endl;
		itr++;
	} while(itr != begin());
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::const_iterator
bi_ring<Key, Info>::begin() const {
	//construct new iterator to any pointer
	return const_iterator(*this);
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::const_iterator
bi_ring<Key, Info>::end() const {
	//construct new iterator to before any
	const_iterator end(*this);
	--end;
	return end;
}

/*
	UTILITY METHODS
*/

template<typename Key, typename Info>
bool bi_ring<Key, Info>::clear_info(const Info& filler) {
	//check for empty list
	if(empty()) {
		return false;
	}
	//go through elements clearing info
	iterator itr(*this);
	do {
		itr.info() = filler;
		itr++;
	} while(itr != begin());
	return true;
}

template<typename Key, typename Info>
bool bi_ring<Key, Info>::swap(iterator what, iterator dest) {
	//at least one of the iterators is invalid
	if(!what.valid() || !dest.valid()) {
		return false;
	}
	//swap payloads
	Element temp = *(what.current);
	what.key() = dest.key();
	what.info() = dest.info();
	dest.key() = temp.key;
	dest.info() = temp.info;
	return true;
}

/*
	HELPERS
*/

template<typename Key, typename Info>
typename bi_ring<Key, Info>::Element* 
bi_ring<Key, Info>::_find(const Key& key, int n_key) const {
	//check if argument is even valid
	if(n_key < 1) {
		throw std::invalid_argument("Key occurrence number cannot be negative");
	}
	//check if there's where to search
	if(empty()) {
		return nullptr;
	}
	//perform a search for target
	Element* current = any;
	int n_ocr = 0;
	do {
		if(current -> key == key) {
			n_ocr++;
		}
		if(n_ocr == n_key) {
			return current;
		}
		current = current -> next;
	} while(current != any);
	//give back empty search result if failed
	return nullptr;
}

template<typename Key, typename Info> 
bool bi_ring<Key, Info>::_clone(const bi_ring<Key, Info>& src) {
	//sequences already equal or self-clone
	if(this == &src || *this == src) {
		return true;
	}
	//start cloning clean
	if(!empty()) purge();
	//iterate through source pushing content into this
	const_iterator itr(src);
	do {
		push(itr.key(), itr.info());
		itr++;
	} while(itr != src.begin());
	return true;
}

/*
	ITERATORS
*/

template<typename Key, typename Info>
bi_ring<Key, Info>::const_iterator::const_iterator() {
	current = nullptr;
}

template<typename Key, typename Info>
bi_ring<Key, Info>::const_iterator::const_iterator(const bi_ring<Key, Info>& of) {
	current = of.any;
}

template<typename Key, typename Info>
bi_ring<Key, Info>::const_iterator::const_iterator(const bi_ring<Key, Info>& of, 
						   int key, int n_key) {
	current = of._find(key, n_key);
}

template<typename Key, typename Info>
bi_ring<Key, Info>::const_iterator::const_iterator(const const_iterator& src) {
	current = src.current;
}

template<typename Key, typename Info>
bi_ring<Key, Info>::const_iterator::const_iterator(Element* at) {
	current = at;
}

template<typename Key, typename Info>
typename bi_ring<Key,Info>::const_iterator&
bi_ring<Key, Info>::const_iterator::operator=(const const_iterator& src) {
	current = src.current;
	return *this;
}

template<typename Key, typename Info>
//prefix
typename bi_ring<Key, Info>::const_iterator& 
bi_ring<Key, Info>::const_iterator::operator++() {
	if(current != nullptr) {
		current = current -> next;
	}
	else { 
		throw std::domain_error(nulldef_exc);
	}
	return *this;
}

template<typename Key, typename Info>
//postfix
typename bi_ring<Key, Info>::const_iterator
bi_ring<Key, Info>::const_iterator::operator++(int ops) {
	const_iterator prev(*this);
	if(!ops) {
		++(*this);
		return prev;
	}
	for(int i = 0; i < ops; i++) {
		++(*this);
	}
	return prev;
}

template<typename Key, typename Info>
//prefix
typename bi_ring<Key, Info>::const_iterator& 
bi_ring<Key, Info>::const_iterator::operator--() {
	if(current != nullptr) {
		current = current -> prev;
	}
	else {
		throw std::domain_error(nulldef_exc);
	}
	return *this;
}

template<typename Key, typename Info>
//postfix
typename bi_ring<Key, Info>::const_iterator
bi_ring<Key, Info>::const_iterator::operator--(int ops) {
	const_iterator prev(*this);
	if(!ops) {
		--(*this);
		return prev;
	}
	for(int i = 0; i < ops; i++) {
		--(*this);
	}
	return prev;
}

template<typename Key, typename Info>
Info bi_ring<Key, Info>::const_iterator::operator*() const {
	if(current != nullptr) {
		return current -> info;
	}
	else {
		throw std::domain_error(nulldef_exc);
	}
}

template<typename Key, typename Info>
bool bi_ring<Key, Info>::const_iterator::operator==(const const_iterator& cmp) const {
	return current == cmp.current;
}

template<typename Key, typename Info>
bool bi_ring<Key, Info>::const_iterator::operator!=(const const_iterator& cmp) const {
	return current != cmp.current;
}

template<typename Key, typename Info>
Key bi_ring<Key, Info>::const_iterator::key() const {
	if(current != nullptr) {
		return current -> key;
	}
	else { 
		throw std::domain_error(nulldef_exc);
	}
}

template<typename Key, typename Info>
Info bi_ring<Key, Info>::const_iterator::info() const {
	if(current != nullptr) {
		return current -> info;
	}
	else {
		throw std::domain_error(nulldef_exc);
	}
}

template<typename Key, typename Info>
bool bi_ring<Key, Info>::const_iterator::valid() const {
	return current != nullptr;
}

template<typename Key, typename Info>
bi_ring<Key, Info>::iterator::iterator(const const_iterator& src) {
	iterator::current = src.current;
}

template<typename Key, typename Info>
Info& bi_ring<Key, Info>::iterator::operator*() {
	return info();
}

template<typename Key, typename Info>
Key& bi_ring<Key, Info>::iterator::key() {
	if(iterator::current != nullptr) {
		return iterator::current -> key;
	}
	else {
		throw std::domain_error(nulldef_exc);
	}
}

template<typename Key, typename Info>
Info& bi_ring<Key, Info>::iterator::info() {
	if(iterator::current != nullptr) {
		return iterator::current -> info;
	}
	else {
		throw std::domain_error(nulldef_exc);
	}
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator
bi_ring<Key, Info>::iterator::insert_after(const Key& key, const Info& inf) {
	Element* item = iterator::current;
	//invalid iterator, abort
	if(item == nullptr) {
		throw std::domain_error(itrinvl_exc);
	}
	//insert after found element 
	item -> next = new Element({key, inf, item -> next, item});
	//connect old successor back to new
	item -> next -> next -> prev = item -> next;
	//return iterator to new element
	return iterator(item -> next);
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator
bi_ring<Key, Info>::iterator::insert_before(const Key& key, const Info& inf) {
	if(!iterator::valid()) {
		throw std::domain_error(itrinvl_exc);
	}
	//construct new iterator to element before current
	iterator elementBefore(iterator::current -> prev);
	//insert after previous element - before current
	elementBefore = elementBefore.insert_after(key, inf);
	return elementBefore;
}

template<typename Key, typename Info>
typename bi_ring<Key, Info>::iterator
bi_ring<Key, Info>::iterator::remove(bi_ring<Key, Info>& parent) {
	//invalid iterator, abort
	if(!iterator::valid()) {
		throw std::domain_error(nulldef_exc);
	}
	//check for one-element case
	if(iterator::current -> next == iterator::current) {
		delete parent.any;
		parent.any = nullptr;
		iterator::current = nullptr;
		return *this;
	}
	//removing from multielement list
	iterator::current -> prev -> next = iterator::current -> next;
	iterator::current -> next -> prev = iterator::current -> prev;
	//keep address
	Element* elementToBeDeleted = iterator::current;
	//move on
	++(*this);
	//check for removing any
	if(elementToBeDeleted == parent.any) {
		parent.any = iterator::current;
	}
	delete elementToBeDeleted;
	return *this;
}

/*
	EXTERNAL FUNCTIONS
*/

template <typename Key, typename Info>
bi_ring<Key, Info> shuffle(const bi_ring<Key, Info>& first, unsigned int fcnt,
			   const bi_ring<Key, Info>& secnd, unsigned int scnt,
			   unsigned int reps) {
	//require two rings to be non-empty
	if( !first.size() || !secnd.size() ) {
		throw std::invalid_argument("One of the rings is empty.");
	}
	//require parameters to be non-zero
	if( !fcnt || !scnt || !reps ) {
		throw std::invalid_argument("These count parameters result in no shuffling.");
	}
	//start shuffling
	bi_ring<Key, Info> newRing;
	typename bi_ring<Key, Info>::const_iterator itr_f(first);
	typename bi_ring<Key, Info>::const_iterator itr_s(secnd);
	for(unsigned int i = 0; i < reps; i++) {
		for(unsigned int j = 0; j < fcnt; j++) {
			newRing.push(itr_f.key(), itr_f.info());
			itr_f++;
		}
		for(unsigned int j = 0; j < scnt; j++) {
			newRing.push(itr_s.key(), itr_s.info());
			itr_s++;
		}
	}
	//everything succeeded, return shuffled result
	return newRing;
}






