#pragma once

#include "pch.h"
#include"CommonHeader.h"
#include <ProDtmCsys.h>
#include <locale>
#include <string>
#include <vector>
#include"VesModelTree.h"
#include "ProDtmPnt.h"
#include <ProUILabel.h>
#include <set>
#include <unordered_map>
#include <ProUILayout.h>
#include <ProTKRunTime.h>
#include <ProFit.h>
#include <ProUITextarea.h>
#include <ProExtdata.h>
#include <cmath>
#include <algorithm>

// STRUCTS
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

struct Node
{
	std::string ref;
	std::string pin;
};

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

struct PointCoOrd 
{	// POINT CO ORDS
	double x{}, y{}, z{};
	bool operator<(const PointCoOrd& other) const {
		if (x != other.x) {
			return x < other.x;
		}
		else if (y != other.y) {
			return y < other.y;
		}
		else {
			return z < other.z;
		}
	}
	double derX, derY, derZ;
};


// GLOBAL
string sMiddleCurveName = "";
ProSelection* placementSurfSel;
asmCompData lAsmData;
int iWindId = -1;
vector<asmCompData> g_reqComp;
ProMdl capacitor_model;
ProMdl Antenna_model;
static double identity_matrix[4][4] = { {1.0, 0.0, 0.0, 0.0},
										{0.0, 1.0, 0.0, 0.0},
										{0.0, 0.0, 1.0, 0.0},
										{0.0, 0.0, 0.0, 1.0} };
vector<ProEdge> contourEdges;
ProExtdataClass extDataClass;
ProPath wTextDirPath;
ProCharPath cTextDirPath;
bool isFanoutOverlap = false;

// UI----Name
static char* PushButton1 = "PushButton1";
static char* PushButton2 = "PushButton2";
static char* PushButton3 = "PushButton3";
static char* PushButton4 = "PushButton4";
static char* PushButton5 = "PushButton5";
static char* pb_1 = "pb_1";
static char* pb_2 = "pb_2";
static char* pb_3 = "pb_3";
static char* pb_4 = "pb_4";
static char* pb_5 = "pb_5";
static char* pb_6 = "pb_6";
static char* pb_7 = "pb_7";
static char* pb_8 = "pb_8";
static char* pb_9 = "pb_9";
static char* pb_10 = "pb_10";
static char* pb_11 = "pb_11";
static char* pb_fanout = "fanout_pb";
static char* paramsDialog = "params_dialog";
static char* paramsDialogLCR = "params_dialog_lcr";
static char* resistorParamsDialog = "resistor_params_dialog";
static char* serpentineResistorParamsDialog = "serp_resistor_params_dialog";
static char* capacitorParamsDialog = "capacitor_params_dialog";
static char* UDFmodifyDialog = "udf_modify_dialog";
static char* ip_height = "height_ip";
static char* ip_width = "width_ip";
static char* ip_angle = "angle_ip";
static char* mainDialog = "circuit_design_tools_ui";
static char* capacitorRadio = "capacitorRadio1";
static char* capacitorLabel = "capacitorLabel";
static char* capacitorAsm = "capacitorAsm";
static char* capacitorUpdate = "capacitorUpdate";
static char* antennaRadio = "antennaRadio1";
static char* antennaLabel = "antennaLabel";
static char* antennaAsm = "antennaAsm";
static char* antennaUpdate = "antennaUpdate";
static char* Preview = "btnPreview";
static char* okbtn = "CommitOK";
static char* closebtn = "CommitCancel";
// UpdateAntennaUI
static char* updateDimdialogname = "UPDATEDIMENSION";
static char* capacitorHeightInput = "capacitorHeight";
static char* capacitorDiaInput = "capacitorDia";
static char* applycapupdatebtn = "applyCapUpdate";
static char* cancelcapupdatebtn = "cancelCapUpdate";

static char* ip_x_ofst = "offset_x_ip";
static char* ip_y_ofst = "offset_y_ip";
static char* ip_trace_margin = "trace_margin_ip";
//static char* ip_clearance = "clearance_ip";
static char* layout_dim = "dimension_layout";
//static char* layout_clearance = "clearance_layout";
static char* layout_fanout_attributes = "fanout_attributes_layout";
static char* textarea_desc = "desc_textarea";

// LCR network UI
static char* ip_fanout_height = "fanout_height_ip";
static char* ip_circle_radius = "circle_radius_ip";
static char* ip_bondpad_side = "bondpad_side_ip";


// FUNCTION PROTOTYPES
void FillUDFDataWithReferenceForCopyFeat(ProUdfdata& UdfData, vector<ProSelection>& sel2, ProBoolean RefAsm);
bool LoadBondpadUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/, ProBoolean modifyDim = PRO_B_FALSE);
bool LoadCurveUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm, double widthDimVal /*For Asm it is TRUE*/);
ProError ProDemoFieldPointCreate(ProSelection placementSel, ProFeature* pointFeat);
int createCurveBw2Points(const ProMdl& CurMdl1, std::string& point1, ProFeature& csoFeat, PointData& curPointData, std::string& point2, ProFeature& csoFeat1, ProError& status, int surfaceid, ProAsmcomppath& comp_path, std::vector<PointData> vectPointData, std::vector<ProSelection>& UdfInputSel, ProFeature& udfGrpFeat);
//void CreateOffsetCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfCurveFeat, ProFeature& udfOffsetCurve1Feat, ProFeature& udfOffsetCurve2Feat);
void CreateOffsetCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfCurveFeat, ProFeature& udfOffsetCurveFeat);
void CreateSurfaceBwCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfOffsetCurve1Feat, ProFeature& udfOffsetCurve2Feat, ProFeature& surfFeature);
ProError bondPadModifyAction(char* dialog, char* component, ProAppData appdata);
ProError executeLcrNetworkPbAction(char* dialog, char* component, ProAppData appdata);
ProError executeSerpentineResistorPbAction(char* dialog, char* component, ProAppData appdata);
ProError executeInterdigitatedCapacitorPbAction(char* dialog, char* component, ProAppData appdata);
ProError placeSerpentineResistorPbAction(char* dialog, char* component, ProAppData appdata);
int createFanOutLCR(string point1Name, string point2Name, ProMdl CurMdl1, vector<PointCoOrd>& fanoutMidpointsCoords, double widthDimVal, ProAsmcomppath& comp_path, vector<PointData>& vectPointData);
ProError executeBondpadPairPbAction(char* dialog, char* component, ProAppData appdata);
ProError executeLargeBondpadPairPbAction(char* dialog, char* component, ProAppData appdata);
ProError executeResistorNetwork1Action(char* dialog, char* component, ProAppData appdata);
ProError executeResistorNetwork2Action(char* dialog, char* component, ProAppData appdata);
bool LoadResistorUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/, ProBoolean modifyDim);
ProError executeResistorNetwork2InvAction(char* dialog, char* component, ProAppData appdata);
bool LoadCapacitorUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProBoolean modifyDim);
bool loadLegBasePointsUDF(ProMdl Mdl, vector<ProSelection>& UDFInputs, double lenBetweenPoints, ProFeature& UDFGrpFeat);

void Create();
ProError CreateMenuItemAndAction();

ProError Collect_SubLevelComponents(ProAsmcomppath* p_path, ProSolid handle, ProBoolean xBool, ProAppData app_data);
ProError SolidCsysFeatVisitAction(ProFeature* p_feature, ProError status, ProAppData app_data);
ProError PointFeatVisitAction(ProFeature* p_feature, ProError status, ProAppData app_data);
ProError SolidCsysFeatVisit(ProMdl xMdl, vector<ProFeature>& xProfeature);
ProError PointsFeatVisit(ProMdl xMdl, vector<ProFeature>& xProfeature);
ProError FeatVisitActionMap(ProFeature* feature, ProError status, ProAppData appdata);
void GetModelFeatures(ProMdl Mdl, std::map<string, ProFeature>& FeatureVector);
bool GetFeatureByName(IN ProMdl Mdl, IN string FeatName, OUT ProFeature& outFeat);
ProError VesGeomItemVisitAction(ProGeomitem* p_handle, ProError status, ProAppData app_data);
void GetGeomItems(IN ProMdl Mdl, IN ProFeature Feat, OUT vector<ProGeomitem>& vecGeomItems);
void GetCurveGeomItems(IN ProMdl Mdl, IN ProFeature Feat, OUT vector<ProGeomitem>& vecGeomItems);
std::vector<std::vector<Node>> extractNodePairsFromFile(const std::string& filename);
std::vector<std::pair<std::string, std::string>> readCSV(const std::string& filename);
void closeMYdialog(char* dialog, char* component, ProAppData appdata);
ProError placemetnSurfContourVisitAction(ProContour p_contour, ProError status, ProAppData app_data);
ProError contourEdgeVisitAction(ProEdge edge, ProError status, ProAppData data);
ProError featPointVisitAction(ProGeomitem* geomitem, ProError err, ProAppData appdata);
ProError placeBondpadAction(char* dialog, char* component, ProAppData appdata);
ProError placeBondpadAction2(char* dialog, char* component, ProAppData appdata);
ProError ProUtilCollectCurveComponentVisitAction(ProCurve p_curve, ProCurve p_comp, int comp_idx, ProBoolean comp_flip, ProError status, vector<PointCoOrd>* pnt_co_ords);
ProError featCurveGeomitemVisit(ProGeomitem& p_handle, ProError status, ProAppData app_data);
int placeFanoutAction(char* dialog, char* component, ProAppData appdata);
ProError executeBondpadPbAction(char* dialog, char* component, ProAppData appdata);
ProError executeResistorBondpadPbAction(char* dialog, char* component, ProAppData appdata);
ProError executeFanoutPbAction(char* dialog, char* component, ProAppData appdata);
void GetMamximumCurveId(std::vector<ProFeature>& vecGeomItems, ProError& status, const ProMdl& CurMdl1, int& FullCurveGEomitemId);
ProError ProDemoFieldPointCreate(ProSelection placementSel, ProFeature* pointFeat);
void FillUDFDataWithReferenceForCopyFeat(ProUdfdata& UdfData, vector<ProSelection>& sel2, ProBoolean RefAsm);
bool LoadUDFwithoutDims(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/);
bool LoadBondpadUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/, ProBoolean modifyDim);
bool LoadCurveUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm, double widthDimVal /*For Asm it is TRUE*/);
int createCurveBw2Points(const ProMdl& CurMdl1, std::string& point1, ProFeature& csoFeat, PointData& curPointData, std::string& point2, ProFeature& csoFeat1, ProError& status, int surfaceid, ProAsmcomppath& comp_path, std::vector<PointData> vectPointData, std::vector<ProSelection>& UdfInputSel, ProFeature& udfGrpFeat);
//void CreateSurfaceBwCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfOffsetCurve1Feat, ProFeature& udfOffsetCurve2Feat);
void CreateOffsetCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfCurveFeat, ProFeature& udfOffsetCurveFeat);
void createTwoOffsetCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfCurveFeat, ProFeature& udfOffsetCurve1Feat, ProFeature& udfOffsetCurve2Feat);

ProError solidFeatCollectAction(ProFeature* p_feature, ProError status, ProAppData app_data);
ProError fanoutMidpointFilter(ProFeature* p_feature, ProAppData app_data);
void writeRemarkTxtFile(string remarkFp, vector<string> missingPoints);
ProError mirrorFeatCreatePostAction(ProFeature &createdFeat);
int createFanOut(string point1Name, string point2Name, ProMdl CurMdl1, vector<PointCoOrd>& fanoutMidpointsCoords, double widthDimVal, ProAsmcomppath& comp_path, vector<PointData>& vectPointData);
