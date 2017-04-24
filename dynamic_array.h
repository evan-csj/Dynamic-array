using namespace std;

class Dynamic_array {
public:
	Dynamic_array();
	Dynamic_array(Dynamic_array &);
	Dynamic_array &operator=(Dynamic_array &);
	~Dynamic_array();

	void print_state(void);
	int get_size(void);

	int& operator[](int);

	void insert(int, int);
	void insert(Dynamic_array &, int);

	void remove(int);
	void remove(int, int);

	class Subscript_range_exception {
	};
private:
	enum {
		BLOCK_SIZE = 5,
	};

	class Block {
	public:
		int size;
		int a[BLOCK_SIZE];
		Block* next_p;
	};

	class Block_position {
	public:
		Block* block_p;
		Block* pre_block_p;
		int i;
	};

	Block_position find_block(int i);
	void insert_blocks(Block *, Block *);
	void remove_blocks(Block *, Block *, Block *);
	Block * copy_blocks(Block *);
	void delete_blocks(Block *);

	int size;
	Block* head_p;
};
