#include "TiledMap.h"

using PositionPair = TPair<int, int>;

// ----------------------------------------- STATIC
FJsonSerializableArray* UTiledMap::mCharacterMap = nullptr;

// ----------------------------------------- METHODS

// ----------------------------------------- 
// UTiledMap
// ----------------------------------------- 
// ----------------------------------------- 
TArray<FMapTileData> UTiledMap::LoadMapArray(const FString fileName, const float multiplier)
{
	TArray<FMapTileData> output;
	FMapTileData outputElement;

	PositionPair currentPosition = { 0, 0 };

	if (ReadTextFile(fileName))
	{
		for (const auto& line : *mCharacterMap)
		{
			for (const auto& character : line)
			{
				GetCellData(currentPosition, outputElement);
				if (outputElement.Type != eTileType::NONE)
				{
					outputElement.SetLocation(GetMultipliedLocation(currentPosition, multiplier));
					output.Push(outputElement);

					// Add a floor tiles under walls
					if (isWall(character) || isCross(character))
					{
						outputElement.SetType(eTileType::FLOOR);
						outputElement.SetRotation(0.0);
						output.Push(outputElement);
					}
				}
				// Advance Column
				++currentPosition.Key;
			}

			// Advance Row
			currentPosition.Key = 0;
			++currentPosition.Value;
		}
	}
	
	// Destroy temp array
	ClearCharacterMap();

	return output;
}

// ----------------------------------------- 
inline const bool UTiledMap::ReadTextFile(const FString fileName)
{
	mCharacterMap = new FJsonSerializableArray();
	FString rawText;
	// read file
	const bool success = FFileHelper::LoadFileToString(rawText, *(FPaths::ProjectDir() + fileName));
	
	rawText.ParseIntoArrayLines(*mCharacterMap);

	return success;
}

// ----------------------------------------- 
void UTiledMap::ClearCharacterMap()
{
	mCharacterMap->~TArray();
	mCharacterMap = nullptr;
}

// ----------------------------------------- 
void UTiledMap::GetCellData(const PositionPair position, FMapTileData& output)
{
	output.ClearData();

	const char& character = (*mCharacterMap)[position.Value][position.Key];

	if (isFloor(character))
	{
		output.SetType(eTileType::FLOOR);
	}
	else if (isWall(character))
	{
		output.SetType(eTileType::WALL);
		if (isHorizontal(character))
		{
			output.SetRotation(90.0);
		}
	}
	else if(isCross(character))
	{
		ResolveCross(position, output);
	}
}

// ----------------------------------------- 
void UTiledMap::ResolveCross(const PositionPair position, FMapTileData& output)
{
	FGridNeighborhood neighbors(position);

	const auto& map = *mCharacterMap;
	const auto& InBound = [&map](PositionPair& pos)
		{
			return ( map.IsValidIndex(pos.Value) && map[pos.Value].IsValidIndex(pos.Key) );
		};

	const bool adjacent[4] =
	{
		InBound(neighbors.north) && map[neighbors.north.Value][neighbors.north.Key] == eCharacter::VERTICAL_WALL,
		InBound(neighbors.south) && map[neighbors.south.Value][neighbors.south.Key] == eCharacter::VERTICAL_WALL,
		InBound(neighbors.east) && map[neighbors.east.Value][neighbors.east.Key] == eCharacter::HORIZONTAL_WALL,
		InBound(neighbors.west) && map[neighbors.west.Value][neighbors.west.Key] == eCharacter::HORIZONTAL_WALL,
	};

	const bool isVertical = adjacent[eCoordinates::NORTH] && adjacent[eCoordinates::SOUTH];
	const bool isHorizontal = adjacent[eCoordinates::EAST] && adjacent[eCoordinates::WEST];

	if (isVertical)
	{
		// Walls in T
		if (isHorizontal)
		{
			output.SetType(eTileType::WALL_CROSS);
		}
		else if(adjacent[eCoordinates::EAST])
		{
			output.SetType(eTileType::WALL_PERPENDICULAR);
			output.SetRotation(0.0);
		}
		else if (adjacent[eCoordinates::WEST])
		{
			output.SetType(eTileType::WALL_PERPENDICULAR);
			output.SetRotation(180.0);
		}
		else
		{
			output.SetType(eTileType::WALL);
			output.SetRotation(0.0);
		}
	}
	else if (isHorizontal)
	{
		if (adjacent[eCoordinates::NORTH])
		{
			output.SetType(eTileType::WALL_PERPENDICULAR);
			output.SetRotation(-90.0);
		}
		else if (adjacent[eCoordinates::SOUTH])
		{
			output.SetType(eTileType::WALL_PERPENDICULAR);
			output.SetRotation(90.0);
		}
		else
		{
			output.SetType(eTileType::WALL);
			output.SetRotation(90.0);
		}
	}
	else 
	{
		// Corners
		if (adjacent[eCoordinates::NORTH])
		{
			if(adjacent[eCoordinates::EAST])
			{
				output.SetType(eTileType::WALL_CORNER);
				output.SetRotation(-90.0);
			}
			else if (adjacent[eCoordinates::WEST])
			{
				output.SetType(eTileType::WALL_CORNER);
				output.SetRotation(180.0);
			}
			else
			{
				output.SetType(eTileType::WALL);
				output.SetRotation(0.0);
			}
		}
		else if (adjacent[eCoordinates::SOUTH])
		{
			if (adjacent[eCoordinates::EAST])
			{
				output.SetType(eTileType::WALL_CORNER);
				output.SetRotation(0.0);
			}
			else if (adjacent[eCoordinates::WEST])
			{
				output.SetType(eTileType::WALL_CORNER);
				output.SetRotation(90.0);
			}
			else
			{
				output.SetType(eTileType::WALL);
				output.SetRotation(0.0);
			}
		}
		else
		{
			// Edge cases to cross wall
			output.SetType(eTileType::WALL_CROSS);
		}
	}
}

// ----------------------------------------- 
// FGridNeighborhood
// ----------------------------------------- 
// ----------------------------------------- 
FGridNeighborhood::FGridNeighborhood(const PositionPair cellPosition)
{
	north = {cellPosition.Key, cellPosition.Value + 1};
	south = {cellPosition.Key, cellPosition.Value - 1};
	east  = {cellPosition.Key - 1, cellPosition.Value};
	west  = {cellPosition.Key + 1, cellPosition.Value };
}

// ----------------------------------------- 
// FMapTileData
// ----------------------------------------- 
// ----------------------------------------- 
inline void FMapTileData::ClearData()
{
	Type = eTileType::NONE;
	Rotation = 0.0;
	Location.X = 0.0;
	Location.Y = 0.0;
	Location.Z = 0.0;
}