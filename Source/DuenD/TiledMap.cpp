// Fill out your copyright notice in the Description page of Project Settings.
#include "TiledMap.h"

#include <map>
#include <set>
#include <utility>
#include <vector>

inline void FMapTileData::ClearData()
{
	Type = eTileType::NONE;
	Rotation = 0.0;
	Location.X = 0.0;
	Location.Y = 0.0;
	Location.Z = 0.0;
}

TArray<FMapTileData> UTiledMap::LoadMapArray(const FString fileName, const float multiplier)
{
	using PositionPair = std::pair<int, int>;
	using GridMap = std::map<PositionPair, char>;

	TArray<FMapTileData> output;

	FMapTileData outputElement;
	FString rawText;

	GridMap gridMap; 
	std::vector<PositionPair> unresolvedCrosses;

	if (FFileHelper::LoadFileToString(rawText, *(FPaths::ProjectDir() + fileName)))
	{
		PositionPair offset = std::make_pair(0,0);

		for (const auto& character : rawText)
		{
			outputElement.ClearData();

			if (!UTiledMap::isNewLine(character))
			{
				outputElement.SetLocation(GetMultipliedLocation(offset, multiplier));
				// Wall can be rotated
				if (UTiledMap::isWall(character))
				{
					outputElement.Type = eTileType::WALL;

					if (UTiledMap::isRotated(character))
					{
						outputElement.Rotation = 90.0;
					}
					else
					{
						outputElement.Rotation = 0.0;
					}

					gridMap[offset] = character;
					// Insert to output
					output.Push(outputElement);
					// Also needs floor
					outputElement.SetType(eTileType::FLOOR);
					outputElement.Rotation = 0.0;
					output.Push(outputElement);
				}
				else if (UTiledMap::isCross(character))
				{
					unresolvedCrosses.push_back(offset);
				}
				else if(UTiledMap::isFloor(character))
				{
					outputElement.SetType(eTileType::FLOOR);
					output.Push(outputElement);
				}

				++offset.first;
			}
			else
			{
				offset.first = 0; // Reset columns
				++offset.second;  // Advance Row
			}
		}
	}

	ResolveCrosses(unresolvedCrosses, gridMap, multiplier, output);

	return output;
}

void UTiledMap::ResolveCrosses(std::vector<PositionPair>& unresolvedCrosses, GridMap& gridMap, const float multiplier, TArray<FMapTileData>& output)
{
	using PositionPair = std::pair<int, int>;
	using GridMap = std::map<PositionPair, char>;

	FMapTileData dataToFill;

	for (auto& cross : unresolvedCrosses)
	{
		const PositionPair north = { cross.first, cross.second + 1 };
		const PositionPair south = { cross.first, cross.second - 1 };
		const PositionPair east = { cross.first - 1, cross.second };
		const PositionPair west = { cross.first + 1, cross.second };

		std::set<int> adjacency;

		if (gridMap.find(north) != gridMap.end())
		{
			if (gridMap[north] == eCharacter::VERTICAL_WALL)
			{
				adjacency.insert(eCoordinates::NORTH);
			}
		}
		if (gridMap.find(south) != gridMap.end())
		{
			if (gridMap[south] == eCharacter::VERTICAL_WALL)
			{
				adjacency.insert(eCoordinates::SOUTH);
			}
		}
		if (gridMap.find(east) != gridMap.end())
		{
			if (gridMap[east] == eCharacter::HORIZONTAL_WALL)
			{
				adjacency.insert(eCoordinates::EAST);
			}
		}
		if (gridMap.find(west) != gridMap.end())
		{
			if (gridMap[west] == eCharacter::HORIZONTAL_WALL)
			{
				adjacency.insert(eCoordinates::WEST);
			}
		}

		dataToFill.ClearData();
		dataToFill.SetType(eTileType::WALL_CROSS);
		dataToFill.SetLocation(GetMultipliedLocation(cross, multiplier));

		const bool N_Seek = adjacency.find(eCoordinates::NORTH) != adjacency.end();
		const bool S_Seek = adjacency.find(eCoordinates::SOUTH) != adjacency.end();
		const bool E_Seek = adjacency.find(eCoordinates::EAST) != adjacency.end();
		const bool W_Seek = adjacency.find(eCoordinates::WEST) != adjacency.end();

		const bool isVerticalContinuous = N_Seek && S_Seek;
		const bool isHorizontalContinuous = E_Seek && W_Seek;

		switch (adjacency.size())
		{
		case 3:
			dataToFill.SetType(eTileType::WALL_PERPENDICULAR);
			if (isVerticalContinuous)
			{
				if (W_Seek)
				{
					dataToFill.SetRotation(180.0f);
				}
				else
				{
					dataToFill.SetRotation(0.0f);
				}
			}
			else
			{
				if (S_Seek)
				{
					dataToFill.SetRotation(90.0f);
				}
				else
				{
					dataToFill.SetRotation(-90.0f);
				}
			}
			break;
		case 2:
			if (!( N_Seek && S_Seek) && !isHorizontalContinuous)
			{
				dataToFill.SetType(eTileType::WALL_CORNER);
				if (W_Seek)
				{
					if (S_Seek)
					{
						dataToFill.SetRotation(90.0f);
					}
					else
					{
						dataToFill.SetRotation(180.0f);
					}
				}
				else
				{
					if (S_Seek)
					{
						dataToFill.SetRotation(0.0f);
					}
					else
					{
						dataToFill.SetRotation(-90.0f);
					}
				}
			}
			break;
		}

		output.Push(dataToFill);
		// Also needs floor
		dataToFill.SetType(eTileType::FLOOR);
		dataToFill.Rotation = 0.0;
		output.Push(dataToFill);
	}
}
