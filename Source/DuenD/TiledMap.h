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
    WALL_T
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
        FLOOR = 'f'
    };
    enum eCoordinates
    {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    enum ePatterns
    {
        NONE = 0b0000,
        N    = 0b1000,
        S    = 0b0100,
        E    = 0b0010,
        W    = 0b0001,
        NS   = N | S,
        NE   = N | E,
        NW   = N | W,
        SE   = S | E,
        SW   = S | W,
        EW   = E | W,
        NEW  = NE | W ,
        NSW  = NS | W ,
        NSE  = NS | E ,
        SEW  = SE | W ,
        NEWS = NE | SW,
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

public:

    UFUNCTION(BlueprintCallable, Category = "(DnD)Map Tiles")
    static TArray<FMapTileData> LoadMapArray(const FString fileName, const float multiplier = 1.0f);
};