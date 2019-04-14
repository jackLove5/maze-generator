#define ALL_WALL 15U
#define LEFT_WALL 8U
#define TOP_WALL 4U
#define RIGHT_WALL 2U
#define BOTTOM_WALL 1U

struct cell {
	int x, y;
	cell *parent;
	bool seen;
	unsigned int walls;
};

void initialize_maze(std::vector<std::vector<cell>>&);
void connect_cells(cell&, cell&);
void create_maze(std::vector<std::vector<cell>>& );
std::vector<cell> get_solution(std::vector<std::vector<cell>>& );
