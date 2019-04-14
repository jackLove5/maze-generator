#include <vector>
#include <fstream>

#include "maze.h"
#include "output.h"

#define IMG_HEIGHT 800
#define IMG_WIDTH 300

using namespace std;

// char - 1 byte
// short - 2 bytes
// int - 4 bytes

// Bitmap magic bytes
struct bmagic {
	unsigned char magic[2];
};

// Bitmap header
struct bheader {
	unsigned int bf_size;
	short bf_reserved1;
	short bf_reserved2;
	unsigned int bf_offset;
};

// Bitmap info header
struct infoheader {
	unsigned int bi_size;
	unsigned int bi_width;
	unsigned int bi_height;
	short bi_planes;
	short bi_bit_count;
	unsigned int bi_compression;
	unsigned int bi_size_img;
	unsigned int bi_xpels_per_meter;
	unsigned int bi_ypels_per_meter;
	unsigned int bi_clr_used;
	unsigned int bi_clr_important;
};

struct pixel {
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

static pixel pixels[IMG_HEIGHT][IMG_WIDTH];

void erase_image()
{
	// Set all to white
	for (int i = 0; i < IMG_HEIGHT; i++)
	{
		for (int j = 0; j < IMG_WIDTH; j++)
		{
			pixels[i][j].r = 255;
			pixels[i][j].g = 255;
			pixels[i][j].b = 255;
		}
	}

}

void draw_walls(vector<vector<cell>>& grid)
{
	const int cell_width = IMG_WIDTH / grid[0].size() - 2;
	const int cell_height = IMG_HEIGHT / grid.size() -2 ;

	const int x_offset = (IMG_WIDTH - (grid[0].size() * cell_width+1)) / 4;
	const int y_offset = (IMG_HEIGHT - (grid.size() * cell_height+1)) / 4;

	// Draw walls
	for (int i = 0; i < grid.size(); i++)
	{
		for (int j = 0; j < grid[i].size(); j++)
		{
			int x_pos = j*(cell_width+1) + x_offset;
			int y_pos = i*(cell_height+1) + y_offset;
			if (grid[i][j].walls & LEFT_WALL)
			{
				for (int k = 0; k < cell_height + 2; k++)
				{
					pixels[y_pos+k][x_pos].r = 0;
					pixels[y_pos+k][x_pos].g = 0;
					pixels[y_pos+k][x_pos].b = 0;
				}
			}
			if (grid[i][j].walls & TOP_WALL)
			{
				for (int k = 0; k < cell_width + 2; k++)
				{
					pixels[y_pos][x_pos+k].r = 0;
					pixels[y_pos][x_pos+k].g = 0;
					pixels[y_pos][x_pos+k].b = 0;
				}
			}
			if (grid[i][j].walls & RIGHT_WALL)
			{
				for (int k = 0; k < cell_height + 2; k++)
				{
					pixels[y_pos+k][x_pos+cell_width+1].r = 0;
					pixels[y_pos+k][x_pos+cell_width+1].g = 0;
					pixels[y_pos+k][x_pos+cell_width+1].b = 0;
				}
			}
			if (grid[i][j].walls & BOTTOM_WALL)
			{
				for (int k = 0; k < cell_width + 2; k++)
				{
					pixels[y_pos+cell_height+1][x_pos+k].r = 0;
					pixels[y_pos+cell_height+1][x_pos+k].g = 0;
					pixels[y_pos+cell_height+1][x_pos+k].b = 0;
				}
			}
		}
	}

}

void output_solution(vector<vector<cell>>& grid, vector<cell>& soln, ofstream& outfile)
{
	int cell_width = IMG_WIDTH / grid[0].size() - 2;
	int cell_height = IMG_HEIGHT / grid.size() -2 ;

	const int x_offset = (IMG_WIDTH - (grid[0].size() * cell_width+1)) / 4;
	const int y_offset = (IMG_HEIGHT - (grid.size() * cell_height+1)) / 4;

	cell_width += 1;
	cell_height += 1;

	bmagic b_magic;
	b_magic.magic[0] = 0x42;
	b_magic.magic[1] = 0x4D;

	bheader b_header;

	// Make multiple of 4
	int row_size = sizeof(pixel) * IMG_WIDTH;
	row_size = (row_size + 3) / 4 * 4;
	
	b_header.bf_size = sizeof(bmagic) + sizeof(bheader) + sizeof(infoheader) + row_size * IMG_HEIGHT;

	b_header.bf_reserved1 = b_header.bf_reserved2 = 0;
	b_header.bf_offset = sizeof(bmagic) + sizeof(bheader) + sizeof(infoheader);

	infoheader i_header;
	i_header.bi_size = sizeof(infoheader);
	i_header.bi_width = IMG_WIDTH;
	i_header.bi_height = IMG_HEIGHT;
	i_header.bi_planes = 1;
	i_header.bi_bit_count = 24;
	i_header.bi_compression = 0;
	i_header.bi_size_img = 0;
	i_header.bi_xpels_per_meter = 0;
	i_header.bi_ypels_per_meter = 0;
	i_header.bi_clr_used = 0;
	i_header.bi_clr_important = 0;
	
	erase_image();
	draw_walls(grid);

	// Draw solution
	
	for (int i = 1; i < soln.size(); i++)
	{
		cell dst = soln[i];
		cell src = soln[i-1];

		int src_centerX = src.x * cell_width + cell_width / 2 + x_offset;
		int src_centerY = src.y * cell_height + cell_height / 2 + y_offset;
		int dst_centerX = dst.x * cell_width + cell_width / 2 + x_offset;
		int dst_centerY = dst.y * cell_height + cell_height / 2 + y_offset;

		// Connect cells' center points
		if (src_centerX > dst_centerX)
		{
			for (int x = src_centerX; x >= dst_centerX; x--)
			{
				pixels[src_centerY][x].r = 255;
				pixels[src_centerY][x].g = 0;
				pixels[src_centerY][x].b = 0;
			}
		}
		else if (src_centerX < dst_centerX)
		{
			for (int x = src_centerX; x <= dst_centerX; x++)
			{
				pixels[src_centerY][x].r = 255;
				pixels[src_centerY][x].g = 0;
				pixels[src_centerY][x].b = 0;
			}
		}
		else if (src_centerY > dst_centerY)
		{
			for (int y = src_centerY; y >= dst_centerY; y--)
			{
				pixels[y][src_centerX].r = 255;
				pixels[y][src_centerX].g = 0;
				pixels[y][src_centerX].b = 0;
			}
		}
		else if (src_centerY < dst_centerY)
		{
			for (int y = src_centerY; y <= dst_centerY; y++)
			{
				pixels[y][src_centerX].r = 255;
				pixels[y][src_centerX].g = 0;
				pixels[y][src_centerX].b = 0;
			}
		}
	}

	// Write to file
	outfile.write((char*) &b_magic, sizeof(bmagic));
	outfile.write((char*) &b_header, sizeof(bheader));
	outfile.write((char*) &i_header, sizeof(infoheader));
	
	for (int i = 0; i < IMG_HEIGHT; i++)
	{
		outfile.write((char*) &pixels[i], row_size);
	}
}

void output_maze(vector<vector<cell>>& grid, ofstream& outfile)
{
	bmagic b_magic;
	b_magic.magic[0] = 0x42;
	b_magic.magic[1] = 0x4D;

	bheader b_header;

	// Make multiple of 4
	int row_size = sizeof(pixel) * IMG_WIDTH;
	row_size = (row_size + 3) / 4 * 4;
	
	b_header.bf_size = sizeof(bmagic) + sizeof(bheader) + sizeof(infoheader) + row_size * IMG_HEIGHT;

	b_header.bf_reserved1 = b_header.bf_reserved2 = 0;
	b_header.bf_offset = sizeof(bmagic) + sizeof(bheader) + sizeof(infoheader);

	infoheader i_header;
	i_header.bi_size = sizeof(infoheader);
	i_header.bi_width = IMG_WIDTH;
	i_header.bi_height = IMG_HEIGHT;
	i_header.bi_planes = 1;
	i_header.bi_bit_count = 24;
	i_header.bi_compression = 0;
	i_header.bi_size_img = 0;
	i_header.bi_xpels_per_meter = 0;
	i_header.bi_ypels_per_meter = 0;
	i_header.bi_clr_used = 0;
	i_header.bi_clr_important = 0;
	
	erase_image();
	draw_walls(grid);

	// Write to file
	outfile.write((char*) &b_magic, sizeof(bmagic));
	outfile.write((char*) &b_header, sizeof(bheader));
	outfile.write((char*) &i_header, sizeof(infoheader));
	
	for (int i = 0; i < IMG_HEIGHT; i++)
	{
		outfile.write((char*) &pixels[i], row_size);
	}
}
