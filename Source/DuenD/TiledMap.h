// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TiledMap.generated.h"

using PositionPair = TPair<int, int>;

UENUM(BlueprintType)
enum eTileType
{
    NONE = 0,
    FLOOR,
    WALL,
    WALL_CROSS,
    WALL_CORNER,
    WALL_PERPENDICULAR
};

/** UTiledMap
 *  Gets an array containing elements with info to draw 3D maps from a text file
 */
USTRUCT(BlueprintType)
struct DUEND_API FMapTileData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    TEnumAsByte<eTileType> Type;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FVector Location;
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float Rotation;

    inline void SetType(const eTileType newType) { this->Type = newType; }
    inline void SetLocation(const FVector newLocation) { this->Location = newLocation; }
    inline void SetRotation(const float newRotation) { this->Rotation = newRotation; }

    inline void ClearData();
};

/**
 *  Struct to generate and hold coordinates for neighboring cells around a given one.
 */
USTRUCT()
struct FGridNeighborhood
{
    GENERATED_BODY()

    PositionPair north;
    PositionPair south;
    PositionPair east;
    PositionPair west;

    FGridNeighborhood() = default;
    FGridNeighborhood(const PositionPair cellPosition);
};

//-------------------------------------------------------------
// Main Class
//-------------------------------------------------------------

UCLASS()
class DUEND_API UTiledMap : public UBlueprintFunctionLibrary
{
private:
    GENERATED_BODY()

    // Auxiliar enums for positions and characters
    enum eCharacter
    {
        HORIZONTAL_WALL = '-',
        VERTICAL_WALL = '|',
        CROSS_WALL = '+',
        FLOOR = 'f',
        NEW_LINE = '\n'
    };
    enum eCoordinates
    {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    // Contains the character map in a 2d array
    static FJsonSerializableArray* mCharacterMap;
    // Returns the 3D position vectorresulting of scalar multiply of a location pair.
    static inline const FVector GetMultipliedLocation(const PositionPair location, const float multiplier)
    {
        return FVector(location.Key * multiplier, location.Value * multiplier, 0.0);
    }

    static inline const bool isHorizontal(const char character)
    {
        return character == eCharacter::HORIZONTAL_WALL;
    }

    static inline const bool isWall(const char character)
    {
        return character == eCharacter::HORIZONTAL_WALL
            || character == eCharacter::VERTICAL_WALL;
    }

    static inline const bool isCross(const char character)
    {
        return character == eCharacter::CROSS_WALL;
    }

    static inline const bool isFloor(const char character)
    {
        return character == eCharacter::FLOOR;
    }

    /** 
    * Reads the text contained in file 'filename', and stores in the array of strings mCharacterMap;
    */
    static inline const bool ReadTextFile(const FString fileName);
    /**
    * Destroys Heap mCharacterMap
    */
    static inline void ClearCharacterMap();

    static void GetCellData(const PositionPair position, FMapTileData& output);
    static void ResolveCross(const PositionPair position, FMapTileData& output);
    //static inline void ResolveCrosses(std::vector<PositionPair>& unresolvedCrosses, GridMap& gridMap, const float multiplier, TArray<FMapTileData>& output);

public:

    UFUNCTION(BlueprintCallable, Category = "(DnD)Map Tiles")
    static TArray<FMapTileData> LoadMapArray(const FString fileName, const float multiplier = 1.0f);
};