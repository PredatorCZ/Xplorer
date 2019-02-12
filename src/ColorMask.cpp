/*
	ColorMask Texmap object
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

#include "Xplorer.h"
#include "IXTexmaps.h"

enum
{
	colormask_map,
	colormask_extractchannel
};

class ColorMask : public IColorMask
{
	IParamBlock2 *pblock;
	Texmap *subTex;
	DecomposeType outChannel;
	Interval ivalid;
public:
	ColorMask();

	ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
	void Update(TimeValue t, Interval& valid);
	void Init();
	void Reset();
	Interval Validity(TimeValue t) { Interval v = FOREVER; Update(t, v); return v; }

	//Custom controls
	void SetDecomposeType(DecomposeType type) { outChannel = type; pblock->SetValue(colormask_extractchannel, 0, outChannel); }
	DecomposeType GetDecomposeType() { return outChannel; }

	//Submaps
	int NumSubTexmaps() { return 1; }
	Texmap* GetSubTexmap(int) { return subTex; }
	void SetSubTexmap(int, Texmap *m);
	TSTR GetSubTexmapSlotName(int) { return GetString(IDS_MAP); }

	// Evaluators
	AColor EvalColor(ShadeContext& sc);
	float EvalMono(ShadeContext& sc);
	Point3 EvalNormalPerturb(ShadeContext&);

	// Class handlers
	Class_ID ClassID() { return ColorMask_CLASS_ID; }
	SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
	void GetClassName(TSTR& s) { s = GetString(IDS_COLORMASK_CLASS_NAME); }
	void DeleteThis() { delete this; }

	//Params accessors
	int	NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int) { return pblock; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; }

	//Subanimation handlers
	int NumSubs() { return 2; }
	Animatable* SubAnim(int);
	TSTR SubAnimName(int);
	int SubNumToRefNum(int subNum) { return subNum; }

	// Reference accessors
	int NumRefs() { return 2; }
	RefTargetHandle GetReference(int);
	void SetReference(int, RefTargetHandle rtarg);


	RefTargetHandle Clone(RemapDir &remap);
	

#if VERSION_3DSMAX <= VERSION_3DSMAX_E(2014)
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message);
#else
	RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);
#endif
	// IO
	IOResult Save(ISave *isave);
	IOResult Load(ILoad *iload);

	bool IsLocalOutputMeaningful(ShadeContext&) { return true; }

};

static class : public ClassDesc2 
{
public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL) { return new ColorMask; }
	const TCHAR *	ClassName() { return GetString(IDS_COLORMASK_CLASS_NAME); }
	SClass_ID		SuperClassID() { return TEXMAP_CLASS_ID; }
	Class_ID 		ClassID() { return ColorMask_CLASS_ID; }
	const TCHAR* 	Category() { return TEXMAP_CAT_COMP; }
	const TCHAR*	InternalName() { return _T("decompose"); }
	HINSTANCE		HInstance() { return hInstance; }
}colorMaskDesc;

ClassDesc2* GetColorMaskDesc() { return &colorMaskDesc; }

static ParamBlockDesc2 colorvar_param_blk(0, _T("parameters"), 0, &colorMaskDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, 0,
	IDD_COLORMASK, IDS_PARAMS, 0, 0, NULL,
	colormask_map, _T("map"), TYPE_TEXMAP, P_OWNERS_REF, IDS_MAP,
	p_refno, 1,
	p_subtexno, 0,
	p_ui, TYPE_TEXMAPBUTTON, IDC_CUSTOM1,
	p_end,
	colormask_extractchannel, _T("channel"), TYPE_INT, 0, 0,
	p_default, 0,
	p_ui, TYPE_INT_COMBOBOX, IDC_COMBO1, 5, IDS_ALL, IDS_RED, IDS_GREEN, IDS_BLUE, IDS_ALPHA,
	p_end,
	p_end
);

void ColorMask::Init() 
{
	ivalid.SetEmpty();
	subTex = nullptr;
	outChannel = Decompose_All;
}

void ColorMask::Reset() 
{
	colorMaskDesc.Reset(this);
	Init();
}

ColorMask::ColorMask() 
{
	pblock = NULL;
	colorMaskDesc.MakeAutoParamBlocks(this);
	Init();
}

void ColorMask::SetSubTexmap(int, Texmap * m)
{
	ReplaceReference(1, m);
	colorvar_param_blk.InvalidateUI(colormask_map);
	ivalid.SetEmpty();
}

AColor ColorMask::EvalColor(ShadeContext& sc)
{
	if (gbufID) sc.SetGBufferID(gbufID);

	if (!subTex)
		return AColor(0xffffff);

	AColor col = subTex->EvalColor(sc);

	switch (outChannel)
	{
	case Decompose_Red:
		return AColor(col.r, col.r, col.r);
	case Decompose_Green:
		return AColor(col.g, col.g, col.g);
	case Decompose_Blue:
		return AColor(col.b, col.b, col.b);
	case Decompose_Alpha:
		return AColor(col.a, col.a, col.a);
	default:
		return col;
	}
}

float ColorMask::EvalMono(ShadeContext& sc) 
{
	if (gbufID) sc.SetGBufferID(gbufID);

	if (!subTex)
		return 1.0f;

	AColor col = subTex->EvalColor(sc);

	switch (outChannel)
	{
	case Decompose_Red:
		return col.r;
	case Decompose_Green:
		return col.g;
	case Decompose_Blue:
		return col.b;
	case Decompose_Alpha:
		return col.a;
	default:
		return subTex->EvalMono(sc);
	}

}


Point3 ColorMask::EvalNormalPerturb(ShadeContext& sc)
{
	if (!subTex)
		return Point3(0.0f, 0.0f, 0.0f); 

	return subTex->EvalNormalPerturb(sc);
}

Animatable * ColorMask::SubAnim(int i)
{
	if (!i)
		return pblock;
	else
		return subTex;
}

TSTR ColorMask::SubAnimName(int i)
{
	if (!i)
		return GetString(IDS_PARAMS);
	else
		return GetString(IDS_MAP);
}

RefTargetHandle ColorMask::GetReference(int i)
{
	if (!i)
		return pblock;
	else
		return subTex;
}

void ColorMask::SetReference(int i, RefTargetHandle rtarg)
{
	if (!i)
		pblock = static_cast<IParamBlock2*>(rtarg);
	else
		subTex = static_cast<Texmap*>(rtarg);
}

RefTargetHandle ColorMask::Clone(RemapDir &remap)
{
	ColorMask *mnew = new ColorMask();
	*((MtlBase*)mnew) = *((MtlBase*)this);
	mnew->ReplaceReference(0, remap.CloneRef(pblock));
	mnew->ReplaceReference(1, remap.CloneRef(subTex));
	mnew->ivalid.SetEmpty();
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

ParamDlg* ColorMask::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) 
{
	IAutoMParamDlg* masterDlg = colorMaskDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	return masterDlg;
}

void ColorMask::Update(TimeValue t, Interval& valid) 
{
	if (!ivalid.InInterval(t)) 
	{
		ivalid.SetInfinite();

		if (subTex)
			subTex->Update(t, ivalid);

		pblock->GetValue(colormask_extractchannel, t, reinterpret_cast<int&>(outChannel), ivalid);

		NotifyDependents(FOREVER, PART_TEXMAP, REFMSG_CHANGE); //REFMSG_CHANGE, REFMSG_DISPLAY_MATERIAL_CHANGE
	}

	valid &= ivalid;
}
#if VERSION_3DSMAX <= VERSION_3DSMAX_E(2014)
RefResult ColorMask::NotifyRefChanged(Interval /*changeInt*/, RefTargetHandle hTarget, PartID& /*partID*/, RefMessage message)
#else
RefResult ColorMask::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, PartID& /*partID*/, RefMessage message, BOOL /*propagate*/)
#endif
{
	switch (message) 
	{
	case REFMSG_TARGET_DELETED:
	{
		if (hTarget == pblock)
			pblock = nullptr;
		break;
	}

	case REFMSG_CHANGE:
	{
		ivalid.SetEmpty();

		if (hTarget == pblock)
			colorvar_param_blk.InvalidateUI(pblock->LastNotifyParamID());

		break;
	}
	}

	return(REF_SUCCEED);
}

#define MTL_BASE_CHUNK 0x1000
#define VERSION_CHUNK 0x2000
#define VERSION 1

IOResult ColorMask::Save(ISave *isave) 
{
	IOResult result = IO_OK;
	ULONG dummy;

	isave->BeginChunk(MTL_BASE_CHUNK);
	result = MtlBase::Save(isave);

	if (result != IO_OK)
		return result;

	isave->EndChunk();

	int version = VERSION;
	isave->BeginChunk(VERSION_CHUNK);
	isave->Write(&version, 1, &dummy);
	isave->EndChunk();	

	return result;
}

IOResult ColorMask::Load(ILoad *iload) 
{
	IOResult result = IO_OK;
	int id;

	while ((result = iload->OpenChunk()) == IO_OK)
	{
		switch (id = iload->CurChunkID()) 
		{
		case MTL_BASE_CHUNK:
			result = MtlBase::Load(iload);
			break;
		}

		iload->CloseChunk();

		if (result != IO_OK)
			return result;
	}

	if (result == IO_END)
		result = IO_OK;

	return result;
}