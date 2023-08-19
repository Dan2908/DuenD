// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include <utility>
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "TiledMap.generated.h"

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
 * 
 */
UCLASS()
class DUEND_API UTiledMap : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    using PositionPair = std::pair<int, int>;
    using GridMap = std::map<PositionPair, char>;

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

private:

    static inline const FVector GetMultipliedLocation(const PositionPair location, const float multiplier)
    {
        return FVector(location.first * multiplier, location.second * multiplier, 0.0);
    }

    static inline const bool isRotated(const char character)
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

    static inline const bool isNewLine(const char character)
    {
        return character == eCharacter::NEW_LINE;
    }

    static inline void ResolveCrosses(std::vector<PositionPair>& unresolvedCrosses, GridMap& gridMap, const float multiplier, TArray<FMapTileData>& output);

public:

    UFUNCTION(BlueprintCallable, Category = "(DnD)Map Tiles")
        static TArray<FMapTileData> LoadMapArray(const FString fileName, const float multiplier = 1.0f);
};