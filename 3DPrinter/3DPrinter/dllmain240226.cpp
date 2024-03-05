// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include"CommonHeader.h"
#include <ProDtmCsys.h>
#include <locale>
#include <string>
#include <vector>
#include"VesModelTree.h"
#include "ProDtmPnt.h"
//#include <iomanip>
//#include <boost/foreach.hpp>
//#include "C:\\Users\\kmsma\\Downloads\\windows-cxx-x64\\prebuilds\\windows-cxx-x64\\include\\opencamlib\\stlsurf.hpp"
//#include "C:\\Users\\kmsma\\Downloads\\windows-cxx-x64\\prebuilds\\windows-cxx-x64\\include\\opencamlib\\stlreader.hpp"
//#include "C:\\Users\\kmsma\\Downloads\\windows-cxx-x64\\prebuilds\\windows-cxx-x64\\include\\opencamlib\\cylcutter.hpp"
//#include "C:\\Users\\kmsma\\Downloads\\windows-cxx-x64\\prebuilds\\windows-cxx-x64\\include\\opencamlib\\adaptivepathdropcutter.hpp"
//#include <cmath>
//#include <limits>
//#ifdef _OPENMP
//#include <omp.h>
//#endif
////BOOL APIENTRY DllMain( HMODULE hModule,
////                       DWORD  ul_reason_for_call,
////                       LPVOID lpReserved
////                     )
////{
////    switch (ul_reason_for_call)
////    {
////    case DLL_PROCESS_ATTACH:
////    case DLL_THREAD_ATTACH:
////    case DLL_THREAD_DETACH:
////    case DLL_PROCESS_DETACH:
////        break;
////    }
////    return TRUE;
////}
struct asmCompData
{
	ProMdl lMdlAssm;
	ProAsmcomppath lAsmPath;
	std::string lAsmName;
};
struct PointData
{
	ProMdl lMdlAssm;
	ProAsmcomppath lAsmPath;
	ProFeature lPointFeat;

};

ProSelection* placementSurfSel;

static char* PushButton1 = "PushButton1";
static char* PushButton2 = "PushButton2";
static char* PushButton3 = "PushButton3";
static char* PushButton4 = "PushButton4";
static char* PushButton5 = "PushButton5";
asmCompData lAsmData;
int iWindId = -1;
vector<asmCompData> g_reqComp;
ProMdl capacitor_model;
ProMdl Antenna_model;
static char* dialogname = "3DPRINTER";
static char* capacitorRadio = "capacitorRadio1";
static char* capacitorLabel = "capacitorLabel";
static char* capacitorAsm = "capacitorAsm";
static char* capacitorUpdate = "capacitorUpdate";
static char* antennaRadio = "antennaRadio1";
static char* antennaLabel = "antennaLabel";
static char* antennaAsm = "antennaAsm";
static char* antennaUpdate = "antennaUpdate";

static char* Preview = "btnPreview";
static char* okbtn = "btnOK";
static char* closebtn = "btnCancel";

// UpdateAntennaUI
static char* updateDimdialogname = "UPDATEDIMENSION";
static char* capacitorHeightInput = "capacitorHeight";
static char* capacitorDiaInput = "capacitorDia";
static char* applycapupdatebtn = "applyCapUpdate";
static char* cancelcapupdatebtn = "cancelCapUpdate";
double* ProUtilVectorCopy(
	double from[3],
	double to[3])
{
	if (from == NULL)
		to[0] = to[1] = to[2] = 0.0;
	else
	{
		to[0] = from[0];
		to[1] = from[1];
		to[2] = from[2];
	}
	return(to);
}

/*====================================================================*\
	FUNCTION :	ProUtilPointTrans()
	PURPOSE  :	Transform a 3d point by a matrix
\*====================================================================*/
void ProUtilPointTrans(
	double m[4][4],
	double p[3],
	double output[3])
{
	double buff[4];
	ProError status;

	if (m == NULL)
	{
		ProUtilVectorCopy(p, output);
		return;
	}

	status = ProPntTrfEval(p, m, buff);

	ProUtilVectorCopy(buff, output);

}
static double identity_matrix[4][4] = { {1.0, 0.0, 0.0, 0.0},
					{0.0, 1.0, 0.0, 0.0},
					{0.0, 0.0, 1.0, 0.0},
					{0.0, 0.0, 0.0, 1.0} };
void ProUtilMatrixCopy1(
	double input[4][4],
	double output[4][4])
{
	int i, j;

	if (input == NULL)
	{
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++)
				output[i][j] = identity_matrix[i][j];
	}
	else
	{
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++)
				output[i][j] = input[i][j];
	}
}
/*====================================================================*\
	FUNCTION :	ProUtilMatrixInvert()
	PURPOSE  :	Find the inverse of a transformation matrix
\*====================================================================*/
int ProUtilMatrixInvert(
	double m[4][4],
	double output[4][4])
{
	double vec[3], scale_sq, inv_sq_scale;
	int i, j;

	/*--------------------------------------------------------------------*\
		If the matrix is null, return the identity matrix
	\*--------------------------------------------------------------------*/
	if (m == NULL)
	{
		ProUtilMatrixCopy1(NULL, output);
		return(1);
	}

	/*--------------------------------------------------------------------*\
		Obtain the matrix scale
	\*--------------------------------------------------------------------*/
	vec[0] = m[0][0];
	vec[1] = m[0][1];
	vec[2] = m[0][2];
	scale_sq = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];

	/*--------------------------------------------------------------------*\
		Check whether there is an inverse, and if not, return 0
	\*--------------------------------------------------------------------*/
	if (scale_sq < (.000000001 * .000000001))
		return(0);

	/*--------------------------------------------------------------------*\
		Need the inverse scale squared
	\*--------------------------------------------------------------------*/
	inv_sq_scale = 1.0 / scale_sq;

	/*--------------------------------------------------------------------*\
		The orientation vectors
	\*--------------------------------------------------------------------*/
	for (j = 0; j < 3; j++)
	{
		for (i = 0; i < 3; i++)
			output[j][i] = m[i][j] * inv_sq_scale;
		output[j][3] = 0.0;
	}

	/*--------------------------------------------------------------------*\
		The shift vectors
	\*--------------------------------------------------------------------*/
	for (i = 0; i < 3; i++)
	{
		output[3][i] = 0.0;
		for (j = 0; j < 3; j++)
			output[3][i] -= m[i][j] * m[3][j] * inv_sq_scale;
	}
	output[3][3] = 1.0;

	return(1);
}


struct UserCsysData
{
private:
	double m_XVal, m_YVal, m_ZVal, m_XRVal, m_YRVal, m_ZRVal;
	bool  b_XVal, b_YVal, b_ZVal, b_XRVal, b_YRVal, b_ZRVal;

public:
	UserCsysData()
	{
		b_XVal = b_YVal = b_ZVal = b_XRVal = b_YRVal = b_ZRVal = false;
		m_XVal = m_YVal = m_ZVal = m_XRVal = m_YRVal = m_ZRVal = 0.0;
	}
	void SetXVal(double XVal)
	{
		if (!isnan(XVal))
		{
			m_XVal = XVal;
			b_XVal = true;
		}
	}
	void SetYVal(double YVal)
	{
		if (!isnan(YVal))
		{
			m_YVal = YVal;
			b_YVal = true;
		}
	}
	void SetZVal(double ZVal)
	{
		if (!isnan(ZVal))
		{
			m_ZVal = ZVal;
			b_ZVal = true;
		}
	}
	void SetXRVal(double XRVal)
	{
		if (!isnan(XRVal))
		{
			m_XRVal = XRVal;
			b_XRVal = true;
		}
	}
	void SetYRVal(double YRVal)
	{
		if (!isnan(YRVal))
		{
			m_YRVal = YRVal;
			b_YRVal = true;
		}
	}
	void SetZRVal(double ZRVal)
	{
		if (!isnan(ZRVal))
		{
			m_ZRVal = ZRVal;
			b_ZRVal = true;
		}
	}

	std::tuple <bool, double> GetXVal()
	{
		return make_tuple(b_XVal, m_XVal);
	}

	tuple <bool, double>  GetYVal()
	{
		return make_tuple(b_YVal, m_YVal);
	}
	tuple <bool, double>   GetZVal()
	{
		return make_tuple(b_ZVal, m_ZVal);
	}
	tuple <bool, double>  GetXRVal()
	{
		return make_tuple(b_XRVal, m_XRVal);
	}
	tuple <bool, double>  GetYRVal()
	{
		return make_tuple(b_YRVal, m_YRVal);
	}
	tuple <bool, double>  GetZRVal()
	{
		return make_tuple(b_ZRVal, m_ZRVal);
	}
};
ProError Collect_SubLevelComponents(ProAsmcomppath* p_path, ProSolid handle, ProBoolean xBool, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	if (xBool != PRO_B_FALSE)
	{
		ProName lAssmName;
		std::string lName;

		lErr = ProMdlNameGet((ProMdl)handle, lAssmName);

		char* lChar = new char[260];
		ProWstringToString(lChar, lAssmName);
		lName = lChar;

		lAsmData.lAsmName = lName;
		lAsmData.lAsmPath = *p_path;
		lAsmData.lMdlAssm = handle;

		ProMdlType lMdlType;
		lErr = ProMdlTypeGet(lAsmData.lMdlAssm, &lMdlType);
		if (lMdlType == PRO_MDL_ASSEMBLY /*&& ((lName == "CAPACITOR_1"))*/)
		{
			g_reqComp.push_back(lAsmData);
		}
	}
	return PRO_TK_NO_ERROR;
}

ProError SolidCsysFeatVisitAction(ProFeature* p_feature, ProError status, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	ProFeattype lFeatType;
	lErr = ProFeatureTypeGet(p_feature, &lFeatType);
	ProName feattypename;
	ProFeatureTypenameGet(p_feature, feattypename);
	vector<ProFeature>* lpVecfeature = (vector<ProFeature>*)(app_data);
	if (PRO_FEAT_CSYS == lFeatType)
	{
		lpVecfeature->push_back(*p_feature);
	}

	return lErr;
}
ProError PointFeatVisitAction(ProFeature* p_feature, ProError status, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	ProFeattype lFeatType;
	lErr = ProFeatureTypeGet(p_feature, &lFeatType);
	ProName feattypename;
	ProFeatureTypenameGet(p_feature, feattypename);
	vector<ProFeature>* lpVecfeature = (vector<ProFeature>*)(app_data);
	if (PRO_POINT == lFeatType)
	{
		lpVecfeature->push_back(*p_feature);
	}

	return lErr;
}
ProError SolidCsysFeatVisit(ProMdl xMdl, vector<ProFeature>& xProfeature)
{
	ProError lErr = PRO_TK_NO_ERROR;
	lErr = ProSolidFeatVisit((ProSolid)xMdl, (ProFeatureVisitAction)SolidCsysFeatVisitAction, NULL, (void*)&xProfeature);
	return lErr;
}
ProError PointsFeatVisit(ProMdl xMdl, vector<ProFeature>& xProfeature)
{
	ProError lErr = PRO_TK_NO_ERROR;
	lErr = ProSolidFeatVisit((ProSolid)xMdl, (ProFeatureVisitAction)PointFeatVisitAction, NULL, (void*)&xProfeature);
	return lErr;
}
ProError FeatVisitActionMap(ProFeature* feature, ProError status, ProAppData appdata)
{
	ProFeatStatus fStatus;
	std::map<string, ProFeature>* pFeatMap = (std::map<string, ProFeature>*)appdata;

	ProName FeatName, FeatDefName;
	status = ProModelitemNameGet(feature, FeatName);

	//TODO if the new name is not present then get default name
	//status = ProModelitemDefaultnameGet(feature, FeatDefName);
	ProFeattype FeatType;
	status = ProFeatureTypeGet(feature, &FeatType);
	if (status == PRO_TK_NO_ERROR)
	{
		ProCharName CharFeatName;
		ProWstringToString(CharFeatName, FeatName);
		string sFeatName(CharFeatName);
		pFeatMap->operator[](sFeatName) = (*feature);
	}

	return PRO_TK_NO_ERROR;
}
void GetModelFeatures(ProMdl Mdl, std::map<string, ProFeature>& FeatureVector)
{
	ProError status;
	FeatureVector.clear();
	status = ProSolidFeatVisit((ProSolid)Mdl, FeatVisitActionMap, NULL, (void*)&FeatureVector);
}

bool GetFeatureByName(IN ProMdl Mdl, IN string FeatName, OUT ProFeature& outFeat)
{
	std::map<string, ProFeature> FeatureVector;
	GetModelFeatures(Mdl, FeatureVector);

	auto it = FeatureVector.find(FeatName);
	if (it != FeatureVector.end())
	{
		outFeat = it->second;
		return true;
	}
	else
		return false;
}
ProError VesGeomItemVisitAction(ProGeomitem* p_handle, ProError status, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	vector<ProGeomitem>* lpVecfeature = (vector<ProGeomitem>*)(app_data);
	lpVecfeature->push_back(*p_handle);
	return lErr;
}
void GetGeomItems(IN ProMdl Mdl, IN ProFeature Feat, OUT vector<ProGeomitem>& vecGeomItems)
{
	ProError lErr = PRO_TK_NO_ERROR;
	lErr = ProFeatureGeomitemVisit(&Feat, PRO_TYPE_UNUSED, VesGeomItemVisitAction, NULL, (ProAppData)&vecGeomItems);
}

void GetCurveGeomItems(IN ProMdl Mdl, IN ProFeature Feat, OUT vector<ProGeomitem>& vecGeomItems)
{
	ProError lErr = PRO_TK_NO_ERROR;
	lErr = ProFeatureGeomitemVisit(&Feat, PRO_CURVE, VesGeomItemVisitAction, NULL, (ProAppData)&vecGeomItems);
}

//ProError groupFeatVisitAction(ProFeature* feat, ProError status, ProAppData app_data)
//{
//	ProError lErr = PRO_TK_NO_ERROR;
//	vector<ProGeomitem>* lpVecfeature = (vector<ProGeomitem>*)(app_data);
//	lErr = ProFeatureGeomitemVisit(feat, PRO_TYPE_UNUSED, VesGeomItemVisitAction, NULL, (ProAppData)&lpVecfeature);
//	return lErr;
//}

//void GetGeomItemsFromGroup(IN ProMdl Mdl, IN ProFeature Feat, OUT vector<ProGeomitem>& vecGeomItems)
//{
//	ProError lErr = PRO_TK_NO_ERROR;
//	lErr = ProGroupFeatureVisit(&Feat, groupFeatVisitAction, NULL, (ProAppData)&vecGeomItems);
//}

//void FillUDFDataWithReferenceForCopyFeat(ProUdfdata& UdfData, vector<ProSelection>& sel2, ProBoolean RefAsm)
//{
//	ProError status = PRO_TK_NO_ERROR;
//	ProUdfRequiredRef* requiredRef;
//	status = ProUdfdataRequiredreferencesGet(UdfData, &requiredRef);
//	int sizeRef = 0;
//	status = ProArraySizeGet(requiredRef, &sizeRef);
//	ProUdfreference* udfReference = NULL;
//	status = ProArrayAlloc(sizeRef, sizeof(ProUdfreference), 1, (ProArray*)&udfReference);
//	for (int i = 0; i < sizeRef; i++)
//	{
//		udfReference[i] = NULL;
//		ProLine prompt;
//		ProType type;
//		status = ProUdfrequiredrefPromptGet(requiredRef[i], prompt);
//		status = ProUdfrequiredrefTypeGet(requiredRef[i], &type);
//		status = ProUdfreferenceAlloc(prompt, sel2[i], RefAsm, &udfReference[i]); //For Assembly RefAsm = TRUE
//		status = ProUdfdataReferenceAdd(UdfData, udfReference[i]);
//	}
//	for (int i = 0; i < sizeRef; i++)
//	{
//		status = ProUdfreferenceFree(udfReference[i]);
//		udfReference[i] = NULL;
//	}
//	status = ProArrayFree((ProArray*)&udfReference);
//	udfReference = NULL;
//}
//bool LoadUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat,
//	ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/)
//{
//	ProErr lErr, status;
//	bool isCreatedUDF = false;
//	ProUdfdata UdfData = NULL;
//	lErr = ProUdfdataAlloc(&UdfData);
//
//	ProPath UdfFilePath;
//
//
//	ProStringToWstring(UdfFilePath, (char*)UDFFileName.c_str());
//
//
//	lErr = ProUdfdataPathSet(UdfData, UdfFilePath);
//	FillUDFDataWithReferenceForCopyFeat(UdfData, UDFInputs, RefAsm);
//
//	//ProUdfCreateOption option[] = { PROUDFOPT_EDIT_MENU };
//	ProUdfCreateOption option[] = { PROUDFOPT_NO_REDEFINE };
//
//	ProName udfNameOrginalName = L"NewName";
//	status = ProUdfdataNameSet(UdfData, udfNameOrginalName, udfNameOrginalName); //TODO why this? not working chk
//	status = ProUdfdataInstancenameSet(UdfData, udfNameOrginalName);
//	ProFeature createdFeature;
//	status = ProUdfCreate((ProSolid)Mdl, UdfData, asmCompPath, option, 1, &createdFeature);
//	if (status == PRO_TK_NO_ERROR)
//		isCreatedUDF = true;
//	UDFGrpFeat = createdFeature;
//
//	lErr = ProTreetoolRefresh((ProSolid)Mdl);
//
//	return isCreatedUDF;
//}
void FillUDFDataWithReferenceForCopyFeat(ProUdfdata& UdfData, vector<ProSelection>& sel2, ProBoolean RefAsm)
{
	ProError status = PRO_TK_NO_ERROR;
	ProUdfRequiredRef* requiredRef;
	status = ProUdfdataRequiredreferencesGet(UdfData, &requiredRef);
	int sizeRef = 0;
	status = ProArraySizeGet(requiredRef, &sizeRef);
	ProUdfreference* udfReference = NULL;
	status = ProArrayAlloc(sizeRef, sizeof(ProUdfreference), 1, (ProArray*)&udfReference);
	status = ProUdfdataDimdisplaySet(UdfData, PROUDFDIMDISP_NORMAL);
	for (int i = 0; i < sizeRef; i++)
	{
		udfReference[i] = NULL;
		ProLine prompt;
		ProType type;
		status = ProUdfrequiredrefPromptGet(requiredRef[i], prompt);
		status = ProUdfrequiredrefTypeGet(requiredRef[i], &type);
		status = ProUdfreferenceAlloc(prompt, sel2[i], RefAsm, &udfReference[i]); //For Assembly RefAsm = TRUE
		status = ProUdfdataReferenceAdd(UdfData, udfReference[i]);
	}
	for (int i = 0; i < sizeRef; i++)
	{
		status = ProUdfreferenceFree(udfReference[i]);
		udfReference[i] = NULL;
	}
	status = ProArrayFree((ProArray*)&udfReference);
	udfReference = NULL;
}
bool LoadUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat,
	ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/)
{
	ProErr lErr, status;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	lErr = ProUdfdataAlloc(&UdfData);

	ProPath UdfFilePath;


	ProStringToWstring(UdfFilePath, (char*)UDFFileName.c_str());


	lErr = ProUdfdataPathSet(UdfData, UdfFilePath);
	FillUDFDataWithReferenceForCopyFeat(UdfData, UDFInputs, RefAsm);

	//ProUdfCreateOption option[] = { PROUDFOPT_EDIT_MENU };
	ProUdfCreateOption option[] = { PROUDFOPT_NO_REDEFINE };

	ProName udfNameOrginalName = L"NewName";
	status = ProUdfdataNameSet(UdfData, udfNameOrginalName, udfNameOrginalName); //TODO why this? not working chk
	//status = ProUdfdataInstancenameSet(UdfData, udfNameOrginalName);
	ProFeature createdFeature;
	status = ProUdfCreate((ProSolid)Mdl, UdfData, NULL, option, 1, &createdFeature);
	if (status == PRO_TK_NO_ERROR)
		isCreatedUDF = true;
	UDFGrpFeat = createdFeature;

	lErr = ProTreetoolRefresh((ProSolid)Mdl);

	return isCreatedUDF;
}

void WcharToString(wchar_t* xWString, std::string& xString)
{
	// Converts wchar to string
	std::wstring lStr(xWString);

	std::string test(lStr.begin(), lStr.end());
	xString = test;

}
ProError CompFeatVisitActionMap(ProFeature* feature, ProError status, ProAppData appdata)
{
	ProFeatStatus fStatus;
	std::map<string, vector<ProFeature>>* pFeatMap = (std::map<string, vector<ProFeature>>*)appdata;

	ProFeattype lFeatType;
	status = ProFeatureTypeGet(feature, &lFeatType);

	if (lFeatType == PRO_FEAT_COMPONENT)
	{
		ProMdlfileType r_mdl_type;
		ProFamilyMdlName r_mdl_name;

		status = ProAsmcompMdlMdlnameGet((ProAsmcomp*)feature, &r_mdl_type, r_mdl_name);
		string sFeatName;
		WcharToString(r_mdl_name, sFeatName);
		pFeatMap->operator[](sFeatName).push_back(*feature);

	}
	return PRO_TK_NO_ERROR;
}
void GetModelCompFeatures(ProMdl Mdl, std::map<string, vector<ProFeature>>& FeatureVector)
{
	ProError status;
	FeatureVector.clear();
	status = ProSolidFeatVisit((ProSolid)Mdl, CompFeatVisitActionMap, NULL, (void*)&FeatureVector);
}
bool GetCompFeatureByName(IN ProMdl Mdl, IN string FeatName, OUT vector<ProFeature>& outFeat)
{
	std::map<string, vector<ProFeature>> FeatureVector;
	GetModelCompFeatures(Mdl, FeatureVector);
	std::transform(FeatName.begin(), FeatName.end(), FeatName.begin(), ::toupper);
	auto it = FeatureVector.find(FeatName);
	if (it != FeatureVector.end())
	{
		outFeat = it->second;
		return true;
	}
	else
		return false;

	//outFeat = FeatureVector[FeatName];
}
void stringToWString(IN string schar, OUT  wchar_t* wcharout)
{
	wstring ch(schar.begin(), schar.end());
	wcscpy_s(wcharout, ch.size() + 1, ch.c_str());
}
ProErr GetParamValue(IN ProMdl Mdl, IN string ParamName, OUT string& ParamVal, bool DoRegen)
{
	ProError lErr = PRO_TK_NO_ERROR;
	if (true == DoRegen)
		ProSolidRegenerate((ProSolid)Mdl, PRO_REGEN_NO_FLAGS);

	ProModelitem lModelItem;
	lErr = ProMdlToModelitem(ProMdlToSolid(Mdl), &lModelItem);
	ProName lParameterName;
	stringToWString(ParamName, lParameterName);
	ProParameter lParam;
	lErr = ProParameterInit(&lModelItem, lParameterName, &lParam);
	ProParamvalue lValue;
	//lErr = ProParameterValueGet(&lParam, &lValue);  //" "
	lErr = ProParameterValueWithUnitsGet(&lParam, &lValue, NULL);  //" " " "
	if (lValue.type == PRO_PARAM_STRING)
	{
		WcharToString(lValue.value.s_val, ParamVal);
	}
	return lErr;

}
ProError GroupFeatVisitAction(ProFeature* feature, ProError status, ProAppData appdata)
{
	ProFeatStatus fStatus;
	vector<ProFeature>* pFeatVec = (vector<ProFeature>*)appdata;
	ProFeattype lFeatType;
	ProMdl FeatMdl;
	ProName pGalleryname;
	string strGalleryName;
	status = ProFeatureTypeGet(feature, &lFeatType);
	if (lFeatType == PRO_FEAT_CURVE)
	{
		pFeatVec->push_back(*feature);
	}
	return PRO_TK_NO_ERROR;

}
void GetGroupFeatures(ProGroup group, std::vector<ProFeature>& FeatureVector)
{
	ProError status;
	FeatureVector.clear();
	status = ProGroupFeatureVisit(&group, GroupFeatVisitAction, NULL, (void*)&FeatureVector);
}
void LoadUDFWithInputs(const ProMdl& CurMdl1, std::vector<VesModelTree::AsmTreeData>& plateTreeVec_item1, int j, int i, std::string featName, string Udfpath, ProFeature& UDFGrpFeat)
{
	ProFeature csoFeat;
	ProFeature csoFeat1;
	ProError status;
	std::vector<PointData> vectPointData;
	std::vector<ProSelection> UdfInputSel;
	PointData curPointData = {};
	curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
	curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
	GetFeatureByName(CurMdl1, "PRINTED_ELECTRONICS_ASM", csoFeat);
	curPointData.lPointFeat = csoFeat;

	PointData curPointData1 = {};
	curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
	curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
	GetFeatureByName(CurMdl1, "PRINTED_ELECTRONICS_ASM", csoFeat1);
	curPointData1.lPointFeat = csoFeat1;

	vectPointData.push_back(curPointData1);
	vectPointData.push_back(curPointData);
	for (size_t k = 0; k < vectPointData.size(); k++)
	{
		//Get Point Feature Selection
		vector<ProGeomitem> vecGeomItems;
		GetGeomItems(vectPointData[k].lMdlAssm, vectPointData[k].lPointFeat, vecGeomItems);
		ProSelection pointSelection;
		status = ProSelectionAlloc(&vectPointData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
		UdfInputSel.push_back(pointSelection);
	}

	UdfInputSel.push_back(placementSurfSel[0]);


	//ProFeature UDFGrpFeat;

	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
	{
		//VecAxisUdfs.push_back(UDFGrpFeat);
		ProFeature* feats = NULL;
		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
		int FeatSize;
		status = ProArraySizeGet(feats, &FeatSize);

	}
}

//void LoadUDFWithInputs(const ProMdl& CurMdl1, std::vector<VesModelTree::AsmTreeData>& plateTreeVec_item1, int j, int i, std::string featName, string Udfpath, ProFeature& UDFGrpFeat)
//{
//	ProFeature csoFeat;
//	ProFeature csoFeat1;
//	ProError status;
//	std::vector<PointData> vectPointData;
//	std::vector<ProSelection> UdfInputSel;
//	PointData curPointData = {};
//	curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	PointData curPointData1 = {};
//	curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//	vectPointData.push_back(curPointData);
//	vectPointData.push_back(curPointData1);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(vectPointData[k].lMdlAssm, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&vectPointData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	//ProFeature UDFGrpFeat;
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//}
void ProjectingCurveOnSurface(VesModelTree& mdlObj, const ProMdl& CurMdl1, ProAsmcomppath& comp_path, string PlaneName, string CurveName, ProFeature& UDFGrpFeat, string Udfpath1)
{
	ProError status;
	vector<ProSelection> UdfInputSel1;
	ProFeature curve;
	ProFeature curve1;
	ProFeature frontPlane;
	vector<ProGeomitem> vecPlaneGeomItems;
	vector<ProGeomitem> vecCurveGeomItems;

	string childComp1 = "PRINTED_ELECTRONICS";
	VesModelTree::AsmTreeData  item1;
	bool res = mdlObj.GetAsmCompTreeData(childComp1, item1);
	ProGroup group;

	GetFeatureByName(CurMdl1, PlaneName, frontPlane);
	GetFeatureByName(CurMdl1, CurveName, curve);
	status = ProFeatureGroupGet(&curve, &group);
	vector<ProFeature> vecFeat;
	GetGroupFeatures(group, vecFeat);
	status = ProFeatureInit((ProSolid)CurMdl1, vecFeat[0].id, &curve);
	GetGeomItems(CurMdl1, frontPlane, vecPlaneGeomItems);
	GetGeomItems(CurMdl1, curve, vecCurveGeomItems);


	ProSelection SurfSelection1;
	ProSelection SurfSelection2;
	ProSelection CurveSelection;
	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecPlaneGeomItems[0], &SurfSelection2);
	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecCurveGeomItems[0], &CurveSelection);

	ProSurface lSurface;
	ProGeomitemdata* geomData;
	ProGeomitem comp_datum;
	status = ProSurfaceInit(item1.lMdlAssm, 41245, &lSurface);
	status = ProSurfaceToGeomitem((ProSolid)item1.lMdlAssm, lSurface, &comp_datum);
	status = ProSelectionAlloc(&item1.lAsmPath, &comp_datum, &SurfSelection1);

	UdfInputSel1.push_back(SurfSelection1);
	UdfInputSel1.push_back(CurveSelection);
	UdfInputSel1.push_back(SurfSelection2);

	ProFeature UDFGrpFeat1;

	if (LoadUDF(CurMdl1, Udfpath1, UdfInputSel1, UDFGrpFeat1, NULL, PRO_B_TRUE))
	{
		//VecAxisUdfs.push_back(UDFGrpFeat);
		ProFeature* feats = NULL;
		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
		int FeatSize;
		status = ProArraySizeGet(feats, &FeatSize);

	}
	status = ProModelitemHide(&vecFeat[0]);
}

int createDatumPointFeature(ProPoint3d pnt3d, ProSelection& csys_sel)
{
	ProError status;
	ProElement featElemTree = nullptr;
	status = ProElementAlloc(PRO_E_FEATURE_TREE, &featElemTree);

	//PRO_E_FEATURE_TYPE
	ProElement featureTypeElem = nullptr;
	status = ProElementAlloc(PRO_E_FEATURE_TYPE, &featureTypeElem);
	status = ProElementIntegerSet(featureTypeElem, PRO_FEAT_DATUM_POINT);
	status = ProElemtreeElementAdd(featElemTree, NULL, featureTypeElem);

	//PRO_E_DPOINT_TYPE
	ProElement dpointTypeElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_TYPE, &dpointTypeElem);
	status = ProElementIntegerSet(dpointTypeElem, PRO_DPOINT_TYPE_OFFSET_CSYS);
	status = ProElemtreeElementAdd(featElemTree, NULL, dpointTypeElem);

	//PRO_E_STD_FEATURE_NAME
	ProElement stdFeatureNameElem = nullptr;
	status = ProElementAlloc(PRO_E_STD_FEATURE_NAME, &stdFeatureNameElem);
	status = ProElementWstringSet(stdFeatureNameElem, L"TEST_POINTS");
	status = ProElemtreeElementAdd(featElemTree, NULL, stdFeatureNameElem);

	//PRO_E_DPOINT_OFST_CSYS_REF
	ProElement dpointOfstCsysRefElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_REF, &dpointOfstCsysRefElem);

	ProMdl curMdl = nullptr;
	status = ProMdlCurrentGet(&curMdl);

	ProModelitem csysMdlItem{};
	//status = ProModelitemInit(curMdl, 4156, PRO_CSYS, &csysMdlItem);


	/*ProSelection csysSelection = nullptr;
	status = ProSelectionAlloc(NULL, &csysMdlItem, &csysSelection);*/

	ProReference dpointOfstCsysRefRef = nullptr;
	status = ProSelectionToReference(csys_sel, &dpointOfstCsysRefRef);

	status = ProElementReferenceSet(dpointOfstCsysRefElem, dpointOfstCsysRefRef);
	status = ProElemtreeElementAdd(featElemTree, NULL, dpointOfstCsysRefElem);

	//PRO_E_DPOINT_OFST_CSYS_WITH_DIMS
	ProElement dpointOfstCsysWithDimsElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_WITH_DIMS, &dpointOfstCsysWithDimsElem);
	status = ProElementIntegerSet(dpointOfstCsysWithDimsElem, 1);
	status = ProElemtreeElementAdd(featElemTree, NULL, dpointOfstCsysWithDimsElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	ProElement dpointOfstCsysPntsArrayElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY, &dpointOfstCsysPntsArrayElem);
	status = ProElemtreeElementAdd(featElemTree, NULL, dpointOfstCsysPntsArrayElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	ProElement dpointOfstCsysPntElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNT, &dpointOfstCsysPntElem);
	status = ProElemtreeElementAdd(dpointOfstCsysPntsArrayElem, NULL, dpointOfstCsysPntElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_PNT_NAME
	ProElement dpointOfstCsysPntNameElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNT_NAME, &dpointOfstCsysPntNameElem);
	status = ProElementWstringSet(dpointOfstCsysPntNameElem, L"PNT_0");
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysPntNameElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_DIR1_VAL
	ProElement dpointOfstCsysDir1ValElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR1_VAL, &dpointOfstCsysDir1ValElem);
	status = ProElementDoubleSet(dpointOfstCsysDir1ValElem, 0.0);
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysDir1ValElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_DIR2_VAL
	ProElement dpointOfstCsysDir2ValElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR2_VAL, &dpointOfstCsysDir2ValElem);
	status = ProElementDoubleSet(dpointOfstCsysDir2ValElem, 0.0);
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysDir2ValElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_DIR3_VAL
	ProElement dpointOfstCsysDir3ValElem = nullptr;
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR3_VAL, &dpointOfstCsysDir3ValElem);
	status = ProElementDoubleSet(dpointOfstCsysDir3ValElem, 0.0);
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysDir3ValElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNT, &dpointOfstCsysPntElem);
	status = ProElemtreeElementAdd(dpointOfstCsysPntsArrayElem, NULL, dpointOfstCsysPntElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_PNT_NAME
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNT_NAME, &dpointOfstCsysPntNameElem);
	status = ProElementWstringSet(dpointOfstCsysPntNameElem, L"PNT_1");
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysPntNameElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_DIR1_VAL
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR1_VAL, &dpointOfstCsysDir1ValElem);
	status = ProElementDoubleSet(dpointOfstCsysDir1ValElem, pnt3d[0]);
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysDir1ValElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_DIR2_VAL
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR2_VAL, &dpointOfstCsysDir2ValElem);
	status = ProElementDoubleSet(dpointOfstCsysDir2ValElem, pnt3d[1]);
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysDir2ValElem);

	//PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	//  |--PRO_E_DPOINT_OFST_CSYS_PNT
	//    |--PRO_E_DPOINT_OFST_CSYS_DIR3_VAL
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR3_VAL, &dpointOfstCsysDir3ValElem);
	status = ProElementDoubleSet(dpointOfstCsysDir3ValElem, pnt3d[2]);
	status = ProElemtreeElementAdd(dpointOfstCsysPntElem, NULL, dpointOfstCsysDir3ValElem);

	ProModelitem featOwnerModelItem{};
	status = ProMdlToModelitem(curMdl, &featOwnerModelItem);

	ProSelection featOwnerSelection = nullptr;
	status = ProSelectionAlloc(NULL, &featOwnerModelItem, &featOwnerSelection);

	ProFeatureCreateOptions* featCreationOpts = nullptr;
	status = ProArrayAlloc(1, sizeof(ProFeatureCreateOptions), 1, reinterpret_cast<ProArray*>(&featCreationOpts));
	featCreationOpts[0] = PRO_FEAT_CR_NO_OPTS;

	ProFeature createdFeature{};
	ProErrorlist errorList{};
	status = ProFeatureWithoptionsCreate(
		featOwnerSelection,
		featElemTree,
		featCreationOpts,
		PRO_REGEN_NO_FLAGS,
		&createdFeature,
		&errorList);

	status = ProSelectionFree(&featOwnerSelection);
	featOwnerSelection = nullptr;

	status = ProFeatureElemtreeFree(&createdFeature, featElemTree);
	featElemTree = nullptr;

	status = ProElementFree(&featElemTree);
	featElemTree = nullptr;

	return 0;
}

vector<ProModelitem> curMdlCurves;
ProError curMdlCurveVisitFunction(ProFeature *p_object, ProError status, ProAppData appData)
{
	ProFeattype featType;
	status = ProFeatureTypeGet(p_object, &featType);

	if (featType == PRO_FEAT_CURVE) {
		curMdlCurves.push_back((ProModelitem)*p_object);
	}

	return PRO_TK_NO_ERROR;
}

void CreateCurveBasedOnPoints(const ProMdl& CurMdl1, std::string& point1, ProFeature& csoFeat, PointData& curPointData, std::string& point2, ProFeature& csoFeat1, ProError& status, int surfaceid, ProAsmcomppath& comp_path, std::vector<PointData>& vectPointData, std::vector<ProSelection>& UdfInputSel);

// BBT
ProError ProDemoOffsetPointCreate(ProAsmcomppath& asmComppath, ProVector point, ProFeature &pointFeat)
{
	ProReference REPDEP_ref1;
	ProErrorlist            errors;
	ProMdl                  model;
	ProModelitem            model_item;
	ProSelection            model_sel;
	ProFeatureCreateOptions* opts = 0;
	ProAsmcomppath* p_comp_path = NULL;
	ProValue                value;
	char                    name[PRO_NAME_SIZE];
	ProError		    status;

	ProElement pro_e_feature_tree;
	ProElement pro_e_feature_type;
	ProElement pro_e_dpoint_type;
	ProElement pro_e_dpoint_ofst_csys_type;
	ProElement pro_e_dpoint_ofst_csys_ref;
	ProElement pro_e_dpoint_ofst_csys_with_dims;
	ProElement pro_e_dpoint_ofst_csys_pnts_array;
	ProElement pro_e_dpoint_ofst_csys_pnt;
	ProElement pro_e_dpoint_ofst_csys_pnt_name;
	ProElement pro_e_dpoint_ofst_csys_dir1_val;
	ProElement pro_e_dpoint_ofst_csys_dir2_val;
	ProElement pro_e_dpoint_ofst_csys_dir3_val;

	ProName 	wide_string;
	ProValueData 	value_data;
	ProSelection* p_select;
	int 		n_select;
	ProBoolean 	is_interactive = PRO_B_TRUE;


	/*---------------------------------------------------------------*\
	  Populating root element PRO_E_FEATURE_TREE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_FEATURE_TREE, &pro_e_feature_tree);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_FEATURE_TYPE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_FEATURE_TYPE, &pro_e_feature_type);
	status = ProElementIntegerSet(pro_e_feature_type, PRO_FEAT_DATUM_POINT);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL, pro_e_feature_type);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_TYPE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_TYPE, &pro_e_dpoint_type);
	status = ProElementIntegerSet(pro_e_dpoint_type, PRO_DPOINT_TYPE_OFFSET_CSYS);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL, pro_e_dpoint_type);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_TYPE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_TYPE,
		&pro_e_dpoint_ofst_csys_type);
	status = ProElementIntegerSet(pro_e_dpoint_ofst_csys_type, PRO_DTMPNT_OFFCSYS_CARTESIAN);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_dpoint_ofst_csys_type);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_REF
	\*---------------------------------------------------------------*/
	/*status = ProSelect("csys", 1, NULL, NULL, NULL, NULL, &p_select, &n_select);
	if (n_select <= 0) return -1;*/
	ProModelitem csysItem;
	ProSelection csysSel;
	ProMdl curMdl;
	status = ProMdlCurrentGet(&curMdl);
	status = ProModelitemByNameInit(curMdl, PRO_CSYS, L"DEFAULT", &csysItem);
	status = ProSelectionAlloc(&asmComppath, &csysItem, &csysSel);
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_REF,
		&pro_e_dpoint_ofst_csys_ref);
	status = ProSelectionToReference(csysSel, &REPDEP_ref1);
	status = ProElementReferenceSet(pro_e_dpoint_ofst_csys_ref, REPDEP_ref1);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_dpoint_ofst_csys_ref);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_WITH_DIMS
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_WITH_DIMS,
		&pro_e_dpoint_ofst_csys_with_dims);
	status = ProElementIntegerSet(pro_e_dpoint_ofst_csys_with_dims, PRO_B_TRUE);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_dpoint_ofst_csys_with_dims);

	/*---------------------------------------------------------------*\
	  Populating array element PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY,
		&pro_e_dpoint_ofst_csys_pnts_array);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_dpoint_ofst_csys_pnts_array);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
			  -> PRO_E_DPOINT_OFST_CSYS_PNT
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNT,
		&pro_e_dpoint_ofst_csys_pnt);
	status = ProElemtreeElementAdd(pro_e_dpoint_ofst_csys_pnts_array,
		NULL, pro_e_dpoint_ofst_csys_pnt);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
			  -> PRO_E_DPOINT_OFST_CSYS_PNT
				 -> PRO_E_DPOINT_OFST_CSYS_PNT_NAME
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_PNT_NAME,
		&pro_e_dpoint_ofst_csys_pnt_name);
	ProStringToWstring(wide_string, "PNT2");
	status = ProElementWstringSet(pro_e_dpoint_ofst_csys_pnt_name, wide_string);
	status = ProElemtreeElementAdd(pro_e_dpoint_ofst_csys_pnt, NULL,
		pro_e_dpoint_ofst_csys_pnt_name);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
			  -> PRO_E_DPOINT_OFST_CSYS_PNT
				 -> PRO_E_DPOINT_OFST_CSYS_DIR1_VAL
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR1_VAL,
		&pro_e_dpoint_ofst_csys_dir1_val);
	status = ProElementDecimalsSet(pro_e_dpoint_ofst_csys_dir1_val, 4);
	status = ProElementDoubleSet(pro_e_dpoint_ofst_csys_dir1_val, point[0]);
	status = ProElemtreeElementAdd(pro_e_dpoint_ofst_csys_pnt, NULL,
		pro_e_dpoint_ofst_csys_dir1_val);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
			  -> PRO_E_DPOINT_OFST_CSYS_PNT
				 -> PRO_E_DPOINT_OFST_CSYS_DIR2_VAL
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR2_VAL,
		&pro_e_dpoint_ofst_csys_dir2_val);
	status = ProElementDecimalsSet(pro_e_dpoint_ofst_csys_dir2_val, 4);
	status = ProElementDoubleSet(pro_e_dpoint_ofst_csys_dir2_val, -point[2]);
	status = ProElemtreeElementAdd(pro_e_dpoint_ofst_csys_pnt, NULL,
		pro_e_dpoint_ofst_csys_dir2_val);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_OFST_CSYS_PNTS_ARRAY
			  -> PRO_E_DPOINT_OFST_CSYS_PNT
				 -> PRO_E_DPOINT_OFST_CSYS_DIR3_VAL
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_OFST_CSYS_DIR3_VAL,
		&pro_e_dpoint_ofst_csys_dir3_val);
	status = ProElementDecimalsSet(pro_e_dpoint_ofst_csys_dir3_val, 4);
	status = ProElementDoubleSet(pro_e_dpoint_ofst_csys_dir3_val, point[1]);
	status = ProElemtreeElementAdd(pro_e_dpoint_ofst_csys_pnt, NULL,
		pro_e_dpoint_ofst_csys_dir3_val);

	/*---------------------------------------------------------------*\
	  Creating the feature in the current model.
	\*---------------------------------------------------------------*/
	status = ProMdlCurrentGet(&model);
	if (status != PRO_TK_NO_ERROR) return (status);
	status = ProMdlToModelitem(model, &model_item);
	status = ProSelectionAlloc(p_comp_path, &model_item,
		&model_sel);


	status = ProArrayAlloc(1, sizeof(ProFeatureCreateOptions),
		1, (ProArray*)&opts);

	opts[0] = PRO_FEAT_CR_DEFINE_MISS_ELEMS;

	status = ProFeatureWithoptionsCreate(model_sel, pro_e_feature_tree,
		opts, PRO_REGEN_NO_FLAGS, &pointFeat, &errors);

	status = ProArrayFree((ProArray*)&opts);

	status = ProElementFree(&pro_e_feature_tree);

	return (status);
}

ProError featPointVisitAction(ProGeomitem* geomitem, ProError err, ProAppData appdata)
{
	ProGeomitem* point = (ProGeomitem*)appdata;
	point[0] = geomitem[0];
	return PRO_TK_NO_ERROR;
}


/*===============================================================*\
FUNCTION : ProDemoFieldPointCreate
PURPOSE  : Demonstrates the creation of the Field Datum Point
\*===============================================================*/
ProError ProDemoFieldPointCreate(ProFeature *pointFeat)
{
	ProReference REPDEP_ref1;
	ProErrorlist            errors;
	ProMdl                  model;
	ProModelitem            model_item;
	ProSelection            model_sel;
	ProFeature              feature;
	ProFeatureCreateOptions* opts = 0;
	ProAsmcomppath* p_comp_path = NULL;
	ProValue                value;
	char                    name[PRO_NAME_SIZE];
	ProError		    status;

	ProElement pro_e_feature_tree;
	ProElement pro_e_feature_type;
	ProElement pro_e_dpoint_type;
	ProElement pro_e_std_feature_name;
	ProElement pro_e_dpoint_field_ref;

	ProName 	wide_string;
	ProValueData 	value_data;
	ProSelection* p_select;
	int 		n_select;
	ProBoolean 	is_interactive = PRO_B_TRUE;

	/*---------------------------------------------------------------*\
	  Populating root element PRO_E_FEATURE_TREE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_FEATURE_TREE, &pro_e_feature_tree);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_FEATURE_TYPE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_FEATURE_TYPE, &pro_e_feature_type);
	status = ProElementIntegerSet(pro_e_feature_type, PRO_FEAT_DATUM_POINT);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_feature_type);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_TYPE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_TYPE, &pro_e_dpoint_type);
	status = ProElementIntegerSet(pro_e_dpoint_type, PRO_DPOINT_TYPE_FIELD);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_dpoint_type);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_TYPE
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_STD_FEATURE_NAME,
		&pro_e_std_feature_name);
	ProStringToWstring(wide_string, "MY_FPNT0");
	status = ProElementWstringSet(pro_e_std_feature_name, wide_string);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_std_feature_name);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_FIELD_REF
	\*---------------------------------------------------------------*/
	status = ProSelect("edge,surface", 1, NULL, NULL,
		NULL, NULL, &p_select, &n_select);
	if (n_select <= 0) 
		return status;
	status = ProElementAlloc(PRO_E_DPOINT_FIELD_REF,
		&pro_e_dpoint_field_ref);
	status = ProSelectionToReference(p_select[0], &REPDEP_ref1);
	status = ProElementReferenceSet(pro_e_dpoint_field_ref, REPDEP_ref1);
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_dpoint_field_ref);

	/*---------------------------------------------------------------*\
	  Create the feature in the current model.
	\*---------------------------------------------------------------*/
	status = ProMdlCurrentGet(&model);
	if (status != PRO_TK_NO_ERROR) return (status);
	status = ProMdlToModelitem(model, &model_item);
	status = ProSelectionAlloc(p_comp_path, &model_item,
		&model_sel);


	status = ProArrayAlloc(1, sizeof(ProFeatureCreateOptions),
		1, (ProArray*)&opts);

	opts[0] = PRO_FEAT_CR_DEFINE_MISS_ELEMS;

	status = ProFeatureWithoptionsCreate(model_sel, pro_e_feature_tree,
		opts, PRO_REGEN_NO_FLAGS, pointFeat, &errors);

	status = ProArrayFree((ProArray*)&opts);

	status = ProElementFree(&pro_e_feature_tree);

	return (status);
}


void FanOutCreation()
{
	ProError status;
	ProMdl CurMdl1;
	vector<PointData> vectPointData;
	vector<PointData> vectCsysData;
	vector<PointData> vectCsysData1;
	status = ProMdlCurrentGet(&CurMdl1);
	vector<ProFeature> vecFeat;
	ProAsmcomppath comp_path;
	ProIdTable c_id_table;
	c_id_table[0] = -1;
	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);
	VesModelTree mdlObj(CurMdl1);
	vector< VesModelTree::AsmTreeData>  plateTreeVec_item1;
	VesModelTree::AsmTreeData compHdl;
	//mdlObj.GetAsmCompTreeData("COMPONENT_2", compHdl); //TODO check if this skelton is same as member sk
	mdlObj.GetModelTreeComponents(CurMdl1, plateTreeVec_item1);


	// Curve data check
	//ProCurve curv;
	////status = ProCurveInit((ProSolid)CurMdl1, 82180, &curv);
	//status = ProCurveInit((ProSolid)CurMdl1, 82186, &curv);
	//ProGeomitemdata* curvData;
	//status = ProCurveDataGet(curv, &curvData);
	//ProCurvedata* nurbsData;
	//status = ProCurveToNURBS(curv, &nurbsData);



	ProFeature csoFeat;
	ProFeature csoFeat1;
	//ProError status;
	//std::vector<PointData> vectPointData;
	std::vector<ProSelection> UdfInputSel;
	PointData curPointData = {};
	string point1 = "PNT0";
	string point2 = "PNT1";
	int surfaceid = 41245;
	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
	
	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT4";
	point2 = "PNT6";
	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
	
	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT3";
	point2 = "PNT15";
	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);

	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT9";
	point2 = "PNT10";

	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT11";
	point2 = "PNT12";
	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
	
	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT13";
	point2 = "PNT14";
	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);

	// Create SQUARE surf
	/*ProSelection *pntLocSel;
	int nSel;
	UdfInputSel.clear();
	status = ProSelect((char*)"point", 1, NULL, NULL, NULL, NULL, &pntLocSel, &nSel);
	UdfInputSel.push_back(pntLocSel[0]);

	ProSurface lSurface;
	ProGeomitem comp_datum;
	ProSelection selection1;
	status = ProSurfaceInit(CurMdl1, surfaceid, &lSurface);
	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, lSurface, &comp_datum);
	status = ProSelectionAlloc(&comp_path, &comp_datum, &selection1);
	UdfInputSel.push_back(selection1);

	status = ProSurfaceInit(CurMdl1, 41274, &lSurface);
	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, lSurface, &comp_datum);
	status = ProSelectionAlloc(&comp_path, &comp_datum, &selection1);
	UdfInputSel.push_back(selection1);

	ProEdge lEdge;
	status = ProEdgeInit((ProSolid)CurMdl1, 41246, &lEdge);
	status = ProEdgeToGeomitem((ProSolid)CurMdl1, lEdge, &comp_datum);
	status = ProSelectionAlloc(&comp_path, &comp_datum, &selection1);
	UdfInputSel.push_back(selection1);*/

	
	//GetFeatureByName(CurMdl1, "PNT13", csoFeat);
	//curPointData.lPointFeat = csoFeat;

	//PointData curPointData1 = {};
	//GetFeatureByName(CurMdl1, "PNT29", csoFeat1);
	//curPointData1.lPointFeat = csoFeat1;

	//vectPointData.push_back(curPointData);
	//vectPointData.push_back(curPointData1);
	//for (size_t k = 0; k < vectPointData.size(); k++)
	//{
	//	//Get Point Feature Selection
	//	vector<ProGeomitem> vecGeomItems;
	//	GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
	//	ProSelection pointSelection;
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
	//	UdfInputSel.push_back(pointSelection);
	//}

	//UdfInputSel.push_back(SurfSelection1);


	string squarUDFpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\create_surf_square.gph.1";
	ProFeature UDFGrpFeat;

	//LoadUDF(CurMdl1, squarUDFpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE);

	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT28";
	point2 = "PNT31";
	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);

	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT29";
	point2 = "PNT32";
	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
	


	// BOND PAD UDF SIDE 1
	string bondpadUdfpath = "C:\\Company Data\\3D Printing\\3DPrinter_23-02-2024\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_udf.gph.1";
	vector<ProSelection> offsetUDFsels;
	/*ProGeomitem placementEdgeItem;
	ProSelection placementEdgeSel;
	status = ProModelitemInit(CurMdl1, 43556, PRO_EDGE, &placementEdgeItem);
	status = ProSelectionAlloc(&comp_path, &placementEdgeItem, &placementEdgeSel);

	ProSurface dirSurf1;
	status = ProSurfaceInit(CurMdl1, 44049, &dirSurf1);
	ProGeomitem dirSurfItem1;
	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, dirSurf1, &dirSurfItem1);
	ProSelection dirSurfSelection1;
	status = ProSelectionAlloc(&comp_path, &dirSurfItem1, &dirSurfSelection1);

	ProSelection SurfSelection1;
	ProSurface lSurface;
	ProGeomitem comp_datum;
	status = ProSurfaceInit(CurMdl1, surfaceid, &lSurface);
	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, lSurface, &comp_datum);
	status = ProSelectionAlloc(&comp_path, &comp_datum, &SurfSelection1);*/

	// 1
	ProFeature pntFeat;
	ProFeature UDFOffsetFeat1;
	ProSelection* pntLocSel;
	int nSel;
	//status = ProSelect((char*)"feature", 1, NULL, NULL, NULL, NULL, &pntLocSel, &nSel);


	status = ProDemoFieldPointCreate(&pntFeat);

	//status = ProSelectionModelitemGet(pntLocSel[0], &pntFeat);
	ProModelitem pntItem;
	ProSelection pntSel;
	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);

	ProExtRefInfo* extRefsInfo = NULL;
	int nExtRefsInfo = 0;
	status = ProFeatureExternParentsGet(&pntFeat, PRO_ALL_REF_TYPES, &extRefsInfo, &nExtRefsInfo);

	ProModelitem placementEdgeItem;
	for (int i = 0; i < nExtRefsInfo; ++i)
	{
		for (int j = 0; j < extRefsInfo[i].n_refs; j++)
		{
			status = ProExtRefModelitemGet(extRefsInfo[i].ext_refs[j], &placementEdgeItem);
		}
	}
	ProModelitem placementSurfItem, dirSurfItem;
	ProSelection placementSurfSel = NULL, dirSurfSel = NULL, placementEdgeSel = NULL;
	ProEdge pntRefEdge;
	ProEdge neighborEdge1, neighborEdge2;
	ProSurface neighborSurf1, neighborSurf2;
	int neighborSurf1Id, neighborSurf2Id;
	ProSrftype surfType;

	status = ProSelectionAlloc(&comp_path, &placementEdgeItem, &placementEdgeSel);

	status = ProGeomitemToEdge((ProGeomitem*)&placementEdgeItem, &pntRefEdge);
	status = ProEdgeNeighborsGet(pntRefEdge, &neighborEdge1, &neighborEdge2, &neighborSurf1, &neighborSurf2);

	status = ProSurfaceIdGet(neighborSurf1, &neighborSurf1Id);
	status = ProSurfaceIdGet(neighborSurf2, &neighborSurf2Id);

	status = ProSurfaceTypeGet(neighborSurf1, &surfType);
	if (surfType == PRO_SRF_CYL)
	{
		status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf1, &placementSurfItem);
		status = ProSelectionAlloc(NULL, &placementSurfItem, &placementSurfSel);
	}
	else if (surfType == PRO_SRF_PLANE)
	{
		status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf1, &dirSurfItem);
		status = ProSelectionAlloc(NULL, &dirSurfItem, &dirSurfSel);
	}
	status = ProSurfaceTypeGet(neighborSurf2, &surfType);
	if (surfType == PRO_SRF_CYL)
	{
		status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf2, &placementSurfItem);
		status = ProSelectionAlloc(NULL, &placementSurfItem, &placementSurfSel);
	}
	else if (surfType == PRO_SRF_PLANE)
	{
		status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf2, &dirSurfItem);
		status = ProSelectionAlloc(NULL, &dirSurfItem, &dirSurfSel);
	}

	if ((placementSurfSel != NULL) && (dirSurfSel != NULL))
	{
		offsetUDFsels.push_back(placementEdgeSel);
		offsetUDFsels.push_back(pntSel);
		offsetUDFsels.push_back(placementSurfSel);
		offsetUDFsels.push_back(dirSurfSel);
		LoadUDF(CurMdl1, bondpadUdfpath, offsetUDFsels, UDFOffsetFeat1, NULL, PRO_B_FALSE);
	}
	offsetUDFsels.clear();

	

	// BOND PAD UDF SIDE 2
	/*ProSurface dirSurf2;
	status = ProSurfaceInit(CurMdl1, 43548, &dirSurf2);
	ProGeomitem dirSurfItem2;
	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, dirSurf2, &dirSurfItem2);
	ProSelection dirSurfSelection2;
	status = ProSelectionAlloc(&comp_path, &dirSurfItem2, &dirSurfSelection2);

	// 1
	offsetUDFsels.push_back(placementEdgeSel);
	status = ProSelect((char*)"point", 1, NULL, NULL, NULL, NULL, &pntLocSel, &nSel);
	offsetUDFsels.push_back(pntLocSel[0]);
	offsetUDFsels.push_back(SurfSelection1);
	offsetUDFsels.push_back(dirSurfSelection2);
	LoadUDF(CurMdl1, bondpadUdfpath, offsetUDFsels, UDFOffsetFeat1, NULL, PRO_B_FALSE);
	offsetUDFsels.clear();

	// 2
	offsetUDFsels.push_back(placementEdgeSel);
	status = ProSelect((char*)"point", 1, NULL, NULL, NULL, NULL, &pntLocSel, &nSel);
	offsetUDFsels.push_back(pntLocSel[0]);
	offsetUDFsels.push_back(SurfSelection1);
	offsetUDFsels.push_back(dirSurfSelection2);
	LoadUDF(CurMdl1, bondpadUdfpath, offsetUDFsels, UDFOffsetFeat1, NULL, PRO_B_FALSE);
	offsetUDFsels.clear();*/

	// CURVES
	/*csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT41";
	point2 = "PNT35";
	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);

	csoFeat = {};
	csoFeat1 = {};
	vectPointData.clear();
	UdfInputSel.clear();
	curPointData = {};
	point1 = "PNT38";
	point2 = "PNT44";
	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);*/

}
void GetMamximumCurveId(std::vector<ProFeature>& vecGeomItems, ProError& status, const ProMdl& CurMdl1, int& FullCurveGEomitemId)
{
	ProModelitem p_Curvehandle;
	double Length = 0.0;

	for (int i = 0; i < vecGeomItems.size(); i++)
	{
		if (vecGeomItems[i].type == PRO_CURVE)
		{
			ProCurve Curve;
			double p_templength = 0.0;

			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems[i].id, PRO_CURVE, &p_Curvehandle);
			status = ProGeomitemToCurve(&p_Curvehandle, &Curve);
			status = ProCurveLengthEval(Curve, &p_templength);
			if (p_templength > Length)
			{
				FullCurveGEomitemId = vecGeomItems[i].id;
				Length = p_templength;

			}
		}
	}
}
ProError curveCompAction(ProCurve compCurve, ProCurve p_comp, int comp_idx, ProBoolean flip, ProError status, ProAppData app_data)
{
	ProEnttype crvType;
	status = ProCurveTypeGet(p_comp, &crvType);
	ProGeomitemdata* centerCurve1Data;
	status = ProCurveDataGet(p_comp, &centerCurve1Data);
	int o = 9;
	return PRO_TK_NO_ERROR;
}

void CreateCurveBasedOnPoints(const ProMdl& CurMdl1, std::string& point1, ProFeature& csoFeat, PointData& curPointData, std::string& point2, ProFeature& csoFeat1, ProError& status, int surfaceid, ProAsmcomppath& comp_path, std::vector<PointData>& vectPointData, std::vector<ProSelection>& UdfInputSel)
{
	GetFeatureByName(CurMdl1, point1, csoFeat);
	curPointData.lPointFeat = csoFeat;

	PointData curPointData1 = {};
	GetFeatureByName(CurMdl1, point2, csoFeat1);
	curPointData1.lPointFeat = csoFeat1;
	ProSurface lSurface;
	ProGeomitem comp_datum;
	ProSelection SurfSelection1;

	status = ProSurfaceInit(CurMdl1, surfaceid, &lSurface);
	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, lSurface, &comp_datum);
	status = ProSelectionAlloc(&comp_path, &comp_datum, &SurfSelection1);
	vectPointData.push_back(curPointData);
	vectPointData.push_back(curPointData1);
	for (size_t k = 0; k < vectPointData.size(); k++)
	{
		//Get Point Feature Selection
		vector<ProGeomitem> vecGeomItems;
		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
		ProSelection pointSelection;
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
		UdfInputSel.push_back(pointSelection);
	}

	UdfInputSel.push_back(SurfSelection1);

	string Udfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\create_curve.gph.1";
	ProFeature UDFGrpFeat;

	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
	{
		ProSolidRegenerate((ProSolid)CurMdl1, PRO_REGEN_NO_RESOLVE_MODE);

		vector<ProSelection> UdfInputSel1;
		UdfInputSel1.push_back(SurfSelection1);

		//VecAxisUdfs.push_back(UDFGrpFeat);
		ProFeature* feats = NULL;
		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
		int FeatSize;
		status = ProArraySizeGet(feats, &FeatSize);
		ProGeomitem Curve_item;
		ProSelection SurfSelection2;
		ProSelection CurveSelection1;
		ProSelection CurveSelection2;
		ProCurve p_handle;

		ProModelitem p_CurveStarthandle;
		ProModelitem p_Curvehandle;
		ProModelitem p_CurveEndhandle;
		vector<ProGeomitem> vecGeomItems;
		GetGeomItems(CurMdl1, feats[1], vecGeomItems);
		int FullCurveGEomitemId;
		GetMamximumCurveId(vecGeomItems, status, CurMdl1, FullCurveGEomitemId);
		for (int i = 0; i < vecGeomItems.size(); i++)
		{
			ProModelitem curvMdlItem;
			if (vecGeomItems[i].id == FullCurveGEomitemId && vecGeomItems[i].type == PRO_CURVE)
			{
				ProCurve Curve;
				double p_length;
				ProGeomitemdata* geomitemdata;
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems[i].id, PRO_CURVE, &p_Curvehandle);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems[i].id, PRO_CRV_START, &p_CurveStarthandle);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems[i].id, PRO_CRV_END, &p_CurveEndhandle);
				status = ProSelectionAlloc(NULL, &p_CurveStarthandle, &CurveSelection1);
				status = ProSelectionAlloc(NULL, &p_CurveEndhandle, &CurveSelection2);
				status = ProSelectionAlloc(NULL, &p_Curvehandle, &SurfSelection2);
				//ProSelectionHighlight(CurveSelection1, PRO_COLOR_CURVE);
				//ProSelectionHighlight(CurveSelection2, PRO_COLOR_CURVE);
				break;
			}
		}
		status = ProModelitemHide(&feats[1]);

		UdfInputSel1.push_back(SurfSelection2);
		Udfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\offset_curve_2mm_1.gph";
		ProFeature UDFGrpFeat1;
		LoadUDF(CurMdl1, Udfpath, UdfInputSel1, UDFGrpFeat1, NULL, PRO_B_TRUE);
		UdfInputSel1.clear();

		ProFeature* childfeats2 = NULL;
		vector<ProGeomitem> vecGeomItems3;
		ProModelitem p_CurveStarthandle3;
		ProModelitem p_Curvehandle3;
		ProModelitem p_CurveEndhandle3;
		ProSelection childCurveSelection1_2;
		ProSelection childCurveSelection2_2;
		ProSelection childSelection2_2;
		int FullCurveGEomitemId3;

		status = ProGroupFeaturesCollect(&UDFGrpFeat1, &childfeats2);//Give only non suppressed entities

		GetGeomItems(CurMdl1, childfeats2[1], vecGeomItems3);
		GetMamximumCurveId(vecGeomItems3, status, CurMdl1, FullCurveGEomitemId3);
		for (int i = 0; i < vecGeomItems3.size(); i++)
		{
			ProModelitem curvMdlItem;
			if (vecGeomItems3[i].id == FullCurveGEomitemId3 && vecGeomItems3[i].type == PRO_CURVE)
			{
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CURVE, &p_Curvehandle3);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CRV_START, &p_CurveStarthandle3);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CRV_END, &p_CurveEndhandle3);
				status = ProSelectionAlloc(NULL, &p_CurveStarthandle3, &childCurveSelection1_2);
				status = ProSelectionAlloc(NULL, &p_CurveEndhandle3, &childCurveSelection2_2);
				status = ProSelectionAlloc(NULL, &p_Curvehandle3, &childSelection2_2);
				//ProSelectionHighlight(CurveSelection1, PRO_COLOR_CURVE);
				//ProSelectionHighlight(childSelection2_2, PRO_COLOR_HIGHLITE);
				break;
			}
		}
		vecGeomItems3.clear();
		UdfInputSel1.push_back(childSelection2_2);

		GetGeomItems(CurMdl1, childfeats2[2], vecGeomItems3);
		GetMamximumCurveId(vecGeomItems3, status, CurMdl1, FullCurveGEomitemId3);
		for (int i = 0; i < vecGeomItems3.size(); i++)
		{
			ProModelitem curvMdlItem;
			if (vecGeomItems3[i].id == FullCurveGEomitemId3 && vecGeomItems3[i].type == PRO_CURVE)
			{
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CURVE, &p_Curvehandle3);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CRV_START, &p_CurveStarthandle3);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CRV_END, &p_CurveEndhandle3);
				status = ProSelectionAlloc(NULL, &p_CurveStarthandle3, &childCurveSelection1_2);
				status = ProSelectionAlloc(NULL, &p_CurveEndhandle3, &childCurveSelection2_2);
				status = ProSelectionAlloc(NULL, &p_Curvehandle3, &childSelection2_2);
				break;
			}
		}
		vecGeomItems3.clear();
		UdfInputSel1.push_back(childSelection2_2);
		ProFeature UDFGrpFeat4;
		Udfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\create_surface.gph.1";
		LoadUDF(CurMdl1, Udfpath, UdfInputSel1, UDFGrpFeat4, NULL, PRO_B_TRUE);
		UdfInputSel1.clear();
	}

	//ProFeature curveFeature;
	//ProFeature copyFeature;
	//GetFeatureByName(CurMdl1, "CREATE_CURVE", curveFeature);
	//GetFeatureByName(CurMdl1, "COPY_CURVE", copyFeature);
	//status = ProModelitemHide(&curveFeature);
	//status = ProModelitemHide(&copyFeature);


	// CREATE SQUARE OFFSET
	/*//vector<ProSelection> offsetUDFsels;
	//ProFeature placementPlaneFeat;
	//GetFeatureByName(CurMdl1, "DTM1", placementPlaneFeat);

	//ProFeature refPLaneFeat;
	//GetFeatureByName(CurMdl1, "SIDE", refPLaneFeat);
	//ProSelection placementPlaneSel, refPlaneSel;

	//vector<ProGeomitem> planeItems;
	//GetGeomItems(CurMdl1, placementPlaneFeat, planeItems);
	//status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&planeItems[0], &placementPlaneSel);
	//planeItems.clear();
	//GetGeomItems(CurMdl1, refPLaneFeat, planeItems);
	//status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&planeItems[0], &refPlaneSel);
	//planeItems.clear();

	//offsetUDFsels.push_back(placementPlaneSel);
	//offsetUDFsels.push_back(refPlaneSel);
	//offsetUDFsels.push_back(SurfSelection1);
	//offsetUDFsels.push_back(UdfInputSel[0]);
	//Udfpath = "C:\\Users\\Public\\Documents\\offset_create_1.gph.1";
	//ProFeature UDFOffsetFeat1;
	//LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDFOffsetFeat1, NULL, PRO_B_FALSE);
	//offsetUDFsels.clear();
	//if ((point1 == "PNT0") || (point1 == "PNT4") || (point1 == "PNT15") || (point1 == "PNT10") || (point1 == "PNT12") || (point1 == "PNT14"))
	//{
	//	offsetUDFsels.push_back(placementPlaneSel);
	//	offsetUDFsels.push_back(refPlaneSel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(UdfInputSel[0]);
	//	Udfpath = "C:\\Users\\Public\\Documents\\offset_create_1.gph.1";
	//	ProFeature UDFOffsetFeat2;
	//	LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDFOffsetFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//}
	//if ((point2 == "PNT0") || (point2 == "PNT4") || (point2 == "PNT15") || (point2 == "PNT10") || (point2 == "PNT12") || (point2 == "PNT14"))
	//{
	//	offsetUDFsels.push_back(placementPlaneSel);
	//	offsetUDFsels.push_back(refPlaneSel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(UdfInputSel[1]);
	//	Udfpath = "C:\\Users\\Public\\Documents\\offset_create_1.gph.1";
	//	ProFeature UDFOffsetFeat2;
	//	LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDFOffsetFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//}

	//if ((point1 == "PNT1") || (point1 == "PNT3") || (point1 == "PNT6") || (point1 == "PNT9") || (point1 == "PNT11") || (point1 == "PNT13"))
	//{
	//	offsetUDFsels.push_back(placementPlaneSel);
	//	offsetUDFsels.push_back(refPlaneSel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(UdfInputSel[0]);
	//	Udfpath = "C:\\Users\\Public\\Documents\\offset_create_2.gph.1";
	//	ProFeature UDFOffsetFeat2;
	//	LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDFOffsetFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//}
	//if ((point2 == "PNT1") || (point2 == "PNT3") || (point2 == "PNT6") || (point2 == "PNT9") || (point2 == "PNT11") || (point2 == "PNT13"))
	//{
	//	offsetUDFsels.push_back(placementPlaneSel);
	//	offsetUDFsels.push_back(refPlaneSel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(UdfInputSel[1]);
	//	Udfpath = "C:\\Users\\Public\\Documents\\offset_create_2.gph.1";
	//	ProFeature UDFOffsetFeat2;
	//	LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDFOffsetFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//}*/
	
	//// CREATE OFFSET DTM POINTS
	//vector<ProSelection> offsetUDFsels;
	//ProGeomitem placementEdgeItem;
	//ProSelection placementEdgeSel;
	//status = ProModelitemInit(CurMdl1, 43556, PRO_EDGE, &placementEdgeItem);
	//status = ProSelectionAlloc(&comp_path, &placementEdgeItem, &placementEdgeSel);

	//// POINT 1
	//// Offset points
	//offsetUDFsels.push_back(placementEdgeSel);
	//offsetUDFsels.push_back(UdfInputSel[0]);
	//ProFeature UDF2mmOffsetPointFeat1, UDF1mmOffsetPointFeat1;
	//Udfpath = "C:\\Users\\Public\\Documents\\udf_ofs_pnt_create_2mm.gph.1";
	//LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDF2mmOffsetPointFeat1, NULL, PRO_B_FALSE);
	//Udfpath = "C:\\Users\\Public\\Documents\\udf_ofs_pnt_create_1mm.gph.1";
	//LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDF1mmOffsetPointFeat1, NULL, PRO_B_FALSE);
	//offsetUDFsels.clear();

	//// POINT 2
	//offsetUDFsels.push_back(placementEdgeSel);
	//offsetUDFsels.push_back(UdfInputSel[1]);
	//ProFeature UDF2mmOffsetPointFeat2, UDF1mmOffsetPointFeat2;
	//Udfpath = "C:\\Users\\Public\\Documents\\udf_ofs_pnt_create_2mm
	//status = ProSurfaceToGeomitem((ProSolid)CurMdl1, dirSurf, &dirSurfItem);
	//ProSelection dirSurfSelection;
	//status = ProSelectionAlloc(&comp_path, &dirSurfItem, &dirSurfSelection);
	//offsetUDFsels.push_back(UdfInputSel[0]);
	//offsetUDFsels.push_back(SurfSelection1);
	//offsetUDFsels.push_back(dirSurfSelection);
	//ProFeature UDFCurveFeat1;
	//LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDFOffsetFeat2, NULL, PRO_B_FALSE);
	//offsetUDFsels.clear();*/

	//// Curves 2.gph.1";
	//LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDF2mmOffsetPointFeat2, NULL, PRO_B_FALSE);
	//Udfpath = "C:\\Users\\Public\\Documents\\udf_ofs_pnt_create_1mm.gph.1";
	//LoadUDF(CurMdl1, Udfpath, offsetUDFsels, UDF1mmOffsetPointFeat2, NULL, PRO_B_FALSE);
	//offsetUDFsels.clear();


	//// Curves 1
	///*ProSurface dirSurf;
	//status = ProSurfaceInit(CurMdl1, 42709, &dirSurf);
	//ProGeomitem dirSurfItem;
	//ProSurface dirSurf;
	//status = ProSurfaceInit(CurMdl1, 42709, &dirSurf);
	//ProGeomitem dirSurfItem;
	//status = ProSurfaceToGeomitem((ProSolid)CurMdl1, dirSurf, &dirSurfItem);
	//ProSelection dirSurfSelection;
	//status = ProSelectionAlloc(&comp_path, &dirSurfItem, &dirSurfSelection);


	//vector<ProGeomitem> vecGeomItems;
	//ProSelection offsetDtmPoint1Sel, offsetDtmPoint2Sel;
	//ProFeature* UDFGroupFeats;
	//ProFeature UDF10mmCurveFeat1, UDF10mmCurveFeat2, UDF15mmCurveFeat1_1, UDF15mmCurveFeat1_2, UDF15mmCurveFeat2_1, UDF15mmCurveFeat2_2;
	//string UDFCurve10mmPath, UDFCurve15mmPath;

	//ProFeature boundaryBlendFeat;
	//string UDFBoundaryBlendPath = "C:\\Users\\Public\\Documents\\create_boundary_blend.gph.1";
	//ProSelection curveSel1, curveSel2;
	//ProFeature UDFCurveFeat1, UDFCurveFeat2;

	//if ((point1 == "PNT0") || (point1 == "PNT4") || (point1 == "PNT15") || (point1 == "PNT10") || (point1 == "PNT12") || (point1 == "PNT14"))
	//{
	//	UDFCurve10mmPath = "C:\\Users\\Public\\Documents\\create_10mm_crv_dir_nonflip.gph.1";
	//	UDFCurve15mmPath = "C:\\Users\\Public\\Documents\\create_15mm_crv_dir_nonflip.gph.1";

	//	offsetUDFsels.push_back(UdfInputSel[0]);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDFCurveFeat1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	// 10mm
	//	status = ProGroupFeaturesCollect(&UDF2mmOffsetPointFeat1, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// 15mm

	//	status = ProGroupFeaturesCollect(&UDF1mmOffsetPointFeat1, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat1_1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat2_1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// Boundary Blend
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);


	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();

	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//}
	//if ((point2 == "PNT0") || (point2 == "PNT4") || (point2 == "PNT15") || (point2 == "PNT10") || (point2 == "PNT12") || (point2 == "PNT14"))
	//{
	//	UDFCurve10mmPath = "C:\\Users\\Public\\Documents\\create_10mm_crv_dir_nonflip.gph.1";
	//	UDFCurve15mmPath = "C:\\Users\\Public\\Documents\\create_15mm_crv_dir_nonflip.gph.1";

	//	offsetUDFsels.push_back(UdfInputSel[1]);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDFCurveFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	status = ProGroupFeaturesCollect(&UDF2mmOffsetPointFeat2, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// 15mm
	//	status = ProGroupFeaturesCollect(&UDF1mmOffsetPointFeat2, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat1_2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat2_2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// Boundary Blend
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);


	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();

	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);
	//}
	//
	//
	//if ((point1 == "PNT1") || (point1 == "PNT3") || (point1 == "PNT6") || (point1 == "PNT9") || (point1 == "PNT11") || (point1 == "PNT13"))
	//{
	//	UDFCurve10mmPath = "C:\\Users\\Public\\Documents\\create_10mm_crv_dir_flip.gph.1";
	//	UDFCurve15mmPath = "C:\\Users\\Public\\Documents\\create_15mm_crv_dir_flip.gph.1";

	//	offsetUDFsels.push_back(UdfInputSel[0]);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDFCurveFeat1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	status = ProGroupFeaturesCollect(&UDF2mmOffsetPointFeat1, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// 15mm
	//	status = ProGroupFeaturesCollect(&UDF1mmOffsetPointFeat1, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat1_1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint1Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint1Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat2_1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// Boundary Blend
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);


	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();

	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);
	//}
	//if ((point2 == "PNT1") || (point2 == "PNT3") || (point2 == "PNT6") || (point2 == "PNT9") || (point2 == "PNT11") || (point2 == "PNT13"))
	//{
	//	UDFCurve10mmPath = "C:\\Users\\Public\\Documents\\create_10mm_crv_dir_flip.gph.1";
	//	UDFCurve15mmPath = "C:\\Users\\Public\\Documents\\create_15mm_crv_dir_flip.gph.1";

	//	offsetUDFsels.push_back(UdfInputSel[1]);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDFCurveFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	status = ProGroupFeaturesCollect(&UDF2mmOffsetPointFeat2, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat1, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// 15mm
	//	status = ProGroupFeaturesCollect(&UDF1mmOffsetPointFeat2, &UDFGroupFeats);

	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat1_2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();

	//	GetGeomItems(CurMdl1, UDFGroupFeats[2], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);
	//	vecGeomItems.clear();
	//	offsetUDFsels.push_back(offsetDtmPoint2Sel);
	//	offsetUDFsels.push_back(SurfSelection1);
	//	offsetUDFsels.push_back(dirSurfSelection);
	//	LoadUDF(CurMdl1, UDFCurve15mmPath, offsetUDFsels, UDF15mmCurveFeat2_2, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);

	//	// Boundary Blend
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat1, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();
	//	status = ProGroupFeaturesCollect(&UDF10mmCurveFeat2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);


	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
	//	vecGeomItems.clear();

	//	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_2, &UDFGroupFeats);
	//	GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
	//	vecGeomItems.clear();

	//	offsetUDFsels.push_back(curveSel1);
	//	offsetUDFsels.push_back(curveSel2);
	//	LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
	//	offsetUDFsels.clear();
	//	ProArrayFree((ProArray*)&UDFGroupFeats);
	//}
	
	// TODO: create points at the curve ends
	/*//ProBoolean geomItemIsInactive;
	//ProCurve centerCurve1, centerCurve2;
	//ProVector curveEnd1, curveEnd2;
	//ProFeature point_1, point_2;

	//status = ProGroupFeaturesCollect(&UDFCurveFeat1, &UDFGroupFeats);
	//GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
	//status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1);
	//vecGeomItems.clear();
	//status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
	//status = ProCurveXyzdataEval(centerCurve1, 1, curveEnd2, NULL, NULL);
	////status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
	//status = ProDemoOffsetPointCreate(comp_path, curveEnd2, point_1);
	//ProArrayFree((ProArray*)&UDFGroupFeats);

	//status = ProGroupFeaturesCollect(&UDFCurveFeat2, &UDFGroupFeats);
	//GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
	//status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1);
	//vecGeomItems.clear();
	//status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
	//status = ProCurveXyzdataEval(centerCurve1, 1, curveEnd2, NULL, NULL);
	////status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
	//status = ProDemoOffsetPointCreate(comp_path, curveEnd2, point_2);
	//ProArrayFree((ProArray*)&UDFGroupFeats);

	//string UDFGeodesicCurvePath = "C:\\Users\\Public\\Documents\\create_geodesic_curve.gph.1";
	//ProSelection pointSel1, pointSel2;
	//GetGeomItems(CurMdl1, point_1, vecGeomItems);
	//status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel1);
	//vecGeomItems.clear();
	//GetGeomItems(CurMdl1, point_2, vecGeomItems);
	//status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel2);
	//vecGeomItems.clear();

	//ProFeature geoDesicCurveFeat;
	//offsetUDFsels.push_back(pointSel1);
	//offsetUDFsels.push_back(pointSel2);
	//offsetUDFsels.push_back(SurfSelection1);
	//LoadUDF(CurMdl1, UDFGeodesicCurvePath, offsetUDFsels, geoDesicCurveFeat, NULL, PRO_B_FALSE);
	//offsetUDFsels.clear();*/

	//ProBoolean geomItemIsInactive;
	//ProCurve centerCurve1_1, centerCurve2_1, centerCurve1_2, centerCurve2_2;
	//ProVector curveEnd1, curveEnd2;
	//ProFeature point_1, point_2;
	//string UDFGeodesicCurvePath = "C:\\Users\\Public\\Documents\\create_geodesic_curve.gph.1";
	//ProSelection pointSel1, pointSel2;
	//ProFeature geoDesicCurveFeat1, geoDesicCurveFeat2;
	//ProCurve geoDesicCurve1, geoDesicCurve2;

	//// Geodesic curve 1
	//status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_1, &UDFGroupFeats);
	//GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
	//status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1_1);
	//vecGeomItems.clear();
	////status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
	//status = ProCurveXyzdataEval(centerCurve1_1, 1, curveEnd1, NULL, NULL);
	//status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
	//ProArrayFree((ProArray*)&UDFGroupFeats);

	//status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_2, &UDFGroupFeats);
	//GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
	//status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1_2);
	//vecGeomItems.clear();
	////status = ProCurveXyzdataEval(centerCurve2, 0, curveEnd1, NULL, NULL);
	//status = ProCurveXyzdataEval(centerCurve1_2, 1, curveEnd2, NULL, NULL);
	////status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
	//status = ProDemoOffsetPointCreate(comp_path, curveEnd2, point_2);
	//ProArrayFree((ProArray*)&UDFGroupFeats);

	//GetGeomItems(CurMdl1, point_1, vecGeomItems);
	//status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel1);
	//vecGeomItems.clear();
	//GetGeomItems(CurMdl1, point_2, vecGeomItems);
	//status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel2);
	//vecGeomItems.clear();

	//offsetUDFsels.push_back(pointSel1);
	//offsetUDFsels.push_back(pointSel2);
	//offsetUDFsels.push_back(SurfSelection1);
	////LoadUDF(CurMdl1, UDFGeodesicCurvePath, offsetUDFsels, geoDesicCurveFeat1, NULL, PRO_B_FALSE);
	//offsetUDFsels.clear();


	//// Get the control points of the curve
	//status = ProGroupFeaturesCollect(&geoDesicCurveFeat1, &UDFGroupFeats);
	//GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	//status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
	//status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &geoDesicCurve1);
	//ProGeomitem geodesicCurveItem;
	//status = ProCurveToGeomitem((ProSolid)CurMdl1, geoDesicCurve1, &geodesicCurveItem);
	//ProGeomitemdata *geodesicNURBSdata;
	//status = ProGeomitemdataGet(&geodesicCurveItem, &geodesicNURBSdata);
	////status = ProCompositeCurvedataGet();
	//Pro3dPnt *pnts3d;
	//int p_cnt;
	//status = ProCurveTessellationGet(geoDesicCurve1, 0.8, &pnts3d, &p_cnt);
	//vecGeomItems.clear();


	// Geodesic curve 2
	/*status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_1, &UDFGroupFeats);
	GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
	status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve2_1);
	vecGeomItems.clear();
	status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
	status = ProCurveXyzdataEval(centerCurve2_1, 1, curveEnd1, NULL, NULL);
	status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
	ProArrayFree((ProArray*)&UDFGroupFeats);

	status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_2, &UDFGroupFeats);
	GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
	status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
	status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve2_2);
	vecGeomItems.clear();
	status = ProCurveXyzdataEval(centerCurve2, 0, curveEnd1, NULL, NULL);
	status = ProCurveXyzdataEval(centerCurve2_2, 1, curveEnd2, NULL, NULL);
	status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
	status = ProDemoOffsetPointCreate(comp_path, curveEnd2, point_2);
	ProArrayFree((ProArray*)&UDFGroupFeats);

	GetGeomItems(CurMdl1, point_1, vecGeomItems);
	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel1);
	vecGeomItems.clear();
	GetGeomItems(CurMdl1, point_2, vecGeomItems);
	status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel2);
	vecGeomItems.clear();

	offsetUDFsels.push_back(pointSel1);
	offsetUDFsels.push_back(pointSel2);
	offsetUDFsels.push_back(SurfSelection1);
	LoadUDF(CurMdl1, UDFGeodesicCurvePath, offsetUDFsels, geoDesicCurveFeat2, NULL, PRO_B_FALSE);
	offsetUDFsels.clear();

	if (point1 == "PNT0" || point2 == "PNT0")
	{
		ProModelitem midPoint1Item, midPoint2Item;
		status = ProModelitemByNameInit(CurMdl1, PRO_POINT, L"PNT21", &midPoint1Item);
		status = ProModelitemByNameInit(CurMdl1, PRO_POINT, L"PNT22", &midPoint2Item);
		ProSelection midPoint1Sel, midPoint2Sel;
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&midPoint1Item, &midPoint1Sel);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&midPoint2Item, &midPoint2Sel);

		status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_1, &UDFGroupFeats);
		GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
		status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1_1);
		vecGeomItems.clear();
		status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
		status = ProCurveXyzdataEval(centerCurve1_1, 1, curveEnd1, NULL, NULL);
		status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
		ProArrayFree((ProArray*)&UDFGroupFeats);

		GetGeomItems(CurMdl1, point_1, vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel1);
		vecGeomItems.clear();

		offsetUDFsels.push_back(pointSel1);
		offsetUDFsels.push_back(midPoint1Sel);
		offsetUDFsels.push_back(SurfSelection1);
		LoadUDF(CurMdl1, UDFGeodesicCurvePath, offsetUDFsels, geoDesicCurveFeat1, NULL, PRO_B_FALSE);
		offsetUDFsels.clear();


		status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_1, &UDFGroupFeats);
		GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
		status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1_1);
		vecGeomItems.clear();
		status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
		status = ProCurveXyzdataEval(centerCurve1_1, 1, curveEnd1, NULL, NULL);
		status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
		ProArrayFree((ProArray*)&UDFGroupFeats);

		GetGeomItems(CurMdl1, point_1, vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel1);
		vecGeomItems.clear();

		offsetUDFsels.push_back(pointSel1);
		offsetUDFsels.push_back(midPoint2Sel);
		offsetUDFsels.push_back(SurfSelection1);
		LoadUDF(CurMdl1, UDFGeodesicCurvePath, offsetUDFsels, geoDesicCurveFeat2, NULL, PRO_B_FALSE);
		offsetUDFsels.clear();


		
		 Boundary Blend
		status = ProGroupFeaturesCollect(&geoDesicCurveFeat1, &UDFGroupFeats);
		GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
		vecGeomItems.clear();
		status = ProGroupFeaturesCollect(&geoDesicCurveFeat2, &UDFGroupFeats);
		GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
		vecGeomItems.clear();

		offsetUDFsels.push_back(curveSel1);
		offsetUDFsels.push_back(curveSel2);
		LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
		offsetUDFsels.clear();
		ProArrayFree((ProArray*)&UDFGroupFeats);




		status = ProGroupFeaturesCollect(&UDF15mmCurveFeat2_2, &UDFGroupFeats);
		GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
		status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1_1);
		vecGeomItems.clear();
		status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
		status = ProCurveXyzdataEval(centerCurve1_1, 1, curveEnd1, NULL, NULL);
		status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);

		GetGeomItems(CurMdl1, point_1, vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel1);
		vecGeomItems.clear();

		offsetUDFsels.push_back(pointSel1);
		offsetUDFsels.push_back(midPoint2Sel);
		offsetUDFsels.push_back(SurfSelection1);
		LoadUDF(CurMdl1, UDFGeodesicCurvePath, offsetUDFsels, geoDesicCurveFeat1, NULL, PRO_B_FALSE);
		offsetUDFsels.clear();

		status = ProGroupFeaturesCollect(&UDF15mmCurveFeat1_2, &UDFGroupFeats);
		GetCurveGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProGeomitemIsInactive(&vecGeomItems[0], &geomItemIsInactive);
		status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[0], &centerCurve1_1);
		vecGeomItems.clear();
		status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
		status = ProCurveXyzdataEval(centerCurve1_1, 1, curveEnd1, NULL, NULL);
		status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);

		GetGeomItems(CurMdl1, point_1, vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSel1);
		vecGeomItems.clear();

		offsetUDFsels.push_back(pointSel1);
		offsetUDFsels.push_back(midPoint1Sel);
		offsetUDFsels.push_back(SurfSelection1);
		LoadUDF(CurMdl1, UDFGeodesicCurvePath, offsetUDFsels, geoDesicCurveFeat2, NULL, PRO_B_FALSE);
		offsetUDFsels.clear();


		 Boundary Blend
		status = ProGroupFeaturesCollect(&geoDesicCurveFeat1, &UDFGroupFeats);
		GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel1);
		vecGeomItems.clear();
		status = ProGroupFeaturesCollect(&geoDesicCurveFeat2, &UDFGroupFeats);
		GetGeomItems(CurMdl1, UDFGroupFeats[1], vecGeomItems);
		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &curveSel2);
		vecGeomItems.clear();

		offsetUDFsels.push_back(curveSel1);
		offsetUDFsels.push_back(curveSel2);
		LoadUDF(CurMdl1, UDFBoundaryBlendPath, offsetUDFsels, boundaryBlendFeat, NULL, PRO_B_FALSE);
		offsetUDFsels.clear();
		ProArrayFree((ProArray*)&UDFGroupFeats);

	}*/






	//for (int i = 0; i < vecGeomItems.size(); ++i) {
	//	ProBoolean geomItemIsInactive;
	//	status = ProGeomitemIsInactive(&vecGeomItems[i], &geomItemIsInactive);
	//	if (geomItemIsInactive == PRO_B_TRUE) {
	//		continue;
	//	}
	//	ProCurve centerCurve1, centerCurve2;
	//	status = ProGeomitemToCurve((ProGeomitem*)&vecGeomItems[i], &centerCurve1);
	//	ProVector curveEnd1, curveEnd2;
	//	status = ProCurveXyzdataEval(centerCurve1, 0, curveEnd1, NULL, NULL);
	//	status = ProCurveXyzdataEval(centerCurve1, 1, curveEnd2, NULL, NULL);
	//	
	//	ProFeature point_1, point_2;
	//	status = ProDemoOffsetPointCreate(comp_path, curveEnd1, point_1);
	//	status = ProDemoOffsetPointCreate(comp_path, curveEnd2, point_2);
	//	//status = ProCurveCompVisit(centerCurve1, curveCompAction, NULL, NULL);
	//	/*ProEnttype crvType;
	//	status = ProCurveTypeGet(centerCurve1, &crvType);
	//	ProGeomitemdata* centerCurve1Data;
	//	status = ProCurveDataGet(centerCurve1, &centerCurve1Data);*/
	//	int o = 9;
	//}
	
	//status = ProCurveInit((ProSolid)CurMdl1, 82198, &centerCurve1);


	//status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &offsetDtmPoint2Sel);

	/*offsetUDFsels.push_back(offsetDtmPoint2Sel);
	offsetUDFsels.push_back(SurfSelection1);
	offsetUDFsels.push_back(dirSurfSelection);
	LoadUDF(CurMdl1, UDFCurve10mmPath, offsetUDFsels, UDF10mmCurveFeat1, NULL, PRO_B_FALSE);
	offsetUDFsels.clear();*/
}



//void FanOutCreation()
//{
//	ProError status;
//	ProMdl CurMdl1;
//	vector<PointData> vectPointData;
//	vector<PointData> vectCsysData;
//	vector<PointData> vectCsysData1;
//	status = ProMdlCurrentGet(&CurMdl1);
//	vector<ProFeature> vecFeat;
//	ProAsmcomppath comp_path;
//	ProIdTable c_id_table;
//	c_id_table[0] = -1;
//	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);
//	VesModelTree mdlObj(CurMdl1);
//	vector< VesModelTree::AsmTreeData>  plateTreeVec_item1;
//	VesModelTree::AsmTreeData compHdl;
//	//mdlObj.GetAsmCompTreeData("COMPONENT_2", compHdl); //TODO check if this skelton is same as member sk
//	mdlObj.GetModelTreeComponents(CurMdl1, plateTreeVec_item1);
//
//
//	// Curve data check
//	ProCurve curv;
//	//status = ProCurveInit((ProSolid)CurMdl1, 82180, &curv);
//	status = ProCurveInit((ProSolid)CurMdl1, 82186, &curv);
//	ProGeomitemdata* curvData;
//	status = ProCurveDataGet(curv, &curvData);
//	ProCurvedata* nurbsData;
//	status = ProCurveToNURBS(curv, &nurbsData);
//
//
//
//	ProFeature csoFeat;
//	ProFeature csoFeat1;
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	std::vector<ProSelection> UdfInputSel;
//	PointData curPointData = {};
//	string point1 = "PNT0";
//	string point2 = "PNT1";
//	int surfaceid = 41245;
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
//
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	UdfInputSel.clear();
//	curPointData = {};
//	point1 = "PNT4";
//	point2 = "PNT6";
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
//
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	UdfInputSel.clear();
//	curPointData = {};
//	point1 = "PNT3";
//	point2 = "PNT15";
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
//
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	UdfInputSel.clear();
//	curPointData = {};
//	point1 = "PNT9";
//	point2 = "PNT10";
//
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	UdfInputSel.clear();
//	curPointData = {};
//	point1 = "PNT11";
//	point2 = "PNT12";
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
//
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	UdfInputSel.clear();
//	curPointData = {};
//	point1 = "PNT13";
//	point2 = "PNT14";
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
//
//
//	//for (auto currchildComp : plateTreeVec_item1)
//	//{
//	//vector<string> createdFanouts;
//	//int pintype1 = 0;
//	//int pintype2 = 0;
//	//int pintype3 = 0;
//	//int pintype4 = 0;
//	//int pintype5 = 0;
//	//for (int i = 0; i < plateTreeVec_item1.size(); i++)
//	//{
//	//	ProMdlType mdltyp;
//	//	ProMdlTypeGet(plateTreeVec_item1[i].lMdlAssm, &mdltyp);
//	//	if (mdltyp == ProMdlType::PRO_MDL_PART) {
//	//		string itemparamval;
//	//		GetParamValue(plateTreeVec_item1[i].lMdlAssm, "PIN", itemparamval, false);
//	//		if (itemparamval != "")
//	//		{
//	//			for (int j = 0; j < plateTreeVec_item1.size(); j++)
//	//			{
//	//				if (plateTreeVec_item1[i].lAsmName != plateTreeVec_item1[j].lAsmName)
//	//				{
//	//					ProMdlType mdltyp;
//	//					ProMdlTypeGet(plateTreeVec_item1[j].lMdlAssm, &mdltyp);
//	//					if (mdltyp == ProMdlType::PRO_MDL_PART) {
//	//						string itemparamval1;
//	//						GetParamValue(plateTreeVec_item1[j].lMdlAssm, "PIN", itemparamval1, false);
//	//						bool isAlreadyExists = false;
//	//						if ((std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[i].lAsmName) != createdFanouts.end()) ||
//	//							(std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[j].lAsmName) != createdFanouts.end()))
//	//						{
//	//							isAlreadyExists = true;
//	//						}
//	//						if (itemparamval == itemparamval1 && !isAlreadyExists /*(plateTreeVec_item1[i].lAsmName =="PIN_1" && plateTreeVec_item1[j].lAsmName=="COMPONENT_3")*/)
//	//						{
//	//							createdFanouts.push_back(plateTreeVec_item1[i].lAsmName);
//	//							createdFanouts.push_back(plateTreeVec_item1[j].lAsmName);
//
//	//							vector<ProSelection> UdfInputSel;
//	//							string featName = "";
//	//							string Udfpath = "";
//	//							string PlaneName = "ATOP";
//	//							string CurveName = "CURVE";
//	//							if ((plateTreeVec_item1[i].lAsmName == "PIN_1" /*&& plateTreeVec_item1[j].lAsmName == "COMPONENT_3"*/) ||
//	//								(plateTreeVec_item1[i].lAsmName == "PIN_4" /*&& plateTreeVec_item1[j].lAsmName == "COMPONENT_7"*/) ||
//	//								(plateTreeVec_item1[i].lAsmName == "PIN_7") ||
//	//								(plateTreeVec_item1[i].lAsmName == "PIN_8"))
//	//							{
//	//								featName = "PNT0";
//	//								Udfpath = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\curve.gph.1";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype1;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype1 == 0)
//	//									CurveName = "CURVE";
//	//								else
//	//									CurveName = "CURVE_" + strPinTYpe;
//	//								pintype1++;
//	//							}
//	//							else if (plateTreeVec_item1[i].lAsmName == "PIN_3")
//	//							{
//	//								//Udfpath = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\ob_bend_curve_l.gph.1";
//	//								Udfpath = "C:\\Users\\MUSRI\\Downloads\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\cross_curve_l.gph.1";
//	//								featName = "CS0";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype2;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype2 == 0)
//	//									CurveName = "CROSS_CURVE_L";
//	//								else
//	//									CurveName = "CROSS_CURVE_L_" + strPinTYpe;
//	//								pintype2++;
//	//							}
//	//							else if (plateTreeVec_item1[i].lAsmName == "PIN_5")
//	//							{
//	//								Udfpath = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\ob_bend_curve_l.gph.1";
//	//								featName = "CS0";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype4;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype4 == 0)
//	//									CurveName = "OB_BEND_CURVE_L";
//	//								else
//	//									CurveName = "OB_BEND_CURVE_L_" + strPinTYpe;
//	//								pintype4++;
//	//							}
//	//							else if (plateTreeVec_item1[i].lAsmName == "PIN_6")
//	//							{
//	//								featName = "CS0";
//	//								Udfpath = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\ob_bend_curve.gph.1";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype5;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype5 == 0)
//	//									CurveName = "OB_BEND_CURVE";
//	//								else
//	//									CurveName = "OB_BEND_CURVE_" + strPinTYpe;
//	//								pintype5++;
//	//							}
//	//							else
//	//							{
//	//								featName = "CS0";
//	//								//Udfpath = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\ob_bend_curve.gph.1";
//	//								Udfpath = "C:\\Users\\MUSRI\\Downloads\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\cross_curve.gph.1";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype3;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype3 == 0)
//	//									CurveName = "CROSS_CURVE";
//	//								else
//	//									CurveName = "CROSS_CURVE_" + strPinTYpe;
//	//								pintype3++;
//	//							}
//	//							ProFeature UDFGrpFeat;
//	//							LoadUDFWithInputs(CurMdl1, plateTreeVec_item1, j, i, featName, Udfpath, UDFGrpFeat);
//
//
//	//							string Udfpath1 = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\proj.gph.1";
//
//	//							ProjectingCurveOnSurface(mdlObj, CurMdl1, comp_path, PlaneName, CurveName, UDFGrpFeat, Udfpath1);
//	//							break;
//
//	//						}
//
//	//					}
//
//	//				}
//	//			}
//	//		}
//
//	//	}
//	//}
//	//createdFanouts.clear();
//}

//void FanOutCreation()
//{
//	ProError status;
//	ProMdl CurMdl1;
//	vector<PointData> vectPointData;
//	vector<PointData> vectCsysData;
//	vector<PointData> vectCsysData1;
//	status = ProMdlCurrentGet(&CurMdl1);
//	vector<ProFeature> vecFeat;
//	ProAsmcomppath comp_path;
//	ProIdTable c_id_table;
//	c_id_table[0] = -1;
//	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);
//	VesModelTree mdlObj(CurMdl1);
//	vector< VesModelTree::AsmTreeData>  plateTreeVec_item1;
//	VesModelTree::AsmTreeData compHdl;
//	//mdlObj.GetAsmCompTreeData("COMPONENT_2", compHdl); //TODO check if this skelton is same as member sk
//	mdlObj.GetModelTreeComponents(CurMdl1, plateTreeVec_item1);
//	//for (auto currchildComp : plateTreeVec_item1)
//	//{
//	vector<string> createdFanouts;
//	int pintype1 = 0;
//	int pintype2 = 0;
//	int pintype3 = 0;
//
//	ProModelitem placementSurfItem;
//	/*int n_sel;
//	status = ProSelect((char*)"surface", 1, NULL, NULL, NULL, NULL, &placementSurfSel, &n_sel);
//	status = ProSelectionModelitemGet(placementSurfSel[0], &placementSurfItem);
//	int surfModelId;
//	status = ProMdlIdGet(placementSurfItem.owner, &surfModelId);*/
//	ProMdl surfMdl;
//	//status = ProMdlInit(L"PRINTED_ELECTRONICS", PRO_MDL_PART, &surfMdl);
//
//	status = ProAsmcomppathMdlGet(&plateTreeVec_item1[1].lAsmPath, &surfMdl);
//	status = ProModelitemInit(surfMdl, 41245, PRO_SURFACE , &placementSurfItem);
//	ProSelection surf_sel;
//	status = ProSelectionAlloc(&plateTreeVec_item1[1].lAsmPath, &placementSurfItem, &surf_sel);
//
//
//	// --------------------------------------------------------- CURVE 1  ---------------------------------------------------------
//	ProFeature csoFeat;
//	ProFeature csoFeat1;
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	std::vector<ProSelection> UdfInputSel;
//	PointData curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT40", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	PointData curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT41", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//
//	string Udfpath = "D:\\Project Details\\crowdplat\\UDF\\geodesic_1.gph.1";
//	ProFeature UDFGrpFeat;
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 1 END  ---------------------------------------------------------
//
//	//// --------------------------------------------------------- CURVE 2  ---------------------------------------------------------
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	UdfInputSel.clear();
//	curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT42", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT43", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//	UDFGrpFeat = {};
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 2 END  ---------------------------------------------------------
//
//	//// --------------------------------------------------------- CURVE 3  ---------------------------------------------------------
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	UdfInputSel.clear();
//	curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT44", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT45", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//	UDFGrpFeat = {};
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 3 END  ---------------------------------------------------------
//
//	//// --------------------------------------------------------- CURVE 4  ---------------------------------------------------------
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	UdfInputSel.clear();
//	curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT46", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT47", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//	UDFGrpFeat = {};
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 4 END  ---------------------------------------------------------
//
//	//// --------------------------------------------------------- CURVE 5  ---------------------------------------------------------
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	UdfInputSel.clear();
//	curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT48", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT49", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//	UDFGrpFeat = {};
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 5 END  ---------------------------------------------------------
//
//	//// --------------------------------------------------------- CURVE 6  ---------------------------------------------------------
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	UdfInputSel.clear();
//	curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT50", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT51", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//	UDFGrpFeat = {};
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 6 END  ---------------------------------------------------------
//
//	//// --------------------------------------------------------- CURVE 7  ---------------------------------------------------------
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	UdfInputSel.clear();
//	curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT52", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT53", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//	UDFGrpFeat = {};
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 7 END  ---------------------------------------------------------
//
//	//// --------------------------------------------------------- CURVE 8  ---------------------------------------------------------
//	csoFeat = {};
//	csoFeat1 = {};
//	vectPointData.clear();
//	//ProError status;
//	//std::vector<PointData> vectPointData;
//	UdfInputSel.clear();
//	curPointData = {};
//	//curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//	//curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, featName, csoFeat);
//	GetFeatureByName(CurMdl1, "APNT54", csoFeat);
//	curPointData.lPointFeat = csoFeat;
//
//	curPointData1 = {};
//	//curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//	//curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//	//GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, featName, csoFeat1);
//	GetFeatureByName(CurMdl1, "APNT55", csoFeat1);
//	curPointData1.lPointFeat = csoFeat1;
//
//	vectPointData.push_back(curPointData1);
//	vectPointData.push_back(curPointData);
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(CurMdl1, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	UdfInputSel.push_back(surf_sel);
//
//	UDFGrpFeat = {};
//
//	if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	// --------------------------------------------------------- CURVE 8 END  ---------------------------------------------------------
//
//
//	
//	//for (int i = 0; i < plateTreeVec_item1.size(); i++)
//	//{
//	//	ProMdlType mdltyp;
//	//	ProMdlTypeGet(plateTreeVec_item1[i].lMdlAssm, &mdltyp);
//	//	if (mdltyp == ProMdlType::PRO_MDL_PART) {
//	//		string itemparamval;
//	//		GetParamValue(plateTreeVec_item1[i].lMdlAssm, "PIN", itemparamval, false);
//	//		if (itemparamval != "")
//	//		{
//	//			for (int j = 0; j < plateTreeVec_item1.size(); j++)
//	//			{
//	//				if (plateTreeVec_item1[i].lAsmName != plateTreeVec_item1[j].lAsmName)
//	//				{
//	//					ProMdlType mdltyp;
//	//					ProMdlTypeGet(plateTreeVec_item1[j].lMdlAssm, &mdltyp);
//	//					if (mdltyp == ProMdlType::PRO_MDL_PART) {
//	//						string itemparamval1;
//	//						GetParamValue(plateTreeVec_item1[j].lMdlAssm, "PIN", itemparamval1, false);
//	//						bool isAlreadyExists = false;
//	//						if ((std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[i].lAsmName) != createdFanouts.end()) ||
//	//							(std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[j].lAsmName) != createdFanouts.end()))
//	//						{
//	//							isAlreadyExists = true;
//	//						}
//	//						if (itemparamval == itemparamval1 && !isAlreadyExists /*(plateTreeVec_item1[i].lAsmName =="PIN_1" && plateTreeVec_item1[j].lAsmName=="COMPONENT_3")*/)
//	//						{
//	//							createdFanouts.push_back(plateTreeVec_item1[i].lAsmName);
//	//							createdFanouts.push_back(plateTreeVec_item1[j].lAsmName);
//	//							vector<ProSelection> UdfInputSel;
//	//							string featName = "";
//	//							string Udfpath = "";
//	//							string PlaneName = "ATOP";
//	//							string CurveName = "CURVE";
//	//							if ((plateTreeVec_item1[i].lAsmName == "PIN_1" /*&& plateTreeVec_item1[j].lAsmName == "COMPONENT_3"*/) ||
//	//								(plateTreeVec_item1[i].lAsmName == "PIN_4" /*&& plateTreeVec_item1[j].lAsmName == "COMPONENT_7"*/) ||
//	//								(plateTreeVec_item1[i].lAsmName == "PIN_7") ||
//	//								(plateTreeVec_item1[i].lAsmName == "PIN_8"))
//	//							{
//	//								featName = "PNT0";
//	//								Udfpath = "D:\\Project Details\\crowdplat\\UDF\\geodesic_1.gph.1";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype1;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype1 == 0)
//	//									CurveName = "CURVE";
//	//								else
//	//									CurveName = "CURVE_" + strPinTYpe;
//	//								pintype1++;
//	//							}
//	//							else if ((plateTreeVec_item1[i].lAsmName == "PIN_3") || (plateTreeVec_item1[i].lAsmName == "PIN_5"))
//	//							{
//	//								Udfpath = "D:\\Project Details\\crowdplat\\UDF\\geodesic_1.gph.1";
//	//								featName = "PNT0";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype2;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype2 == 0)
//	//									CurveName = "OB_BEND_CURVE_L";
//	//								else
//	//									CurveName = "OB_BEND_CURVE_L_" + strPinTYpe;
//	//								pintype2++;
//	//							}
//	//							else
//	//							{
//	//								featName = "PNT0";
//	//								Udfpath = "D:\\Project Details\\crowdplat\\UDF\\geodesic_1.gph.1";
//	//								std::stringstream ssDbl;
//	//								ssDbl << pintype3;
//	//								string strPinTYpe(ssDbl.str());
//	//								if (pintype3 == 0)
//	//									CurveName = "OB_BEND_CURVE";
//	//								else
//	//									CurveName = "OB_BEND_CURVE_" + strPinTYpe;
//	//								pintype3++;
//	//							}
//	//							ProFeature UDFGrpFeat;
//	//							//LoadUDFWithInputs(CurMdl1, plateTreeVec_item1, j, i, featName, Udfpath, UDFGrpFeat);
//	//							//string Udfpath1 = "D:\\Project Details\\crowdplat\\Updated model\\13-12-2023\\proper_asm\\UDF\\proj.gph.1";
//	//							//ProjectingCurveOnSurface(mdlObj, CurMdl1, comp_path, PlaneName, CurveName, UDFGrpFeat, Udfpath1);
//	//							break;
//	//						}
//	//					}
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//	//createdFanouts.clear();
//
//}

//
//void FanOutCreation()
//{
//	ProError status;
//	ProMdl CurMdl1;
//	vector<PointData> vectPointData;
//	vector<PointData> vectCsysData;
//	vector<PointData> vectCsysData1;
//	status = ProMdlCurrentGet(&CurMdl1);
//	vector<ProFeature> vecFeat;
//	ProAsmcomppath comp_path;
//	ProIdTable c_id_table;
//	c_id_table[0] = -1;
//	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);
//	VesModelTree mdlObj(CurMdl1);
//	vector< VesModelTree::AsmTreeData>  plateTreeVec_item1;
//	VesModelTree::AsmTreeData compHdl;
//	//mdlObj.GetAsmCompTreeData("COMPONENT_2", compHdl); //TODO check if this skelton is same as member sk
//	mdlObj.GetModelTreeComponents(CurMdl1, plateTreeVec_item1);
//	//for (auto currchildComp : plateTreeVec_item1)
//	//{
//	vector<string> createdFanouts;
//	int pintype1 = 0;
//	int pintype2 = 0;
//	int pintype3 = 0;
//	int pintype4 = 0;
//	int pintype5 = 0;
//	ProSelection* p_select;
//	/*int n_select;
//	status = ProSelect("csys", 1, NULL, NULL, NULL, NULL, &p_select, &n_select);*/
//
//
//	for (int i = 0; i < plateTreeVec_item1.size(); i++)
//	{
//		ProMdlType mdltyp;
//		ProMdlTypeGet(plateTreeVec_item1[i].lMdlAssm, &mdltyp);
//		if (mdltyp == ProMdlType::PRO_MDL_PART) {
//			string itemparamval;
//			GetParamValue(plateTreeVec_item1[i].lMdlAssm, "PIN", itemparamval, false);
//			if (itemparamval != "")
//			{
//				for (int j = 0; j < plateTreeVec_item1.size(); j++)
//				{
//					if (plateTreeVec_item1[i].lAsmName != plateTreeVec_item1[j].lAsmName)
//					{
//						ProMdlType mdltyp;
//						ProMdlTypeGet(plateTreeVec_item1[j].lMdlAssm, &mdltyp);
//						if (mdltyp == ProMdlType::PRO_MDL_PART) {
//							string itemparamval1;
//							GetParamValue(plateTreeVec_item1[j].lMdlAssm, "PIN", itemparamval1, false);
//							bool isAlreadyExists = false;
//							if ((std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[i].lAsmName) != createdFanouts.end()) ||
//								(std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[j].lAsmName) != createdFanouts.end()))
//							{
//								isAlreadyExists = true;
//							}
//							if (itemparamval == itemparamval1 && !isAlreadyExists /*/ (plateTreeVec_item1[i].lAsmName == "PIN_1" && plateTreeVec_item1[j].lAsmName == "COMPONENT_3") /*/)
//							{
//								//// shoot a ray and take the hits
//								//ProRay ray1;
//								///*ray1.start_point[0] = -215.0;
//								//ray1.start_point[1] = -225.0;
//								//ray1.start_point[2] = 337.305;*/
//								// 
//								//ray1.start_point[0] = -61.576;
//								//ray1.start_point[1] = 354.69;
//								//ray1.start_point[2] = -28.84;
//
//								///*ray1.dir_vector[0] = -0.46;
//								//ray1.dir_vector[1] = -0.49;
//								//ray1.dir_vector[2] = 0.73;*/
//								// 
//								//ray1.dir_vector[0] = 0;
//								//ray1.dir_vector[1] = 0;
//								//ray1.dir_vector[2] = 1;
//								// 
//								////createDatumPointFeature();
//								//ProSelection* hitSels;
//								//int selCount;
//								///*ProGraphicsPenPosition(ray1.start_point);
//								//ProGraphicsCircleDraw(ray1.start_point, 100);*/
//								//status = ProSolidRayIntersectionCompute(CurMdl1, 10.0, &ray1, &hitSels, &selCount);
//								//for (int s = 0; s < selCount; ++s) {
//								//	ProPoint3d pnt3d;
//								//	status = ProSelectionPoint3dGet(hitSels[s], pnt3d);
//								//	ProGraphicsCircleDraw(pnt3d, 10);
//								//	//createDatumPointFeature(pnt3d, p_select[0]);
//								//	ProModelitem selMdlitem;
//								//	status = ProSelectionModelitemGet(hitSels[s], &selMdlitem);
//								//	ProMdl mdlitemMdl;
//								//	status = ProModelitemMdlGet(&selMdlitem, &mdlitemMdl);
//								//	ProName mdlName;
//								//	status = ProMdlMdlnameGet(mdlitemMdl, mdlName);
//								//	ProAsmcomppath selAsmcomppath;
//								//	status = ProSelectionAsmcomppathGet(hitSels[s], &selAsmcomppath);
//								//}
//								//ProColor highlite_color;
//								//highlite_color.method = PRO_COLOR_METHOD_TYPE;
//								//highlite_color.value.type = PRO_COLOR_HIGHLITE;
//								//ProGraphicsColorModify(&highlite_color, NULL);
//
//								createdFanouts.push_back(plateTreeVec_item1[i].lAsmName);
//								createdFanouts.push_back(plateTreeVec_item1[j].lAsmName);
//
//								vector<ProSelection> UdfInputSel;
//								string featName = "";
//								string Udfpath = "";
//								string PlaneName = "ATOP";
//								string CurveName = "CURVE";
//								if ((plateTreeVec_item1[i].lAsmName == "PIN_1" /*/ && plateTreeVec_item1[j].lAsmName == "COMPONENT_3" /*/) ||
//									(plateTreeVec_item1[i].lAsmName == "PIN_4" /*/ && plateTreeVec_item1[j].lAsmName == "COMPONENT_7" /*/) ||
//									(plateTreeVec_item1[i].lAsmName == "PIN_7") ||
//									(plateTreeVec_item1[i].lAsmName == "PIN_8"))
//								{
//									featName = "PNT0";
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\03-01-2024\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\curve.gph.1";
//									std::stringstream ssDbl;
//									ssDbl << pintype1;
//									string strPinTYpe(ssDbl.str());
//									if (pintype1 == 0)
//										CurveName = "CURVE";
//									else
//										CurveName = "CURVE_" + strPinTYpe;
//									pintype1++;
//								}
//								else if (plateTreeVec_item1[i].lAsmName == "PIN_3")
//								{
//									//Udfpath = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\ob_bend_curve_l.gph.1";
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\03-01-2024\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\cross_curve_l.gph.1";
//									featName = "CS0";
//									std::stringstream ssDbl;
//									ssDbl << pintype2;
//									string strPinTYpe(ssDbl.str());
//									if (pintype2 == 0)
//										CurveName = "CROSS_CURVE_L";
//									else
//										CurveName = "CROSS_CURVE_L_" + strPinTYpe;
//									pintype2++;
//								}
//								else if (plateTreeVec_item1[i].lAsmName == "PIN_5")
//								{
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\03-01-2024\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\ob_bend_curve_l.gph.1";
//									featName = "CS0";
//									std::stringstream ssDbl;
//									ssDbl << pintype4;
//									string strPinTYpe(ssDbl.str());
//									if (pintype4 == 0)
//										CurveName = "OB_BEND_CURVE_L";
//									else
//										CurveName = "OB_BEND_CURVE_L_" + strPinTYpe;
//									pintype4++;
//								}
//								else if (plateTreeVec_item1[i].lAsmName == "PIN_6")
//								{
//									featName = "CS0";
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\03-01-2024\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\ob_bend_curve.gph.1";
//									std::stringstream ssDbl;
//									ssDbl << pintype5;
//									string strPinTYpe(ssDbl.str());
//									if (pintype5 == 0)
//										CurveName = "OB_BEND_CURVE";
//									else
//										CurveName = "OB_BEND_CURVE_" + strPinTYpe;
//									pintype5++;
//								}
//								else
//								{
//									featName = "CS0";
//									//Udfpath = "C:\\Users\\MUSRI\\Downloads\\fo_proper_asm_to_Murali\\fo_proper_asm_to_Murali\\UDF\\ob_bend_curve.gph.1";
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\03-01-2024\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\cross_curve.gph.1";
//									std::stringstream ssDbl;
//									ssDbl << pintype3;
//									string strPinTYpe(ssDbl.str());
//									if (pintype3 == 0)
//										CurveName = "CROSS_CURVE";
//									else
//										CurveName = "CROSS_CURVE_" + strPinTYpe;
//									pintype3++;
//								}
//								ProFeature UDFGrpFeat;
//								//LoadUDFWithInputs(CurMdl1, plateTreeVec_item1, j, i, featName, Udfpath, UDFGrpFeat);
//
//
//								string Udfpath1 = "D:\\Project Details\\crowdplat\\Updated model\\03-01-2024\\Final asm 03Jan2024\\Final asm 03Jan2024\\UDF\\proj.gph.1";
//
//								//ProjectingCurveOnSurface(mdlObj, CurMdl1, comp_path, PlaneName, CurveName, UDFGrpFeat, Udfpath1);
//								break;
//
//							}
//
//						}
//
//					}
//				}
//			}
//
//		}
//	}
//	createdFanouts.clear();
//
//	ProSelection* p_select1;
//	int n_select1;
//	status = ProSelect("part", 1, NULL, NULL, NULL, NULL, &p_select1, &n_select1);	// need ot figure out the logic to automatically fetch the part
//
//	ProAsmcomppath selAsmcomppath;
//	ProMdl obstaclePart1;
//	status = ProSelectionAsmcomppathGet(p_select1[0], &selAsmcomppath);
//	status = ProAsmcomppathMdlGet(&selAsmcomppath, &obstaclePart1);
//
//	//static ProMatrix matrix = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
//	static ProMatrix matrix;
//	ProSelection* psels;
//	int sel_count;
//	status = ProSelect("csys", 1, NULL, NULL, NULL, NULL, &psels, &sel_count);
//
//	ProModelitem csys_item;
//	status = ProSelectionModelitemGet(psels[0], &csys_item);
//
//	ProGeomitemdata* geomitem_data;
//	status = ProGeomitemdataGet(&csys_item, &geomitem_data);
//
//	ProCsysdata *csys_data = NULL;
//	csys_data = geomitem_data->data.p_csys_data;
//
//	ProMatrix csysMatrix{};
//	status = ProMatrixInit(
//		csys_data->x_vector,
//		csys_data->y_vector,
//		csys_data->z_vector,
//		csys_data->origin,
//		csysMatrix);
//
//	ProMatrix csysMatrixInverted{};
//	ProUtilMatrixInvert(csysMatrix, csysMatrixInverted);
//
//	ProSolidOutlExclTypes excl[6] = { PRO_OUTL_EXC_DATUM_PLANE, PRO_OUTL_EXC_DATUM_POINT, PRO_OUTL_EXC_DATUM_CSYS,
//									  PRO_OUTL_EXC_DATUM_AXES,  PRO_OUTL_EXC_FACETS, PRO_OUTL_EXC_ALL_CRVS };
//	Pro3dPnt r_outline_points[2];
//	status = ProSolidOutlineCompute((ProSolid)obstaclePart1, csysMatrix, excl, 6, r_outline_points);
//
//
//
//	// shoot a ray and take the hits
//	ProRay ray1;
//	ray1.start_point[0] = -95;
//	ray1.start_point[1] = 380;
//	ray1.start_point[2] = -90;
//								 
//	ray1.dir_vector[0] = 120;
//	ray1.dir_vector[1] = 42.695;
//	ray1.dir_vector[2] = 135;
//								 
//	//createDatumPointFeature();
//	ProSelection* hitSels;
//	int selCount;
//	/*ProGraphicsPenPosition(ray1.start_point);
//	ProGraphicsCircleDraw(ray1.start_point, 100);*/
//	status = ProSolidRayIntersectionCompute(CurMdl1, 10.0, &ray1, &hitSels, &selCount);
//
//
//
//	status = ProSolidFeatVisit((ProSolid)CurMdl1, (ProFeatureVisitAction)curMdlCurveVisitFunction, NULL, NULL);
//
//
//
//	for (int i = 0; i < curMdlCurves.size(); ++i) {
//
//		ProCurve curve;
//		//status = ProGeomitemToCurve(&curMdlCurves[i], &curve);
//		ProCurveInit((ProSolid)CurMdl1, curMdlCurves[i].id, &curve);
//		Pro3dPnt* points3D;
//		int pnts_cnt;
//		status = ProCurveTessellationGet(curve, 1, &points3D, &pnts_cnt);
//
//		for (int j = 0; j < pnts_cnt; ++j) {
//			Pro3dPnt tempPnt;
//			tempPnt[0] = points3D[j][0];
//			tempPnt[1] = points3D[j][1];
//			tempPnt[2] = points3D[j][2];
//			string tempp = "D:";
//		}
//	}
//
//	//ProModelitem curve_item;
//	//status = ProModelitemInit((ProSolid)CurMdl1, 10745, PRO_CURVE, &curve_item);	// 11063, 10745, 11060
//
//	/*ProSelection* psels1;
//	int sel_count1;
//	status = ProSelect((char*)"curve", 1, NULL, NULL, NULL, NULL, &psels1, &sel_count1);
//	status = ProSelectionModelitemGet(psels1[0], &curve_item);*/
//
//	//ProCurve curve;
//	//status = ProGeomitemToCurve(&curve_item, &curve);
//
//	//ProGeomitemdata *geomitem_curve_data;
//	//status = ProGeomitemdataGet(&curve_item, &geomitem_curve_data);
//	//ProCurvedata *curve_data;
//	//status = ProCurveToNURBS(curve, &curve_data);
//
//	/*ProPoint3d* points3D;
//	int pnts_cnt;
//	status = ProCurveTessellationGet(curve, 0.5, &points3D, &pnts_cnt);*/
//
//	/*
//	ProEnttype curve_type;
//	status = ProCurveTypeGet(curve, &curve_type);
//	ProGeomitemdata* curve_item_data;
//	status = ProCurveDataGet(curve, &curve_item_data);
//	int noOfCurvePnts = curve_item_data->data.p_curve_data->spline.num_points;*/
//
//
//}

//void FanOutCreation()
//{
//	ProError status;
//	ProMdl CurMdl1;
//	vector<PointData> vectPointData;
//	vector<PointData> vectCsysData;
//	vector<PointData> vectCsysData1;
//	status = ProMdlCurrentGet(&CurMdl1);
//	vector<ProFeature> vecFeat;
//	ProAsmcomppath comp_path;
//	ProIdTable c_id_table;
//	c_id_table[0] = -1;
//	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);
//	VesModelTree mdlObj(CurMdl1);
//	vector< VesModelTree::AsmTreeData>  plateTreeVec_item1;
//	VesModelTree::AsmTreeData compHdl;
//	//mdlObj.GetAsmCompTreeData("COMPONENT_2", compHdl); //TODO check if this skelton is same as member sk
//	mdlObj.GetModelTreeComponents(CurMdl1, plateTreeVec_item1);
//	//for (auto currchildComp : plateTreeVec_item1)
//	//{
//	vector<string> createdFanouts;
//	int pintype1 = 0;
//	int pintype2 = 0;
//	int pintype3 = 0;
//	for (int i = 0; i < plateTreeVec_item1.size(); i++)
//	{
//		ProMdlType mdltyp;
//		ProMdlTypeGet(plateTreeVec_item1[i].lMdlAssm, &mdltyp);
//		if (mdltyp == ProMdlType::PRO_MDL_PART) {
//			string itemparamval;
//			GetParamValue(plateTreeVec_item1[i].lMdlAssm, "PIN", itemparamval, false);
//			if (itemparamval != "")
//			{
//				for (int j = 0; j < plateTreeVec_item1.size(); j++)
//				{
//					if (plateTreeVec_item1[i].lAsmName != plateTreeVec_item1[j].lAsmName)
//					{
//						ProMdlType mdltyp;
//						ProMdlTypeGet(plateTreeVec_item1[j].lMdlAssm, &mdltyp);
//						if (mdltyp == ProMdlType::PRO_MDL_PART) {
//							string itemparamval1;
//							GetParamValue(plateTreeVec_item1[j].lMdlAssm, "PIN", itemparamval1, false);
//							bool isAlreadyExists = false;
//							if ((std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[i].lAsmName) != createdFanouts.end()) ||
//								(std::find(createdFanouts.begin(), createdFanouts.end(), plateTreeVec_item1[j].lAsmName) != createdFanouts.end()))
//							{
//								isAlreadyExists = true;
//							}
//							if (itemparamval == itemparamval1 && !isAlreadyExists /*(plateTreeVec_item1[i].lAsmName =="PIN_1" && plateTreeVec_item1[j].lAsmName=="COMPONENT_3")*/)
//							{
//								createdFanouts.push_back(plateTreeVec_item1[i].lAsmName);
//								createdFanouts.push_back(plateTreeVec_item1[j].lAsmName);
//
//								vector<ProSelection> UdfInputSel;
//								string featName = "";
//								string Udfpath = "";
//								string PlaneName = "ATOP";
//								string CurveName = "CURVE";
//								if ((plateTreeVec_item1[i].lAsmName == "PIN_1" /*&& plateTreeVec_item1[j].lAsmName == "COMPONENT_3"*/) ||
//									(plateTreeVec_item1[i].lAsmName == "PIN_4" /*&& plateTreeVec_item1[j].lAsmName == "COMPONENT_7"*/) ||
//									(plateTreeVec_item1[i].lAsmName == "PIN_7") ||
//									(plateTreeVec_item1[i].lAsmName == "PIN_8"))
//								{
//									featName = "PNT0";
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\13-12-2023\\proper_asm\\UDF\\curve.gph.1";
//									std::stringstream ssDbl;
//									ssDbl << pintype1;
//									string strPinTYpe(ssDbl.str());
//									if (pintype1 == 0)
//										CurveName = "CURVE";
//									else
//										CurveName = "CURVE_" + strPinTYpe;
//									pintype1++;
//								}
//								else if ((plateTreeVec_item1[i].lAsmName == "PIN_3") || (plateTreeVec_item1[i].lAsmName == "PIN_5"))
//								{
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\13-12-2023\\proper_asm\\UDF\\ob_bend_curve_l.gph.1";
//									featName = "CS0";
//									std::stringstream ssDbl;
//									ssDbl << pintype2;
//									string strPinTYpe(ssDbl.str());
//									if (pintype2 == 0)
//										CurveName = "OB_BEND_CURVE_L";
//									else
//										CurveName = "OB_BEND_CURVE_L_" + strPinTYpe;
//									pintype2++;
//								}
//								else
//								{
//									featName = "CS0";
//									Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\13-12-2023\\proper_asm\\UDF\\ob_bend_curve.gph.1";
//									std::stringstream ssDbl;
//									ssDbl << pintype3;
//									string strPinTYpe(ssDbl.str());
//									if (pintype3 == 0)
//										CurveName = "OB_BEND_CURVE";
//									else
//										CurveName = "OB_BEND_CURVE_" + strPinTYpe;
//									pintype3++;
//								}
//								ProFeature UDFGrpFeat;
//								LoadUDFWithInputs(CurMdl1, plateTreeVec_item1, j, i, featName, Udfpath, UDFGrpFeat);
//
//
//								string Udfpath1 = "D:\\Project Details\\crowdplat\\Updated model\\13-12-2023\\proper_asm\\UDF\\proj.gph.1";
//
//								ProjectingCurveOnSurface(mdlObj, CurMdl1, comp_path, PlaneName, CurveName, UDFGrpFeat, Udfpath1);
//								break;
//
//							}
//
//						}
//
//					}
//				}
//			}
//
//		}
//	}
//	createdFanouts.clear();
//}

//void FanOutCreation()
//{
//	ProError status;
//	ProMdl CurMdl1;
//	vector<PointData> vectPointData;
//	vector<PointData> vectCsysData;
//	vector<PointData> vectCsysData1;
//	status = ProMdlCurrentGet(&CurMdl1);
//	vector<ProFeature> vecFeat;
//	ProAsmcomppath comp_path;
//	ProIdTable c_id_table;
//	c_id_table[0] = -1;
//	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);
//	VesModelTree mdlObj(CurMdl1);
//	vector< VesModelTree::AsmTreeData>  plateTreeVec_item1;
//	VesModelTree::AsmTreeData compHdl;
//	//mdlObj.GetAsmCompTreeData("COMPONENT_2", compHdl); //TODO check if this skelton is same as member sk
//	mdlObj.GetModelTreeComponents(CurMdl1, plateTreeVec_item1);
//	//for (auto currchildComp : plateTreeVec_item1)
//	//{
//	for (int i = 0; i < plateTreeVec_item1.size(); i++)
//	{
//		ProMdlType mdltyp;
//		ProMdlTypeGet(plateTreeVec_item1[i].lMdlAssm, &mdltyp);
//		if (mdltyp == ProMdlType::PRO_MDL_PART) {
//			string itemparamval;
//			GetParamValue(plateTreeVec_item1[i].lMdlAssm, "PIN", itemparamval, false);
//			if (itemparamval != "")
//			{
//				for (int j = 0; j < plateTreeVec_item1.size(); j++)
//				{
//					if (plateTreeVec_item1[i].lAsmName != plateTreeVec_item1[j].lAsmName)
//					{
//						ProMdlType mdltyp;
//						ProMdlTypeGet(plateTreeVec_item1[j].lMdlAssm, &mdltyp);
//						if (mdltyp == ProMdlType::PRO_MDL_PART) {
//							string itemparamval1;
//							GetParamValue(plateTreeVec_item1[j].lMdlAssm, "PIN", itemparamval1, false);
//							if (itemparamval == itemparamval1 && (plateTreeVec_item1[i].lAsmName == "PIN_1" && plateTreeVec_item1[j].lAsmName == "COMPONENT_3"))
//							{
//								ProFeature csoFeat;
//								ProFeature csoFeat1;
//								PointData curPointData = {};
//								curPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//								curPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//								GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, "PNT0", csoFeat);
//								curPointData.lPointFeat = csoFeat;
//
//								PointData curPointData1 = {};
//								curPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//								curPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//								GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, "PNT0", csoFeat1);
//								curPointData1.lPointFeat = csoFeat1;
//								vectPointData.push_back(curPointData);
//								vectPointData.push_back(curPointData1);
//								vector<ProSelection> UdfInputSel;
//								for (size_t k = 0; k < vectPointData.size(); k++)
//								{
//									//Get Point Feature Selection
//									vector<ProGeomitem> vecGeomItems;
//									GetGeomItems(vectPointData[k].lMdlAssm, vectPointData[k].lPointFeat, vecGeomItems);
//									ProSelection pointSelection;
//									status = ProSelectionAlloc(&vectPointData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//									UdfInputSel.push_back(pointSelection);
//								}
//
//								string Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\12-12-2023\\fo_proper_asm\\UDF\\curve.gph.1";
//
//								ProFeature UDFGrpFeat;
//
//								if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//								{
//									//VecAxisUdfs.push_back(UDFGrpFeat);
//									ProFeature* feats = NULL;
//									status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//									int FeatSize;
//									status = ProArraySizeGet(feats, &FeatSize);
//
//								}
//								vector<ProSelection> UdfInputSel1;
//								ProFeature curve;
//								ProFeature curve1;
//								ProFeature frontPlane;
//								vector<ProGeomitem> vecPlaneGeomItems;
//								vector<ProGeomitem> vecCurveGeomItems;
//
//								string childComp1 = "PRINTED_ELECTRONICS";
//								VesModelTree::AsmTreeData  item1;
//								bool res = mdlObj.GetAsmCompTreeData(childComp1, item1);
//								ProGroup group;
//
//								GetFeatureByName(CurMdl1, "ATOP", frontPlane);
//								GetFeatureByName(CurMdl1, "CURVE", curve);
//								status = ProFeatureGroupGet(&curve, &group);
//								vector<ProFeature> vecFeat;
//								GetGroupFeatures(group, vecFeat);
//								status = ProFeatureInit((ProSolid)CurMdl1, vecFeat[0].id, &curve);
//								GetGeomItems(CurMdl1, frontPlane, vecPlaneGeomItems);
//								GetGeomItems(CurMdl1, curve, vecCurveGeomItems);
//
//
//								ProSelection SurfSelection1;
//								ProSelection SurfSelection2;
//								ProSelection CurveSelection;
//								status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecPlaneGeomItems[0], &SurfSelection2);
//								status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecCurveGeomItems[0], &CurveSelection);
//
//								ProSurface lSurface;
//								ProGeomitemdata* geomData;
//								ProGeomitem comp_datum;
//								status = ProSurfaceInit(item1.lMdlAssm, 41245, &lSurface);
//								status = ProSurfaceToGeomitem((ProSolid)item1.lMdlAssm, lSurface, &comp_datum);
//								status = ProSelectionAlloc(&item1.lAsmPath, &comp_datum, &SurfSelection1);
//
//								UdfInputSel1.push_back(SurfSelection1);
//								UdfInputSel1.push_back(CurveSelection);
//								UdfInputSel1.push_back(SurfSelection2);
//
//								string Udfpath1 = "D:\\Project Details\\crowdplat\\Updated model\\12-12-2023\\fo_proper_asm\\UDF\\proj.gph.1";
//								ProFeature UDFGrpFeat1;
//
//								if (LoadUDF(CurMdl1, Udfpath1, UdfInputSel1, UDFGrpFeat1, NULL, PRO_B_TRUE))
//								{
//									//VecAxisUdfs.push_back(UDFGrpFeat);
//									ProFeature* feats = NULL;
//									status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//									int FeatSize;
//									status = ProArraySizeGet(feats, &FeatSize);
//
//								}
//								status = ProModelitemHide(&vecFeat[0]);
//								break;
//
//							}
//							else if (itemparamval == itemparamval1 && (plateTreeVec_item1[i].lAsmName == "PIN_2" && plateTreeVec_item1[j].lAsmName == "COMPONENT_2"))
//							{
//								ProFeature csoFeat;
//								ProFeature csoFeat1;
//								PointData curCsysPointData = {};
//								curCsysPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//								curCsysPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//								GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, "CS0", csoFeat);
//								curCsysPointData.lPointFeat = csoFeat;
//
//								PointData curCsysPointData1 = {};
//								curCsysPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//								curCsysPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//								GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, "CS0", csoFeat1);
//								curCsysPointData1.lPointFeat = csoFeat1;
//
//								vectCsysData.push_back(curCsysPointData);
//								vectCsysData.push_back(curCsysPointData1);
//
//								vector<ProSelection> UdfInputSel;
//								for (size_t k = 0; k < vectCsysData.size(); k++)
//								{
//									//Get Point Feature Selection
//									vector<ProGeomitem> vecGeomItems;
//									GetGeomItems(vectCsysData[k].lMdlAssm, vectCsysData[k].lPointFeat, vecGeomItems);
//									ProSelection pointSelection;
//									status = ProSelectionAlloc(&vectCsysData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//									UdfInputSel.push_back(pointSelection);
//								}
//
//								string Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\12-12-2023\\fo_proper_asm\\UDF\\ob_bend_curve.gph.1";
//
//								ProFeature UDFGrpFeat;
//
//								if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//								{
//									//VecAxisUdfs.push_back(UDFGrpFeat);
//									ProFeature* feats = NULL;
//									status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//									int FeatSize;
//									status = ProArraySizeGet(feats, &FeatSize);
//
//								}
//								vector<ProSelection> UdfInputSel1;
//								ProFeature curve;
//								ProFeature frontPlane;
//								vector<ProGeomitem> vecPlaneGeomItems;
//								vector<ProGeomitem> vecCurveGeomItems;
//
//								string childComp1 = "PRINTED_ELECTRONICS";
//								VesModelTree::AsmTreeData  item1;
//								bool res = mdlObj.GetAsmCompTreeData(childComp1, item1);
//								ProGroup group;
//
//								GetFeatureByName(CurMdl1, "ATOP", frontPlane);
//								GetFeatureByName(CurMdl1, "OB_BEND_CURVE", curve);
//								status = ProFeatureGroupGet(&curve, &group);
//								vector<ProFeature> vecFeat;
//								GetGroupFeatures(group, vecFeat);
//								status = ProFeatureInit((ProSolid)CurMdl1, vecFeat[0].id, &curve);
//								GetGeomItems(CurMdl1, frontPlane, vecPlaneGeomItems);
//								GetGeomItems(CurMdl1, curve, vecCurveGeomItems);
//
//
//								ProSelection SurfSelection1;
//								ProSelection SurfSelection2;
//								ProSelection CurveSelection;
//								status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecPlaneGeomItems[0], &SurfSelection2);
//								status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecCurveGeomItems[0], &CurveSelection);
//
//								ProSurface lSurface;
//								ProGeomitemdata* geomData;
//								ProGeomitem comp_datum;
//								status = ProSurfaceInit(item1.lMdlAssm, 41245, &lSurface);
//								status = ProSurfaceToGeomitem((ProSolid)item1.lMdlAssm, lSurface, &comp_datum);
//								status = ProSelectionAlloc(&item1.lAsmPath, &comp_datum, &SurfSelection1);
//
//								UdfInputSel1.push_back(SurfSelection1);
//								UdfInputSel1.push_back(CurveSelection);
//								UdfInputSel1.push_back(SurfSelection2);
//
//								string Udfpath1 = "D:\\Project Details\\crowdplat\\Updated model\\12-12-2023\\fo_proper_asm\\UDF\\proj.gph.1";
//								ProFeature UDFGrpFeat1;
//
//								if (LoadUDF(CurMdl1, Udfpath1, UdfInputSel1, UDFGrpFeat1, NULL, PRO_B_TRUE))
//								{
//									//VecAxisUdfs.push_back(UDFGrpFeat);
//									ProFeature* feats = NULL;
//									status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//									int FeatSize;
//									status = ProArraySizeGet(feats, &FeatSize);
//
//								}
//								status = ProModelitemHide(&vecFeat[0]);
//								break;
//
//							}
//
//							else if (itemparamval == itemparamval1 && (plateTreeVec_item1[i].lAsmName == "PIN_3" && plateTreeVec_item1[j].lAsmName == "COMPONENT_4"))
//							{
//								ProFeature csoFeat;
//								ProFeature csoFeat1;
//								PointData curCsysPointData = {};
//								curCsysPointData.lAsmPath = plateTreeVec_item1[j].lAsmPath;
//								curCsysPointData.lMdlAssm = plateTreeVec_item1[j].lMdlAssm;
//								GetFeatureByName(plateTreeVec_item1[j].lMdlAssm, "CS0", csoFeat);
//								curCsysPointData.lPointFeat = csoFeat;
//
//								PointData curCsysPointData1 = {};
//								curCsysPointData1.lAsmPath = plateTreeVec_item1[i].lAsmPath;
//								curCsysPointData1.lMdlAssm = plateTreeVec_item1[i].lMdlAssm;
//								GetFeatureByName(plateTreeVec_item1[i].lMdlAssm, "CS0", csoFeat1);
//								curCsysPointData1.lPointFeat = csoFeat1;
//
//								vectCsysData1.push_back(curCsysPointData);
//								vectCsysData1.push_back(curCsysPointData1);
//
//								vector<ProSelection> UdfInputSel;
//								for (size_t k = 0; k < vectCsysData1.size(); k++)
//								{
//									//Get Point Feature Selection
//									vector<ProGeomitem> vecGeomItems;
//									GetGeomItems(vectCsysData1[k].lMdlAssm, vectCsysData1[k].lPointFeat, vecGeomItems);
//									ProSelection pointSelection;
//									status = ProSelectionAlloc(&vectCsysData1[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//									UdfInputSel.push_back(pointSelection);
//								}
//
//								string Udfpath = "D:\\Project Details\\crowdplat\\Updated model\\12-12-2023\\fo_proper_asm\\UDF\\ob_bend_curve_l.gph.1";
//
//								ProFeature UDFGrpFeat;
//
//								if (LoadUDF(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//								{
//									//VecAxisUdfs.push_back(UDFGrpFeat);
//									ProFeature* feats = NULL;
//									status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//									int FeatSize;
//									status = ProArraySizeGet(feats, &FeatSize);
//
//								}
//
//								vector<ProSelection> UdfInputSel1;
//								ProFeature curve;
//								ProFeature frontPlane;
//								vector<ProGeomitem> vecPlaneGeomItems;
//								vector<ProGeomitem> vecCurveGeomItems;
//
//								string childComp1 = "PRINTED_ELECTRONICS";
//								VesModelTree::AsmTreeData  item1;
//								bool res = mdlObj.GetAsmCompTreeData(childComp1, item1);
//								ProGroup group;
//
//								GetFeatureByName(CurMdl1, "ATOP", frontPlane);
//								GetFeatureByName(CurMdl1, "OB_BEND_CURVE_L", curve);
//								status = ProFeatureGroupGet(&curve, &group);
//								vector<ProFeature> vecFeat;
//								GetGroupFeatures(group, vecFeat);
//								status = ProFeatureInit((ProSolid)CurMdl1, vecFeat[0].id, &curve);
//								GetGeomItems(CurMdl1, frontPlane, vecPlaneGeomItems);
//								GetGeomItems(CurMdl1, curve, vecCurveGeomItems);
//
//
//								ProSelection SurfSelection1;
//								ProSelection SurfSelection2;
//								ProSelection CurveSelection;
//								status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecPlaneGeomItems[0], &SurfSelection2);
//								status = ProSelectionAlloc(&comp_path, (ProGeomitem*)&vecCurveGeomItems[0], &CurveSelection);
//
//								ProSurface lSurface;
//								ProGeomitemdata* geomData;
//								ProGeomitem comp_datum;
//								status = ProSurfaceInit(item1.lMdlAssm, 41245, &lSurface);
//								status = ProSurfaceToGeomitem((ProSolid)item1.lMdlAssm, lSurface, &comp_datum);
//								status = ProSelectionAlloc(&item1.lAsmPath, &comp_datum, &SurfSelection1);
//
//								UdfInputSel1.push_back(SurfSelection1);
//								UdfInputSel1.push_back(CurveSelection);
//								UdfInputSel1.push_back(SurfSelection2);
//
//								string Udfpath1 = "D:\\Project Details\\crowdplat\\Updated model\\12-12-2023\\fo_proper_asm\\UDF\\proj.gph.1";
//								ProFeature UDFGrpFeat1;
//
//								if (LoadUDF(CurMdl1, Udfpath1, UdfInputSel1, UDFGrpFeat1, NULL, PRO_B_TRUE))
//								{
//									//VecAxisUdfs.push_back(UDFGrpFeat);
//									ProFeature* feats = NULL;
//									status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//									int FeatSize;
//									status = ProArraySizeGet(feats, &FeatSize);
//
//								}
//								status = ProModelitemHide(&vecFeat[0]);
//								break;
//
//							}
//						}
//
//					}
//				}
//			}
//			/*if (VesUtility::_String::StringStartsWith(itemparamval, ITEMName)) {
//				platepartname = currchildComp.lAsmName;
//				break;*/
//				//}
//
//		}
//	}
//	//}
//
//	//GetCompFeatureByName(CurMdl1, /*"INTL_MAST_SEC_BTM_SKEL"*/"PRINTED_ELECTRONICS", vecFeat);
//	//ProMdl compMdl;
//	//status = ProAsmcompMdlGet((ProAsmcomp*)&vecFeat[0], &compMdl);
//	////CreateNewWindow(compMdl);
//	////status = ProSolidRegenerate((ProAssembly)compMdl, PRO_REGEN_NO_FLAGS);
//
//	///*ProMdl activeModel;
//	//ProName activeModelName;
//	//status = ProMdlCurrentGet(&activeModel);
//	//status = ProMdlNameGet(activeModel, activeModelName);*/
//
//	//ProAsmcomppath comp_path;
//	//ProIdTable c_id_table;
//	//c_id_table[0] = -1;
//	//status = ProAsmcomppathInit((ProSolid)compMdl, c_id_table, 0, &comp_path);
//	//PointData curPointData = {};
//	//curPointData.lAsmPath = comp_path;
//	//curPointData.lMdlAssm = compMdl;
//	//ProFeature outFeat;
//	//ProFeature outFeat1;
//	//status = ProFeatureInit((ProSolid)compMdl, 41013, &outFeat);
//	//status = ProFeatureInit((ProSolid)compMdl, 41015, &outFeat1);
//	//curPointData.lPointFeat = outFeat;
//	//vectPointData.push_back(curPointData);
//	//curPointData.lPointFeat = outFeat1;
//	//vectPointData.push_back(curPointData);
//
//	//vector<ProSelection> UdfInputSel;
//	//for (size_t k = 0; k < vectPointData.size(); k++)
//	//{
//	//	//Get Point Feature Selection
//	//	vector<ProGeomitem> vecGeomItems;
//	//	GetGeomItems(vectPointData[k].lMdlAssm, vectPointData[k].lPointFeat, vecGeomItems);
//	//	ProSelection pointSelection;
//	//	status = ProSelectionAlloc(&vectPointData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//	//	UdfInputSel.push_back(pointSelection);
//	//}
//
//	
//
//	ProFeature UDFGrpFeat;
//
//	//if (LoadUDF(compMdl, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_FALSE))
//	//{
//	//	//VecAxisUdfs.push_back(UDFGrpFeat);
//	//	ProFeature* feats = NULL;
//	//	status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//	//	int FeatSize;
//	//	status = ProArraySizeGet(feats, &FeatSize);
//
//	//}
//	//vector<ProSelection> UdfInputSel1;
//	//ProFeature curve;
//	//ProFeature frontPlane;
//	//vector<ProGeomitem> vecPlaneGeomItems;
//	//vector<ProGeomitem> vecCurveGeomItems;
//
//	//GetFeatureByName(compMdl, "FRONT", frontPlane);
//	//status = ProFeatureInit((ProSolid)compMdl, 41030, &curve);
//	//GetGeomItems(compMdl, frontPlane, vecPlaneGeomItems);
//	//GetGeomItems(compMdl, curve, vecCurveGeomItems);
//
//
//	//ProSelection SurfSelection1;
//	//ProSelection SurfSelection2;
//	//ProSelection CurveSelection;
//	//status = ProSelectionAlloc(&vectPointData[0].lAsmPath, (ProGeomitem*)&vecPlaneGeomItems[0], &SurfSelection2);
//	//status = ProSelectionAlloc(&vectPointData[0].lAsmPath, (ProGeomitem*)&vecCurveGeomItems[0], &CurveSelection);
//
//	//ProSurface lSurface;
//	//ProGeomitemdata* geomData;
//	//ProGeomitem comp_datum;
//	//status = ProSurfaceInit(compMdl, 1588, &lSurface);
//	//status = ProSurfaceToGeomitem((ProSolid)compMdl, lSurface, &comp_datum);
//	//status = ProSelectionAlloc(&vectPointData[0].lAsmPath, &comp_datum, &SurfSelection1);
//	//
//	//UdfInputSel1.push_back(SurfSelection1);
//	//UdfInputSel1.push_back(CurveSelection);
//	//UdfInputSel1.push_back(SurfSelection2);
//
//	//string Udfpath1 = "C:\\Users\\MUSRI\\Downloads\\FO_curve_proj_27Nov23\\FO_curve_proj_27Nov23\\proj.gph.1";
//	//ProFeature UDFGrpFeat1;
//
//	//if (LoadUDF(compMdl, Udfpath1, UdfInputSel1, UDFGrpFeat1, NULL, PRO_B_FALSE))
//	//{
//	//	//VecAxisUdfs.push_back(UDFGrpFeat);
//	//	ProFeature* feats = NULL;
//	//	status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//	//	int FeatSize;
//	//	status = ProArraySizeGet(feats, &FeatSize);
//
//	//}
//	//status = ProModelitemHide(&curve);
//
//}

//void FanOutCreation()
//{
//	ProError status;
//	ProMdl CurMdl1;
//	vector<PointData> vectPointData;
//	status = ProMdlCurrentGet(&CurMdl1);
//	vector<ProFeature> vecFeat;
//
//	GetCompFeatureByName(CurMdl1, /*"INTL_MAST_SEC_BTM_SKEL"*/"PRINTED_ELECTRONICS", vecFeat);
//	ProMdl compMdl;
//	status = ProAsmcompMdlGet((ProAsmcomp*)&vecFeat[0], &compMdl);
//	//CreateNewWindow(compMdl);
//	//status = ProSolidRegenerate((ProAssembly)compMdl, PRO_REGEN_NO_FLAGS);
//
//	/*ProMdl activeModel;
//	ProName activeModelName;
//	status = ProMdlCurrentGet(&activeModel);
//	status = ProMdlNameGet(activeModel, activeModelName);*/
//
//	ProAsmcomppath comp_path;
//	ProIdTable c_id_table;
//	c_id_table[0] = -1;
//	status = ProAsmcomppathInit((ProSolid)compMdl, c_id_table, 0, &comp_path);
//	PointData curPointData = {};
//	curPointData.lAsmPath = comp_path;
//	curPointData.lMdlAssm = compMdl;
//	ProFeature outFeat;
//	ProFeature outFeat1;
//	status = ProFeatureInit((ProSolid)compMdl, 41013, &outFeat);
//	status = ProFeatureInit((ProSolid)compMdl, 41015, &outFeat1);
//	curPointData.lPointFeat = outFeat;
//	vectPointData.push_back(curPointData);
//	curPointData.lPointFeat = outFeat1;
//	vectPointData.push_back(curPointData);
//
//	vector<ProSelection> UdfInputSel;
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(vectPointData[k].lMdlAssm, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&vectPointData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	string Udfpath = "D:\\Project Details\\crowdplat\\Model for minimum trace route\\Curved UDF\\curve.gph.1";
//
//	ProFeature UDFGrpFeat;
//
//	if (LoadUDF(compMdl, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_FALSE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	vector<ProSelection> UdfInputSel1;
//	ProFeature curve;
//	ProFeature frontPlane;
//	vector<ProGeomitem> vecPlaneGeomItems;
//	vector<ProGeomitem> vecCurveGeomItems;
//
//	GetFeatureByName(compMdl, "FRONT", frontPlane);
//	status = ProFeatureInit((ProSolid)compMdl, 41030, &curve);
//	GetGeomItems(compMdl, frontPlane, vecPlaneGeomItems);
//	GetGeomItems(compMdl, curve, vecCurveGeomItems);
//
//
//	ProSelection SurfSelection1;
//	ProSelection SurfSelection2;
//	ProSelection CurveSelection;
//	status = ProSelectionAlloc(&vectPointData[0].lAsmPath, (ProGeomitem*)&vecPlaneGeomItems[0], &SurfSelection2);
//	status = ProSelectionAlloc(&vectPointData[0].lAsmPath, (ProGeomitem*)&vecCurveGeomItems[0], &CurveSelection);
//
//	ProSurface lSurface;
//	ProGeomitemdata* geomData;
//	ProGeomitem comp_datum;
//	status = ProSurfaceInit(compMdl, 1588, &lSurface);
//	status = ProSurfaceToGeomitem((ProSolid)compMdl, lSurface, &comp_datum);
//	status = ProSelectionAlloc(&vectPointData[0].lAsmPath, &comp_datum, &SurfSelection1);
//
//	UdfInputSel1.push_back(SurfSelection1);
//	UdfInputSel1.push_back(CurveSelection);
//	UdfInputSel1.push_back(SurfSelection2);
//
//	string Udfpath1 = "D:\\Project Details\\crowdplat\\Model for minimum trace route\\Curved UDF\\proj.gph.1";
//	ProFeature UDFGrpFeat1;
//
//	if (LoadUDF(compMdl, Udfpath1, UdfInputSel1, UDFGrpFeat1, NULL, PRO_B_FALSE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//	status = ProModelitemHide(&curve);
//
//}
//void FanOutCreation()
//{
//
//	ProError status;
//	ProSelection* sels;
//	int nSels = -1;
//	status = ProSelect((char*)"part", 2, NULL, NULL, NULL, NULL, &sels, &nSels);
//	//string Pointname;
//
//	vector<ProFeature> vecpointFeatures;
//	vector< PointData> vectPointData;
//	for (size_t selcnt = 0; selcnt < nSels; selcnt++)
//	{
//		ProFeature feature;
//		status = ProSelectionModelitemGet(sels[selcnt], &feature);
//		ProAsmcomppath MdlCompPath;
//		status = ProSelectionAsmcomppathGet(sels[selcnt], &MdlCompPath);
//		ProMdl CurselectionMdl;
//		status = ProModelitemMdlGet(&feature, &CurselectionMdl);
//		PointData curPointData = {};
//		curPointData.lAsmPath = MdlCompPath;
//		curPointData.lMdlAssm = CurselectionMdl;
//		ProMdl CapMdl;
//		status = ProModelitemMdlGet(&feature, &CapMdl);
//
//		ProName mdlName;
//
//		status = ProMdlNameGet(CapMdl, mdlName);
//
//		ProCharName CharMdlname;
//		ProWstringToString(CharMdlname, mdlName);
//		string strMdlName(CharMdlname);
//		if (feature.type == PRO_PART)
//		{
//
//
//			ProFeature outFeat;
//			if (strMdlName == "CAPACITOR_1" || strMdlName == "ANTENA_1")
//			{
//				GetFeatureByName(CapMdl, "PNT0", outFeat);
//				ProFeattype type;
//				status = ProFeatureTypeGet(&outFeat, &type);
//
//				if (status == PRO_TK_NO_ERROR && type == PRO_FEAT_DATUM_POINT)
//				{
//					curPointData.lPointFeat = outFeat;
//				}
//
//			}
//			else
//			{
//				ProName lCsysDefName1;
//				string sFeatName = "";
//
//				if (strMdlName == "COMP_CAPACITOR")
//					sFeatName = "PNT_CAPACITOR";
//				else if (strMdlName == "COMP_ANTENNA")
//					sFeatName = "PNT_ANTENNA";
//				//status = ProModelitemByNameInit(CapMdl, PRO_CSYS, lCsysDefName, &comp_datum);
//				GetFeatureByName(CapMdl, sFeatName, outFeat);
//				ProFeattype type;
//				status = ProFeatureTypeGet(&outFeat, &type);
//
//				if (status == PRO_TK_NO_ERROR && type == PRO_FEAT_DATUM_POINT)
//				{
//					curPointData.lPointFeat = outFeat;
//				}
//
//			}
//
//
//			vectPointData.push_back(curPointData);
//
//		}
//	}
//
//
//	vector<ProSelection> UdfInputSel;
//	for (size_t k = 0; k < vectPointData.size(); k++)
//	{
//		//Get Point Feature Selection
//		vector<ProGeomitem> vecGeomItems;
//		GetGeomItems(vectPointData[k].lMdlAssm, vectPointData[k].lPointFeat, vecGeomItems);
//		ProSelection pointSelection;
//		status = ProSelectionAlloc(&vectPointData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
//		UdfInputSel.push_back(pointSelection);
//	}
//
//	string Udfpath = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\curve.gph.1";
//	ProMdl CurMdl;
//	status = ProMdlCurrentGet(&CurMdl);
//	ProFeature UDFGrpFeat;
//
//	if (LoadUDF(CurMdl, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
//	{
//		//VecAxisUdfs.push_back(UDFGrpFeat);
//		ProFeature* feats = NULL;
//		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
//		int FeatSize;
//		status = ProArraySizeGet(feats, &FeatSize);
//
//	}
//
//}

ProError UserAssembleByDatums(ProAssembly asm_model, ProSolid comp_model, string TemplateName, ProAsmcomp& asmcomp)
{
	ProError status;
	ProName asm_datums;
	ProName comp_datums;
	ProMatrix identity_matrix = { { 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 } };

	ProAsmcompconstraint* constraints;
	ProAsmcompconstraint constraint;
	int i;
	ProBoolean interact_flag = PRO_B_FALSE;
	ProModelitem asm_datum, comp_datum;
	ProSelection asm_sel, comp_sel;
	ProAsmcomppath comp_path;
	ProIdTable c_id_table;
	c_id_table[0] = -1;
	ProSolid Skt_Part;
	//c_id_table[1] = -1;
	//ProStringToWstring(asm_datums, /*"DEFAULT"*/(char*)AsmCsysName.c_str());
	//ProStringToWstring(comp_datums, /*"DEFAULT"*/(char*)CompSysName.c_str());


	status = ProAsmcompAssemble(asm_model, comp_model, identity_matrix, &asmcomp);
	status = ProArrayAlloc(0, sizeof(ProAsmcompconstraint), 1, (ProArray*)&constraints);

	/*-----------------------------------------------------------------*\
	Find the assembly datum
	\*-----------------------------------------------------------------*/
	//asm_model - Root Assembly

	//Skt Part ?
	//ProSolidVisit FRom asm_model get the SktPart (Sub Part)
	/*ProMdl aModel;
	status = ProMdlCurrentGet(&aModel);
	if (status != PRO_TK_NO_ERROR)
		return status;*/

	std::vector<ProAsmcomppath > lVecSolid;
	vector<ProMdl> mdls;

	g_reqComp.clear();
	status = ProSolidDispCompVisit((ProSolid)asm_model, Collect_SubLevelComponents, NULL, &lVecSolid);

	vector<ProFeature> lCsysProfeatureVec1;
	SolidCsysFeatVisit(asm_model, lCsysProfeatureVec1);
	ProName lCsysDefName1;
	if (TemplateName == "antenna")
	{
		string csysName = "ANTENNA";
		ProStringToWstring(lCsysDefName1, (char*)(csysName.c_str()));
	}
	else if (TemplateName == "capacitor")
	{
		string csysName1 = "CAPACITOR";
		ProStringToWstring(lCsysDefName1, (char*)(csysName1.c_str()));
	}
	status = ProModelitemByNameInit(asm_model, PRO_CSYS, lCsysDefName1, &asm_datum);

	//default csys is not present take the first csys in pdm model
	vector<ProFeature> lCsysProfeatureVec;
	SolidCsysFeatVisit(comp_model, lCsysProfeatureVec);
	if (0 == lCsysProfeatureVec.size())
	{
		//WriteToErrLog("Component CSYS have no CSYS to constraint.");
		return PRO_TK_GENERAL_ERROR;
	}
	//comp_datum = (ProModelitem)lCsysProfeatureVec[0]; //Assuming First Csys is the Zeroth one

	ProName lCsysDefName;
	status = ProModelitemNameGet((ProModelitem*)&lCsysProfeatureVec[1], lCsysDefName);
	status = ProModelitemByNameInit(comp_model, PRO_CSYS, lCsysDefName, &comp_datum);
	//}

	//	status = ProAsmcomppathInit(asm_model, c_id_table, 0, &comp_path);
		/*-----------------------------------------------------------------*\
		Allocate the references
		\*-----------------------------------------------------------------*/
		//status = ProSelectionAlloc(&comp_path, &asm_datum, &asm_sel);
	status = ProSelectionAlloc(&g_reqComp[0].lAsmPath, &asm_datum, &asm_sel);

	status = ProSelectionAlloc(NULL, &comp_datum, &comp_sel);
	/*-----------------------------------------------------------------*\
	Allocate and fill the constraint.
	\*-----------------------------------------------------------------*/
	status = ProAsmcompconstraintAlloc(&constraint);
	status = ProAsmcompconstraintTypeSet(constraint, PRO_ASM_CSYS);
	status = ProAsmcompconstraintAsmreferenceSet(constraint, asm_sel, PRO_DATUM_SIDE_YELLOW);
	status = ProAsmcompconstraintCompreferenceSet(constraint, comp_sel, PRO_DATUM_SIDE_YELLOW);
	status = ProArrayObjectAdd((ProArray*)&constraints, -1, 1, &constraint);

	status = ProAsmcompConstraintsSet(NULL, &asmcomp, constraints);
	//ProSolidRegenerate((ProSolid)asmcomp.owner, PRO_REGEN_NO_RESOLVE_MODE);
	ProArrayFree((ProArray*)&constraints);
	return (PRO_TK_NO_ERROR);
}
void WindowFit()
{
	int currentWindowID;
	ProError status = ProWindowCurrentGet(&currentWindowID);
	status = ProWindowRefit(currentWindowID);
}
void closeMYdialog(char* dialog, char* component, ProAppData appdata)
{
	ProError status;

	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);
	status = ProUIDialogDestroy(dialog);
}

void executeOKAction(char* dialog, char* component, ProAppData appdata)
{
	wchar_t* wview_Name;

}
ProError UserAssembleByDatumsCsys(ProAssembly asm_model, ProSolid comp_model, string TemplateName, ProAsmcomp& asmcomp, ProFeature AsmCsysfeature)
{
	ProError status;
	ProName asm_datums;
	ProName comp_datums;
	ProMatrix identity_matrix = { { 1.0, 0.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 0.0, 1.0 } };

	ProAsmcompconstraint* constraints;
	ProAsmcompconstraint constraint;
	int i;
	ProBoolean interact_flag = PRO_B_FALSE;
	ProModelitem asm_datum, comp_datum;
	ProSelection asm_sel, comp_sel;
	ProAsmcomppath comp_path;
	ProIdTable c_id_table;
	c_id_table[0] = -1;
	ProSolid Skt_Part;
	//c_id_table[1] = -1;
	//ProStringToWstring(asm_datums, /*"DEFAULT"*/(char*)AsmCsysName.c_str());
	//ProStringToWstring(comp_datums, /*"DEFAULT"*/(char*)CompSysName.c_str());


	status = ProAsmcompAssemble(asm_model, comp_model, identity_matrix, &asmcomp);
	status = ProArrayAlloc(0, sizeof(ProAsmcompconstraint), 1, (ProArray*)&constraints);

	/*-----------------------------------------------------------------*\
	Find the assembly datum
	\*-----------------------------------------------------------------*/
	//asm_model - Root Assembly

	//Skt Part ?
	//ProSolidVisit FRom asm_model get the SktPart (Sub Part)
	/*ProMdl aModel;
	status = ProMdlCurrentGet(&aModel);
	if (status != PRO_TK_NO_ERROR)
		return status;*/

	std::vector<ProAsmcomppath > lVecSolid;
	vector<ProMdl> mdls;

	g_reqComp.clear();
	status = ProSolidDispCompVisit((ProSolid)asm_model, Collect_SubLevelComponents, NULL, &lVecSolid);

	vector<ProFeature> lCsysProfeatureVec1;
	SolidCsysFeatVisit(asm_model, lCsysProfeatureVec1);
	ProName lCsysDefName1;
	/*if (TemplateName == "antenna")
	{
		status = ProModelitemNameGet((ProModelitem*)&lCsysProfeatureVec1[2], lCsysDefName1);

	}
	else if (TemplateName == "capacitor")
	{
		status = ProModelitemNameGet((ProModelitem*)&lCsysProfeatureVec1[1], lCsysDefName1);

	}*/
	status = ProModelitemNameGet((ProModelitem*)&AsmCsysfeature, lCsysDefName1);



	status = ProModelitemByNameInit(asm_model, PRO_CSYS, lCsysDefName1, &asm_datum);
	//status = ProModelitemByNameInit(/*g_reqComp[0].lMdlAssm*/asm_model, PRO_CSYS, asm_datums, &asm_datum);

	//status = ProModelitemByNameInit(asm_model, PRO_CSYS, asm_datums, &asm_datum);
	//status = ProModelitemByNameInit(comp_model, PRO_CSYS, comp_datums, &comp_datum);

	//if (PRO_TK_NO_ERROR != status)
	//{
		//default csys is not present take the first csys in pdm model
	vector<ProFeature> lCsysProfeatureVec;
	SolidCsysFeatVisit(comp_model, lCsysProfeatureVec);
	if (0 == lCsysProfeatureVec.size())
	{
		//WriteToErrLog("Component CSYS have no CSYS to constraint.");
		return PRO_TK_GENERAL_ERROR;
	}
	//comp_datum = (ProModelitem)lCsysProfeatureVec[0]; //Assuming First Csys is the Zeroth one

	ProName lCsysDefName;
	status = ProModelitemNameGet((ProModelitem*)&lCsysProfeatureVec[1], lCsysDefName);
	status = ProModelitemByNameInit(comp_model, PRO_CSYS, lCsysDefName, &comp_datum);
	//}

	//	status = ProAsmcomppathInit(asm_model, c_id_table, 0, &comp_path);
		/*-----------------------------------------------------------------*\
		Allocate the references
		\*-----------------------------------------------------------------*/
		//status = ProSelectionAlloc(&comp_path, &asm_datum, &asm_sel);
	status = ProSelectionAlloc(&g_reqComp[0].lAsmPath, &asm_datum, &asm_sel);

	status = ProSelectionAlloc(NULL, &comp_datum, &comp_sel);
	/*-----------------------------------------------------------------*\
	Allocate and fill the constraint.
	\*-----------------------------------------------------------------*/
	status = ProAsmcompconstraintAlloc(&constraint);
	status = ProAsmcompconstraintTypeSet(constraint, PRO_ASM_CSYS);
	status = ProAsmcompconstraintAsmreferenceSet(constraint, asm_sel, PRO_DATUM_SIDE_YELLOW);
	status = ProAsmcompconstraintCompreferenceSet(constraint, comp_sel, PRO_DATUM_SIDE_YELLOW);
	status = ProArrayObjectAdd((ProArray*)&constraints, -1, 1, &constraint);

	status = ProAsmcompConstraintsSet(NULL, &asmcomp, constraints);
	//ProSolidRegenerate((ProSolid)asmcomp.owner, PRO_REGEN_NO_RESOLVE_MODE);
	ProArrayFree((ProArray*)&constraints);
	return (PRO_TK_NO_ERROR);
}
void AssembleModel(string modelPath, /*string mdlName,*/string mdlName, ProFeature csysFeature)
{
	ProError lErrCode;
	ProMdl p_new_handle;
	ProAsmcomp lAsmComp_Child;
	wchar_t wName[100];
	ProMdl currModel;
	lErrCode = ProMdlCurrentGet(&currModel);
	ProStringToWstring(wName, (char*)(modelPath.c_str()));
	lErrCode = ProMdlFiletypeLoad(wName, PRO_MDLFILE_UNUSED, PRO_B_FALSE, &p_new_handle);
	if (mdlName == "capacitor")
		capacitor_model = p_new_handle;
	else if (mdlName == "antenna")
		Antenna_model = p_new_handle;
	UserAssembleByDatumsCsys(ProAssembly(currModel), ProSolid(p_new_handle), mdlName, lAsmComp_Child, csysFeature);
	//ProSolidRegenerate(ProAssembly(asmMaster), PRO_REGEN_NO_RESOLVE_MODE);
	WindowFit();
	lErrCode = ProWindowActivate(iWindId);
}

bool SetParamValue(IN ProMdl Mdl, IN string ParamName, IN double ParamVal)
{
	ProError lErr = PRO_TK_NO_ERROR;
	if (ParamVal == NAN)
		return false;

	ProParameter lParam;

	ProModelitem lModelItem;
	lErr = ProMdlToModelitem(ProMdlToSolid(Mdl), &lModelItem);
	ProName lParameterName;
	stringToWString(ParamName, lParameterName);

	lErr = ProParameterInit(&lModelItem, lParameterName, &lParam);
	ProParamvalue lValue;
	lValue.type = PRO_PARAM_DOUBLE;
	lValue.value.d_val = ParamVal;

	//lErr = ProParameterValueSet(&lParam, &lValue);  //" "
	lErr = ProParameterValueWithUnitsSet(&lParam, &lValue, NULL);  //" " " "
	if (lErr == PRO_TK_NO_ERROR)
	{
		lErr = ProSolidRegenerate((ProSolid)Mdl, PRO_REGEN_NO_FLAGS);
		return true;
	}
	else
		return false;
}
void executeAssembleAction(char* dialog, char* component, ProAppData appdata)
{
	wchar_t* wview_Name;
	//ProError status;

	////status = ProUIInputpanelValueGet(dialog, capacitorRadio, &wview_Name);
	//int n_name;
	//char** names;
	//status = ProUIRadiogroupSelectednamesGet(dialog, capacitorRadio, &n_name, &names);
	//if (status == PRO_TK_NO_ERROR)
	//{
	//	string strName(names[0]);
	//	if (strName == "capacitor")
	//	{
	//		ProMouseButton btn;
	//		ProVector loc;
	//		if (ProMousePickGet(PRO_ANY_BUTTON, &btn, loc) == PRO_TK_NO_ERROR)
	//		{
	//			/*double x = loc[0];
	//			double y = loc[1];
	//			double z = loc[2];*/
	//			string strMdlPath = "C:\\Murali\\My Projects\\3DPrinter\\x64\\Debug\\3DPrinterModel\\capacitor_1.prt";
	//			AssembleModel(strMdlPath, strName, "capacitor");

	//		}
	//	}
	//	else if (strName == "antenna")
	//	{
	//		ProMouseButton btn1;
	//		ProVector loc1;
	//		if (ProMousePickGet(PRO_ANY_BUTTON, &btn1, loc1) == PRO_TK_NO_ERROR)
	//		{
	//			/*double x = loc[0];
	//			double y = loc[1];
	//			double z = loc[2];*/
	//			string strMdlPath1 = "C:\\Murali\\My Projects\\3DPrinter\\x64\\Debug\\3DPrinterModel\\antena_1.prt";
	//			AssembleModel(strMdlPath1, strName, "antenna");

	//		}

	//	}

	//}
}
void executeApplyCapUpdateAction(char* dialog, char* component, ProAppData appdata)
{
	wchar_t* wheight_input;
	wchar_t* wdia_input;
	ProError status;
	status = ProUIInputpanelValueGet(dialog, capacitorHeightInput, &wheight_input);
	status = ProUIInputpanelValueGet(dialog, capacitorDiaInput, &wdia_input);

	wstring ws(wheight_input);
	//your new String
	string heightstr(ws.begin(), ws.end());
	double dblHeigt = stod(heightstr);
	wstring ws1(wdia_input);
	//your new String
	string diastr(ws1.begin(), ws1.end());
	double dbldia = stod(diastr);
	int n_name;
	char** names;
	status = ProUIRadiogroupSelectednamesGet(dialogname, capacitorRadio, &n_name, &names);
	if (status == PRO_TK_NO_ERROR)
	{
		if (status == PRO_TK_NO_ERROR)
		{
			string str(names[0]);
			if (str == "capacitor")
			{
				SetParamValue(capacitor_model, "CAP_DIA", dbldia);
				SetParamValue(capacitor_model, "CAP_HT", dblHeigt);
			}
			else if (str == "antenna")
			{
				SetParamValue(Antenna_model, "ANT_DIA", dbldia);
				SetParamValue(Antenna_model, "ANT_HT", dblHeigt);
			}
			else
			{


			}
		}

	}
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);
	status = ProUIDialogDestroy(dialog);
}

void executeUpdateAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;

	wchar_t* wview_Name;
	//status = ProUIInputpanelValueGet(dialog, capacitorRadio, &wview_Name);
	int n_name;
	char** names;
	status = ProUIRadiogroupSelectednamesGet(dialog, capacitorRadio, &n_name, &names);

	if (status == PRO_TK_NO_ERROR)
	{
		string str(names[0]);

		if (str == "capacitor")
		{

			status = ProUIDialogCreate(updateDimdialogname, updateDimdialogname);
			if (status == PRO_TK_NO_ERROR)
			{
				status = ProUIDialogDialogstyleSet(updateDimdialogname, PROUIDIALOGSTYLE_MODELESS); // Setting the Dialogh style to me Modeless

				status = ProUIPushbuttonActivateActionSet(updateDimdialogname, applycapupdatebtn, executeApplyCapUpdateAction, NULL);
				status = ProUIPushbuttonActivateActionSet(updateDimdialogname, cancelcapupdatebtn, closeMYdialog, NULL);

				int stat = -1;
				status = ProUIDialogActivate(updateDimdialogname, &stat);
			}

		}
		else if (str == "antenna")
		{
			status = ProUIDialogCreate(updateDimdialogname, updateDimdialogname);
			if (status == PRO_TK_NO_ERROR)
			{
				status = ProUIDialogDialogstyleSet(updateDimdialogname, PROUIDIALOGSTYLE_MODELESS); // Setting the Dialogh style to me Modeless

				status = ProUIPushbuttonActivateActionSet(updateDimdialogname, applycapupdatebtn, executeApplyCapUpdateAction, NULL);
				status = ProUIPushbuttonActivateActionSet(updateDimdialogname, cancelcapupdatebtn, closeMYdialog, NULL);

				int stat = -1;
				status = ProUIDialogActivate(updateDimdialogname, &stat);
			}

		}
		else
		{


		}
		int w_id = -1;
		status = ProWindowCurrentGet(&w_id);
		if (status == PRO_TK_NO_ERROR)
		{
			ProMdl currMdl;
			status = ProWindowMdlGet(w_id, &currMdl);
			if (status == PRO_TK_NO_ERROR)
			{
				ProView view;
				//status = ProViewRetrieve(currMdl, wview_Name, &view);
			}
		}
	}
}
//void executeUpdateAction(char* dialog, char* component, ProAppData appdata)
//{
//	wchar_t* wview_Name;
//	ProError status;
//	status = ProUIInputpanelValueGet(dialog, capacitorRadio, &wview_Name);
//	if (status == PRO_TK_NO_ERROR)
//	{
//
//		//wchar_t* txt = L"Hello World";
//		wstring ws(wview_Name);
//		// your new String
//		string str(ws.begin(), ws.end());
//
//		if (str == "capacitor")
//		{
//			int test = 0;
//			SetParamValue(capacitor_model, "CAP_DIA", 75);
//			SetParamValue(capacitor_model, "CAP_HT", 150);
//		}
//		else if (str == "antenna")
//		{
//			SetParamValue(Antenna_model, "ANT_DIA", 75);
//			SetParamValue(Antenna_model, "ANT_HT", 150);
//			int test1 = 0;
//
//		}
//	}
//}
void GetChildrenElement(IN ProElement CurrElement, int NodeName, OUT ProElement& ChildElement)
{
	ProErr status;
	ProElempathItem csysOrientmoveMoveTypeElemPathItem[] = { {PRO_ELEM_PATH_ITEM_TYPE_ID, /*PRO_E_CSYS_ORIENTMOVE_MOVE_TYPE*/NodeName} };

	ProElempath csysOrientmoveMoveTypeElemPath;
	status = ProElempathAlloc(&csysOrientmoveMoveTypeElemPath);
	status = ProElempathDataSet(csysOrientmoveMoveTypeElemPath, csysOrientmoveMoveTypeElemPathItem, 1);

	status = ProElemtreeElementGet(CurrElement, csysOrientmoveMoveTypeElemPath, &ChildElement);

}
void GetChildrenElements(IN ProElement CurrElement, int NodeName, OUT vector<ProElement>& ChildElements)
{
	ProErr status;
	ProElempathItem ElemPathItem[] = { {PRO_ELEM_PATH_ITEM_TYPE_ID, /*PRO_E_CSYS_ORIENTMOVES*/NodeName} };

	ProElempath ElemPath;
	status = ProElempathAlloc(&ElemPath);
	status = ProElempathDataSet(ElemPath, ElemPathItem, 1);

	ProElement* Elements = NULL;
	status = ProArrayAlloc(0, sizeof(ProElement), 1, reinterpret_cast<ProArray*>(&Elements));

	status = ProElementArrayGet(CurrElement, ElemPath, &Elements);

	int nCsysOrientmovesElems = 0;
	status = ProArraySizeGet(Elements, &nCsysOrientmovesElems);
	for (int i = 0; i < nCsysOrientmovesElems; ++i)
	{
		ChildElements.push_back(Elements[i]);
	}
}
void SetDoubleValInElemtree(int PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN, ProElement csysOrientmovesElem, double XVal)
{
	ProErr status;
	ProElempathItem csysOrientmoveMoveValElemPathItem[] = { { PRO_ELEM_PATH_ITEM_TYPE_ID, PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN } };

	ProElempath csysOrientmoveMoveValElemPath;
	status = ProElempathAlloc(&csysOrientmoveMoveValElemPath);
	status = ProElempathDataSet(csysOrientmoveMoveValElemPath, csysOrientmoveMoveValElemPathItem, 1);

	ProElement csysOrientmoveMoveValElem = NULL;
	status = ProElemtreeElementGet(csysOrientmovesElem, csysOrientmoveMoveValElemPath, &csysOrientmoveMoveValElem);

	status = ProElementDoubleSet(csysOrientmoveMoveValElem, /*90.0*/XVal);
}
ProErr EditCSys(IN ProModelitem feature, IN UserCsysData CSysDataVal)
{
	ProElement featTree;
	ProErr status = ProFeatureElemtreeExtract(&feature, NULL, PRO_FEAT_EXTRACT_NO_OPTS, &featTree);
	vector<ProElement> ChildElements;
	GetChildrenElements(featTree, PRO_E_CSYS_ORIENTMOVES, ChildElements);

	int PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN = PRO_E_CSYS_ORIENTMOVE_MOVE_VAL;    double XVal, YVal, ZVal, XRVal, YRVal, ZRVal = 0.0;

	for (auto currElem : ChildElements)
	{
		//Check movement type.            //PRO_E_CSYS_ORIENTMOVE_MOVE_TYPE
		ProElement csysOrientmoveMoveTypeElem = NULL;
		GetChildrenElement(currElem, PRO_E_CSYS_ORIENTMOVE_MOVE_TYPE, csysOrientmoveMoveTypeElem);

		ProCsysOrientMoveMoveOpt moveOption;
		status = ProElementIntegerGet(csysOrientmoveMoveTypeElem, NULL, reinterpret_cast<int*>(&moveOption));
		switch (moveOption)
		{
		case PRO_CSYS_ORIENTMOVE_MOVE_OPT_TRAN_X:
			if (get<0>(CSysDataVal.GetXVal()))
				SetDoubleValInElemtree(PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN, currElem, get<1>(CSysDataVal.GetXVal()));
			break;
		case PRO_CSYS_ORIENTMOVE_MOVE_OPT_TRAN_Y:
			if (get<0>(CSysDataVal.GetYVal()))
				SetDoubleValInElemtree(PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN, currElem, get<1>(CSysDataVal.GetYVal()));
			break;
		case PRO_CSYS_ORIENTMOVE_MOVE_OPT_TRAN_Z:
			if (get<0>(CSysDataVal.GetZVal()))
				SetDoubleValInElemtree(PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN, currElem, get<1>(CSysDataVal.GetZVal()));
			break;
		case PRO_CSYS_ORIENTMOVE_MOVE_OPT_ROT_X:
			if (get<0>(CSysDataVal.GetXRVal()))
				SetDoubleValInElemtree(PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN, currElem, get<1>(CSysDataVal.GetXRVal()));
			break;
		case PRO_CSYS_ORIENTMOVE_MOVE_OPT_ROT_Y:
			if (get<0>(CSysDataVal.GetYRVal()))
				SetDoubleValInElemtree(PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN, currElem, get<1>(CSysDataVal.GetYRVal()));
			break;
		case PRO_CSYS_ORIENTMOVE_MOVE_OPT_ROT_Z:
			if (get<0>(CSysDataVal.GetZRVal()))
				SetDoubleValInElemtree(PRO_E_CSYS_ORIENTMOVE_MOVE_VAL_IN, currElem, get<1>(CSysDataVal.GetZRVal()));
			break;
		}
	}

	ProErrorlist errorList;
	ProFeatureCreateOptions* opts = NULL;
	status = ProArrayAlloc(1, sizeof(ProFeatureCreateOptions), 1, reinterpret_cast<ProArray*>(&opts));
	opts[0] = PRO_FEAT_CR_NO_OPTS;
	status = ProFeatureWithoptionsRedefine(NULL, &feature, featTree, opts, PRO_REGEN_NO_FLAGS, &errorList);

	return PRO_TK_NO_ERROR;

}

ProError GetDefaultCsysSelection(ProMdl Asmmodel, ProAsmcomppath* AsmComppath, ProSelection& DefCsysSel, string& selectedCsyName, ProFeature& DefcsysFeature, string Csysname)
{
	ProError status = PRO_TK_GENERAL_ERROR;
	//vector<ProCsys> lCsysProfeatureVec;
	//VesCSys::GetAllCsys(Asmmodel, lCsysProfeatureVec);


	vector<ProFeature> lCsysProfeatureVec1;
	SolidCsysFeatVisit(Asmmodel, lCsysProfeatureVec1);
	ProSelection DefCsysSelection;

	if (lCsysProfeatureVec1.size() != 0)
	{
		for (size_t j = 0; j < lCsysProfeatureVec1.size(); j++)
		{
			/*ProGeomitem r_geom_item;
			status = ProCsysToGeomitem((ProSolid)Asmmodel, lCsysProfeatureVec1[j], &r_geom_item);*/
			ProName CsysName;
			status = ProModelitemNameGet((ProModelitem*)&lCsysProfeatureVec1[j], CsysName);
			char* lpPathName = new char[360];
			ProWstringToString(lpPathName, CsysName);
			std::string Harnname(lpPathName);
			delete[] lpPathName;
			//if (Harnname == "DEFAULT")
			if (Harnname.find(Csysname) != std::string::npos)
			{
				/*    continue;
				}
				if (Harnname == Csysname)
				{*/
				selectedCsyName = Harnname;
				DefcsysFeature = lCsysProfeatureVec1[j];
				/*	status = ProSelectionAlloc(AsmComppath, (ProModelitem*)&lCsysProfeatureVec1[j], &DefCsysSelection);*/

				vector<ProGeomitem> vecGeomItems;

				GetGeomItems(Asmmodel, lCsysProfeatureVec1[j], vecGeomItems);
				ProCsys Csys1;
				status = ProSelectionAlloc(NULL, (ProGeomitem*)&vecGeomItems[0], &DefCsysSelection);
				DefCsysSel = DefCsysSelection;


				break;
			}


		}
	}
	return status;
}

void executeCapacitorAction(char* dialog, char* component, ProAppData appdata)
{
	ProMouseButton btn;
	ProVector loc;
	ProError status;
	if (ProMousePickGet(PRO_ANY_BUTTON, &btn, loc) == PRO_TK_NO_ERROR)
	{
		/*double x = loc[0];
		double y = loc[1];
		double z = loc[2];*/
		ProMdl currModel;
		status = ProMdlCurrentGet(&currModel);
		ProSelection DefCsysSel;
		string selectedCsyName;
		ProFeature DefcsysFeature;

		/*ProFeature outFeat;
		GetFeatureByName(currModel, "COMP_1", outFeat);
		ProFeattype type;
		status = ProFeatureTypeGet(&outFeat, &type);*/

		//ProAsmcomppath Acomppath;
		//ProMdl CompModel;
		////if (status == PRO_TK_NO_ERROR)
		//{
		//	status = ProModelitemMdlGet(&outFeat, &CompModel);
		//	ProIdTable CableCompidTbl = { outFeat.id };
		//	status = ProAsmcomppathInit((ProSolid)outFeat.owner, CableCompidTbl, 1, &Acomppath);

		//}

		GetDefaultCsysSelection(currModel, NULL, DefCsysSel, selectedCsyName, DefcsysFeature, "DEFAULT");

		status = ProSelectionHighlight(DefCsysSel, PRO_COLOR_CURVE);
		vector<ProSelection> UDFstartInputs;
		UDFstartInputs.push_back(DefCsysSel);


		ProAsmcomppath MdlCompPath;
		status = ProSelectionAsmcomppathGet(DefCsysSel, &MdlCompPath);
		ProMdl CurselectionMdl;
		status = ProModelitemMdlGet(&DefcsysFeature, &CurselectionMdl);
		PointData curPointData = {};
		curPointData.lAsmPath = MdlCompPath;
		curPointData.lMdlAssm = CurselectionMdl;

		string Udfpath = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\cys_single.gph";

		ProFeature UDFGrpFeat;
		vector<ProFeature> VecAxisFeature;
		if (LoadUDF(currModel, Udfpath, UDFstartInputs, UDFGrpFeat, NULL, PRO_B_FALSE))
		{
			//VecAxisUdfs.push_back(UDFGrpFeat);
			ProFeature* feats = NULL;
			status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
			int FeatSize;
			status = ProArraySizeGet(feats, &FeatSize);
			for (int j = 1; j < FeatSize; j++) //First one is the Group feature which is not required
			{
				ProFeattype feattype;
				status = ProFeatureTypeGet(&feats[j], &feattype);

				if (feattype == PRO_FEAT_CSYS)
				{
					VecAxisFeature.push_back(feats[j]);
				}

			}

		}


		double x = loc[0];
		double y = loc[1];
		double z = loc[2];
		ProMatrix matrix;
		ProPoint3d t_point;
		ProPoint3d scrpnt;
		int i, j, k;
		int window;
		double scale;

		t_point[0] = loc[0];
		t_point[1] = loc[1];
		t_point[2] = loc[2];


		ProMatrix currViewMat, invMat;
		status = ProViewMatrixGet(currModel, NULL, currViewMat);
		ProUtilMatrixInvert(currViewMat, invMat);
		ProUtilPointTrans(invMat, t_point, scrpnt);



		//ProViewMatrixGet(currModel, NULL, matrix);
		///*-----------------------------------------------------------------*\
				//	Transform the solid model point to screen coordinates
				//\*-----------------------------------------------------------------*/
				//ProPntTrfEval(t_point, matrix, scrpnt);




		UserCsysData CSysDataVal;

		CSysDataVal.SetXVal(scrpnt[0]);
		CSysDataVal.SetYVal(-scrpnt[2]); //change the direction of Matrix info from Creo
		CSysDataVal.SetZVal(scrpnt[1]);//change the direction of Matrix info from Creo
		CSysDataVal.SetYRVal(270.0);//change the direction of Matrix info from Creo

		EditCSys(VecAxisFeature[0], CSysDataVal);
		string strMdlPath = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\capacitor_1.prt";
		AssembleModel(strMdlPath, "capacitor", VecAxisFeature[0]);

	}

}
void executeAntennaAction(char* dialog, char* component, ProAppData appdata)
{
	ProMouseButton btn1;
	ProVector loc1;
	ProError status;

	if (ProMousePickGet(PRO_ANY_BUTTON, &btn1, loc1) == PRO_TK_NO_ERROR)
	{
		/*double x = loc[0];
		double y = loc[1];
		double z = loc[2];*/
		ProMdl currModel;
		status = ProMdlCurrentGet(&currModel);
		ProSelection DefCsysSel;
		string selectedCsyName;
		ProFeature DefcsysFeature;

		/*ProFeature outFeat;
		GetFeatureByName(currModel, "COMP_1", outFeat);
		ProFeattype type;
		status = ProFeatureTypeGet(&outFeat, &type);*/

		//ProAsmcomppath Acomppath;
		//ProMdl CompModel;
		////if (status == PRO_TK_NO_ERROR)
		//{
		//	status = ProModelitemMdlGet(&outFeat, &CompModel);
		//	ProIdTable CableCompidTbl = { outFeat.id };
		//	status = ProAsmcomppathInit((ProSolid)outFeat.owner, CableCompidTbl, 1, &Acomppath);

		//}

		GetDefaultCsysSelection(currModel, NULL, DefCsysSel, selectedCsyName, DefcsysFeature, "DEFAULT");

		status = ProSelectionHighlight(DefCsysSel, PRO_COLOR_CURVE);
		vector<ProSelection> UDFstartInputs;
		UDFstartInputs.push_back(DefCsysSel);


		ProAsmcomppath MdlCompPath;
		status = ProSelectionAsmcomppathGet(DefCsysSel, &MdlCompPath);
		ProMdl CurselectionMdl;
		status = ProModelitemMdlGet(&DefcsysFeature, &CurselectionMdl);
		PointData curPointData = {};
		curPointData.lAsmPath = MdlCompPath;
		curPointData.lMdlAssm = CurselectionMdl;

		string Udfpath = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\cys_single.gph";

		ProFeature UDFGrpFeat;
		vector<ProFeature> VecAxisFeature;
		if (LoadUDF(currModel, Udfpath, UDFstartInputs, UDFGrpFeat, NULL, PRO_B_FALSE))
		{
			//VecAxisUdfs.push_back(UDFGrpFeat);
			ProFeature* feats = NULL;
			status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
			int FeatSize;
			status = ProArraySizeGet(feats, &FeatSize);
			for (int j = 1; j < FeatSize; j++) //First one is the Group feature which is not required
			{
				ProFeattype feattype;
				status = ProFeatureTypeGet(&feats[j], &feattype);

				if (feattype == PRO_FEAT_CSYS)
				{
					VecAxisFeature.push_back(feats[j]);
				}

			}

		}

		double x = loc1[0];
		double y = loc1[1];
		double z = loc1[2];
		ProMatrix matrix;
		ProPoint3d t_point;
		ProPoint3d scrpnt;
		int i, j, k;
		int window;
		double scale;

		t_point[0] = loc1[0];
		t_point[1] = loc1[1];
		t_point[2] = loc1[2];


		ProMatrix currViewMat, invMat;
		status = ProViewMatrixGet(currModel, NULL, currViewMat);
		ProUtilMatrixInvert(currViewMat, invMat);
		ProUtilPointTrans(invMat, t_point, scrpnt);



		//ProViewMatrixGet(currModel, NULL, matrix);
		///*-----------------------------------------------------------------*\
				//	Transform the solid model point to screen coordinates
				//\*-----------------------------------------------------------------*/
				//ProPntTrfEval(t_point, matrix, scrpnt);




		UserCsysData CSysDataVal;

		CSysDataVal.SetXVal(scrpnt[0]);
		CSysDataVal.SetYVal(-scrpnt[2]); //change the direction of Matrix info from Creo
		CSysDataVal.SetZVal(scrpnt[1]);//change the direction of Matrix info from Creo
		CSysDataVal.SetYRVal(270.0);//change the direction of Matrix info from Creo

		EditCSys(VecAxisFeature[0], CSysDataVal);
		string strMdlPath1 = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\antena_1.prt";
		AssembleModel(strMdlPath1, "antenna", VecAxisFeature[0]);

	}


}
void executeDiodeAction(char* dialog, char* component, ProAppData appdata)
{
	wchar_t* wview_Name;

}
void executeResistorAction(char* dialog, char* component, ProAppData appdata)
{
	wchar_t* wview_Name;

}
void executeTransformerAction(char* dialog, char* component, ProAppData appdata)
{
	wchar_t* wview_Name;

}
void Create()
{
	ProError lErr;

	lErr = ProUIDialogCreate(dialogname, dialogname);
	if (lErr == PRO_TK_NO_ERROR)
	{
		lErr = ProUIDialogDialogstyleSet(dialogname, PROUIDIALOGSTYLE_MODELESS); // Setting the Dialogh style to me Modeless
		lErr = ProUIPushbuttonActivateActionSet(dialogname, okbtn, executeOKAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(dialogname, closebtn, closeMYdialog, NULL);
		//lErr = ProUIPushbuttonActivateActionSet(dialogname, capacitorAsm, executeAssembleAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(dialogname, capacitorUpdate, executeUpdateAction, NULL);
		lErr = ProUIDialogCloseActionSet(dialogname, closeMYdialog, NULL);

		lErr = ProUIPushbuttonBitmapSet(dialogname, PushButton1, "Capacitor1.png");
		lErr = ProUIPushbuttonBitmapSet(dialogname, PushButton2, "Antenna2.png");
		lErr = ProUIPushbuttonBitmapSet(dialogname, PushButton3, "diode1.png");
		lErr = ProUIPushbuttonBitmapSet(dialogname, PushButton4, "resistor.png");
		lErr = ProUIPushbuttonBitmapSet(dialogname, PushButton5, "Transform.png");
		lErr = ProUIPushbuttonActivateActionSet(dialogname, PushButton1, executeCapacitorAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(dialogname, PushButton2, executeAntennaAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(dialogname, PushButton3, executeDiodeAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(dialogname, PushButton4, executeResistorAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(dialogname, PushButton5, executeTransformerAction, NULL);
		int stat = -1;
		lErr = ProUIDialogActivate(dialogname, &stat);
	}



}
uiCmdAccessState IsAsmMode(uiCmdAccessMode access_mode)
{
	return ACCESS_AVAILABLE;
}
char* stringToChar(string strValue)
{
	char* char_Value = new char[strValue.length() + 1];
	strcpy(char_Value, strValue.c_str());
	return char_Value;
}
void Create1()
{
	FanOutCreation();

}

//using namespace std;
//using namespace ocl;
//
//wstring wstr(const char* str) { wstring wstr(str, str + strlen(str)); return wstr; }
//
//class APDC : public AdaptivePathDropCutter {
//public:
//	APDC() : AdaptivePathDropCutter() {}
//	virtual ~APDC() {}
//	vector<CLPoint> getPoints() {
//		return clpoints;
//	}
//};
//
//class GCodeWriter {
//public:
//	GCodeWriter() {};
//	/// destructor
//	virtual ~GCodeWriter() {};
//	void g1(const double x, const double y, const double z) {
//		cout << "G1 X" << x << " Y" << y << " Z" << z << endl;
//	}
//	void g0(const double x, const double y, const double z) {
//		cout << "G0 X" << x << " Y" << y << " Z" << z << endl;
//	}
//};
//
//Path zigzag_x(double minx, double dx, double maxx,
//	double miny, double dy, double maxy, double z) {
//	Path p;
//
//	int rev = 0;
//	for (double i = miny; i < maxy; i += dy) {
//		if (rev == 0) {
//			p.append(Line(Point(minx, i, z), Point(maxx, i, z)));
//			rev = 1;
//		}
//		else {
//			p.append(Line(Point(maxx, i, z), Point(minx, i, z)));
//			rev = 0;
//		}
//	}
//
//	return p;
//}
//
//Path zigzag_y(double minx, double dx, double maxx,
//	double miny, double dy, double maxy, double z) {
//	Path p;
//
//	int rev = 0;
//	for (double i = minx; i < maxx; i += dx) {
//		if (rev == 0) {
//			p.append(Line(Point(i, miny, z), Point(i, maxy, z)));
//			rev = 1;
//		}
//		else {
//			p.append(Line(Point(i, maxy, z), Point(i, miny, z)));
//			rev = 0;
//		}
//	}
//
//	return p;
//}
//
//bool isNearlyEqual(double a, double b) {
//    double factor = 0.00001;
//
//    double min_a = a - (a - std::nexttoward(a, std::numeric_limits<double>::lowest())) * factor;
//	//double max_a = a + (std::nextafter(a, std::numeric_limits<double>::max()) - a) * factor;
//
//   // return min_a <= b && max_a >= b;
//	return true;
//}
//
//int sample() {
//	double zsafe = 5;
//	double zstep = 3;
//
//	double d = 2.0;
//	double d_overlap = 1 - 0.75; // step percentage
//
//	double corner = 0; // d
//
//	locale::global(locale("C"));
//
//	cerr << "stl2ngc  Copyright (C) 2016 - 2023 Jakob Flierl" << endl;
//	cerr << "This program comes with ABSOLUTELY NO WARRANTY;" << endl;
//	cerr << "This is free software, and you are welcome to redistribute it" << endl;
//	cerr << "under certain conditions." << endl << endl;
//
//	cout.setf(ios::fixed, ios::floatfield);
//	cout.setf(ios::showpoint);
//
//	cerr.setf(ios::fixed, ios::floatfield);
//	cerr.setf(ios::showpoint);
//
//	//assert(argc == 2);
//	//assert(argv[1]);
//
//	STLSurf s;
//	STLReader r(wstr("/dev/stdin"), s);
//	//STLReader r(wstr(argv[1]), s);
//	// s.rotate(0,0,0);
//	cerr << s << endl;
//	cerr << s.bb << endl;
//
//	double zdim = s.bb.maxpt.z - s.bb.minpt.z;
//	cerr << "zdim " << zdim << endl;
//
//	double zstart = s.bb.maxpt.z - zstep;
//	cerr << "zstart " << zstart << endl;
//
//	CylCutter* c = new CylCutter(d, 6);
//	cerr << *c << endl;
//
//	APDC apdc;
//	apdc.setSTL(s);
//	apdc.setCutter(c);
//	apdc.setSampling(d * d_overlap);
//	apdc.setMinSampling(d * d_overlap / 100);
//
//	double minx, miny, maxx, maxy, z;
//	minx = s.bb.minpt.x - corner;
//	miny = s.bb.minpt.y - corner;
//	maxx = s.bb.maxpt.x + corner;
//	maxy = s.bb.maxpt.y + corner;
//	z = s.bb.minpt.z - zsafe;
//
//	double dx = d * d_overlap, dy = d * d_overlap;
//
//	Path p = zigzag_x(minx, dx, maxx, miny, dy, maxy, z);
//	apdc.setPath(&p);
//	apdc.setZ(z);
//
//	cerr << "calculating.. " << flush;
//
//	apdc.setThreads(4);
//	apdc.run();
//
//	cerr << "done." << endl;
//
//	GCodeWriter w;
//
//	cout << "G21 F900" << endl;
//	cout << "G64 P0.001" << endl; // path control mode : constant velocity
//	cout << "M03 S13500" << endl; // start spindle
//
//	cout << "G0" <<
//		" X" << s.bb.minpt.x <<
//		" Y" << s.bb.minpt.y <<
//		" Z" << zsafe << endl;
//
//	double zcurr = zstart;
//
//	vector<CLPoint> pts = apdc.getPoints();
//
//	bool fst = true;
//	while (zcurr > s.bb.minpt.z - zstep) {
//
//		cerr << "zcurr " << zcurr << endl;
//
//		BOOST_FOREACH(Point cp, pts) 
//		{
//			double z = -fmin(-cp.z, -zcurr) - s.bb.maxpt.z;
//			//if (!isNearlyEqual(z, 0)) {
//			if (fst) {
//				w.g0(cp.x, cp.y, zsafe);
//				w.g0(cp.x, cp.y, 0);
//				fst = false;
//			}
//			w.g1(cp.x, cp.y, z);
//			//}
//		}
//
//
//		for each (Point cp in pts)
//		{
//			double z = -fmin(-cp.z, -zcurr) - s.bb.maxpt.z;
//			//if (!isNearlyEqual(z, 0)) {
//			if (fst) {
//				w.g0(cp.x, cp.y, zsafe);
//				w.g0(cp.x, cp.y, 0);
//				fst = false;
//			}
//			w.g1(cp.x, cp.y, z);
//			//}
//		}
//
//		zcurr -= zstep;
//		reverse(pts.begin(), pts.end());
//	}
//
//	cout << "G0Z" << zsafe << endl;
//	cout << "M05" << endl; // stop spindle
//	cout << "G0X" << s.bb.minpt.x << " Y" << s.bb.minpt.y << endl;
//	cout << "M2" << endl;
//	cout << "%" << endl;
//
//	return 0;
//
//
//}

void Tessellate()
{
	//sample();
}

ProError CreateMenuItemAndAction()
{
	ProError lErr = PRO_TK_NO_ERROR;
	uiCmdCmdId cmd_id1;
	wchar_t* msgfil = L"Message.txt";


	ProFileName FileName = L"resource.txt";

	uiCmdCmdId lCmd;
	uiCmdCmdId lCmd1;
	lErr = ProMenubarMenuAdd("TADDll_Menu", "TADDll_Menu", "Help", PRO_B_TRUE, FileName);

	lErr = ProCmdActionAdd("Action", (uiCmdCmdActFn)Create, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_FALSE, &lCmd);
	lErr = ProCmdActionAdd("Action1", (uiCmdCmdActFn)Create1, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_FALSE, &lCmd1);
	lErr = ProCmdActionAdd("tool", (uiCmdCmdActFn)Tessellate, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_TRUE, &cmd_id1);

	lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "TADDll_PushButton", "Template_PushButton", "Template_ToolTip", NULL, PRO_B_TRUE, lCmd, FileName);
	lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "TADll_PushButton", "Fanout_PushButton", "Template_ToolTip", NULL, PRO_B_TRUE, lCmd1, FileName);
	//lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "Tag", "Tag", "Tag", NULL, PRO_B_TRUE, cmd_id1, FileName);

	lErr = ProCmdIconSet(lCmd, "angrybird.png");

	return lErr;
}
extern "C" int user_initialize()
{
	CreateMenuItemAndAction();
	return 0;
}


extern "C" void user_terminate()

{
}