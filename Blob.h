#ifndef BLOB_H
#define BLOB_H

#include "olcPixelGameEngine.h"
#include "Cell.h"

struct Blob {

	float speed = 0.01f;
	olc::Vector2 currentPosition;
	olc::Vector2 currentVelocity;

	Blob(olc::Vector2 _currentPosition)
	{
		currentPosition = _currentPosition;
	}

	void UpdatePosition(Cell* _currentCell) {
		currentVelocity = _currentCell->vectorToGoal;
		//currentPosition.x += currentVelocity.x * speed;
		//currentPosition.y += currentVelocity.x * speed;
		currentPosition.x += 0.1f;
		currentPosition.y += 0.1f;

		//std::cout << currentPosition.x << " " << currentPosition.y << std::endl;
	}
};
#endif // !BLOB_H

