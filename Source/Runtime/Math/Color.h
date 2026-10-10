#pragma once

#include "Core/Types.h"

struct FColor
{
	union
	{
		struct
		{
			uint8 R, G, B, A;
		};
		uint32 Bits;
	};

	FColor() = default;

	FVector4 ToVector4(const FColor& C)
	{
		return FVector4(C.R / 255.0f, C.G / 255.0f, C.B / 255.0f, C.A / 255.0f);
	}

	//FColor(EForceInit);
	FColor(uint32 InColor);
	constexpr FColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA);
	//FColor(const FLinearColor& LinearColor);

	uint32& DWColor(void) { return Bits; }
	const uint32& DWColor(void) const { return Bits; }

	friend FArchive& operator<< (FArchive& Ar, FColor& Color)
	{
		return Ar << Color.DWColor();
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}

	// Serializer.
	friend void operator<< (FStructuredArchive::FSlot Slot, FColor& Color)
	{
		return Slot << Color.DWColor();
	}

	[[nodiscard]] inline bool operator==(const FColor& C) const
	{
		return DWColor() == C.DWColor();
	}

	[[nodiscard]] inline bool operator!=(const FColor& C) const
	{
		return DWColor() != C.DWColor();
	}

	inline void operator+=(const FColor& C)
	{
		R = (uint8)FMath::Min((int32)R + (int32)C.R, 255);
		G = (uint8)FMath::Min((int32)G + (int32)C.G, 255);
		B = (uint8)FMath::Min((int32)B + (int32)C.B, 255);
		A = (uint8)FMath::Min((int32)A + (int32)C.A, 255);
	}

	//static const FColor White;
	//static const FColor Black;
	//static const FColor Transparent;
	//static const FColor Red;
	//static const FColor Green;
	//static const FColor Blue;
	//static const FColor Yellow;
	//static const FColor Cyan;
	//static const FColor Magenta;
	//static const FColor Orange;
	//static const FColor Purple;
	//static const FColor Turquoise;
	//static const FColor Silver;
	//static const FColor Emerald;
};