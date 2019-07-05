#ifndef CELL_H
#define CELL_H

#include "olcPixelGameEngine.h"


struct Cell {
	static int maxDistance;
	olc::Vector2 positionOnGrid;
	olc::Vector2 positionInCellArray;
	bool isObstacle = false;
	bool isBorderCell = false;
	float distanceFromGoal = 99999;
	bool visited = false;
	std::vector<Cell*> neighbors;
	olc::Vector2 vectorToGoal;

	Cell() {
		neighbors.reserve(8);
	}

	Cell(olc::Vector2 _position, bool _isObst, bool _isBorder) {
		positionOnGrid = _position;
		isObstacle = _isObst;
		isBorderCell = _isBorder;
	}

	void UpdateDistanceToNeighbors() {
		if (this->distanceFromGoal > maxDistance) maxDistance = this->distanceFromGoal;
		this->visited = true;
		float distToAdd = 0;
		for (auto& cell : neighbors) {
			if (cell->positionOnGrid.x != this->positionOnGrid.x && cell->positionOnGrid.y != this->positionOnGrid.y) {
				distToAdd = 1.4;
			}
			else {
				distToAdd = 1;
			}
			if (!cell->visited) {
				cell->distanceFromGoal = this->distanceFromGoal + distToAdd;
				cell->UpdateDistanceToNeighbors();
			}
			else if (this->distanceFromGoal < cell->distanceFromGoal - 1) {
				cell->distanceFromGoal = this->distanceFromGoal + distToAdd;
				cell->UpdateDistanceToNeighbors();
			}
		}
	}

	void SetVector() {
		//std::cout << neighbors.size() << std::endl;
		if (isObstacle || neighbors.size() == 0) return;
		Cell* closestNeighbor = neighbors[0];
		for (auto& neighbor : neighbors) {
			if (neighbor->distanceFromGoal < closestNeighbor->distanceFromGoal) {
				closestNeighbor = neighbor;
			}
		}
		vectorToGoal = olc::Vector2(closestNeighbor->positionOnGrid.x - this->positionOnGrid.x, closestNeighbor->positionOnGrid.y - this->positionOnGrid.y);
	}
};


#endif // !CELL_H

