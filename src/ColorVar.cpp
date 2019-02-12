/*
	ColorVar Texmap object
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
	colorvar_color
};

class ColorVar : public IColorVar
{
	IParamBlock2 *pblock;
	Color mColor;
	Interval ivalid;
public:
	ColorVar();

	ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp);
	void Update(TimeValue t, Interval& valid);
	void Init();
	void Reset();
	Interval Validity(TimeValue t) { Interval v = FOREVER; Update(t, v); return v; }

	//Custom controls
	void SetColor(Color c, TimeValue t);
	Color GetColor(TimeValue t);

	// Evaluators
	AColor EvalColor(ShadeContext& sc);
	float EvalMono(ShadeContext& sc);
	Point3 EvalNormalPerturb(ShadeContext&) { return Point3(0.0f, 0.0f, 0.0f); }

	// Class handlers
	Class_ID ClassID() { return ColorVar_CLASS_ID; }
	SClass_ID SuperClassID() { return TEXMAP_CLASS_ID; }
	void GetClassName(TSTR& s) { s = GetString(IDS_COLORVAR_CLASS_NAME); }
	void DeleteThis() { delete this; }

	//Params accessors
	int	NumParamBlocks() { return 1; }
	IParamBlock2* GetParamBlock(int) { return pblock; }
	IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; }

	//Subanimation handlers
	int NumSubs() { return 1; }
	Animatable* SubAnim(int) { return pblock; }
	TSTR SubAnimName(int) { return GetString(IDS_PARAMS); }
	int SubNumToRefNum(int subNum) { return subNum; }

	// Reference accessors
	int NumRefs() { return 1; }
	RefTargetHandle GetReference(int) { return pblock; }
	void SetReference(int, RefTargetHandle rtarg) { pblock = (IParamBlock2 *)rtarg; }


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
	void *			Create(BOOL) { return new ColorVar; }
	const TCHAR *	ClassName() { return GetString(IDS_COLORVAR_CLASS_NAME); }
	SClass_ID		SuperClassID() { return TEXMAP_CLASS_ID; }
	Class_ID 		ClassID() { return ColorVar_CLASS_ID; }
	const TCHAR* 	Category() { return TEXMAP_CAT_COMP; }
	const TCHAR*	InternalName() { return _T("ColorVar"); }
	HINSTANCE		HInstance() { return hInstance; }
}colorVarDesc;

ClassDesc2* GetColorVarDesc() { return &colorVarDesc; }

static ParamBlockDesc2 colorvar_param_blk(0, _T("parameters"), 0, &colorVarDesc, P_AUTO_CONSTRUCT + P_AUTO_UI, 0,
	IDD_COLORVAR, IDS_COLORVARPARAMS, 0, 0, NULL,
	colorvar_color, _T("color"), TYPE_RGBA, P_ANIMATABLE, IDS_COLOR,
	p_default, Color(1.0f, 1.0f, 1.0f),
	p_ui, TYPE_COLORSWATCH, IDC_COLORVAR,
	p_end,
	p_end
);

void ColorVar::Init() 
{
	ivalid.SetEmpty();
	SetColor(Color(1.0f, 1.0f, 1.0f), TimeValue(0));
}

void ColorVar::Reset() 
{
	colorVarDesc.Reset(this);
	Init();
}

ColorVar::ColorVar() 
{
	pblock = NULL;
	colorVarDesc.MakeAutoParamBlocks(this);
	Init();
}

AColor ColorVar::EvalColor(ShadeContext& sc) 
{
	if (gbufID) sc.SetGBufferID(gbufID);
	return mColor;
}

float ColorVar::EvalMono(ShadeContext& sc) 
{
	if (gbufID) sc.SetGBufferID(gbufID);
	return Intens(mColor);
}

RefTargetHandle ColorVar::Clone(RemapDir &remap) 
{
	ColorVar *mnew = new ColorVar();
	*((MtlBase*)mnew) = *((MtlBase*)this);
	mnew->ReplaceReference(0, remap.CloneRef(pblock));
	mnew->ivalid.SetEmpty();
	BaseClone(this, mnew, remap);
	return (RefTargetHandle)mnew;
}

ParamDlg* ColorVar::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp) 
{
	IAutoMParamDlg* masterDlg = colorVarDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	return masterDlg;
}

void ColorVar::Update(TimeValue t, Interval& valid) 
{
	if (!ivalid.InInterval(t)) 
	{
		ivalid.SetInfinite();
		pblock->GetValue(colorvar_color, t, mColor, ivalid);
		mColor.ClampMinMax();
		NotifyDependents(FOREVER, PART_TEXMAP, REFMSG_CHANGE); //REFMSG_CHANGE, REFMSG_DISPLAY_MATERIAL_CHANGE
	}

	valid &= ivalid;
}


void ColorVar::SetColor(Color c, TimeValue t)
{
	mColor = c;
	pblock->SetValue(0, t, c);
}

Color ColorVar::GetColor(TimeValue t)
{
	Color retval;
	ivalid.SetInfinite();
	pblock->GetValue(colorvar_color, t, retval, ivalid);
	return retval;
}


#if VERSION_3DSMAX <= VERSION_3DSMAX_E(2014)
RefResult ColorVar::NotifyRefChanged(Interval /*changeInt*/, RefTargetHandle hTarget, PartID& /*partID*/, RefMessage message)
#else
RefResult ColorVar::NotifyRefChanged(const Interval& /*changeInt*/, RefTargetHandle hTarget, PartID& /*partID*/, RefMessage message, BOOL /*propagate*/)
#endif
{
	switch (message) 
	{
	case REFMSG_TARGET_DELETED:
	{
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
#define COLORVAR_VERSION_CHUNK 0x2000
#define COLORVAR_VERSION 1

IOResult ColorVar::Save(ISave *isave) 
{
	IOResult result = IO_OK;
	ULONG dummy;

	isave->BeginChunk(MTL_BASE_CHUNK);
	result = MtlBase::Save(isave);

	if (result != IO_OK)
		return result;

	isave->EndChunk();

	int version = COLORVAR_VERSION;
	isave->BeginChunk(COLORVAR_VERSION_CHUNK);
	isave->Write(&version, 1, &dummy);
	isave->EndChunk();	

	return result;
}

IOResult ColorVar::Load(ILoad *iload) 
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