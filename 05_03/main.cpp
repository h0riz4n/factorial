#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <thread>
#include <chrono>

using namespace std;

constexpr int GRID_WIDTH = 100;
constexpr int GRID_HEIGHT = 20;
constexpr int MAX_STEPS = 50;

using Grid = std::vector<std::vector<int>>;

void initialize(Grid &grid) {
	srand(static_cast<unsigned>(time(nullptr)));
	for (auto &row : grid) {
		for (auto &cell : row) {
			cell = rand() % 2;
		}
	}
}

void render(const Grid &grid) {
	system("clear");
	for (const auto &row : grid) {
		for (int cell : row) {
			cout << (cell ? "#" : " ");
		}
		cout << "\n";
	}
}

int countNeighbors(const Grid &grid, int x, int y) {
	static const int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
	static const int dy[] = {-1, 0, 1, 1, 1, 0, -1, -1};
	int count = 0;
	
	for (int i = 0; i < 8; ++i) {
		int nx = x + dx[i], ny = y + dy[i];
		if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
			count += grid[ny][nx];
		}
	}
	return count;
}

void computeNextGeneration(const Grid &grid, Grid &nextGrid) {
	#pragma omp parallel for collapse(2)
	for (int y = 0; y < GRID_HEIGHT; ++y) {
		for (int x = 0; x < GRID_WIDTH; ++x) {
			int neighbors = countNeighbors(grid, x, y);
			nextGrid[y][x] = (grid[y][x] == 1) ? (neighbors == 2 || neighbors == 3) : (neighbors == 3);
		}
	}
}

int main() {
	Grid grid(GRID_HEIGHT, vector<int>(GRID_WIDTH, 0));
	Grid nextGrid = grid;
	initialize(grid);
	
	for (int step = 0; step < MAX_STEPS; ++step) {
		auto startTime = chrono::high_resolution_clock::now();
		if (step == 1 || step == 49) render(grid);
		computeNextGeneration(grid, nextGrid);
		swap(grid, nextGrid);
		auto endTime = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed = endTime - startTime;
		cout << "Step " << step + 1 << " Time: " << elapsed.count() << " sec\n";
		this_thread::sleep_for(chrono::milliseconds(100));
	}

	return 0;
}

