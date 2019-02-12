/*
	Xplorer plugin for 3ds max, include header
	Copyright(C) 2019 Lukas Cone

	This program is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.If not, see < https://www.gnu.org/licenses/>.
*/

#pragma once
#include <maxapi.h>
#include <stdmat.h>


#define ColorVar_CLASS_ID	Class_ID(0x40a1bc89, 0xa4184816)

class IColorVar : public Texmap
{
public:
	virtual void SetColor(Color c, TimeValue t) = 0;
	virtual Color GetColor(TimeValue t) = 0;
	static IColorVar *Create()
	{
		return static_cast<IColorVar*>(GetCOREInterface()->CreateInstance(TEXMAP_CLASS_ID, ColorVar_CLASS_ID));
	}
};

#define ColorMask_CLASS_ID Class_ID(0x10174a63, 0x4c915867)

enum DecomposeType
{
	Decompose_All,
	Decompose_Red,
	Decompose_Green,
	Decompose_Blue,
	Decompose_Alpha,
};

class IColorMask : public Texmap
{
public:
	virtual void SetDecomposeType(DecomposeType type) = 0;
	virtual DecomposeType GetDecomposeType() = 0;
	static IColorMask *Create()
	{
		return static_cast<IColorMask*>(GetCOREInterface()->CreateInstance(TEXMAP_CLASS_ID, ColorMask_CLASS_ID));
	}
};