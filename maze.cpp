#include <vector>
#include <deque>
#include <algorithm>

#include "maze.h"

using namespace std;

void initialize_maze(vector<vector<cell>>& grid)
{
	// Initialize cells
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			grid[i][j].x = j;
			grid[i][j].y = i;
			grid[i][j].parent = NULL;
			grid[i][j].seen = false;
			grid[i][j].walls = ALL_WALL;
		}
	}
}

// Reset cells' parent and seen variables
// (Call at the beginning of every new traversal
void reset_maze(vector<vector<cell>>& grid)
{
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			grid[i][j].parent = NULL;
			grid[i][j].seen = false;
		}
	}
}

vector<cell> get_solution(vector<vector<cell>>& grid)
{
	// Get path from upper left cell to bottom right cell
	// Trying BFS again
	
	reset_maze(grid);
	deque<cell*> queue;

	// Start at upper left
	grid[0][0].seen = true;
	queue.push_back(&grid[0][0]);

	// End at bottom right
	cell dst = grid[grid.size()-1][grid[0].size()-1];
	
	bool done = false;
	while (!queue.empty() && !done)
	{
		cell *current_cell = queue.front();
		queue.pop_front();

		// Enqueue any undiscovered, accessible neighbors
		if ( current_cell->x > 0 && !(current_cell->walls & LEFT_WALL) && !grid[current_cell->y][current_cell->x-1].seen)
		{
			cell *to_push = &grid[current_cell->y][current_cell->x-1];
			to_push->seen = true;
			to_push->parent = current_cell;
			
			if (to_push->x == dst.x && to_push->y == dst.y)
				done = true;

			queue.push_back(to_push);
		}

		if ( current_cell->x < grid[0].size() -1 && !(current_cell->walls & RIGHT_WALL) && !grid[current_cell->y][current_cell->x+1].seen)
		{
			cell *to_push = &grid[current_cell->y][current_cell->x+1];
			to_push->seen = true;
			to_push->parent = current_cell;
			
			if (to_push->x == dst.x && to_push->y == dst.y)
				done = true;
			
			queue.push_back(to_push);
		}

		if ( current_cell->y > 0 && !(current_cell->walls & TOP_WALL) && !grid[current_cell->y-1][current_cell->x].seen)
		{
			cell *to_push = &grid[current_cell->y-1][current_cell->x];
			to_push->seen = true;
			to_push->parent = current_cell;
			
			if (to_push->x == dst.x && to_push->y == dst.y)
				done = true;

			queue.push_back(to_push);
		}

		if ( current_cell->y < grid.size() - 1 && !(current_cell->walls & BOTTOM_WALL) && !grid[current_cell->y+1][current_cell->x].seen)
		{
			cell *to_push = &grid[current_cell->y+1][current_cell->x];
			to_push->seen = true;

			if (to_push->x == dst.x && to_push->y == dst.y)
				done = true;

			to_push->parent = current_cell;
			queue.push_back(to_push);
		}
	}

	// Store path and return
	vector<cell> res;
	cell *current = &grid[grid.size()-1][grid[0].size()-1];
	res.push_back(*current);
	while (current->parent)
	{
		current = current->parent;
		res.push_back(*current);
	}

	return vector<cell>(res.rbegin(), res.rend());
}

void connect_cells(cell& current_cell, cell& parent_cell)
{

	int dx = parent_cell.x - current_cell.x;
	int dy = parent_cell.y - current_cell.y;
	
	// Cells are left/right adjacent
	if (dy == 0)
	{
		// destroy parent's left wall and current's right wall
		if (dx == 1)
		{
			parent_cell.walls &= ~( LEFT_WALL );
			current_cell.walls &= ~( RIGHT_WALL );
		}
		else if (dx == -1)
		{
			// destroy current's left wall and parent's right wall
			parent_cell.walls &= ~( RIGHT_WALL );
			current_cell.walls &= ~( LEFT_WALL );
		}
	}
	else
	{
		// Cells are top/bottom adjacent
		if (dy == 1)
		{
			// destroy parent's top wall and current's bottom wall
			parent_cell.walls &= ~( TOP_WALL );
			current_cell.walls &= ~( BOTTOM_WALL );
		}
		else if (dy == -1)
		{
			// destroy parent's bottom wall and current's top wall
			parent_cell.walls &= ~( BOTTOM_WALL );
			current_cell.walls &= ~( TOP_WALL );
		}
	}

}

void create_maze(vector<vector<cell>>& grid)
{
	cell *start = &grid[rand() % grid.size()][rand() % grid[0].size()];
	start->seen = true;

	deque<cell*> border_cells;
	
	// Add bordering cells to the list
	if (start->x > 0 && !grid[start->y][start->x-1].seen)
	{
		grid[start->y][start->x-1].parent = start;
		grid[start->y][start->x-1].seen = true;
		border_cells.push_back(&grid[start->y][start->x-1]);
	}

	if (start->x < grid[0].size() - 1 && !grid[start->y][start->x+1].seen)
	{
		grid[start->y][start->x+1].parent = &grid[start->y][start->x];
		grid[start->y][start->x+1].seen = true;
		border_cells.push_back(&grid[start->y][start->x+1]);
	}

	if (start->y > 0 && !grid[start->y-1][start->x].seen)
	{
		grid[start->y-1][start->x].parent = &grid[start->y][start->x];
		grid[start->y-1][start->x].seen = true;
		border_cells.push_back(&grid[start->y-1][start->x]);
	}

	if (start->y < grid.size() - 1 && !grid[start->y+1][start->x].seen)
	{
		grid[start->y+1][start->x].parent = &grid[start->y][start->x];
		grid[start->y+1][start->x].seen = true;
		border_cells.push_back(&grid[start->y+1][start->x]);
	}

	while (!border_cells.empty())
	{
		std::random_shuffle(border_cells.begin(), border_cells.end());
		
		// Pick a random cell and connect it to its parent
		cell *current_cell = border_cells.front();
		border_cells.pop_front();

		connect_cells(*current_cell, *(current_cell->parent));

		// Add undiscovered neighbors
		if (current_cell->x > 0 && !grid[current_cell->y][current_cell->x-1].seen)
		{
			grid[current_cell->y][current_cell->x-1].parent = current_cell;
			grid[current_cell->y][current_cell->x-1].seen = true;
			border_cells.push_back(&grid[current_cell->y][current_cell->x-1]);
		}

		if (current_cell->x < grid[0].size() - 1 && !grid[current_cell->y][current_cell->x+1].seen)
		{
			grid[current_cell->y][current_cell->x+1].parent = current_cell;
			grid[current_cell->y][current_cell->x+1].seen = true;
			border_cells.push_back(&grid[current_cell->y][current_cell->x+1]);
		}

		if (current_cell->y > 0 && !grid[current_cell->y-1][current_cell->x].seen)
		{
			grid[current_cell->y-1][current_cell->x].parent = current_cell;
			grid[current_cell->y-1][current_cell->x].seen = true;
			border_cells.push_back(&grid[current_cell->y-1][current_cell->x]);
		}

		if (current_cell->y < grid.size() - 1 && !grid[current_cell->y+1][current_cell->x].seen)
		{
			grid[current_cell->y+1][current_cell->x].parent = current_cell;
			grid[current_cell->y+1][current_cell->x].seen = true;
			border_cells.push_back(&grid[current_cell->y+1][current_cell->x]);
		}
	}

	// Add entrance / exit
	grid[0][0].walls &= ~( LEFT_WALL );
	grid[grid.size()-1][grid[0].size()-1].walls &= ~( BOTTOM_WALL );
}
