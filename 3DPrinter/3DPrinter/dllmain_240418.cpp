// dllmain.cpp : Defines the entry point for the DLL application.
#include "dllmain.h"

extern "C" int user_initialize()
{
	CreateMenuItemAndAction();
	return 0;
}

ProError CreateMenuItemAndAction()
{
	ProError lErr = PRO_TK_NO_ERROR;
	uiCmdCmdId cmd_id1;
	wchar_t* msgfil = L"Message.txt";
	ProFileName FileName = L"resource.txt";
	uiCmdCmdId lCmd;

	lErr = ProMenubarMenuAdd("TADDll_Menu", "TADDll_Menu", "Help", PRO_B_TRUE, FileName);
	lErr = ProCmdActionAdd("Action", (uiCmdCmdActFn)Create, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_FALSE, &lCmd);
	lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "TADDll_PushButton", "Template_PushButton", "Template_ToolTip", NULL, PRO_B_TRUE, lCmd, FileName);

	return lErr;
}

void Create()
{
	sMiddleCurveName = "";
	ProError lErr;
	lErr = ProUIDialogCreate(mainDialog, mainDialog);
	if (lErr == PRO_TK_NO_ERROR)
	{
		lErr = ProUIDialogAboveactivewindowSet(mainDialog, PRO_B_FALSE);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_1, (ProUIAction)executeBondpadPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_2, (ProUIAction)executeResistorBondpadPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_fanout, (ProUIAction)executeFanoutPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, closebtn, closeMYdialog, NULL);
		lErr = ProUIDialogCloseActionSet(mainDialog, closeMYdialog, NULL);
		int stat = -1;
		lErr = ProUIDialogActivate(mainDialog, &stat);
	}
	lErr = ProUIDialogDestroy(mainDialog);
}


// ----------------------------------------------  CALL BACKS   ----------------------------------------------
ProError executeBondpadPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	status = ProUIDialogCreate(paramsDialog, paramsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	double defaultHeight = 0, defaultWidth = 0;
	ProErr lErr;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	lErr = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char* bondpadUdfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_with_dims.gph.1";
	//char* bondpadUdfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_udf.gph.1";

	status = ProUIDialogAboveactivewindowSet(paramsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, okbtn, placeBondpadAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialog, closeMYdialog, NULL);
	/*status = ProUILabelHide(paramsDialog, "offset_x_lbl");
	status = ProUIInputpanelHide(paramsDialog, ip_x_ofst);
	status = ProUILabelHide(paramsDialog, "offset_y_lbl");
	status = ProUIInputpanelHide(paramsDialog, ip_y_ofst);
	status = ProUILabelHide(paramsDialog, "fanout_boundary_lbl");
	status = ProUIInputpanelHide(paramsDialog, ip_trace_margin);*/
	//status = ProUILayoutHide(paramsDialog, layout_fanout_attributes);
	status = ProUILayoutHide(paramsDialog, layout_clearance);

	string sBondpadUdfpath = string(bondpadUdfpath);
	ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
	lErr = ProUdfdataPathSet(UdfData, UdfFilePath);

	ProUdfvardim* var_dim_array;
	int size;
	int i;
	status = ProUdfdataVardimsGet(UdfData, &var_dim_array);
	if (status == PRO_TK_NO_ERROR)
	{
		status = ProArraySizeGet(var_dim_array, &size);
		for (i = 0; i < size; i++)
		{
			wchar_t wDimName[PRO_NAME_SIZE];
			char cDimName[PRO_NAME_SIZE];

			ProLine wPrompt;
			status = ProUdfvardimPromptGet(var_dim_array[i], wPrompt);
			ProUdfVardimType value_type;
			double defaultDimVal;
			status = ProUdfvardimDefaultvalueGet(var_dim_array[i], &value_type, &defaultDimVal);

			char cPrompt[PRO_NAME_SIZE];
			ProWstringToString(cPrompt, wPrompt);
			if (strcmp(cPrompt, "width1") == 0) {
				defaultWidth = 2 * defaultDimVal;
			}
			else if (strcmp(cPrompt, "height1") == 0) {
				defaultHeight = defaultDimVal;
			}
		}
		ProUdfvardimProarrayFree(var_dim_array);
	}
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_height, defaultHeight);
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);

	int stat = -1;
	status = ProUIDialogActivate(paramsDialog, &stat);
	status = ProUIDialogDestroy(paramsDialog);

	return PRO_TK_NO_ERROR;
}

ProError executeResistorBondpadPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	double defaultHeight = 0, defaultWidth = 0;
	ProErr lErr;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	ProPath UdfFilePath;
	char* bondpadUdfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_with_dims.gph.1";
	//char* bondpadUdfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_udf.gph.1";

	status = ProUIDialogCreate(paramsDialog, paramsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	lErr = ProUdfdataAlloc(&UdfData);

	status = ProUIDialogAboveactivewindowSet(paramsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, okbtn, placeBondpadAction2, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialog, closeMYdialog, NULL);
	/*status = ProUILabelHide(paramsDialog, "offset_x_lbl");
	status = ProUIInputpanelHide(paramsDialog, "offset_x_ip");
	status = ProUILabelHide(paramsDialog, "offset_y_lbl");
	status = ProUIInputpanelHide(paramsDialog, "offset_y_ip");*/
	status = ProUILayoutHide(paramsDialog, layout_fanout_attributes);
	status = ProUILayoutHide(paramsDialog, layout_clearance);

	string sBondpadUdfpath = string(bondpadUdfpath);
	ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
	lErr = ProUdfdataPathSet(UdfData, UdfFilePath);

	ProUdfvardim* var_dim_array;
	int size;
	int i;
	status = ProUdfdataVardimsGet(UdfData, &var_dim_array);
	if (status == PRO_TK_NO_ERROR)
	{
		status = ProArraySizeGet(var_dim_array, &size);
		for (i = 0; i < size; i++)
		{
			wchar_t wDimName[PRO_NAME_SIZE];
			char cDimName[PRO_NAME_SIZE];

			ProLine wPrompt;
			status = ProUdfvardimPromptGet(var_dim_array[i], wPrompt);
			ProUdfVardimType value_type;
			double defaultDimVal;
			status = ProUdfvardimDefaultvalueGet(var_dim_array[i], &value_type, &defaultDimVal);

			char cPrompt[PRO_NAME_SIZE];
			ProWstringToString(cPrompt, wPrompt);
			if (strcmp(cPrompt, "width1") == 0) {
				defaultWidth = 2 * defaultDimVal;
			}
			else if (strcmp(cPrompt, "height1") == 0) {
				defaultHeight = defaultDimVal;
			}
		}
		ProUdfvardimProarrayFree(var_dim_array);
	}
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_height, 4);
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, 6);

	int stat = -1;
	status = ProUIDialogActivate(paramsDialog, &stat);
	status = ProUIDialogDestroy(paramsDialog);

	return PRO_TK_NO_ERROR;
}

ProError executeFanoutPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProPath wNetlistPath = L"";
	double defaultWidth = 0;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	ProPath UdfFilePath;

	status = ProFileMdlnameOpen(L"Open Netlist", L"*.net", NULL, NULL, NULL, NULL, wNetlistPath);
	if (status != PRO_TK_NO_ERROR)
		return status;

	status = ProUIDialogCreate(paramsDialog, paramsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	status = ProUdfdataAlloc(&UdfData);

	status = ProUIDialogAboveactivewindowSet(paramsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, okbtn, (ProUIAction)placeCurveAction, (ProAppData)wNetlistPath);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialog, closeMYdialog, NULL);
	status = ProUILabelHide(paramsDialog, "height_lbl");
	status = ProUILabelHide(paramsDialog, "height_units");
	status = ProUIInputpanelHide(paramsDialog, "height_ip");

	/*string sOffsetCurveUDFpath = string(offsetCurveUDFpath);
	ProStringToWstring(UdfFilePath, (char*)sOffsetCurveUDFpath.c_str());
	status = ProUdfdataPathSet(UdfData, UdfFilePath);

	ProUdfvardim* var_dim_array;
	int size;
	int i;
	status = ProUdfdataVardimsGet(UdfData, &var_dim_array);
	if (status == PRO_TK_NO_ERROR)
	{
		status = ProArraySizeGet(var_dim_array, &size);
		for (i = 0; i < size; i++)
		{
			wchar_t wDimName[PRO_NAME_SIZE];
			char cDimName[PRO_NAME_SIZE];

			ProLine wPrompt;
			status = ProUdfvardimPromptGet(var_dim_array[i], wPrompt);
			ProUdfVardimType value_type;
			double defaultDimVal;
			status = ProUdfvardimDefaultvalueGet(var_dim_array[i], &value_type, &defaultDimVal);

			char cPrompt[PRO_NAME_SIZE];
			ProWstringToString(cPrompt, wPrompt);
			if (strcmp(cPrompt, "offset_1") == 0) {
				defaultWidth = 2 * defaultDimVal;
			}
		}
		ProUdfvardimProarrayFree(var_dim_array);
	}
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	int stat = -1;
	status = ProUIDialogActivate(paramsDialog, &stat);
	status = ProUIDialogDestroy(paramsDialog);

	return PRO_TK_NO_ERROR;
}

int placeCurveAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	vector<PointData> vectPointData;
	ProAsmcomppath comp_path;
	wchar_t* wNetlistPath = (wchar_t*)appdata;
	ProCharPath cNetlistPath;
	ProWstringToString(cNetlistPath, wNetlistPath);

	ProIdTable c_id_table;
	c_id_table[0] = -1;
	char* offsetCurveUDFpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\create_offset_curve_with_dim.gph.1";

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(paramsDialog);
	status = ProMdlCurrentGet(&CurMdl1);
	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);

	//ProSelection* pntLocSel{};
	//int nSel=0;
	////status = ProSelect((char*)"point", 2, NULL, NULL, NULL, NULL, &pntLocSel, &nSel);
	//if (status == PRO_TK_NO_ERROR && nSel > 0) {
	//	ProModelitem pnt1Mdlitem, pnt2Mdlitem;
	//	wchar_t wPnt1name[PRO_NAME_SIZE], wPnt2name[PRO_NAME_SIZE];
	//	char cPoint1name[PRO_NAME_SIZE], cPoint2name[PRO_NAME_SIZE];
	//	status = ProSelectionModelitemGet(pntLocSel[0], &pnt1Mdlitem);
	//	status = ProSelectionModelitemGet(pntLocSel[1], &pnt2Mdlitem);
	//	status = ProModelitemNameGet(&pnt1Mdlitem, wPnt1name);
	//	ProWstringToString(cPoint1name, wPnt1name);
	//	status = ProModelitemNameGet(&pnt2Mdlitem, wPnt2name);
	//	ProWstringToString(cPoint2name, wPnt2name);
	//	ProFeature csoFeat, csoFeat1;
	//	std::vector<ProSelection> UdfInputSel;
	//	PointData curPointData = {};
	//	string point1 = string(cPoint1name);
	//	string point2 = string(cPoint2name);
	//	int surfaceid = 41245;
	//	//CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
	//}

	//string netListFileName = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\3DPrinter\\board_training.net";

	std::vector<std::vector<Node>> nodePairs = extractNodePairsFromFile(string(cNetlistPath));

	//for (const auto& pair : nodePairs)
	//{
	//	if (pair.size() == 2)	// temparory purpose
	//	{
	//		// Create Initial curve
	//		ProFeature _csoFeat, _csoFeat1;
	//		std::vector<ProSelection> _UdfInputSel;
	//		PointData _curPointData = {};
	//		string _point1Name = pair[0].ref;
	//		string _point2Name = pair[1].ref;
	//		int _surfaceid = 41245;
	//		ProFeature grpCurveFeat;
	//		int curve_create_sts = createCurveBw2Points(CurMdl1, _point1Name, _csoFeat, _curPointData, _point2Name, _csoFeat1, status, _surfaceid, comp_path, vectPointData, _UdfInputSel, grpCurveFeat);
	//		if (curve_create_sts != 0)
	//			return PRO_TK_NO_ERROR;

	//		vectPointData.clear();

	//		ProFeature* feats = NULL;
	//		status = ProGroupFeaturesCollect(&grpCurveFeat, &feats);//Give only non suppressed entities

	//		/*double bendPntCoOrd[3];
	//		status = ProFeatureGeomitemVisit(&feats[1], PRO_CURVE, (ProGeomitemAction)featCurveGeomitemVisit, NULL, (ProAppData*)&bendPntCoOrd);

	//		ProPoint3d pnt3D, nearPnt3d;
	//		pnt3D[0] = bendPntCoOrd[0];
	//		pnt3D[1] = bendPntCoOrd[1];
	//		pnt3D[2] = bendPntCoOrd[2];*/

	//		/*ProSurface onSurface;
	//		ProUvParam surfaceUV;
	//		status = ProSolidProjectPoint((ProSolid)CurMdl1, pnt3D, 3, &onSurface, surfaceUV);
	//		int surfId;
	//		status = ProSurfaceIdGet(onSurface, &surfId);
	//		ProModelitem surfItem;
	//		status = ProSurfaceToGeomitem((ProSolid)CurMdl1, onSurface, &surfItem);
	//		ProSelection surfSel;
	//		status = ProSelectionAlloc(NULL, &surfItem, &surfSel);
	//		surfaceUV[0] = surfaceUV[0] + 0.05;
	//		surfaceUV[1] = surfaceUV[1] + 3;
	//		status = ProSelectionUvParamSet(surfSel, surfaceUV);
	//		ProVector projectedPoint;
	//		status = ProSurfaceXyzdataEval(onSurface, surfaceUV, projectedPoint, NULL, NULL, NULL);

	//		ProSurface obstacleSurf;
	//		status = ProSurfaceInit(CurMdl1, 44181, &obstacleSurf);

	//		ProFeature pointFeature;
	//		status = ProDemoFieldPointCreate(surfSel, &pointFeature);
	//		ProName wBendPointName;
	//		status = ProModelitemNameGet(&pointFeature, wBendPointName);
	//		ProCharName cBendPointName;
	//		ProWstringToString(cBendPointName, wBendPointName);

	//		status = ProModelitemHide(&feats[1]);*/

	//		// Inside obstacle surface verification
	//		/*ProUvStatus uv_sts;
	//		status = ProSurfaceUvpntVerify((ProSolid)CurMdl1, obstacleSurf, surfaceUV, &uv_sts);
	//		if (uv_sts != PRO_UV_OUTSIDE) {
	//			return -1;
	//		}*/

	//		//ProFeature csoFeat, csoFeat1;
	//		std::vector<ProSelection> UdfInputSel;
	//		//PointData curPointData = {};
	//		//string point1 = _point1Name;
	//		////string point2 = sMiddleCurveName;
	//		//string point2 = string(cBendPointName);
	//		int surfaceid = 41245;
	//		ProFeature curveFeat, offsetCurve1Feat, offsetCurve2Feat;
	//		//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat);
	//		//status = ProGroupFeaturesCollect(&curveFeat, &feats);//Give only non suppressed entities
	//		//status = ProModelitemHide(&feats[1]);

	//		//CreateOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat, offsetCurve1Feat, offsetCurve2Feat);
	//		//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat, offsetCurve2Feat);

	//		////point1 = sMiddleCurveName;
	//		//point1 = string(cBendPointName);
	//		//point2 = _point2Name;
	//		//UdfInputSel.clear();
	//		//vectPointData.clear();
	//		//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat);
	//		//status = ProGroupFeaturesCollect(&curveFeat, &feats);//Give only non suppressed entities
	//		//status = ProModelitemHide(&feats[1]);

	//		CreateOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, grpCurveFeat, offsetCurve1Feat);
	//		//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, grpCurveFeat, offsetCurve1Feat);
	//	}
	//}

	double xOffsetVal, yOffsetVal, traceMarginVal, widthDimVal;
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_x_ofst, &xOffsetVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_y_ofst, &yOffsetVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_trace_margin, &traceMarginVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);

	vector<pnt_ord> fanoutMidpointsCoords{};
	for (int i = 0; i < nodePairs.size(); ++i)
	{
		if (nodePairs[i].size() == 2)	// temparory purpose
		{
			ProFeature _csoFeat, _csoFeat1;
			std::vector<ProSelection> _UdfInputSel;
			PointData _curPointData = {};
			string _point1Name = nodePairs[i][0].ref;
			string _point2Name = nodePairs[i][1].ref;
			int _surfaceid = 41245;
			ProFeature grpCurveFeat;
			int create_curve_sts = createCurveBw2Points(CurMdl1, _point1Name, _csoFeat, _curPointData, _point2Name, _csoFeat1, status, _surfaceid, comp_path, vectPointData, _UdfInputSel, grpCurveFeat);
			if (create_curve_sts != 0)
				return -1;

			vectPointData.clear();

			ProFeature* feats = NULL;
			status = ProGroupFeaturesCollect(&grpCurveFeat, &feats);//Give only non suppressed entities

			double bendPntCoOrd[3];
			status = ProFeatureGeomitemVisit(&feats[1], PRO_CURVE, (ProGeomitemAction)featCurveGeomitemVisit, NULL, (ProAppData*)&bendPntCoOrd);

			ProPoint3d pnt3D, nearPnt3d;
			pnt3D[0] = bendPntCoOrd[0];
			pnt3D[1] = bendPntCoOrd[1];
			pnt3D[2] = bendPntCoOrd[2];

			// check if the same obstacle is hit twice
			double epsilon = 0.0001;
			bool isSameObstacle = false;
			for (auto &point : fanoutMidpointsCoords)
			{
				if ((fabs(point.x - pnt3D[0]) < epsilon) && (fabs(point.y - pnt3D[1]) < epsilon) && (fabs(point.z - pnt3D[2]) < epsilon)) {
					pnt3D[0] += 5;
					pnt3D[1] += 5;
					pnt3D[2] += 5;
					isSameObstacle = true;
				}
			}

			ProSurface onSurface;
			ProUvParam surfaceUV;
			status = ProSolidProjectPoint((ProSolid)CurMdl1, pnt3D, 3, &onSurface, surfaceUV);
			int surfId;
			status = ProSurfaceIdGet(onSurface, &surfId);
			ProModelitem surfItem;
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, onSurface, &surfItem);
			ProSelection surfSel;
			status = ProSelectionAlloc(NULL, &surfItem, &surfSel);
			/*surfaceUV[0] = surfaceUV[0] + 0.05;
			surfaceUV[1] = surfaceUV[1] + 3;*/

			surfaceUV[0] = surfaceUV[0] + ((xOffsetVal+ traceMarginVal + widthDimVal) * 0.001);
			surfaceUV[1] = surfaceUV[1] + yOffsetVal+ traceMarginVal + widthDimVal;

			/*if (isSameObstacle) {
				surfaceUV[0] -= 5;
				surfaceUV[1] -=	5;
			}*/

			status = ProSelectionUvParamSet(surfSel, surfaceUV);
			ProVector projectedPoint;
			status = ProSurfaceXyzdataEval(onSurface, surfaceUV, projectedPoint, NULL, NULL, NULL);

			ProFeature pointFeature;
			status = ProDemoFieldPointCreate(surfSel, &pointFeature);
			ProCharName cFoMpntName;
			ProName wFoMpntName;
			sprintf(cFoMpntName, "FO_MP_%s_%s", _point1Name.c_str(), _point2Name.c_str());
			ProStringToWstring(wFoMpntName, cFoMpntName);
			status = ProModelitemNameSet(&pointFeature, wFoMpntName);

			fanoutMidpointsCoords.push_back({ pnt3D[0], pnt3D[1], pnt3D[2] });

			// check if any other point lies within the specified radius
			vector<ProFeature> fanoutMidpoints;
			status = ProSolidFeatVisit((ProSolid)CurMdl1, (ProFeatureVisitAction)solidFeatCollectAction, (ProFeatureFilterAction)fanoutMidpointFilter, (ProAppData)&fanoutMidpoints);

			bool fanoutInterferenceFound = false;
			for (int j = 0; j < fanoutMidpoints.size(); ++j)
			{
				ProName wName; ProCharName cName;
				status = ProModelitemNameGet(&fanoutMidpoints[j], wName);
				ProWstringToString(cName, wName);
				if (string(cName).find(cFoMpntName) == string::npos)
				{
					ProSelection pnt1Sel, pnt2Sel;
					vector<ProGeomitem> vecGeomItems;

					GetGeomItems(CurMdl1, pointFeature, vecGeomItems);
					status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt1Sel);
					vecGeomItems.clear();

					GetGeomItems(CurMdl1, fanoutMidpoints[j], vecGeomItems);
					status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt2Sel);
					vecGeomItems.clear();

					Pro2dPnt param1, param2;
					Pro3dPnt pnt_1, pnt_2;
					double distBwPnts;
					status = ProSelectionWithOptionsDistanceEval(pnt1Sel, PRO_B_FALSE, pnt2Sel, PRO_B_FALSE, param1, param1, pnt_1, pnt_1, &distBwPnts);
					
					if (distBwPnts < traceMarginVal)
						fanoutInterferenceFound = true;
				}
			}
			fanoutMidpoints.clear();

			ProName wBendPointName;
			status = ProModelitemNameGet(&pointFeature, wBendPointName);
			ProCharName cBendPointName;
			ProWstringToString(cBendPointName, wBendPointName);

			status = ProModelitemHide(&feats[1]);

			if (fanoutInterferenceFound) {
				ProUIMessageButton* buttons = nullptr;
				ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
				buttons[0] = PRO_UI_MESSAGE_OK;
				ProUIMessageButton resultBtn;
				status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Fanout intereference found",
					L"Alert: Fanouts detected within the specified boundary. Application will now exit.",
					buttons, buttons[0], &resultBtn);
				status = ProArrayFree((ProArray*)&buttons);
				status = ProUIDialogShow(mainDialog);
				status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);
				return -1;
			}

			// Inside obstacle surface verification
			/*ProUvStatus uv_sts;
			ProSurface obstacleSurf;
			status = ProSurfaceInit(CurMdl1, 44181, &obstacleSurf);
			status = ProSurfaceUvpntVerify((ProSolid)CurMdl1, obstacleSurf, surfaceUV, &uv_sts);
			if (uv_sts != PRO_UV_OUTSIDE) {
				return -1;
			}*/

			ProFeature csoFeat, csoFeat1;
			std::vector<ProSelection> UdfInputSel;
			PointData curPointData = {};
			string point1 = _point1Name;
			//string point2 = sMiddleCurveName;
			string point2 = string(cBendPointName);
			int surfaceid = 41245;
			ProFeature curveFeat, offsetCurve1Feat, offsetCurve2Feat;
			createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat);
			status = ProGroupFeaturesCollect(&curveFeat, &feats);//Give only non suppressed entities
			status = ProModelitemHide(&feats[1]);

			createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat, offsetCurve1Feat, offsetCurve2Feat);
			CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat, offsetCurve2Feat);

			//point1 = sMiddleCurveName;
			point1 = string(cBendPointName);
			point2 = _point2Name;
			UdfInputSel.clear();
			vectPointData.clear();
			createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat);
			status = ProGroupFeaturesCollect(&curveFeat, &feats);//Give only non suppressed entities
			status = ProModelitemHide(&feats[1]);

			createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat, offsetCurve1Feat, offsetCurve2Feat);
			CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat, offsetCurve2Feat);
		}
	}
	fanoutMidpointsCoords.clear();

	/*ProFeature csoFeat, csoFeat1;
	std::vector<ProSelection> UdfInputSel;
	PointData curPointData = {};
	string point1 = "PNT4";
	string point2 = "PNT6";
	int surfaceid = 41245;
	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);*/


	//ProCurve middleCurve;
	//ProFeature curveFeat;
	//double bendPntCoOrd[3];
	//status = ProFeatureInit((ProSolid)CurMdl1, 91351, &curveFeat);
	////status = ProFeatureInit((ProSolid)CurMdl1, 97271, &curveFeat);
	//status = ProFeatureGeomitemVisit(&curveFeat, PRO_CURVE, (ProGeomitemAction)featCurveGeomitemVisit, NULL, (ProAppData*)&bendPntCoOrd);

	//ProPoint3d pnt3D;
	////status = ProSelectionPoint3dGet(pntPlacementSel[0], pnt3D);
	///*pnt3D[0] = bendPntCoOrd[0]-10;
	//pnt3D[1] = bendPntCoOrd[1]-10;
	//pnt3D[2] = bendPntCoOrd[2]-10;*/

	//pnt3D[0] = bendPntCoOrd[0];
	//pnt3D[1] = bendPntCoOrd[1];
	//pnt3D[2] = bendPntCoOrd[2];

	////ProBoolean onSurface;
	//ProPoint3d nearPnt3d;
	////status = ProPoint3dOnsurfaceFind(pnt3D, placement_surf, &onSurface, nearPnt3d);
	//ProSurface onSurface;
	//ProUvParam surfaceUV;
	//status = ProSolidProjectPoint((ProSolid)CurMdl1, pnt3D, 3, &onSurface, surfaceUV);
	//int surfId;
	//status = ProSurfaceIdGet(onSurface, &surfId);
	//ProModelitem surfItem;
	//status = ProSurfaceToGeomitem((ProSolid)CurMdl1, onSurface, &surfItem);
	//ProSelection surfSel;
	//status = ProSelectionAlloc(NULL, &surfItem, &surfSel);
	//surfaceUV[0] = surfaceUV[0] + 0.01;
	//surfaceUV[1] = surfaceUV[1] + 1;
	//status = ProSelectionUvParamSet(surfSel, surfaceUV);
	//ProVector projectedPoint;
	//status = ProSurfaceXyzdataEval(onSurface, surfaceUV, projectedPoint, NULL, NULL, NULL);

	//ProSurface obstacleSurf;
	//status = ProSurfaceInit(CurMdl1, 44181, &obstacleSurf);

	//ProUvStatus uv_sts;
	//status = ProSurfaceUvpntVerify((ProSolid)CurMdl1, obstacleSurf, surfaceUV, &uv_sts);
	//ProFeature pointFeature;
	//status = ProDemoFieldPointCreate(surfSel, &pointFeature);

	/*ProFeature csoFeat, csoFeat1;
	std::vector<ProSelection> UdfInputSel;
	PointData curPointData = {};
	string point1 = "PNT10";
	string point2 = "FPNT4";
	int surfaceid = 41245;
	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);

	point1 = "FPNT4";
	point2 = "PNT9";
	UdfInputSel.clear();
	vectPointData.clear();
	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);*/


	//status = ProCurveInit((ProSolid)CurMdl1, 96597, &middleCurve);
	//status = ProCurveCompVisit(middleCurve, ProUtilCollectCurveComponentVisitAction, NULL, NULL);
	/*ProVector xyz_pnt, derv1, derv2;
	status = ProCurveXyzdataEval(middleCurve, 1, xyz_pnt, derv1, derv2);*/

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);

	return 0;

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

ProError solidFeatCollectAction(ProFeature* p_feature, ProError status, ProAppData app_data)
{
	vector<ProFeature>* lpVecfeature = (vector<ProFeature>*)(app_data);
	lpVecfeature->push_back(*p_feature);
	return PRO_TK_NO_ERROR;
}

ProError fanoutMidpointFilter(ProFeature* p_feature, ProAppData app_data)
{
	ProError status = PRO_TK_NO_ERROR;
	ProFeattype lFeatType;
	status = ProFeatureTypeGet(p_feature, &lFeatType);
	if (PRO_FEAT_DATUM_POINT == lFeatType){
		ProName wPntName;
		status = ProModelitemNameGet((ProModelitem*)p_feature, wPntName);
		ProCharName cPntName;
		ProWstringToString(cPntName, wPntName);
		if (string(cPntName).find("FO_MP") != string::npos){
			return(PRO_TK_NO_ERROR);
		}
	}
	return(PRO_TK_CONTINUE);
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

ProError VesGeomItemVisitAction(ProGeomitem* p_handle, ProError status, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	vector<ProGeomitem>* lpVecfeature = (vector<ProGeomitem>*)(app_data);
	lpVecfeature->push_back(*p_handle);
	return lErr;
}

void closeMYdialog(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);
	//status = ProUIDialogDestroy(dialog);
}

ProError placemetnSurfContourVisitAction(ProContour p_contour, ProError status, ProAppData app_data)
{
	ProContour* extContour = (ProContour*)app_data;
	ProContourTraversal contourTraversal;
	status = ProContourTraversalGet(p_contour, &contourTraversal);
	if (contourTraversal == PRO_CONTOUR_TRAV_EXTERNAL) {
		extContour[0] = p_contour;
	}
	return PRO_TK_NO_ERROR;
}

ProError contourEdgeVisitAction(ProEdge edge, ProError status, ProAppData data)
{
	ProEnttype p_edge_type;
	status = ProEdgeTypeGet(edge, &p_edge_type);
	if (p_edge_type == PRO_ENT_LINE) {
		contourEdges.push_back(edge);
		int id;
		status = ProEdgeIdGet(edge, &id);
		int i = 9;
	}
	return PRO_TK_NO_ERROR;
}

ProError featPointVisitAction(ProGeomitem* geomitem, ProError err, ProAppData appdata)
{
	ProGeomitem* point = (ProGeomitem*)appdata;
	point[0] = geomitem[0];
	return PRO_TK_NO_ERROR;
}

void placeBondpadAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(paramsDialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("edge,surface", 1, NULL, NULL,
		NULL, NULL, &pntPlacementSel, &nSel);

	/*ProUvParam sel_uv_param, sel_uv_param_1;
	status = ProSelectionUvParamGet(pntPlacementSel[0], sel_uv_param);
	sel_uv_param_1[0] = 2.3;
	sel_uv_param_1[1] = 300;
	status = ProSelectionUvParamSet(pntPlacementSel[0], sel_uv_param_1);*/

	ProName wPointName;
	ProCharName cPointName;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node name:");
	status = ProMessageStringRead(10, wPointName);
	ProWstringToString(cPointName, wPointName);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	//ProSelection* pntLocSel;
	//int nSel;
	//status = ProSelect((char*)"point", 1, NULL, NULL, NULL, NULL, &pntLocSel, &nSel);
	//status = ProSelectionModelitemGet(pntLocSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	/*ProExtRefInfo* extRefsInfo = NULL;
	int nExtRefsInfo = 0;
	status = ProFeatureExternParentsGet(&pntFeat, PRO_ALL_REF_TYPES, &extRefsInfo, &nExtRefsInfo);
	for (int i = 0; i < nExtRefsInfo; ++i)
		for (int j = 0; j < extRefsInfo[i].n_refs; j++)
			status = ProExtRefModelitemGet(extRefsInfo[i].ext_refs[j], &placementRefItem);
	//status = ProSelectionAlloc(&comp_path, &placementRefItem, &placementRef1Sel);	// selection ref*/

	status = ProSelectionModelitemGet(pntPlacementSel[0], &placementRefItem);
	// IF SELECTED REF IS EDGE
	if (placementRefItem.type == PRO_EDGE)
	{
		ProEdge pntRefEdge, neighborEdge1, neighborEdge2;
		ProSurface neighborSurf1, neighborSurf2;
		int neighborSurf1Id, neighborSurf2Id;
		ProSrftype surfType;

		status = ProGeomitemToEdge((ProGeomitem*)&placementRefItem, &pntRefEdge);
		status = ProEdgeNeighborsGet(pntRefEdge, &neighborEdge1, &neighborEdge2, &neighborSurf1, &neighborSurf2);
		status = ProSurfaceIdGet(neighborSurf1, &neighborSurf1Id);
		status = ProSurfaceIdGet(neighborSurf2, &neighborSurf2Id);

		status = ProSurfaceTypeGet(neighborSurf1, &surfType);
		if (surfType == PRO_SRF_CYL)
		{
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf1, &placementSurfItem);
			status = ProSelectionAlloc(NULL, &placementSurfItem, &placementSurfSel);	//#3
		}
		else if (surfType == PRO_SRF_PLANE)
		{
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf1, &dirSurfItem);
			status = ProSelectionAlloc(NULL, &dirSurfItem, &dirSurfSel);	//#4
		}

		status = ProSurfaceTypeGet(neighborSurf2, &surfType);
		if (surfType == PRO_SRF_CYL)
		{
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf2, &placementSurfItem);
			status = ProSelectionAlloc(NULL, &placementSurfItem, &placementSurfSel);	//#3
		}
		else if (surfType == PRO_SRF_PLANE)
		{
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf2, &dirSurfItem);
			status = ProSelectionAlloc(NULL, &dirSurfItem, &dirSurfSel);	//#4
		}

		if ((placementSurfSel != NULL) && (dirSurfSel != NULL))
		{
			bondpadUDFsels.push_back(pntPlacementSel[0]);
			bondpadUDFsels.push_back(pntSel);
			bondpadUDFsels.push_back(placementSurfSel);
			bondpadUDFsels.push_back(dirSurfSel);
			LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
		}
		bondpadUDFsels.clear();
	}

	// IF SELECTED REF IS SURFACE
	else if (placementRefItem.type == PRO_SURFACE)
	{
		ProSurface pntRefSurface;
		ProContour externalContour;
		ProSelection reqNearestEdgeSel;
		ProEdge reqNearestEdge{};
		status = ProGeomitemToSurface((ProGeomitem*)&placementRefItem, &pntRefSurface);
		status = ProSurfaceContourVisit(pntRefSurface, placemetnSurfContourVisitAction, NULL, (ProAppData)&externalContour);
		contourEdges.clear();
		status = ProContourEdgeVisit(pntRefSurface, externalContour, contourEdgeVisitAction, NULL, NULL);

		// finding the direction edge
		double edge1Dist, edge2Dist;
		ProSelection edge1Sel, edge2Sel;
		ProGeomitem edgeGeomitem;
		ProModelitem edgeModelitem;
		Pro2dPnt param_1, param_2;
		Pro3dPnt pnt_1, pnt_2;
		// 1
		status = ProEdgeToGeomitem((ProSolid)CurMdl1, contourEdges[0], &edgeGeomitem);
		status = ProModelitemInit(edgeGeomitem.owner, edgeGeomitem.id, edgeGeomitem.type, &edgeModelitem);
		status = ProSelectionAlloc(NULL, &edgeModelitem, &edge1Sel);
		status = ProSelectionWithOptionsDistanceEval(pntSel, PRO_B_FALSE, edge1Sel, PRO_B_TRUE, param_1, param_2, pnt_1, pnt_1, &edge1Dist);
		// 2
		status = ProEdgeToGeomitem((ProSolid)CurMdl1, contourEdges[1], &edgeGeomitem);
		status = ProModelitemInit(edgeGeomitem.owner, edgeGeomitem.id, edgeGeomitem.type, &edgeModelitem);
		status = ProSelectionAlloc(NULL, &edgeModelitem, &edge2Sel);
		status = ProSelectionWithOptionsDistanceEval(pntSel, PRO_B_FALSE, edge2Sel, PRO_B_TRUE, param_1, param_2, pnt_1, pnt_1, &edge2Dist);

		if (edge1Dist < edge2Dist) {
			ProSelectionCopy(edge1Sel, &reqNearestEdgeSel);
			reqNearestEdge = contourEdges[0];
		}
		else if (edge2Dist < edge1Dist) {
			ProSelectionCopy(edge2Sel, &reqNearestEdgeSel);
			reqNearestEdge = contourEdges[1];
		}

		// finding the direction surface
		ProEdge pntRefEdge, neighborEdge1, neighborEdge2;
		ProSurface neighborSurf1, neighborSurf2;
		ProSrftype surfType;
		status = ProEdgeNeighborsGet(reqNearestEdge, &neighborEdge1, &neighborEdge2, &neighborSurf1, &neighborSurf2);
		status = ProSurfaceTypeGet(neighborSurf1, &surfType);
		if (surfType == PRO_SRF_PLANE)
		{
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf1, &dirSurfItem);
			status = ProSelectionAlloc(NULL, &dirSurfItem, &dirSurfSel);
		}
		status = ProSurfaceTypeGet(neighborSurf2, &surfType);
		if (surfType == PRO_SRF_PLANE)
		{
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, neighborSurf2, &dirSurfItem);
			status = ProSelectionAlloc(NULL, &dirSurfItem, &dirSurfSel);
		}

		bondpadUDFsels.push_back(reqNearestEdgeSel);
		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		bondpadUDFsels.push_back(dirSurfSel);
		LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);//Give only non suppressed entities

	ProParamvalue nodeNameParamVal;
	ProParameter nodeNameParam;

	status = ProParamvalueSet(&nodeNameParamVal, (void*)wPointName, PRO_PARAM_STRING);
	status = ProParameterInit(&feats[7], L"NODE_NAME", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[7], L"NODE_NAME", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	status = ProModelitemNameSet(&feats[7], wPointName);

	double clearanceVal, traceMarginVal;

	status = ProUIInputpanelDoubleGet(paramsDialog, ip_clearance, &clearanceVal);
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&clearanceVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[7], L"FANOUT_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[7], L"FANOUT_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	status = ProUIInputpanelDoubleGet(paramsDialog, ip_trace_margin, &traceMarginVal);
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&traceMarginVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[7], L"FANOUT_TRACE_MARGIN", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[7], L"FANOUT_TRACE_MARGIN", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);

}

void placeBondpadAction2(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(paramsDialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirAxisSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	ProModelitem sidePlaneItem;
	status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);
	status = ProSelectionAlloc(NULL, &sidePlaneItem, &dirSurfSel);

	ProModelitem dirAxisItem;
	status = ProModelitemByNameInit(CurMdl1, PRO_AXIS, L"A_2", &dirAxisItem);
	status = ProSelectionAlloc(NULL, &dirAxisItem, &dirAxisSel);

	bondpadUdfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\bondpads_with_dummy_resistor.gph.1";
	bondpadUDFsels.push_back(pntSel);
	bondpadUDFsels.push_back(pntPlacementSel[0]);
	bondpadUDFsels.push_back(dirSurfSel);
	bondpadUDFsels.push_back(dirAxisSel);
	LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE);

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);//Give only non suppressed entities

	ProName wPointName;
	ProCharName cPointName;
	wchar_t* msgfil = L"Message.txt";
	ProParamvalue nodeNameParamVal;
	ProParameter nodeNameParam;

	ProSelection nodeSelection;
	ProIdTable c_id_table;
	c_id_table[0] = -1;
	ProAsmcomppath comp_path;
	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);

	// set FEAT[18] name
	status = ProSelectionAlloc(&comp_path, &feats[18], &nodeSelection);
	status = ProSelectionHighlight(nodeSelection, PRO_COLOR_HIGHLITE);

	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPointName);
	ProWstringToString(cPointName, wPointName);

	status = ProParamvalueSet(&nodeNameParamVal, (void*)wPointName, PRO_PARAM_STRING);
	status = ProParameterInit(&feats[18], L"NODE_NAME", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[18], L"NODE_NAME", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	status = ProModelitemNameSet(&feats[18], wPointName);

	// set FEAT[19] name
	status = ProSelectionAlloc(&comp_path, &feats[19], &nodeSelection);
	status = ProSelectionHighlight(nodeSelection, PRO_COLOR_HIGHLITE);

	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPointName);
	ProWstringToString(cPointName, wPointName);

	status = ProParamvalueSet(&nodeNameParamVal, (void*)wPointName, PRO_PARAM_STRING);
	status = ProParameterInit(&feats[19], L"NODE_NAME", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[19], L"NODE_NAME", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	status = ProModelitemNameSet(&feats[19], wPointName);

	bondpadUDFsels.clear();

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);
}

ProError ProUtilCollectCurveComponentVisitAction(
	ProCurve     p_curve,   /* In:  The composite curve being processed */
	ProCurve     p_comp,    /* In:  The current component being visited */
	int          comp_idx,  /* In:  The index of the current component in the curve */
	ProBoolean   comp_flip, /* In:  This is PRO_TK_TRUE if the current component is flipped */
	ProError     status,	/* In:  The status returned by the filter */
	vector<pnt_ord>* pnt_co_ords)  /* In:  In fact it's CurveComponent**  */
{
	ProVector xyz_pnt, derv1, derv2;
	status = ProCurveXyzdataEval(p_comp, 0, xyz_pnt, derv1, derv2);
	pnt_co_ords->push_back({ xyz_pnt[0], xyz_pnt[1], xyz_pnt[2] });
	status = ProCurveXyzdataEval(p_comp, 1, xyz_pnt, derv1, derv2);
	pnt_co_ords->push_back({ xyz_pnt[0], xyz_pnt[1], xyz_pnt[2] });

	int i = 0;
	return PRO_TK_NO_ERROR;
}

ProError featCurveGeomitemVisit(ProGeomitem& p_handle, ProError status, ProAppData app_data)
{
	double* reqPnt = (double*)app_data;
	ProCurve middleCurve;
	vector<pnt_ord> pointCoOrds;
	status = ProCurveInit((ProSolid)p_handle.owner, p_handle.id, &middleCurve);
	ProModelitem curveItem;
	//status=ProCurveToGeomitem((ProSolid)p_handle.owner , middleCurve, &curveItem);

	status = ProModelitemInit(p_handle.owner, p_handle.id, PRO_CURVE, &curveItem);
	wchar_t wMidleCurveName[PRO_NAME_SIZE]; char cMiddleCurveName[PRO_NAME_SIZE];
	status = ProModelitemNameGet(&curveItem, wMidleCurveName);
	ProWstringToString(cMiddleCurveName, wMidleCurveName);
	sMiddleCurveName = string(cMiddleCurveName);

	status = ProCurveCompVisit(middleCurve, (ProCurveCompAction)ProUtilCollectCurveComponentVisitAction, NULL, &pointCoOrds);
	if (status == PRO_TK_NO_ERROR && pointCoOrds.size() > 0)
	{
		// Set to store unique values
		std::set<pnt_ord> uniqueValues;

		// Vector to store repeated values
		//std::vector<pnt_ord> repeatedValues;

		// Iterate through the vector
		for (auto& myStruct : pointCoOrds) {
			// Check if the value is already in the set
			if (!uniqueValues.insert(myStruct).second) {
				// Value is repeated
				//repeatedValues.push_back(myStruct);
				reqPnt[0] = myStruct.x;
				reqPnt[1] = myStruct.y;
				reqPnt[2] = myStruct.z;
				break;
			}
		}
	}

	return PRO_TK_NO_ERROR;
}




// ----------------------------------------------   SUPPORTING FUNCTIONS   ----------------------------------------------
vector<vector<Node>> extractNodePairsFromFile(const string& filename)
{
	std::vector<std::vector<Node>> nodePairs;
	std::ifstream file(filename);
	std::string line;

	while (std::getline(file, line))
	{
		if (line.find("(net") != std::string::npos)
		{
			std::vector<Node> nodes;
			while (std::getline(file, line))
			{
				if (line.find("(net") != std::string::npos)
				{
					if (!nodes.empty()) {
						nodePairs.push_back(nodes);
						nodes.clear();
					}
					//break;
				}
				if (line.find("(node") != std::string::npos)
				{
					Node node;
					size_t pos = line.find("ref");
					if (pos != std::string::npos)
					{
						node.ref = line.substr(pos + 5, line.find("\"", pos + 5) - pos - 5);
					}
					pos = line.find("pin");
					if (pos != std::string::npos)
					{
						node.pin = line.substr(pos + 5, line.find("\"", pos + 5) - pos - 5);
					}
					nodes.push_back(node);
				}
				if (file.eof())
				{ // Handle the end of the file
					if (!nodes.empty()) {
						nodePairs.push_back(nodes);
					}
					break;
				}
			}
		}
	}

	return nodePairs;
}

int createCurveBw2Points(const ProMdl& CurMdl1, std::string& point1, ProFeature& csoFeat, PointData& curPointData, std::string& point2, ProFeature& csoFeat1, ProError& status, int surfaceid, ProAsmcomppath& comp_path, std::vector<PointData> vectPointData, std::vector<ProSelection>& UdfInputSel, ProFeature& udfGrpFeat)
{
	vectPointData.clear();
	UdfInputSel.clear();
	bool isFeatFound = false;
	isFeatFound = GetFeatureByName(CurMdl1, point1, csoFeat);
	if (!isFeatFound)
		return -1;
	curPointData.lPointFeat = csoFeat;

	PointData curPointData1 = {};
	isFeatFound = GetFeatureByName(CurMdl1, point2, csoFeat1);
	if (!isFeatFound)
		return -1;

	curPointData1.lPointFeat = csoFeat1;
	ProSurface lSurface;
	ProGeomitem comp_datum;
	ProSelection SurfSelection1;

	status = ProSurfaceInit(CurMdl1, surfaceid, &lSurface);
	ProGeomitemdata* curveSurfData;
	status = ProSurfaceDataGet(lSurface, &curveSurfData);
	ProSrftype srftype;
	ProUvParam uv_min, uv_max;
	ProSurfaceOrient s_orient;
	ProSurfaceshapedata s_shapedata;
	int s_id;
	status = ProSurfacedataGet((ProSurfacedata*)&curveSurfData, &srftype, uv_min, uv_max, &s_orient, &s_shapedata, &s_id);

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

	string Udfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\create_curve.gph.1";
	LoadUDFwithoutDims(CurMdl1, Udfpath, UdfInputSel, udfGrpFeat, NULL, PRO_B_TRUE);

	return 0;
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

double* ProUtilVectorCopy(double from[3], double to[3])
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

void ProUtilPointTrans(double m[4][4], double p[3], double output[3])
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

void ProUtilMatrixCopy1(double input[4][4], double output[4][4])
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

int ProUtilMatrixInvert(double m[4][4], double output[4][4])
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

std::vector<std::pair<std::string, std::string>> readCSV(const std::string& filename)
{
	std::vector<std::pair<std::string, std::string>> pairs;

	std::ifstream file(filename);
	if (!file.is_open()) {
		return pairs;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string value1, value2;

		if (std::getline(iss, value1, ',') && std::getline(iss, value2, ',')) {
			value1.erase(0, value1.find_first_not_of(" \t"));
			value1.erase(value1.find_last_not_of(" \t") + 1);

			value2.erase(0, value2.find_first_not_of(" \t"));
			value2.erase(value2.find_last_not_of(" \t") + 1);

			pairs.emplace_back(value1, value2);
		}
	}

	return pairs;
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

ProError ProDemoFieldPointCreate(ProSelection placementSel, ProFeature* pointFeat)
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
	//ProStringToWstring(wide_string, cPointName);
	status = ProElementWstringSet(pro_e_std_feature_name, L"BONDPAD_PNT");
	status = ProElemtreeElementAdd(pro_e_feature_tree, NULL,
		pro_e_std_feature_name);

	/*---------------------------------------------------------------*\
	  Populating element PRO_E_DPOINT_FIELD_REF
	\*---------------------------------------------------------------*/
	status = ProElementAlloc(PRO_E_DPOINT_FIELD_REF,
		&pro_e_dpoint_field_ref);
	status = ProSelectionToReference(placementSel, &REPDEP_ref1);
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

void FillUDFDataWithReferenceForCopyFeat(ProUdfdata& UdfData, vector<ProSelection>& sel2, ProBoolean RefAsm)
{
	ProError status = PRO_TK_NO_ERROR;
	ProUdfRequiredRef* requiredRef;
	int sizeRef = 0;
	ProUdfreference* udfReference = NULL;

	status = ProUdfdataRequiredreferencesGet(UdfData, &requiredRef);
	status = ProArraySizeGet(requiredRef, &sizeRef);
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

	// UPDATE DIMS
	double heightDimVal, widthDimVal;
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_height, &heightDimVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);

	ProUdfvardim* var_dim_array;
	status = ProUdfdataVardimsGet(UdfData, &var_dim_array);
	if (status == PRO_TK_NO_ERROR)
	{
		int sizeVarDims;
		status = ProArraySizeGet(var_dim_array, &sizeVarDims);
		for (int i = 0; i < sizeVarDims; i++)
		{
			wchar_t wDimName[PRO_NAME_SIZE];
			char cDimName[PRO_NAME_SIZE], cPrompt[PRO_NAME_SIZE];
			ProLine wPrompt;
			ProUdfVardimType value_type;
			double defaultDimVal=0;

			status = ProUdfvardimDefaultvalueGet(var_dim_array[i], &value_type, &defaultDimVal);
			status = ProUdfvardimPromptGet(var_dim_array[i], wPrompt);
			ProWstringToString(cPrompt, wPrompt);

			if (strcmp(cPrompt, "width1") == 0) {
				status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal/2));	// user entered width/2
			}
			else if (strcmp(cPrompt, "width2") == 0) {
				status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal / 2));	// user entered width/2
			}
			else if (strcmp(cPrompt, "height1") == 0) {
				status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal));	// user entered height
			}
			else if (strcmp(cPrompt, "height2") == 0) {
				status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal));	// user entered height
			}
			else if (strcmp(cPrompt, "mid_height") == 0) {
				status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal- (0.3*heightDimVal)));	// user entered height - certain value
			}
			status = ProUdfdataUdfvardimAdd(UdfData, var_dim_array[i]);	// adding the vardim to UdfData
		}
	}
	ProUdfvardimProarrayFree(var_dim_array);

	for (int i = 0; i < sizeRef; i++)
	{
		status = ProUdfreferenceFree(udfReference[i]);
		udfReference[i] = NULL;
	}
	status = ProArrayFree((ProArray*)&udfReference);
	udfReference = NULL;
}

bool LoadUDFwithoutDims(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/)
{
	ProError status = PRO_TK_NO_ERROR;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	ProUdfRequiredRef* requiredRef;
	int sizeRef = 0;
	ProUdfreference* udfReference = NULL;
	ProPath UdfFilePath;

	status = ProUdfdataAlloc(&UdfData);
	ProStringToWstring(UdfFilePath, (char*)UDFFileName.c_str());
	status = ProUdfdataPathSet(UdfData, UdfFilePath);

	status = ProUdfdataRequiredreferencesGet(UdfData, &requiredRef);
	status = ProArraySizeGet(requiredRef, &sizeRef);
	status = ProArrayAlloc(sizeRef, sizeof(ProUdfreference), 1, (ProArray*)&udfReference);
	status = ProUdfdataDimdisplaySet(UdfData, PROUDFDIMDISP_NORMAL);
	for (int i = 0; i < sizeRef; i++)
	{
		udfReference[i] = NULL;
		ProLine prompt;
		ProType type;
		status = ProUdfrequiredrefPromptGet(requiredRef[i], prompt);
		status = ProUdfrequiredrefTypeGet(requiredRef[i], &type);
		status = ProUdfreferenceAlloc(prompt, UDFInputs[i], RefAsm, &udfReference[i]); //For Assembly RefAsm = TRUE
		status = ProUdfdataReferenceAdd(UdfData, udfReference[i]);
	}

	for (int i = 0; i < sizeRef; i++)
	{
		status = ProUdfreferenceFree(udfReference[i]);
		udfReference[i] = NULL;
	}
	status = ProArrayFree((ProArray*)&udfReference);
	udfReference = NULL;


	ProUdfCreateOption option[] = { PROUDFOPT_NO_REDEFINE };
	ProName udfNameOrginalName = L"NewName";
	status = ProUdfdataNameSet(UdfData, udfNameOrginalName, udfNameOrginalName); //TODO why this? not working chk
	//status = ProUdfdataInstancenameSet(UdfData, udfNameOrginalName);
	ProFeature createdFeature;
	status = ProUdfCreate((ProSolid)Mdl, UdfData, NULL, option, 1, &createdFeature);
	if (status == PRO_TK_NO_ERROR)
		isCreatedUDF = true;
	UDFGrpFeat = createdFeature;

	status = ProTreetoolRefresh((ProSolid)Mdl);

	return isCreatedUDF;
}

bool LoadBondpadUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/, ProBoolean modifyDim)
{
	ProError status = PRO_TK_NO_ERROR;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);

	ProPath UdfFilePath;


	ProStringToWstring(UdfFilePath, (char*)UDFFileName.c_str());

	status = ProUdfdataPathSet(UdfData, UdfFilePath);
	//FillUDFDataWithReferenceForCopyFeat(UdfData, UDFInputs, RefAsm);

	ProUdfRequiredRef* requiredRef;
	int sizeRef = 0;
	ProUdfreference* udfReference = NULL;

	status = ProUdfdataRequiredreferencesGet(UdfData, &requiredRef);
	status = ProArraySizeGet(requiredRef, &sizeRef);
	status = ProArrayAlloc(sizeRef, sizeof(ProUdfreference), 1, (ProArray*)&udfReference);
	status = ProUdfdataDimdisplaySet(UdfData, PROUDFDIMDISP_NORMAL);
	for (int i = 0; i < sizeRef; i++)
	{
		udfReference[i] = NULL;
		ProLine prompt;
		ProType type;
		status = ProUdfrequiredrefPromptGet(requiredRef[i], prompt);
		status = ProUdfrequiredrefTypeGet(requiredRef[i], &type);
		status = ProUdfreferenceAlloc(prompt, UDFInputs[i], RefAsm, &udfReference[i]); //For Assembly RefAsm = TRUE
		status = ProUdfdataReferenceAdd(UdfData, udfReference[i]);
	}

	if(modifyDim)
	{
		// UPDATE DIMS
		double heightDimVal, widthDimVal;
		status = ProUIInputpanelDoubleGet(paramsDialog, ip_height, &heightDimVal);
		status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);

		ProUdfvardim* var_dim_array;
		status = ProUdfdataVardimsGet(UdfData, &var_dim_array);
		if (status == PRO_TK_NO_ERROR)
		{
			int sizeVarDims;
			status = ProArraySizeGet(var_dim_array, &sizeVarDims);
			for (int i = 0; i < sizeVarDims; i++)
			{
				wchar_t wDimName[PRO_NAME_SIZE];
				char cDimName[PRO_NAME_SIZE], cPrompt[PRO_NAME_SIZE];
				ProLine wPrompt;
				ProUdfVardimType value_type;
				double defaultDimVal = 0;

				status = ProUdfvardimDefaultvalueGet(var_dim_array[i], &value_type, &defaultDimVal);
				status = ProUdfvardimPromptGet(var_dim_array[i], wPrompt);
				ProWstringToString(cPrompt, wPrompt);

				if (strcmp(cPrompt, "width1") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal / 2));	// user entered width/2
				}
				else if (strcmp(cPrompt, "width2") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal / 2));	// user entered width/2
				}
				else if (strcmp(cPrompt, "height1") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal));	// user entered height
				}
				else if (strcmp(cPrompt, "height2") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal));	// user entered height
				}
				else if (strcmp(cPrompt, "mid_height") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal - (0.3 * heightDimVal)));	// user entered height - certain value
				}
				status = ProUdfdataUdfvardimAdd(UdfData, var_dim_array[i]);	// adding the vardim to UdfData
			}
		}
		ProUdfvardimProarrayFree(var_dim_array);
	}
	
	for (int i = 0; i < sizeRef; i++)
	{
		status = ProUdfreferenceFree(udfReference[i]);
		udfReference[i] = NULL;
	}
	status = ProArrayFree((ProArray*)&udfReference);
	udfReference = NULL;

	//ProUdfCreateOption option[] = { PROUDFOPT_EDIT_MENU };
	ProUdfCreateOption option[] = { PROUDFOPT_NO_REDEFINE };

	//ProName udfNameOrginalName = L"ghfghf";
	//status = ProUdfdataNameSet(UdfData, udfNameOrginalName, NULL); //TODO why this? not working chk
	//status = ProUdfdataInstancenameSet(UdfData, udfNameOrginalName);
	ProFeature createdFeature;
	status = ProUdfCreate((ProSolid)Mdl, UdfData, NULL, option, 1, &createdFeature);
	if (status == PRO_TK_NO_ERROR)
		isCreatedUDF = true;
	UDFGrpFeat = createdFeature;

	status = ProTreetoolRefresh((ProSolid)Mdl);

	return isCreatedUDF;
}

bool LoadCurveUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm, double widthDimVal /*For Asm it is TRUE*/)
{
	ProError status = PRO_TK_NO_ERROR;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	ProUdfRequiredRef* requiredRef;
	int sizeRef = 0;
	ProUdfreference* udfReference = NULL;
	ProPath UdfFilePath;

	status = ProUdfdataAlloc(&UdfData);
	ProStringToWstring(UdfFilePath, (char*)UDFFileName.c_str());
	status = ProUdfdataPathSet(UdfData, UdfFilePath);

	status = ProUdfdataRequiredreferencesGet(UdfData, &requiredRef);
	status = ProArraySizeGet(requiredRef, &sizeRef);
	status = ProArrayAlloc(sizeRef, sizeof(ProUdfreference), 1, (ProArray*)&udfReference);
	status = ProUdfdataDimdisplaySet(UdfData, PROUDFDIMDISP_NORMAL);
	for (int i = 0; i < sizeRef; i++)
	{
		udfReference[i] = NULL;
		ProLine prompt;
		ProType type;
		status = ProUdfrequiredrefPromptGet(requiredRef[i], prompt);
		status = ProUdfrequiredrefTypeGet(requiredRef[i], &type);
		status = ProUdfreferenceAlloc(prompt, UDFInputs[i], RefAsm, &udfReference[i]); //For Assembly RefAsm = TRUE
		status = ProUdfdataReferenceAdd(UdfData, udfReference[i]);
	}

	// UPDATE DIMS
	/*double widthDimVal;
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);*/

	ProUdfvardim* var_dim_array;
	status = ProUdfdataVardimsGet(UdfData, &var_dim_array);
	if (status == PRO_TK_NO_ERROR)
	{
		int sizeVarDims;
		status = ProArraySizeGet(var_dim_array, &sizeVarDims);
		for (int i = 0; i < sizeVarDims; i++)
		{
			wchar_t wDimName[PRO_NAME_SIZE];
			char cDimName[PRO_NAME_SIZE], cPrompt[PRO_NAME_SIZE];
			ProLine wPrompt;
			ProUdfVardimType value_type;
			double defaultDimVal = 0;

			status = ProUdfvardimDefaultvalueGet(var_dim_array[i], &value_type, &defaultDimVal);
			status = ProUdfvardimPromptGet(var_dim_array[i], wPrompt);
			ProWstringToString(cPrompt, wPrompt);

			if (strcmp(cPrompt, "width_by_2") == 0) {
				status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal / 2));	// user entered width/2
			}
			else if (strcmp(cPrompt, "offset_end") == 0) {
				status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal/2)+(widthDimVal/4));	// user entered width/2
			}
			status = ProUdfdataUdfvardimAdd(UdfData, var_dim_array[i]);	// adding the vardim to UdfData
		}
	}
	ProUdfvardimProarrayFree(var_dim_array);

	for (int i = 0; i < sizeRef; i++)
	{
		status = ProUdfreferenceFree(udfReference[i]);
		udfReference[i] = NULL;
	}
	status = ProArrayFree((ProArray*)&udfReference);
	udfReference = NULL;


	ProUdfCreateOption option[] = { PROUDFOPT_NO_REDEFINE };
	ProName udfNameOrginalName = L"NewName";
	status = ProUdfdataNameSet(UdfData, udfNameOrginalName, udfNameOrginalName); //TODO why this? not working chk
	//status = ProUdfdataInstancenameSet(UdfData, udfNameOrginalName);
	ProFeature createdFeature;
	status = ProUdfCreate((ProSolid)Mdl, UdfData, NULL, option, 1, &createdFeature);
	if (status == PRO_TK_NO_ERROR)
		isCreatedUDF = true;
	UDFGrpFeat = createdFeature;

	status = ProTreetoolRefresh((ProSolid)Mdl);

	return isCreatedUDF;
}

void CreateSurfaceBwCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfOffsetCurve1Feat, ProFeature& udfOffsetCurve2Feat)
{
	ProSurface lSurface;
	ProGeomitem comp_datum;
	ProSelection SurfSelection1;

	status = ProSurfaceInit(CurMdl1, surfaceid, &lSurface);

	UdfInputSel.clear();
	
	ProFeature* childfeats1 = NULL;
	status = ProGroupFeaturesCollect(&udfOffsetCurve1Feat, &childfeats1);//Give only non suppressed entities
	vector<ProGeomitem> vecGeomItems2;
	GetGeomItems(CurMdl1, childfeats1[1], vecGeomItems2);
	ProModelitem p_CurveStarthandle2;
	ProModelitem p_Curvehandle2;
	ProModelitem p_CurveEndhandle2;

	ProSelection childCurveSelection1_1;
	ProSelection childCurveSelection2_1;
	ProSelection childSelection2_1;

	int FullCurveGEomitemId2;
	GetMamximumCurveId(vecGeomItems2, status, CurMdl1, FullCurveGEomitemId2);
	for (int i = 0; i < vecGeomItems2.size(); i++)
	{
		ProModelitem curvMdlItem;
		if (vecGeomItems2[i].id == FullCurveGEomitemId2 && vecGeomItems2[i].type == PRO_CURVE)
		{
			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems2[i].id, PRO_CURVE, &p_Curvehandle2);
			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems2[i].id, PRO_CRV_START, &p_CurveStarthandle2);
			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems2[i].id, PRO_CRV_END, &p_CurveEndhandle2);
			status = ProSelectionAlloc(&comp_path, &p_CurveStarthandle2, &childCurveSelection1_1);
			status = ProSelectionAlloc(&comp_path, &p_CurveEndhandle2, &childCurveSelection2_1);
			status = ProSelectionAlloc(&comp_path, &p_Curvehandle2, &childSelection2_1);
			break;
		}
	}
	status = ProModelitemHide(&childfeats1[1]);

	ProFeature* childfeats2 = NULL;
	status = ProGroupFeaturesCollect(&udfOffsetCurve2Feat, &childfeats2);//Give only non suppressed entities
	vector<ProGeomitem> vecGeomItems3;
	GetGeomItems(CurMdl1, childfeats2[1], vecGeomItems3);
	ProModelitem p_CurveStarthandle3;
	ProModelitem p_Curvehandle3;
	ProModelitem p_CurveEndhandle3;
	
	ProSelection childCurveSelection1_2;
	ProSelection childCurveSelection2_2;
	ProSelection childSelection2_2;
	int FullCurveGEomitemId3;
	GetMamximumCurveId(vecGeomItems3, status, CurMdl1, FullCurveGEomitemId3);
	for (int i = 0; i < vecGeomItems3.size(); i++)
	{
		ProModelitem curvMdlItem;
		//if (i == vecGeomItems3.size() - 3)
		{
			if (vecGeomItems3[i].id == FullCurveGEomitemId3 && vecGeomItems3[i].type == PRO_CURVE)
			{
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CURVE, &p_Curvehandle3);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CRV_START, &p_CurveStarthandle3);
				status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems3[i].id, PRO_CRV_END, &p_CurveEndhandle3);
				status = ProSelectionAlloc(&comp_path, &p_CurveStarthandle3, &childCurveSelection1_2);
				status = ProSelectionAlloc(&comp_path, &p_CurveEndhandle3, &childCurveSelection2_2);
				status = ProSelectionAlloc(&comp_path, &p_Curvehandle3, &childSelection2_2);
				//ProSelectionHighlight(CurveSelection1, PRO_COLOR_CURVE);
				//ProSelectionHighlight(childSelection2_2, PRO_COLOR_HIGHLITE);
				break;
			}
		}
	}
	status = ProModelitemHide(&childfeats2[1]);

	UdfInputSel.push_back(childSelection2_1);
	UdfInputSel.push_back(childSelection2_2);
	ProSelectionHighlight(childSelection2_1, PRO_COLOR_HIGHLITE);
	ProSelectionHighlight(childSelection2_2, PRO_COLOR_HIGHLITE);
	ProFeature UDFGrpFeat4;
	string Udfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\create_surface.gph.1";
	
	LoadUDFwithoutDims(CurMdl1, Udfpath, UdfInputSel, UDFGrpFeat4, NULL, PRO_B_TRUE);

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&UDFGrpFeat4, &feats);//Give only non suppressed entities

	//status = ProModelitemNameSet(&feats[7], wPointName);
}

void createTwoOffsetCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfCurveFeat, ProFeature& udfOffsetCurve1Feat, ProFeature& udfOffsetCurve2Feat)
{
	ProSelection SurfSelection1;

	ProSurface lSurface;
	ProGeomitem comp_datum;
	status = ProSurfaceInit(CurMdl1, surfaceid, &lSurface);
	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, lSurface, &comp_datum);
	status = ProSelectionAlloc(&comp_path, &comp_datum, &SurfSelection1);

	ProFeature* childfeats = NULL;
	status = ProGroupFeaturesCollect(&udfCurveFeat, &childfeats);//Give only non suppressed entities

	vector<ProGeomitem> vecGeomItems1;
	GetGeomItems(CurMdl1, childfeats[1], vecGeomItems1);

	ProModelitem p_CurveStarthandle1;
	ProModelitem p_Curvehandle1;
	ProModelitem p_CurveEndhandle1;

	ProSelection childCurveSelection1;
	ProSelection childCurveSelection2;
	ProSelection childSelection2;

	int FullCurveGEomitemId1;
	GetMamximumCurveId(vecGeomItems1, status, CurMdl1, FullCurveGEomitemId1);
	for (int i = 0; i < vecGeomItems1.size(); i++)
	{
		ProModelitem curvMdlItem;
		if (vecGeomItems1[i].id == FullCurveGEomitemId1 && vecGeomItems1[i].type == PRO_CURVE)
		{
			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems1[i].id, PRO_CURVE, &p_Curvehandle1);
			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems1[i].id, PRO_CRV_START, &p_CurveStarthandle1);
			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems1[i].id, PRO_CRV_END, &p_CurveEndhandle1);
			status = ProSelectionAlloc(&comp_path, &p_CurveStarthandle1, &childCurveSelection1);
			status = ProSelectionAlloc(&comp_path, &p_CurveEndhandle1, &childCurveSelection2);
			status = ProSelectionAlloc(&comp_path, &p_Curvehandle1, &childSelection2);
			break;
		}
	}
	status = ProModelitemHide(&childfeats[1]);

	UdfInputSel.clear();
	UdfInputSel.push_back(SurfSelection1);
	UdfInputSel.push_back(childSelection2);
	UdfInputSel.push_back(childCurveSelection2);
	//UdfInputSel.push_back(SurfSelection1);
	string Udfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\end_2_offset_curve.gph.1";

	double widthDimVal;
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);

	LoadCurveUDF(CurMdl1, Udfpath, UdfInputSel, udfOffsetCurve1Feat, NULL, PRO_B_TRUE, widthDimVal);
	widthDimVal *= -1;
	LoadCurveUDF(CurMdl1, Udfpath, UdfInputSel, udfOffsetCurve2Feat, NULL, PRO_B_TRUE, widthDimVal);

}


//void CreateOffsetCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfCurveFeat, ProFeature& udfOffsetCurveFeat)
//{
//	ProSelection SurfSelection1;
//
//	ProSurface lSurface;
//	ProGeomitem comp_datum;
//	status = ProSurfaceInit(CurMdl1, surfaceid, &lSurface);
//	status = ProSurfaceToGeomitem((ProSolid)CurMdl1, lSurface, &comp_datum);
//	status = ProSelectionAlloc(&comp_path, &comp_datum, &SurfSelection1);
//
//	ProFeature* childfeats = NULL;
//	status = ProGroupFeaturesCollect(&udfCurveFeat, &childfeats);//Give only non suppressed entities
//
//	vector<ProGeomitem> vecGeomItems1;
//	GetGeomItems(CurMdl1, childfeats[1], vecGeomItems1);
//
//	ProModelitem p_CurveStarthandle1;
//	ProModelitem p_Curvehandle1;
//	ProModelitem p_CurveEndhandle1;
//
//	ProSelection childCurveSelection1;
//	ProSelection childCurveSelection2;
//	ProSelection childSelection2;
//
//	int FullCurveGEomitemId1;
//	GetMamximumCurveId(vecGeomItems1, status, CurMdl1, FullCurveGEomitemId1);
//	for (int i = 0; i < vecGeomItems1.size(); i++)
//	{
//		ProModelitem curvMdlItem;
//		if (vecGeomItems1[i].id == FullCurveGEomitemId1 && vecGeomItems1[i].type == PRO_CURVE)
//		{
//			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems1[i].id, PRO_CURVE, &p_Curvehandle1);
//			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems1[i].id, PRO_CRV_START, &p_CurveStarthandle1);
//			status = ProModelitemInit((ProSolid)CurMdl1, vecGeomItems1[i].id, PRO_CRV_END, &p_CurveEndhandle1);
//			status = ProSelectionAlloc(&comp_path, &p_CurveStarthandle1, &childCurveSelection1);
//			status = ProSelectionAlloc(&comp_path, &p_CurveEndhandle1, &childCurveSelection2);
//			status = ProSelectionAlloc(&comp_path, &p_Curvehandle1, &childSelection2);
//			break;
//		}
//	}
//	status = ProModelitemHide(&childfeats[1]);
//
//	UdfInputSel.clear();
//	UdfInputSel.push_back(SurfSelection1);
//	UdfInputSel.push_back(childSelection2);
//	UdfInputSel.push_back(childCurveSelection1);
//	string Udfpath = "C:\\Users\\venki\\Downloads\\3DPrinter (28)\\3DPrinter (28)\\3DPrinter\\text\\UDF_LATEST\\offset_curve.gph.1";
//
//	LoadUDFwithoutDims(CurMdl1, Udfpath, UdfInputSel, udfOffsetCurveFeat, NULL, PRO_B_TRUE);
//}

extern "C" void user_terminate()
{
}
