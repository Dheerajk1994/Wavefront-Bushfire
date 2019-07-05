#define OLC_PGE_APPLICATION

#include <iostream>
#include "olcPixelGameEngine.h"
#include "Cell.h"
#include "Blob.h"
#include <vector>

const int WINDOW_SIZE = 800;
const int WINDOW_RATIO = 1;

int Cell::maxDistance = 0;

class Application : public olc::PixelGameEngine {
private:
	int segments = 20;
	float cellSize = 0;
	Cell* cells;
	std::vector<Blob*> blobs;

public:
	Application() {
		sAppName = "WaveFront";
	}

	bool OnUserCreate() override
	{
		cellSize = ScreenWidth() / segments;
		GenerateCells();
		DrawGrid();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override 
	{
		if (GetMouse(0).bHeld) {
			AddObstacle(GetMouseX(), GetMouseY());
		}
		else if (GetMouse(1).bHeld) {
			WaveFront(GetMouseX(), GetMouseY());
		}
		else if (GetKey(olc::Key::R).bHeld) {
			RemoveObstacle(GetMouseX(), GetMouseY());
		}
		else if (GetKey(olc::Key::A).bReleased) {
			int index = CellIndexAtPosition(GetMouseX(), GetMouseY());
			Cell cell = cells[index];
			olc::Vector2 pos = olc::Vector2(GetMouseX(), GetMouseY());
			Blob* b = new Blob(pos);
			blobs.emplace_back(b);
		}
		
		for (auto *b : blobs) {
			b->UpdatePosition( &cells[CellIndexAtPosition(b->currentPosition.x, b->currentPosition.y)]);
		}
		
		RedrawScreen();

		return true;
	}
	
private:

	void RedrawScreen() {
		for (int x = 0; x < segments; ++x) {
			for (int y = 0; y < segments; ++y) {
				DrawCell(&cells[x + y * segments]);
			}
		}
		DrawGrid();
		DrawBlobs();
	}

	int CellIndexAtPosition(float posX, float posY) {
		std::cout << (posX / cellSize) + (posY / cellSize) * segments << std::endl;
		return((posX / cellSize) + (posY / cellSize) * segments);
	}
	
	void WaveFront(float posX, float posY) {
		//AssignNeighbors();
		ResetCells();
		Cell::maxDistance = 0;
		int cellX = posX / cellSize;
		int cellY = posY / cellSize;
		cells[cellX + cellY * segments].distanceFromGoal = 0;
		cells[cellX + cellY * segments].UpdateDistanceToNeighbors();
		for (int x = 0; x < segments; ++x) {
			for (int y = 0; y < segments; ++y) {
				cells[x + y * segments].SetVector();
			}
		}
		WaveFrontSimulation();
	}
	
	void WaveFrontSimulation() {
		//std::cout << "max distance: " << maxDistance << std::endl;
		Cell cell;
		for (int x = 0; x < segments; ++x) {
			for (int y = 0; y < segments; ++y) {
				cell = cells[x + y * segments];
				if (cell.isObstacle) continue;
				if (cell.visited) {
					olc::Pixel p;
					p.r = Map(cell.distanceFromGoal, 0, 30, 0, 250);
					p.g = 0;
					p.b = 0;
					p.a = 250;
					FillRect(cell.positionOnGrid.x + 1, cell.positionOnGrid.y  + 1, cellSize - 1, cellSize- 1, p);
				}
				else {
					FillRect(cells[x + y * segments].positionOnGrid.x  + 1, cell.positionOnGrid.y  + 1, cellSize - 1, cellSize- 1, olc::RED);
				}
			}
		}
	}

	void GenerateCells() {
		cells = new Cell[segments * segments];
		for (int x = 0; x < segments; ++x) {
			for (int y = 0; y < segments; ++y) {
				cells[x + y * segments].positionOnGrid = olc::Vector2(x * cellSize, y * cellSize);
				cells[x + y * segments].positionInCellArray = olc::Vector2(x, y);
			}
		}
		AddBorders();
		AssignNeighbors();
	}

	void ResetCells() {
		for (int x = 0; x < segments; ++x) {
			for (int y = 0; y < segments; ++y) {
				cells[x + y * segments].visited = false;
			}
		}
	}

	void AssignNeighbors() {
		ResetCells();
		Cell* currentCell;
		for (int x = 0; x < segments; ++x) {
			for (int y = 0; y < segments; ++y) {
				currentCell = &cells[x + y * segments];
				if (!currentCell->isObstacle) {
					UpdateNeighbors(currentCell, x, y);
				}
			}
		}
	}

	void UpdateNeighbors(Cell* currentCell, int x, int y) {
		
		currentCell->neighbors.clear();

		//top left
		if (!cells[(x - 1) + ((y + 1) * segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x - 1) + ((y + 1) * segments)]);
		}
		//middle left
		if (!cells[(x - 1) + ((y)* segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x - 1) + ((y)* segments)]);
		}
		//bottom left
		if (!cells[(x - 1) + ((y - 1) * segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x - 1) + ((y - 1) * segments)]);
		}

		//top middle
		if (!cells[(x)+((y + 1) * segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x)+((y + 1) * segments)]);
		}
		//bottom middle
		if (!cells[(x)+((y - 1) * segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x)+((y - 1) * segments)]);
		}

		//top right
		if (!cells[(x + 1) + ((y + 1) * segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x + 1) + ((y + 1) * segments)]);
		}
		//middle right
		if (!cells[(x + 1) + ((y)* segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x + 1) + ((y)* segments)]);
		}
		//bottom right
		if (!cells[(x + 1) + ((y - 1) * segments)].isObstacle) {
			currentCell->neighbors.emplace_back(&cells[(x + 1) + ((y - 1) * segments)]);
		}
	}

	void DrawGrid() {
		//horizontal lines
		for (int x = 0; x < ScreenWidth(); x += cellSize) {
			DrawLine(x, 0, x, ScreenHeight(), olc::BLUE);
		}
		//vertical lines
		for (int y = 0; y < ScreenWidth(); y += cellSize) {
			DrawLine(0, y, ScreenWidth(), y, olc::BLUE);
		}
	}

	void AddObstacle(float posX, float posY) {
		int cellX = posX / cellSize;
		int cellY = posY / cellSize;

		int posInArray = cellX + cellY * segments;

		cells[posInArray].isObstacle = true;

		for (auto cell : cells[posInArray].neighbors) {
			UpdateNeighbors(cell, cell->positionInCellArray.x, cell->positionInCellArray.y);
		}

		cells[posInArray].neighbors.clear();

		DrawCell(&cells[posInArray]);
	}

	void RemoveObstacle(float posX, float posY) {
		int cellX = posX / cellSize;
		int cellY = posY / cellSize;

		int posInArray = cellX + cellY * segments;

		if (!cells[posInArray].isBorderCell) {
			cells[posInArray].isObstacle = false;
			
			UpdateNeighbors(&cells[posInArray], cellX, cellY);

			for (auto cell : cells[posInArray].neighbors) {
				UpdateNeighbors(cell, cell->positionInCellArray.x, cell->positionInCellArray.y);
			}

			DrawCell(&cells[posInArray]);
		}
	}

	void AddBorders() {
		//top and bottom borders
		int yPos = (segments - 1) * segments;
		for (int x = 0; x < segments; ++x) {
			cells[x].isObstacle = true;
			cells[x].isBorderCell = true;
			DrawCell(&cells[x]);

			cells[x + yPos].isObstacle = true;
			cells[x + yPos].isBorderCell = true;
			DrawCell(&cells[x + yPos]);

		}

		//left and right bordes
		for (int y = 1; y < segments - 1; ++y) {
			cells[y * segments].isObstacle = true;
			cells[y * segments].isBorderCell = true;
			DrawCell(&cells[y * segments]);

			cells[(segments - 1) +  y * segments].isObstacle = true;
			cells[(segments - 1) + y * segments].isBorderCell = true;
			DrawCell(&cells[(segments - 1) + y * segments]);

		}
	}

	void DrawCell(const Cell* cell) {
		if (cell->isObstacle) {
			if (cell->isBorderCell) {
				FillRect(cell -> positionOnGrid.x  + 1, cell -> positionOnGrid.y  + 1, cellSize - 1, cellSize - 1, olc::YELLOW);
			}
			else {
				FillRect(cell->positionOnGrid.x  + 1, cell->positionOnGrid.y  + 1, cellSize - 1, cellSize - 1, olc::YELLOW);
			}
		}
		else {
			FillRect(cell->positionOnGrid.x + 1, cell->positionOnGrid.y + 1, cellSize - 1, cellSize - 1, olc::BLACK);
		}
	}

	void DrawBlobs() {
		for (auto *b : blobs) {
			FillCircle(b->currentPosition.x, b->currentPosition.y, 5.0f);
		}
	}

	double Map(double mapValue, double valMin, double valMax, double mapMin, double mapMax) {
		if (mapValue >= valMax) return mapMax;
		double ratio = (mapMax - mapMin) / (valMax - valMin);
		return((mapValue - valMin) * ratio + mapMin);
	}
};


int main(int argc, char* argv[]) {

	Application myApplication;

	if (myApplication.Construct(WINDOW_SIZE, WINDOW_SIZE, WINDOW_RATIO, WINDOW_RATIO)) {
		myApplication.Start();
	}

	return 0;
}
