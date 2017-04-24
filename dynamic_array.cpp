#include <iostream>										//-
#include <string.h>										//-
#include "dynamic_array.h"									//-
												//-
using namespace std;										//-
												//-
// ********** public functions **********							//-
												//-
Dynamic_array::Dynamic_array() {								//-
	head_p = NULL;										//-
	size = 0;										//-
}												//-
												//-
Dynamic_array::Dynamic_array(Dynamic_array & d) {						//-
	try{
		head_p = copy_blocks(d.head_p);
		size = d.size;
	}catch(Dynamic_array::Subscript_range_exception){
		throw;
	}
}												//-
												//-
Dynamic_array &Dynamic_array::operator=(Dynamic_array & d) {					//-
	// clean the existing array
	Block * del = this -> head_p;
	while(del != NULL){
		Block * temp = del -> next_p;
		delete del;
		del = temp;
	}

	// copy the blocks from d to the array
	try{
		this -> head_p = copy_blocks(d.head_p);
		this -> size = d.size;
	}catch(Dynamic_array::Subscript_range_exception){
		throw;
	}
	return *this;										//-
}												//-
												//-
Dynamic_array::~Dynamic_array() {								//-
	Block * del = this -> head_p;
	while(del != NULL){
		Block * temp = del -> next_p;
		delete del;
		del = temp;
	}
}												//-
												//-
void Dynamic_array::print_state(void) {								//-
	cout << "size:" << size << endl;							//-
	for (Block * p = head_p; p != NULL; p = p->next_p) {					//-
		cout << "\tsize:" << p->size << " elements:";					//-
		for (int i = 0; i < p->size; i++) {						//-
			cout << p->a[i] << ' ';							//-
		}										//-
		cout << endl;									//-
	}											//-
}												//-
												//-
int Dynamic_array::get_size(void) {								//-
	return size;
}												//-
												//-
int& Dynamic_array::operator[](int i) {								//-
	// check i for range error								//-
	if (i < 0 || i >= size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// return element at index i								//-
	return position.block_p->a[position.i];							//-
}												//-
												//-
void Dynamic_array::insert(int x, int i) {							//-
	// case 1: range error									//-
	if (i < 0 || i > size) {								//-
		throw Subscript_range_exception();						//-
	}											//-
												//-
	// case 2: empty array									//-
	if (size == 0) {									//-
		// create linked list consisting of a single new block				//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->a[0] = x;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// insert new block								//-
		insert_blocks(NULL, new_block_p);						//-
												//-
		// update total size								//-
		size++;										//-
												//-
		return;										//-
	}											//-
												//-
	// find target block and index								//-
	Block_position position = find_block(i);						//-
												//-
	// case 3: non-empty array; new block not needed					//-
	if (position.block_p->size < BLOCK_SIZE) {						//-
		// shift block array right							//-
		for (int j = position.block_p->size; j > position.i; j--) {			//-
			position.block_p->a[j] = position.block_p->a[j-1];			//-
		}										//-
												//-
		// assign x									//-
		position.block_p->a[position.i] = x;						//-
												//-
		// update array and block size							//-
		size++;										//-
		position.block_p->size++;							//-
												//-
	// case 4: non-empty array; new block needed						//-
	} else {										//-
		// create new block								//-
		Block * new_block_p = new Block;						//-
		new_block_p->size = 1;								//-
		new_block_p->next_p = NULL;							//-
												//-
		// case 4.a: insert x into old block						//-
		if (position.i < BLOCK_SIZE) {							//-
			// copy last array element to new block					//-
			new_block_p->a[0] = position.block_p->a[BLOCK_SIZE-1];			//-
												//-
			// shift old block array right						//-
			for (int j = position.block_p->size-1; j > position.i; j--) {		//-
				position.block_p->a[j] = position.block_p->a[j-1];		//-
			}									//-
												//-
			// assign x into old block						//-
			position.block_p->a[position.i] = x;					//-
												//-
		// case 4.b: insert x into new block						//-
		} else {									//-
			new_block_p->a[0] = x;							//-
		}										//-
												//-
		// update total size								//-
		size++;										//-
												//-
		// insert new block into linked list						//-
		insert_blocks(position.block_p, new_block_p);					//-
	}											//-
}												//-
												//-
void Dynamic_array::insert(Dynamic_array &p, int i) {						//-
	// case 1: range error									//-
	if (i < 0 || i > size) {
		throw Subscript_range_exception();
		return;
	}

	// case 2: parameter array empty							//-
	if (p.size == 0){
		return;
	}

	// case 3: array empty									//-
	if (size == 0) {
		Block * copy_p = copy_blocks(p.head_p);
		insert_blocks(head_p, copy_p);
		size += p.size;
		return;
	}

	// find target block and index								//-
	Block_position position = find_block(i);						//-

	// case 4: array non-empty; new blocks not needed					//-
	if (position.block_p->size + p.size <= 5) {
		int y;
		for (int j = 0; j < p.size; j++){
			y = p.operator[](j);
			insert(y, i);
			i++;
		}
		return;

	// case 5: array non-empty; new blocks needed						//-
	}else if (position.block_p->size + p.size > 5) {
		// copy p									//-
		Block * copy_p = copy_blocks(p.head_p);						//-

		// case 5.a: insert position at start of block					//-
		if (position.i == 0) {
			insert_blocks(position.pre_block_p, copy_p);

		// case 5.b: insert position at middle of block					//-
		}else if (position.i > 0 && position.i < position.block_p->size) {
			int n = position.block_p->size - position.i;
			int p = position.i;
			Block * new_block_p = new Block;
			new_block_p->size = 0;
			for (int j = 0; j < n; j++) {
				new_block_p->size++;
				new_block_p->a[j] = position.block_p->a[p++];
			}
			position.block_p->size -= n;
			insert_blocks(position.block_p, new_block_p);
			insert_blocks(position.block_p, copy_p);
			

		// case 5.c: insert position after end of block					//-
		}else if (position.i == position.block_p->size) {
			insert_blocks(position.block_p, copy_p);
		}
	}
		// update total size								//-
	size += p.size;
	return;
}												//-
												//-
void Dynamic_array::remove(int i) {								//-
	// case 1: range error									//-
	if (i < 0 || i >= size) {
		throw Subscript_range_exception();
		return;
	}

	// find target block and index								//-
	Block_position position = find_block(i);

	// case 2: block size == 1								//-
	if (position.block_p->size == 1) {
		remove_blocks(position.pre_block_p ,position.block_p, position.block_p);
		//position.pre_block_p->next_p = position.block_p->next_p;
		//delete position.block_p;
	

	// case 3: block size > 1								//-
	}else if (position.block_p->size > 1) {
		//shift to left
		int n = position.block_p->size - position.i - 1;
		int k = position.i;
		for  (int j = 0; j < n; j++) {
			position.block_p->a[k] = position.block_p->a[k+1];
			k++;
		}
		position.block_p->size--;
	}

	// update total size									//-
	size--;
}												//-
												//-
void Dynamic_array::remove(int start, int end) {						//-
	// case 1: range error
	if (start < 0 || start > end || end > size){
		throw Subscript_range_exception();
		return;
	}

	// case 2: start = end
	if (start == end) {
		return;
	}

	Block_position positionStart = find_block(start);//find the blocks
	Block_position positionEnd = find_block(end);

	//case 3: start and end in the same block
	if (positionStart.i < positionEnd.i && (end - start < 5) && (positionEnd.i < positionEnd.block_p->size)){
		int temp = positionStart.i;
		int n = end - start;
		for (int b = 0; b < (positionStart.block_p->size - positionEnd.i); b++){
			positionStart.block_p->a[temp] = positionStart.block_p->a[temp + n];
			temp++;
		}

		positionStart.block_p->size -= n;
		size -= n;
		return;
	}

	if (positionStart.i == 0){
		// case 4: start is 1st of a block; end is the last of the dynamic array
		if (positionEnd.i >= positionEnd.block_p->size){
			remove_blocks(positionStart.pre_block_p, positionStart.block_p, positionEnd.block_p);

		//case 5: start is the 1st of a blocks; end is at the middle of a block
		}else if (positionEnd.i < positionEnd.block_p->size){
			remove_blocks(positionStart.pre_block_p, positionStart.block_p, positionEnd.pre_block_p);

			int n = positionEnd.block_p->size - positionEnd.i;
			for (int j = 0; j < n; j++){
				positionEnd.block_p->a[j] = positionEnd.block_p->a[j + positionEnd.i];
			}

			positionEnd.block_p->size -= (positionEnd.i);
		}

	}else if (positionStart.i > 0){
		positionStart.block_p->size = positionStart.i;
		
		//case 6: start is not the 1st one; end is the last of the dynamic array
		if (positionEnd.i >= positionEnd.block_p->size){
			remove_blocks(positionStart.block_p, positionStart.block_p->next_p,positionEnd.block_p);

		//case 7: Both start and end are at the middle of the block
		}else if (positionEnd.i < positionEnd.block_p->size){;
			if (positionStart.block_p != positionEnd.pre_block_p){
				remove_blocks(positionStart.block_p, positionStart.block_p->next_p,positionEnd.pre_block_p);
			}
			
			int n = positionEnd.block_p->size - positionEnd.i;
			for (int j = 0; j < n; j++){
				positionEnd.block_p->a[j] = positionEnd.block_p->a[j + positionEnd.i];
			}
			positionEnd.block_p->size -= (positionEnd.i);
		}
	}

	size -= end - start;
	return;
}												//-
												//-
// ********** private functions **********							//-
												//-
// purpose											//-
//	return b where										//-
//		if i < size									//-
//			b.block_p->a[b.i] is the ith element overall				//-
//		else										//-
//			b.block_p->a[b.i-1] is the i-1st element overall			//-
//												//-
//		if b.block_p == head_p								//-
//			b.pre_block_p is NULL							//-
//		else										//-
//			b.pre_block_p points to block preceding b.block_p			//-
// preconditions										//-
//	i in [1..size]										//-
Dynamic_array::Block_position Dynamic_array::find_block(int i) {				//-
	Block_position position;								//-
												//-
	// scan Block list									//-
	position.i = i;										//-
	position.pre_block_p = NULL;								//-
	for (position.block_p = head_p;								//-
	position.block_p != NULL;								//-
	position.block_p = position.block_p->next_p) {						//-
		// found in current block							//-
		if (position.i < position.block_p->size) {					//-
			break;									//-
		}										//-
												//-
		// special case: insert just after last element					//-
		if (i == size && position.block_p->next_p == NULL) {				//-
			break;									//-
		}										//-
												//-
		// not found yet: advance							//-
		position.pre_block_p = position.block_p;					//-
		position.i -= position.block_p->size;						//-
	}											//-
												//-
	return position;									//-
}												//-
												//-
// purpose											//-
//	create a new linked list which is a copy of the list pointed to p			//-
//	return a pointer to the head of the new linked list					//-
// preconditions										//-
//	p is the head of a possibly empty linked list of blocks					//-
Dynamic_array::Block * Dynamic_array::copy_blocks(Block * p) {					//-
	Block * new_head_p = NULL;								//-
	Block * new_p;										//-
	while (p != NULL) {									//-
		// allocate and link in new block						//-
		if (new_head_p == NULL) {							//-
			new_p = new Block;							//-
			new_head_p = new_p;							//-
		} else {									//-
			new_p->next_p = new Block;						//-
			new_p = new_p->next_p;							//-
		}										//-
												//-
		// copy the elements								//-
		new_p->size = p->size;								//-
		for (int i = 0; i < p->size; i++) {						//-
			new_p->a[i] = p->a[i];							//-
		}										//-
												//-
		// advance									//-
		p = p->next_p;									//-
	}											//-
												//-
	// terminate new list									//-
	if (new_head_p != NULL) {								//-
		new_p->next_p = NULL;								//-
	}											//-
												//-
	return new_head_p;									//-
}												//-
												//-
// purpose											//-
//	insert the list headed by src_p into the list headed by head_p				//-
//	if dst_p is NULL									//-
//		insert the list at the start of the list headed by head_p			//-
//	else											//-
//		insert the list just after block dst_p						//-
// preconditions										//-
//	list headed by src_p is non-empty							//-
//	list headed by src_p has no blocks in common with the list headed by head_p		//-
void Dynamic_array::insert_blocks(Block * dst_p, Block * src_p) {				//-
	// find the last block in the list headed by src_p					//-
	Block * p = src_p;									//-
	while (p->next_p != NULL) {								//-
		p = p->next_p;									//-
	}											//-
												//-
	// insert at head									//-
	if (dst_p == NULL) { // insert at head							//-
		p->next_p = head_p;								//-
		head_p = src_p;									//-
												//-
	// insert after dst_p									//-
	} else { // insert after dst_p								//-
		p->next_p = dst_p->next_p;							//-
		dst_p->next_p = src_p;								//-
	}											//-
}												//-
												//-
// purpose											//-
//	remove the blocks pointed to by start_p and end_p, and all the blocks between		//-
// preconditions										//-
//	start_p and end_p point to blocks in the list headed by head_p				//-
//	end_p points to either the same block as start_p or a block to its right		//-
//	if start_p == head_p									//-
//		pre_start_p == NULL								//-
//	else											//-
//		pre_start_p points to the block immediately preceding start_p			//-
//												//-
void Dynamic_array::remove_blocks(Block * pre_start_p, Block * start_p, Block * end_p) {	//-
	// release the blocks									//-
	while (1) {										//-
		// release start_p								//-
		Block * p = start_p->next_p;							//-
		delete start_p;									//-
												//-
		// advance									//-
		if (start_p == end_p) {								//-
			break;									//-
		} else {									//-
			start_p = p;								//-
		}										//-
	}											//-
												//-
	// link left and right sublists								//-
	if (pre_start_p == NULL) {								//-
		head_p = end_p->next_p;								//-
	} else {										//-
		pre_start_p->next_p = start_p->next_p;						//-
	}											//-
}												//-
