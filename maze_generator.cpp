/*
 * Generates an image of a random maze and a separate image showing its solution
 * BMP format implementation help from
 * stackoverflow.com/questions/2654480/
 * and
 * gist.github.com/martinlk/1846867
 */

#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include "maze.h"
#include "output.h"

#define HEIGHT 100
#define WIDTH 50

using namespace std;

int main()
{
	vector<vector<cell>> cells(HEIGHT, vector<cell>(WIDTH));
	srand(time(0));

	initialize_maze(cells);
	create_maze(cells);

	ofstream outfile;
	outfile.open("img.bmp", ios::binary);
	output_maze(cells, outfile);
	outfile.close();

	outfile.open("soln.bmp", ios::binary);
	vector<cell> soln = get_solution(cells);
	
	output_solution(cells, soln, outfile);
	outfile.close();

	return 0;
}
