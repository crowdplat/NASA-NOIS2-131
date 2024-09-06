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
	uiCmdCmdId cmdPlaceTemplates, cmdModifyBondpad;
	lErr = ProToolkitApplTextPathGet(wTextDirPath);
	ProWstringToString(cTextDirPath, wTextDirPath);

	lErr = ProMenubarMenuAdd("TADDll_Menu", "TADDll_Menu", "Help", PRO_B_TRUE, FileName);
	lErr = ProCmdActionAdd("place_templates_action", (uiCmdCmdActFn)Create, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_FALSE, &cmdPlaceTemplates);
	lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "TADDll_PushButton", "Template_PushButton", "Template_ToolTip", NULL, PRO_B_TRUE, cmdPlaceTemplates, FileName);

	lErr = ProCmdActionAdd("update_bondpad_action", (uiCmdCmdActFn)bondPadModifyAction, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_FALSE, &cmdModifyBondpad);
	lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "update_bondpad_pb", "update_bondpad_pb_lbl", "update_bondpad_pb_tooltip", NULL, PRO_B_TRUE, cmdModifyBondpad, FileName);


	//lErr = ProNotificationSet(PRO_FEATURE_CREATE_POST, (ProFunction)mirrorFeatCreatePostAction);

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
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_3, (ProUIAction)bondPadModifyAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_4, (ProUIAction)executeLcrNetworkPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_5, (ProUIAction)executeSerpentineResistorPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_6, (ProUIAction)executeInterdigitatedCapacitorPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_7, (ProUIAction)executeBondpadPairPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_8, (ProUIAction)executeLargeBondpadPairPbAction, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_9, (ProUIAction)executeResistorNetwork1Action, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_10, (ProUIAction)executeResistorNetwork2Action, NULL);
		lErr = ProUIPushbuttonActivateActionSet(mainDialog, pb_11, (ProUIAction)executeResistorNetwork2InvAction, NULL);
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
	//char* bondpadUdfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_with_dims.gph.1";
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	//strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\bonpad_with_ang_dim.gph.1");
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\bondpad_tilted_with_dims.gph.1");
	//char* bondpadUdfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_udf.gph.1";

	status = ProUIDialogAboveactivewindowSet(paramsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, okbtn, (ProUIAction)placeBondpadAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialog, closeMYdialog, NULL);
	/*status = ProUILabelHide(paramsDialog, "offset_x_lbl");
	status = ProUIInputpanelHide(paramsDialog, ip_x_ofst);
	status = ProUILabelHide(paramsDialog, "offset_y_lbl");
	status = ProUIInputpanelHide(paramsDialog, ip_y_ofst);
	status = ProUILabelHide(paramsDialog, "fanout_boundary_lbl");
	status = ProUIInputpanelHide(paramsDialog, ip_trace_margin);*/
	//status = ProUILayoutHide(paramsDialog, layout_fanout_attributes);
	//status = ProUILayoutHide(paramsDialog, layout_clearance);

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
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\bondpad_placement_with_dims.gph.1");
	//char* bondpadUdfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_with_dims.gph.1";

	status = ProUIDialogCreate(paramsDialog, paramsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	lErr = ProUdfdataAlloc(&UdfData);

	status = ProUIDialogAboveactivewindowSet(paramsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, okbtn,(ProUIAction) placeBondpadAction2, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialog, closeMYdialog, NULL);
	/*status = ProUILabelHide(paramsDialog, "offset_x_lbl");
	status = ProUIInputpanelHide(paramsDialog, "offset_x_ip");
	status = ProUILabelHide(paramsDialog, "offset_y_lbl");
	status = ProUIInputpanelHide(paramsDialog, "offset_y_ip");*/
	//status = ProUILayoutHide(paramsDialog, layout_fanout_attributes);
	//status = ProUILayoutHide(paramsDialog, layout_clearance);

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
	status = ProUIPushbuttonActivateActionSet(paramsDialog, okbtn, (ProUIAction)placeFanoutAction, (ProAppData)wNetlistPath);
	status = ProUIPushbuttonActivateActionSet(paramsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialog, closeMYdialog, NULL);
	status = ProUILabelHide(paramsDialog, "height_lbl");
	status = ProUILabelHide(paramsDialog, "height_units");
	status = ProUIInputpanelHide(paramsDialog, "height_ip");
	status = ProUILabelHide(paramsDialog, "angle_lbl");
	status = ProUILabelHide(paramsDialog, "angle_lbl1");
	status = ProUILabelHide(paramsDialog, "angle_units");
	status = ProUIInputpanelHide(paramsDialog, "angle_ip");
	status = ProUILayoutHide(paramsDialog, "desc_layout");
	status = ProUILayoutHide(paramsDialog, "fanout_attributes_layout");

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

int placeFanoutAction(char* dialog, char* component, ProAppData appdata)
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
	char offsetCurveUDFpath[PRO_PATH_SIZE] = "";
	strcpy(offsetCurveUDFpath, cTextDirPath);
	strcat(offsetCurveUDFpath, "\\text\\UDF_LATEST\\create_offset_curve_with_dim.gph.1");

	double widthDimVal;
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);
	if (widthDimVal <= 0) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Input error", L"Please provide proper Width value to proceed.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return -1;
	}

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(paramsDialog);

	std::vector<std::vector<Node>> nodePairs = extractNodePairsFromFile(string(cNetlistPath));

	ProUIMessageButton* buttons;
	ProUIMessageButton result = PRO_UI_MESSAGE_YES;
	ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
	buttons[0] = PRO_UI_MESSAGE_YES;
	buttons[1] = PRO_UI_MESSAGE_NO;

	status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"All nodes or Selected nodes",
		L"Do you want to create Fanouts for all the nodes in the netlist?\n\nClick \"Yes\" to create fanouts for all the nodes present in selected netlist. or\nClick \"No\" to select the required bondpad surfaces.",
		buttons, buttons[0], &result);

	if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
	{
		ProSelection* surfSels;
		int nSurfSels;
		ProSelectionEnvOption selEnvOpt = { PRO_SELECT_BY_BOX_ALLOWED, 1 };
		ProSelectionEnv selEnv;
		status = ProSelectionEnvAlloc(&selEnvOpt, 1, &selEnv);
		status = ProSelect((char*)"point", -1, NULL, NULL, selEnv, NULL, &surfSels, &nSurfSels);
		if (status != PRO_TK_NO_ERROR || nSurfSels < 1)
		{
			ProUIMessageButton* buttons = nullptr;
			ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			buttons[0] = PRO_UI_MESSAGE_OK;
			ProUIMessageButton resultBtn;
			status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Input error", L"Please provide proper inputs to proceed.", buttons, buttons[0], &resultBtn);
			status = ProArrayFree((ProArray*)&buttons);
			return -1;
		}

		for (int i = 0; i < nodePairs.size();)
		{
			if (nodePairs[i].size() == 2)	// temparory purpose
			{
				bool keepNode = false;
				for (int j = 0; j < 2; ++j)
				{
					char nodeNameInFile[PRO_NAME_SIZE];
					strcpy(nodeNameInFile, nodePairs[i][j].ref.c_str());
					strcat(nodeNameInFile, "_P");
					strcat(nodeNameInFile, nodePairs[i][j].pin.c_str());
					//isPointFeatFound = GetFeatureByName(CurMdl1, nodeNameInFile, pointFeat);
					for (int k = 0; k < nSurfSels; ++k)
					{
						ProModelitem selSurfItem;
						ProName wSelSurfName;
						ProCharName cSelSurfName;
						status = ProSelectionModelitemGet(surfSels[k], &selSurfItem);
						status = ProModelitemNameGet(&selSurfItem, wSelSurfName);
						ProWstringToString(cSelSurfName, wSelSurfName);
						if (string(cSelSurfName).find("NODE_") != string::npos)
						{
							string nodeName = string(cSelSurfName).substr(5);
							if (strcmp(nodeName.c_str(), nodeNameInFile) == 0) 
							{
								keepNode = true;
								break;
							}
						}
					}
					
					if (keepNode)
						break;
				}
				if (!keepNode) {
					nodePairs.erase(nodePairs.begin() + i);
				}
				else {
					++i;
				}
			}
			/*else if (nodePairs[i].size() == 3)
			{
				nodePairs.erase(nodePairs.begin() + i);
			}*/
			else {
				++i;
			}
		}
	}
	status = ProMdlCurrentGet(&CurMdl1);
	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);

	double /*xOffsetVal, yOffsetVal, traceMarginVal,*/ widthDimVal1;
	/*status = ProUIInputpanelDoubleGet(paramsDialog, ip_x_ofst, &xOffsetVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_y_ofst, &yOffsetVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_trace_margin, &traceMarginVal);*/

	// verify if the points are present in the model
	vector<string> missingPoints;
	for (int i = 0; i < nodePairs.size();)
	{
		if (nodePairs[i].size() == 2)	// temparory purpose
		{
			bool removeNode = false;
			for (int j = 0; j < 2; ++j) 
			{
				bool isPointFeatFound;
				ProFeature pointFeat;
				char nodeName[PRO_NAME_SIZE];
				strcpy(nodeName, "NODE_");
				strcat(nodeName, nodePairs[i][j].ref.c_str());
				strcat(nodeName, "_P");
				strcat(nodeName, nodePairs[i][j].pin.c_str());
				isPointFeatFound = GetFeatureByName(CurMdl1, nodeName, pointFeat);
				if (!isPointFeatFound)
				{
					missingPoints.push_back(nodePairs[i][0].ref +"_P" + nodePairs[i][0].pin + " - " + nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin);
					removeNode = true;
					break;
				}
			}
			if (removeNode) {
				nodePairs.erase(nodePairs.begin() + i);
			}
			else {
				++i;
			}
		}
		else if(nodePairs[i].size() == 3)
		{
			bool removeNode = false;
			for (int j = 0; j < 3; ++j)
			{
				bool isPointFeatFound;
				ProFeature pointFeat;
				char nodeName[PRO_NAME_SIZE];
				strcpy(nodeName, nodePairs[i][j].ref.c_str());
				strcat(nodeName, "_P");
				strcat(nodeName, nodePairs[i][j].pin.c_str());
				isPointFeatFound = GetFeatureByName(CurMdl1, nodeName, pointFeat);
				if (!isPointFeatFound)
				{
					missingPoints.push_back(nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin + " - " + nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin + " - " + nodePairs[i][2].ref + "_P" + nodePairs[i][2].pin);
					removeNode = true;
					break;
				}
			}
			if (removeNode) {
				nodePairs.erase(nodePairs.begin() + i);
			}
			else {
				++i;
			}
		}
		else {
			++i;
		}
	}


	ProUIMessageButton* fanoutBtns = nullptr;
	ProArrayAlloc(2, sizeof(ProUIMessageButton), 2, (ProArray*)&fanoutBtns);
	fanoutBtns[0] = PRO_UI_MESSAGE_YES;
	fanoutBtns[1] = PRO_UI_MESSAGE_NO;
	ProUIMessageButton resultBtn{};
	if (missingPoints.size() > 0){
		ProCharPath remarkFile;
		wchar_t wRemarkFp[PRO_PATH_SIZE];
		//strcpy(remarkFile, "C:\\temp\\");
		strcpy(remarkFile, "C:\\ProgramData\\");
		strcat(remarkFile, "PRINTED_ELECTRONICS.txt");
		writeRemarkTxtFile(remarkFile, missingPoints);
		status = ProInfoWindowDisplay(ProStringToWstring(wRemarkFp, remarkFile), NULL, NULL);
	}

	if (nodePairs.size() > 0){
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_INFO, L"Info", L"Would you like to create the fanout for the available nodes?", fanoutBtns, fanoutBtns[0], &resultBtn);
	}

	if (resultBtn == PRO_UI_MESSAGE_NO)
	{
		status = ProArrayFree((ProArray*)&fanoutBtns);
		return -1;
	}
	status = ProArrayFree((ProArray*)&fanoutBtns);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);

	vector<PointCoOrd> fanoutMidpointsCoords{};
	for (int i = 0; i < nodePairs.size(); ++i)
	{
		if (nodePairs[i].size() == 2){	// temparory purpose
			//createFanOut("NODE_"+nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin,"NODE_"+ nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
			createFanOutLCR("NODE_"+nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin,"NODE_"+ nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
			fanoutMidpointsCoords.clear();
			vectPointData.clear();
		}
		else if (nodePairs[i].size() == 3){
			createFanOut(nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin, nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
			fanoutMidpointsCoords.clear();
			vectPointData.clear();
			createFanOut(nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, nodePairs[i][2].ref + "_P" + nodePairs[i][2].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
			fanoutMidpointsCoords.clear();
			vectPointData.clear();
		}
	}

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

//int placeStraightFanoutAction(char* dialog, char* component, ProAppData appdata)
//{
//	ProError status;
//	ProMdl CurMdl1;
//	vector<PointData> vectPointData;
//	ProAsmcomppath comp_path;
//	wchar_t* wNetlistPath = (wchar_t*)appdata;
//	ProCharPath cNetlistPath;
//	ProWstringToString(cNetlistPath, wNetlistPath);
//
//	ProIdTable c_id_table;
//	c_id_table[0] = -1;
//	char offsetCurveUDFpath[PRO_PATH_SIZE] = "";
//	strcpy(offsetCurveUDFpath, cTextDirPath);
//	strcat(offsetCurveUDFpath, "\\text\\UDF_LATEST\\straight_fanout_udf.gph.1");
//
//	status = ProUIDialogHide(mainDialog);
//	status = ProUIDialogHide(paramsDialog);
//
//	std::vector<std::vector<Node>> nodePairs = extractNodePairsFromFile(string(cNetlistPath));
//
//	ProUIMessageButton* buttons;
//	ProUIMessageButton result = PRO_UI_MESSAGE_YES;
//	ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
//	buttons[0] = PRO_UI_MESSAGE_YES;
//	buttons[1] = PRO_UI_MESSAGE_NO;
//
//	status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"All nodes or Selected nodes",
//		L"Do you want to create Fanouts for all the nodes in the netlist?\n\nClick \"Yes\" to create fanouts for all the nodes present in selected netlist. or\nClick \"No\" to select the required bondpad surfaces.",
//		buttons, buttons[0], &result);
//
//	if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
//	{
//		ProSelection* surfSels;
//		int nSurfSels;
//		ProSelectionEnvOption selEnvOpt = { PRO_SELECT_BY_BOX_ALLOWED, 1 };
//		ProSelectionEnv selEnv;
//		status = ProSelectionEnvAlloc(&selEnvOpt, 1, &selEnv);
//		status = ProSelect((char*)"point", -1, NULL, NULL, selEnv, NULL, &surfSels, &nSurfSels);
//		if (status != PRO_TK_NO_ERROR || nSurfSels < 1)
//		{
//			ProUIMessageButton* buttons = nullptr;
//			ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
//			buttons[0] = PRO_UI_MESSAGE_OK;
//			ProUIMessageButton resultBtn;
//			status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Input error", L"Please provide proper inputs to proceed.", buttons, buttons[0], &resultBtn);
//			status = ProArrayFree((ProArray*)&buttons);
//			return -1;
//		}
//
//		for (int i = 0; i < nodePairs.size();)
//		{
//			if (nodePairs[i].size() == 2)	// temparory purpose
//			{
//				bool keepNode = false;
//				for (int j = 0; j < 2; ++j)
//				{
//					char nodeNameInFile[PRO_NAME_SIZE];
//					strcpy(nodeNameInFile, nodePairs[i][j].ref.c_str());
//					strcat(nodeNameInFile, "_P");
//					strcat(nodeNameInFile, nodePairs[i][j].pin.c_str());
//					//isPointFeatFound = GetFeatureByName(CurMdl1, nodeNameInFile, pointFeat);
//					for (int k = 0; k < nSurfSels; ++k)
//					{
//						ProModelitem selSurfItem;
//						ProName wSelSurfName;
//						ProCharName cSelSurfName;
//						status = ProSelectionModelitemGet(surfSels[k], &selSurfItem);
//						status = ProModelitemNameGet(&selSurfItem, wSelSurfName);
//						ProWstringToString(cSelSurfName, wSelSurfName);
//						if (string(cSelSurfName).find("NODE_") != string::npos)
//						{
//							string nodeName = string(cSelSurfName).substr(5);
//							if (strcmp(nodeName.c_str(), nodeNameInFile) == 0)
//							{
//								keepNode = true;
//								break;
//							}
//						}
//					}
//
//					if (keepNode)
//						break;
//				}
//				if (!keepNode) {
//					nodePairs.erase(nodePairs.begin() + i);
//				}
//				else {
//					++i;
//				}
//			}
//			/*else if (nodePairs[i].size() == 3)
//			{
//				nodePairs.erase(nodePairs.begin() + i);
//			}*/
//			else {
//				++i;
//			}
//		}
//	}
//	status = ProMdlCurrentGet(&CurMdl1);
//	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &comp_path);
//
//	double /*xOffsetVal, yOffsetVal, traceMarginVal,*/ widthDimVal1;
//	/*status = ProUIInputpanelDoubleGet(paramsDialog, ip_x_ofst, &xOffsetVal);
//	status = ProUIInputpanelDoubleGet(paramsDialog, ip_y_ofst, &yOffsetVal);
//	status = ProUIInputpanelDoubleGet(paramsDialog, ip_trace_margin, &traceMarginVal);*/
//
//	// verify if the points are present in the model
//	vector<string> missingPoints;
//	for (int i = 0; i < nodePairs.size();)
//	{
//		if (nodePairs[i].size() == 2)	// temparory purpose
//		{
//			bool removeNode = false;
//			for (int j = 0; j < 2; ++j)
//			{
//				bool isPointFeatFound;
//				ProFeature pointFeat;
//				char nodeName[PRO_NAME_SIZE];
//				strcpy(nodeName, "NODE_");
//				strcat(nodeName, nodePairs[i][j].ref.c_str());
//				strcat(nodeName, "_P");
//				strcat(nodeName, nodePairs[i][j].pin.c_str());
//				isPointFeatFound = GetFeatureByName(CurMdl1, nodeName, pointFeat);
//				if (!isPointFeatFound)
//				{
//					missingPoints.push_back(nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin + " - " + nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin);
//					removeNode = true;
//					break;
//				}
//			}
//			if (removeNode) {
//				nodePairs.erase(nodePairs.begin() + i);
//			}
//			else {
//				++i;
//			}
//		}
//		else if (nodePairs[i].size() == 3)
//		{
//			bool removeNode = false;
//			for (int j = 0; j < 3; ++j)
//			{
//				bool isPointFeatFound;
//				ProFeature pointFeat;
//				char nodeName[PRO_NAME_SIZE];
//				strcpy(nodeName, nodePairs[i][j].ref.c_str());
//				strcat(nodeName, "_P");
//				strcat(nodeName, nodePairs[i][j].pin.c_str());
//				isPointFeatFound = GetFeatureByName(CurMdl1, nodeName, pointFeat);
//				if (!isPointFeatFound)
//				{
//					missingPoints.push_back(nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin + " - " + nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin + " - " + nodePairs[i][2].ref + "_P" + nodePairs[i][2].pin);
//					removeNode = true;
//					break;
//				}
//			}
//			if (removeNode) {
//				nodePairs.erase(nodePairs.begin() + i);
//			}
//			else {
//				++i;
//			}
//		}
//		else {
//			++i;
//		}
//	}
//
//
//	ProUIMessageButton* fanoutBtns = nullptr;
//	ProArrayAlloc(2, sizeof(ProUIMessageButton), 2, (ProArray*)&fanoutBtns);
//	fanoutBtns[0] = PRO_UI_MESSAGE_YES;
//	fanoutBtns[1] = PRO_UI_MESSAGE_NO;
//	ProUIMessageButton resultBtn{};
//	if (missingPoints.size() > 0) {
//		ProCharPath remarkFile;
//		wchar_t wRemarkFp[PRO_PATH_SIZE];
//		//strcpy(remarkFile, "C:\\temp\\");
//		strcpy(remarkFile, "C:\\ProgramData\\");
//		strcat(remarkFile, "PRINTED_ELECTRONICS.txt");
//		writeRemarkTxtFile(remarkFile, missingPoints);
//		status = ProInfoWindowDisplay(ProStringToWstring(wRemarkFp, remarkFile), NULL, NULL);
//	}
//
//	if (nodePairs.size() > 0) {
//		status = ProUIMessageDialogDisplay(PROUIMESSAGE_INFO, L"Info", L"Would you like to create the fanout for the available nodes?", fanoutBtns, fanoutBtns[0], &resultBtn);
//	}
//
//	if (resultBtn == PRO_UI_MESSAGE_NO)
//	{
//		status = ProArrayFree((ProArray*)&fanoutBtns);
//		return -1;
//	}
//	status = ProArrayFree((ProArray*)&fanoutBtns);
//	//status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);
//
//	vector<PointCoOrd> fanoutMidpointsCoords{};
//	for (int i = 0; i < nodePairs.size(); ++i)
//	{
//		if (nodePairs[i].size() == 2) {	// temparory purpose
//			//createFanOut("NODE_"+nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin,"NODE_"+ nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
//			createFanOutLCR("NODE_" + nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin, "NODE_" + nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
//			fanoutMidpointsCoords.clear();
//			vectPointData.clear();
//		}
//		/*else if (nodePairs[i].size() == 3) {
//			createFanOut(nodePairs[i][0].ref + "_P" + nodePairs[i][0].pin, nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
//			fanoutMidpointsCoords.clear();
//			vectPointData.clear();
//			createFanOut(nodePairs[i][1].ref + "_P" + nodePairs[i][1].pin, nodePairs[i][2].ref + "_P" + nodePairs[i][2].pin, CurMdl1, fanoutMidpointsCoords, widthDimVal, comp_path, vectPointData);
//			fanoutMidpointsCoords.clear();
//			vectPointData.clear();
//		}*/
//	}
//
//	/*ProFeature csoFeat, csoFeat1;
//	std::vector<ProSelection> UdfInputSel;
//	PointData curPointData = {};
//	string point1 = "PNT4";
//	string point2 = "PNT6";
//	int surfaceid = 41245;
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);*/
//
//
//	//ProCurve middleCurve;
//	//ProFeature curveFeat;
//	//double bendPntCoOrd[3];
//	//status = ProFeatureInit((ProSolid)CurMdl1, 91351, &curveFeat);
//	////status = ProFeatureInit((ProSolid)CurMdl1, 97271, &curveFeat);
//	//status = ProFeatureGeomitemVisit(&curveFeat, PRO_CURVE, (ProGeomitemAction)featCurveGeomitemVisit, NULL, (ProAppData*)&bendPntCoOrd);
//
//	//ProPoint3d pnt3D;
//	////status = ProSelectionPoint3dGet(pntPlacementSel[0], pnt3D);
//	///*pnt3D[0] = bendPntCoOrd[0]-10;
//	//pnt3D[1] = bendPntCoOrd[1]-10;
//	//pnt3D[2] = bendPntCoOrd[2]-10;*/
//
//	//pnt3D[0] = bendPntCoOrd[0];
//	//pnt3D[1] = bendPntCoOrd[1];
//	//pnt3D[2] = bendPntCoOrd[2];
//
//	////ProBoolean onSurface;
//	//ProPoint3d nearPnt3d;
//	////status = ProPoint3dOnsurfaceFind(pnt3D, placement_surf, &onSurface, nearPnt3d);
//	//ProSurface onSurface;
//	//ProUvParam surfaceUV;
//	//status = ProSolidProjectPoint((ProSolid)CurMdl1, pnt3D, 3, &onSurface, surfaceUV);
//	//int surfId;
//	//status = ProSurfaceIdGet(onSurface, &surfId);
//	//ProModelitem surfItem;
//	//status = ProSurfaceToGeomitem((ProSolid)CurMdl1, onSurface, &surfItem);
//	//ProSelection surfSel;
//	//status = ProSelectionAlloc(NULL, &surfItem, &surfSel);
//	//surfaceUV[0] = surfaceUV[0] + 0.01;
//	//surfaceUV[1] = surfaceUV[1] + 1;
//	//status = ProSelectionUvParamSet(surfSel, surfaceUV);
//	//ProVector projectedPoint;
//	//status = ProSurfaceXyzdataEval(onSurface, surfaceUV, projectedPoint, NULL, NULL, NULL);
//
//	//ProSurface obstacleSurf;
//	//status = ProSurfaceInit(CurMdl1, 44181, &obstacleSurf);
//
//	//ProUvStatus uv_sts;
//	//status = ProSurfaceUvpntVerify((ProSolid)CurMdl1, obstacleSurf, surfaceUV, &uv_sts);
//	//ProFeature pointFeature;
//	//status = ProDemoFieldPointCreate(surfSel, &pointFeature);
//
//	/*ProFeature csoFeat, csoFeat1;
//	std::vector<ProSelection> UdfInputSel;
//	PointData curPointData = {};
//	string point1 = "PNT10";
//	string point2 = "FPNT4";
//	int surfaceid = 41245;
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);
//
//	point1 = "FPNT4";
//	point2 = "PNT9";
//	UdfInputSel.clear();
//	vectPointData.clear();
//	CreateCurveBasedOnPoints(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel);*/
//
//
//	//status = ProCurveInit((ProSolid)CurMdl1, 96597, &middleCurve);
//	//status = ProCurveCompVisit(middleCurve, ProUtilCollectCurveComponentVisitAction, NULL, NULL);
//	/*ProVector xyz_pnt, derv1, derv2;
//	status = ProCurveXyzdataEval(middleCurve, 1, xyz_pnt, derv1, derv2);*/
//
//	status = ProUIDialogShow(mainDialog);
//	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);
//
//	return 0;
//
//}

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

ProError featureDimensionVisitAction(ProDimension* p_handle, ProError status, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	vector<ProDimension>* lpVecfeature = (vector<ProDimension>*)(app_data);
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
	}
	return PRO_TK_NO_ERROR;
}

ProError visitFanoutSurfsForOverlap(ProGroup* p_group, ProError status, ProAppData app_data)
{
	ProMdl curMdl;
	ProFeattype featType;
	//int* fanoutSurfitemId = (int*)app_data;
	vector<int>* fanoutSurfitemIds = (vector<int>*)(app_data);

	status = ProMdlCurrentGet(&curMdl);

	ProFeature* fanoutSurfGrpFeats = NULL;
	status = ProGroupFeaturesCollect(p_group, &fanoutSurfGrpFeats);

	wchar_t wfanoutSurfGrpName[PRO_NAME_SIZE]; char cfanoutSurfGrpName[PRO_NAME_SIZE];
	status = ProModelitemNameGet(&fanoutSurfGrpFeats[0], wfanoutSurfGrpName);
	ProWstringToString(cfanoutSurfGrpName, wfanoutSurfGrpName);

	if (string(cfanoutSurfGrpName).find("CREATE_SURFACE") != string::npos)
	{
		ProSelection fanoutSel1, fanoutSel2;
		ProAsmcomppath fanoutComppath;
		ProModelitem fanoutSurf1Item;
		ProIdTable c_id_table;
		c_id_table[0] = -1;
		vector<ProGeomitem> vecGeomitems;

		GetGeomItems(curMdl, fanoutSurfGrpFeats[1], vecGeomitems);
		status = ProAsmcomppathInit((ProSolid)curMdl, c_id_table, 0, &fanoutComppath);
		status = ProSelectionAlloc(&fanoutComppath, &vecGeomitems[0], &fanoutSel2);

		for (int i = 0; i < fanoutSurfitemIds->size(); ++i)
		{
			status = ProModelitemInit(curMdl, fanoutSurfitemIds->at(i), PRO_SURFACE, &fanoutSurf1Item);
			status = ProSelectionAlloc(&fanoutComppath, &fanoutSurf1Item, &fanoutSel1);
			Pro2dPnt param_1, param_2;
			Pro3dPnt pnt_1, pnt_2;
			double distance;
			status = ProSelectionWithOptionsDistanceEval(fanoutSel1, PRO_B_FALSE, fanoutSel2, PRO_B_TRUE, param_1, param_2, pnt_1, pnt_1, &distance);
			bool isPresentInVec = false;
			for (int j = 0; j < fanoutSurfitemIds->size(); ++j){
				if (vecGeomitems[0].id == fanoutSurfitemIds->at(j))
					isPresentInVec = true;
			}
			
			if (round(distance) == 0 && !isPresentInVec)
			{
				isFanoutOverlap = true;
				break;
			}
		}
	}
	return PRO_TK_NO_ERROR;
}

ProError featPointVisitAction(ProGeomitem* geomitem, ProError err, ProAppData appdata)
{
	ProGeomitem* point = (ProGeomitem*)appdata;
	point[0] = geomitem[0];
	return PRO_TK_NO_ERROR;
}

ProError placeBondpadAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	double heightDimVal, widthDimVal, angleDimVal;
	double xOffsetVal, yOffsetVal, traceMarginVal;
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_height, &heightDimVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_angle, &angleDimVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_x_ofst, &xOffsetVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_y_ofst, &yOffsetVal);
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_trace_margin, &traceMarginVal);

	if (heightDimVal <= 0 || widthDimVal <= 0 || xOffsetVal <= 0 || yOffsetVal <= 0 || traceMarginVal <= 0) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Input error",L"Please provide proper inputs to proceed.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
	}

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

	status = ProSelect("edge,surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	/*ProUvParam sel_uv_param, sel_uv_param_1;
	status = ProSelectionUvParamGet(pntPlacementSel[0], sel_uv_param);
	sel_uv_param_1[0] = 2.3;
	sel_uv_param_1[1] = 300;
	status = ProSelectionUvParamSet(pntPlacementSel[0], sel_uv_param_1);*/

	ProName wPointName, wSurfName, wNodeName;
	ProCharName cPointName;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node name:");
	status = ProMessageStringRead(10, wPointName);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

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
			/*bondpadUDFsels.push_back(pntPlacementSel[0]);
			bondpadUDFsels.push_back(pntSel);
			bondpadUDFsels.push_back(placementSurfSel);
			bondpadUDFsels.push_back(dirSurfSel);*/
			bondpadUDFsels.push_back(pntSel);
			bondpadUDFsels.push_back(placementSurfSel);
			bondpadUDFsels.push_back(pntPlacementSel[0]);
			bondpadUDFsels.push_back(dirSurfSel);
			//udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
			udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
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

		/*bondpadUDFsels.push_back(reqNearestEdgeSel);
		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		bondpadUDFsels.push_back(dirSurfSel);*/
		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		bondpadUDFsels.push_back(reqNearestEdgeSel);
		bondpadUDFsels.push_back(dirSurfSel);
		udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}
	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);//Give only non suppressed entities

	ProParamvalue nodeNameParamVal;
	ProParameter nodeNameParam;

	wcscpy(wNodeName, L"NODE_");
	wcscat(wNodeName, wPointName);
	status = ProParamvalueSet(&nodeNameParamVal, (void*)wNodeName, PRO_PARAM_STRING);
	status = ProParameterInit(&feats[6], L"NODE_NAME", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[7], L"NODE_NAME", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	status = ProModelitemNameSet(&feats[6], wNodeName);
	wcscpy(wSurfName, L"BP_SURF_");
	wcscat(wSurfName, wPointName);
	status = ProModelitemNameSet(&feats[7], wSurfName);

	// X_OFFSET_CLEARANCE
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&xOffsetVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[6], L"X_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[6], L"X_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	status = ProParameterInit(&feats[0], L"X_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[0], L"X_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	// Y_OFFSET_CLEARANCE
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&yOffsetVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[6], L"Y_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[6], L"Y_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	status = ProParameterInit(&feats[0], L"Y_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[0], L"Y_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	// FANOUT_TRACE_MARGIN
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&traceMarginVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[6], L"FANOUT_TRACE_MARGIN", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[6], L"FANOUT_TRACE_MARGIN", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	status = ProParameterInit(&feats[0], L"FANOUT_TRACE_MARGIN", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[0], L"FANOUT_TRACE_MARGIN", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	// HEIGHT
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&heightDimVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[0], L"HEIGHT", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[0], L"HEIGHT", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	// WIDTH
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[0], L"WIDTH", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	// ANGLE
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&angleDimVal, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&feats[0], L"ANGLE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&feats[0], L"ANGLE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	// DESCRIPTION
	ProExtdataErr err;
	err = ProExtdataInit(CurMdl1);
	err = ProExtdataClassRegister(CurMdl1, L"DESC_SLOT", &extDataClass);
	if (err == PROEXTDATA_TK_CLASS_OR_SLOT_EXISTS){
		extDataClass.p_model = CurMdl1;
		wcscpy(extDataClass.class_name, L"DESC_SLOT");
	}
	wchar_t* wDesc;
	status = ProUITextareaValueGet(paramsDialog, textarea_desc, &wDesc);
	ProExtdataSlot data_slot;
	char cSlotName[PRO_NAME_SIZE];
	wchar_t wSlotName[PRO_NAME_SIZE];
	strcpy(cSlotName, "BP_DESC_");
	strcat(cSlotName, to_string(feats[0].id).c_str());
	ProStringToWstring(wSlotName, cSlotName);

	err = ProExtdataSlotCreate(&extDataClass, wSlotName, &data_slot);
	if(err == PROEXTDATA_TK_NO_ERROR)
		err = ProExtdataSlotWrite(&data_slot, 4098, PRO_WIDE_STRING_TYPE, 0, &wDesc[0]);

	/*if (err == PROEXTDATA_TK_NO_ERROR){
		void* readData;
		int type;
		err = ProExtdataSlotRead(&data_slot, 4098, &type, 0, &readData);
		wchar_t* wData = (wchar_t*)readData;
		int i = 0;
	}*/

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);

	return status;

}

ProError placeBondpadAction2(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

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
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	ProModelitem sidePlaneItem;
	status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);
	status = ProSelectionAlloc(NULL, &sidePlaneItem, &dirSurfSel);

	ProModelitem dirAxisItem;
	status = ProModelitemByNameInit(CurMdl1, PRO_AXIS, L"A_2", &dirAxisItem);
	status = ProSelectionAlloc(NULL, &dirAxisItem, &dirAxisSel);

	bondpadUdfpath= "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\bondpads_with_dummy_resistor.gph.1");

	bondpadUDFsels.push_back(pntSel);
	bondpadUDFsels.push_back(pntPlacementSel[0]);
	bondpadUDFsels.push_back(dirSurfSel);
	bondpadUDFsels.push_back(dirAxisSel);
	udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE);
	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

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

	return status;
}

ProError ProUtilCollectCurveComponentVisitAction(
	ProCurve     p_curve,   /* In:  The composite curve being processed */
	ProCurve     p_comp,    /* In:  The current component being visited */
	int          comp_idx,  /* In:  The index of the current component in the curve */
	ProBoolean   comp_flip, /* In:  This is PRO_TK_TRUE if the current component is flipped */
	ProError     status,	/* In:  The status returned by the filter */
	vector<PointCoOrd>* pnt_co_ords)  /* In:  In fact it's CurveComponent**  */
{
	ProVector xyz_pnt, derv1, derv2;
	status = ProCurveXyzdataEval(p_comp, 0, xyz_pnt, derv1, derv2);
	pnt_co_ords->push_back({ xyz_pnt[0], xyz_pnt[1], xyz_pnt[2], derv1[0], derv1[1], derv1[2]});
	status = ProCurveXyzdataEval(p_comp, 1, xyz_pnt, derv1, derv2);
	pnt_co_ords->push_back({ xyz_pnt[0], xyz_pnt[1], xyz_pnt[2], derv1[0], derv1[1], derv1[2]});

	int i = 0;
	return PRO_TK_NO_ERROR;
}

ProError featCurveGeomitemVisit(ProGeomitem& p_handle, ProError status, ProAppData app_data)
{
	//double* reqPnt = (double*)app_data;
	vector<PointCoOrd>* reqPoints = (vector<PointCoOrd>*)app_data;
	ProCurve middleCurve;
	vector<PointCoOrd> pointCoOrds;
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
		std::set<PointCoOrd> uniqueValues;

		// Vector to store repeated values
		//std::vector<PointCoOrd> repeatedValues;

		// Iterate through the vector
		for (auto& myStruct : pointCoOrds) {
			// Check if the value is already in the set
			if (!uniqueValues.insert(myStruct).second) {
				// Value is repeated
				reqPoints->push_back(myStruct);
				/*reqPnt[0] = myStruct.x;
				reqPnt[1] = myStruct.y;
				reqPnt[2] = myStruct.z;*/
				//break;
			}
		}
	}

	return PRO_TK_NO_ERROR;
}

ProError mirrorFeatCreatePostAction(ProFeature &createdFeat)
{
	ProError status;
	ProName wFeatName;
	ProMdl curMdl;
	vector<ProGeomitem> featsGeomitems;
	ProMdlCurrentGet(&curMdl);
	/*status = ProFeatureGeomitemVisit(&createdFeat, PRO_TYPE_UNUSED, VesGeomItemVisitAction, NULL, (ProAppData)&featsGeomitems);*/

	status = ProModelitemDefaultnameGet(&createdFeat, wFeatName);
	ProCharName cFeatName;
	ProWstringToString(cFeatName, wFeatName);
	if (string(cFeatName).find("MIRROR") != string::npos)
	{
		ProSelection* sel;
		int           n_sel;
		wchar_t       w_str[5];
		char          str[100], a_str[5];
		double        volume;
		FILE* fp;
		char          fname[PRO_NAME_SIZE + 4];
		wchar_t       w_fname[PRO_NAME_SIZE + 4];
		ProMdl        p_model;
		ProFileName   msg_fil;
		ProInterferenceData interf_data;

		status = ProSelect("surface", 2, NULL, NULL, NULL, NULL, &sel, &n_sel);
		if (n_sel > 0) {
			status = ProFitInterferenceCompute(sel[0], sel[1], PRO_B_FALSE, PRO_B_FALSE, &interf_data);
		}
		int i = 0;
	}
	return PRO_TK_NO_ERROR;
}

ProError updateBondpadAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProGroup* bondpadGroup = (ProGroup*)appdata;
	ProFeature* grpFeats;
	ProMdl curMdl;
	status = ProMdlCurrentGet(&curMdl);
	status = ProGroupFeaturesCollect(bondpadGroup, &grpFeats);
	vector<ProDimension> vecFeatDims;
	ProBoolean is_regened_negative;

	double height, width, ofstX, ofstY, traceMargin, angle, existAngle, alterAngle;
	status = ProUIInputpanelDoubleGet(UDFmodifyDialog, ip_height, &height);
	status = ProUIInputpanelDoubleGet(UDFmodifyDialog, ip_width, &width);
	status = ProUIInputpanelDoubleGet(UDFmodifyDialog, ip_angle, &angle);
	status = ProUIInputpanelDoubleGet(UDFmodifyDialog, ip_x_ofst, &ofstX);
	status = ProUIInputpanelDoubleGet(UDFmodifyDialog, ip_y_ofst, &ofstY);
	status = ProUIInputpanelDoubleGet(UDFmodifyDialog, ip_trace_margin, &traceMargin);

	if (height <= 0 || width <= 0 || ofstX <= 0 || ofstY <= 0 || traceMargin <= 0) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Input error", L"Please provide proper inputs to proceed.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	// WIDTH AND ANGLE
	status = ProFeatureDimensionVisit(&grpFeats[1], featureDimensionVisitAction, NULL, (ProAppData)&vecFeatDims);
	status = ProDimensionValueSet(&vecFeatDims[1], width);
	is_regened_negative = PRO_B_FALSE;
	status = ProDimensionIsRegenednegative(&vecFeatDims[0], &is_regened_negative);
	if (!is_regened_negative/* && fabs(existAngle) == fabs(angle)*/) {
		alterAngle = -angle;
	}
	else {
		alterAngle = angle;
	}
	status = ProDimensionValueSet(&vecFeatDims[0], 0.0);
	status = ProDimensionValueSet(&vecFeatDims[0], alterAngle);
	vecFeatDims.clear();

	/*status = ProFeatureDimensionVisit(&grpFeats[4], featureDimensionVisitAction, NULL, (ProAppData)&vecFeatDims);
	status = ProDimensionValueSet(&vecFeatDims[1], width);
	status = ProDimensionValueSet(&vecFeatDims[0], 0.0);
	status = ProDimensionValueSet(&vecFeatDims[0], alterAngle);	vecFeatDims.clear();
	vecFeatDims.clear();*/

	// HEIGHT AND ANGLE
	status = ProFeatureDimensionVisit(&grpFeats[4], featureDimensionVisitAction, NULL, (ProAppData)&vecFeatDims);
	status = ProDimensionValueGet(&vecFeatDims[1], &height);
	status = ProDimensionValueSet(&vecFeatDims[0], 0.0);
	status = ProDimensionValueSet(&vecFeatDims[0], alterAngle);
	vecFeatDims.clear();
	status = ProFeatureDimensionVisit(&grpFeats[5], featureDimensionVisitAction, NULL, (ProAppData)&vecFeatDims);
	status = ProDimensionValueSet(&vecFeatDims[1], height);
	status = ProDimensionValueSet(&vecFeatDims[0], 0.0);
	status = ProDimensionValueSet(&vecFeatDims[0], alterAngle);
	vecFeatDims.clear();
	status = ProFeatureDimensionVisit(&grpFeats[6], featureDimensionVisitAction, NULL, (ProAppData)&vecFeatDims);
	status = ProDimensionValueSet(&vecFeatDims[1], height - (height * 0.3));
	status = ProDimensionValueSet(&vecFeatDims[0], 0.0);                                                                                                                      
	status = ProDimensionValueSet(&vecFeatDims[0], alterAngle);
	vecFeatDims.clear();

	ProParamvalue nodeNameParamVal;
	ProParameter nodeNameParam;
	// X_OFFSET_CLEARANCE
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&ofstX, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&grpFeats[7], L"X_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[7], L"X_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	status = ProParameterInit(&grpFeats[0], L"X_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[0], L"X_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	// Y_OFFSET_CLEARANCE
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&ofstY, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&grpFeats[7], L"Y_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[7], L"Y_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	status = ProParameterInit(&grpFeats[0], L"Y_OFFSET_CLEARANCE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[0], L"Y_OFFSET_CLEARANCE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	// FANOUT_TRACE_MARGIN
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&traceMargin, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&grpFeats[7], L"FANOUT_TRACE_MARGIN", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[7], L"FANOUT_TRACE_MARGIN", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	status = ProParameterInit(&grpFeats[0], L"FANOUT_TRACE_MARGIN", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[0], L"FANOUT_TRACE_MARGIN", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}

	// HEIGHT
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&height, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&grpFeats[0], L"HEIGHT", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[0], L"HEIGHT", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	// WIDTH
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&width, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&grpFeats[0], L"WIDTH", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}
	// ANGLE
	status = ProParamvalueSet(&nodeNameParamVal, (void*)&angle, PRO_PARAM_DOUBLE);
	status = ProParameterInit(&grpFeats[0], L"ANGLE", &nodeNameParam);
	if (status != PRO_TK_NO_ERROR) {
		status = ProParameterWithUnitsCreate(&grpFeats[0], L"ANGLE", &nodeNameParamVal, NULL, &nodeNameParam);
	}
	else {
		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
	}


	// DESCRIPTION
	wchar_t* wDesc;
	status = ProUITextareaValueGet(UDFmodifyDialog, textarea_desc, &wDesc);
	if (status == PRO_TK_NO_ERROR)
	{
		ProExtdataSlot data_slot;
		ProExtdataErr err;
		//wcscpy(extDataClass.class_name, L"DESC_SLOT");
		//extDataClass.p_model = curMdl;
		data_slot.p_class = &extDataClass;

		char cSlotName[PRO_NAME_SIZE];
		strcpy(cSlotName, "BP_DESC_");
		strcat(cSlotName, to_string(grpFeats[0].id).c_str());
		ProStringToWstring(data_slot.slot_name, cSlotName);
		err = ProExtdataSlotWrite(&data_slot, 4098, PRO_WIDE_STRING_TYPE, 0, &wDesc[0]);
		if (err == PROEXTDATA_TK_SLOT_NOT_FOUND)
		{
			wchar_t wSlotName[PRO_NAME_SIZE];
			ProStringToWstring(wSlotName, cSlotName);
			err = ProExtdataSlotCreate(&extDataClass, wSlotName, &data_slot);
			err = ProExtdataSlotWrite(&data_slot, 4098, PRO_WIDE_STRING_TYPE, 0, &wDesc[0]);
		}

		if (err == PROEXTDATA_TK_NO_ERROR) {
			void* readData;
			int type;
			err = ProExtdataSlotRead(&data_slot, 4098, &type, 0, &readData);
			wchar_t* wData = (wchar_t*)readData;
		}
	}

	status = ProSolidRegenerate((ProSolid)curMdl, PRO_REGEN_NO_FLAGS);
	status = ProUIDialogExit(UDFmodifyDialog, PRO_TK_NO_ERROR);

	return status;

}

ProError bondPadModifyAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProSelection* surfFeatSels;
	int nSurfFeatSels;
	ProModelitem surfFeatItem{};
	vector<ProGeomitem> vecGeomItems;
	vector<ProDimension> vecFeatDims;
	ProGroup bondpadGroup;
	ProFeature surfFeat;

	status = ProSelbufferSelectionsGet(&surfFeatSels);
	if (status != PRO_TK_NO_ERROR)
		return PRO_TK_NO_ERROR;

	status = ProSelectionModelitemGet(surfFeatSels[0], &surfFeatItem);

	/*status = ProSelect((char*)"feature", 1, NULL, NULL, NULL, NULL, &surfFeatSels, &nSurfFeatSels);
	if (status != PRO_TK_NO_ERROR && nSurfFeatSels < 1){
		return PRO_TK_NO_ERROR;
	}*/
	if (surfFeatItem.type == PRO_SURFACE) {
		int *parentIds;
		int nParents;
		status = ProGeomitemFeatureGet((ProGeomitem*)&surfFeatItem, &surfFeat);
		status = ProFeatureGroupGet(&surfFeat, &bondpadGroup);
	}
	else if (surfFeatItem.type == PRO_FEATURE){
		ProFeattype featType;
		status = ProFeatureTypeGet((ProFeature*)&surfFeatItem, &featType);
		if (featType != PRO_FEAT_DATUM_SURF)
			return PRO_TK_NO_ERROR;
		status = ProFeatureGroupGet((ProFeature*)&surfFeatItem, &bondpadGroup);
		if (status != PRO_TK_NO_ERROR)
			return PRO_TK_NO_ERROR;
	}
	else{
		return PRO_TK_GENERAL_ERROR;
	}

	ProFeature* grpFeats;
	status = ProGroupFeaturesCollect(&bondpadGroup, &grpFeats);
	if (status != PRO_TK_NO_ERROR)
		return PRO_TK_NO_ERROR;

	/*ProPoint heightPnt;
	status = ProFeatureGeomitemVisit(&grpFeats[0], PRO_POINT, VesGeomItemVisitAction, NULL, (ProAppData)&vecGeomItems);
	ProGeomitemToPoint(&vecGeomItems[0], &heightPnt);*/
	status = ProUIDialogCreate(UDFmodifyDialog, UDFmodifyDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;
	status = ProUIDialogAboveactivewindowSet(UDFmodifyDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(UDFmodifyDialog, okbtn, (ProUIAction)updateBondpadAction, (ProAppData)&bondpadGroup);
	status = ProUIPushbuttonActivateActionSet(UDFmodifyDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(UDFmodifyDialog, closeMYdialog, NULL);

	double height=0.0, width = 0.0, angle=0.0, ofstX = 0.0, ofstY = 0.0, traceMargin = 0.0;
	ProParameter dimParam;
	ProParamvalue dimParamValue;
	status = ProParameterInit(&grpFeats[0], L"HEIGHT", &dimParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&dimParam, &dimParamValue, NULL);
		height = dimParamValue.value.d_val;
	}
	status = ProParameterInit(&grpFeats[0], L"WIDTH", &dimParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&dimParam, &dimParamValue, NULL);
		width = dimParamValue.value.d_val;
	}
	status = ProParameterInit(&grpFeats[0], L"ANGLE", &dimParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&dimParam, &dimParamValue, NULL);
		angle = dimParamValue.value.d_val;
	}
	status = ProParameterInit(&grpFeats[0], L"X_OFFSET_CLEARANCE", &dimParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&dimParam, &dimParamValue, NULL);
		ofstX = dimParamValue.value.d_val;
	}
	status = ProParameterInit(&grpFeats[0], L"Y_OFFSET_CLEARANCE", &dimParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&dimParam, &dimParamValue, NULL);
		ofstY = dimParamValue.value.d_val;
	}
	status = ProParameterInit(&grpFeats[0], L"FANOUT_TRACE_MARGIN", &dimParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&dimParam, &dimParamValue, NULL);
		traceMargin = dimParamValue.value.d_val;
	}
	
	status = ProUIInputpanelDoubleSet(UDFmodifyDialog, ip_height, height);
	status = ProUIInputpanelDoubleSet(UDFmodifyDialog, ip_width, width);
	status = ProUIInputpanelDoubleSet(UDFmodifyDialog, ip_angle, angle);
	status = ProUIInputpanelDoubleSet(UDFmodifyDialog, ip_x_ofst, ofstX);
	status = ProUIInputpanelDoubleSet(UDFmodifyDialog, ip_y_ofst, ofstY);
	status = ProUIInputpanelDoubleSet(UDFmodifyDialog, ip_trace_margin, traceMargin);

	// DESC
	ProExtdataErr err;
	ProExtdataSlot data_slot;
	//ProExtdataClass extDataClass;
	ProMdl curMdl;
	status = ProMdlCurrentGet(&curMdl);
	extDataClass.p_model = curMdl;
	wcscpy(extDataClass.class_name, L"DESC_SLOT");

	data_slot.p_class = &extDataClass;
	char cSlotName[PRO_NAME_SIZE];
	strcpy(cSlotName, "BP_DESC_");
	strcat(cSlotName, to_string(grpFeats[0].id).c_str());
	ProStringToWstring(data_slot.slot_name, cSlotName);
	void* readData;
	int type;
	err = ProExtdataSlotRead(&data_slot, 4098, &type, 0, &readData);
	if (err == PROEXTDATA_TK_NO_ERROR) {
		wchar_t* wData = (wchar_t*)readData;
		status = ProUITextareaValueSet(UDFmodifyDialog, textarea_desc, wData);
	}

	int stat = -1;
	status = ProUIDialogActivate(UDFmodifyDialog, &stat);
	status = ProUIDialogDestroy(UDFmodifyDialog);
	
	//double defaultHeight = 0, defaultWidth = 0;
	//ProErr lErr;
	//bool isCreatedUDF = false;
	//ProUdfdata UdfData = NULL;
	//lErr = ProUdfdataAlloc(&UdfData);
	//ProPath UdfFilePath;
	//char* bondpadUdfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_with_dims.gph.1";
	////char* bondpadUdfpath = "D:\\Project Details\\crowdplat\\NASA-CREO\\3DPrinter\\text\\UDF_LATEST\\bondpad_placement_udf.gph.1";

	//status = ProUIDialogAboveactivewindowSet(paramsDialog, PRO_B_FALSE);
	//status = ProUIPushbuttonActivateActionSet(paramsDialog, okbtn, placeBondpadAction, (ProAppData)bondpadUdfpath);
	//status = ProUIPushbuttonActivateActionSet(paramsDialog, closebtn, closeMYdialog, NULL);
	//status = ProUIDialogCloseActionSet(paramsDialog, closeMYdialog, NULL);
	///*status = ProUILabelHide(paramsDialog, "offset_x_lbl");
	//status = ProUIInputpanelHide(paramsDialog, ip_x_ofst);
	//status = ProUILabelHide(paramsDialog, "offset_y_lbl");
	//status = ProUIInputpanelHide(paramsDialog, ip_y_ofst);
	//status = ProUILabelHide(paramsDialog, "fanout_boundary_lbl");
	//status = ProUIInputpanelHide(paramsDialog, ip_trace_margin);*/
	////status = ProUILayoutHide(paramsDialog, layout_fanout_attributes);
	////status = ProUILayoutHide(paramsDialog, layout_clearance);

	//string sBondpadUdfpath = string(bondpadUdfpath);
	//ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
	//lErr = ProUdfdataPathSet(UdfData, UdfFilePath);

	//ProUdfvardim* var_dim_array;
	//int size;
	//int i;
	//status = ProUdfdataVardimsGet(UdfData, &var_dim_array);
	//if (status == PRO_TK_NO_ERROR)
	//{
	//	status = ProArraySizeGet(var_dim_array, &size);
	//	for (i = 0; i < size; i++)
	//	{
	//		wchar_t wDimName[PRO_NAME_SIZE];
	//		char cDimName[PRO_NAME_SIZE];

	//		ProLine wPrompt;
	//		status = ProUdfvardimPromptGet(var_dim_array[i], wPrompt);
	//		ProUdfVardimType value_type;
	//		double defaultDimVal;
	//		status = ProUdfvardimDefaultvalueGet(var_dim_array[i], &value_type, &defaultDimVal);

	//		char cPrompt[PRO_NAME_SIZE];
	//		ProWstringToString(cPrompt, wPrompt);
	//		if (strcmp(cPrompt, "width1") == 0) {
	//			defaultWidth = 2 * defaultDimVal;
	//		}
	//		else if (strcmp(cPrompt, "height1") == 0) {
	//			defaultHeight = defaultDimVal;
	//		}
	//	}
	//	ProUdfvardimProarrayFree(var_dim_array);
	//}



	return PRO_TK_NO_ERROR;
}

ProError placeLCRnetworkAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	double fanoutHeightVal, circleRadiusVal, bondpadSideVal;
	status = ProUIInputpanelDoubleGet(dialog, ip_fanout_height, &fanoutHeightVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_circle_radius, &circleRadiusVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_side, &bondpadSideVal);

	/*if (bondpadSideVal <= 0 || circleRadiusVal <= 0 || fanoutHeightVal <= 0) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Input error", L"Please provide proper inputs to proceed.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
	}*/

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(dialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProName wPoint1Name, wPoint2Name, wSurfName, wNode1Name, wNode2Name;
	ProCharName cPoint1Name, cPoint2Name;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPoint1Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPoint2Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

	ProWstringToString(cPoint1Name, wPoint1Name);
	ProWstringToString(cPoint2Name, wPoint2Name);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

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
			/*bondpadUDFsels.push_back(pntPlacementSel[0]);
			bondpadUDFsels.push_back(pntSel);
			bondpadUDFsels.push_back(placementSurfSel);
			bondpadUDFsels.push_back(dirSurfSel);*/
			bondpadUDFsels.push_back(pntSel);
			bondpadUDFsels.push_back(placementSurfSel);
			bondpadUDFsels.push_back(pntPlacementSel[0]);
			bondpadUDFsels.push_back(dirSurfSel);
			//udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
			udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
		}
		bondpadUDFsels.clear();
	}

	// IF SELECTED REF IS SURFACE
	else if (placementRefItem.type == PRO_SURFACE)
	{
		ProModelitem topPlaneItem, sidePlaneItem;
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"TOP", &topPlaneItem);
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);

		ProSelection topPlaneSel, sidePlaneSel;
		status = ProSelectionAlloc(NULL, &topPlaneItem, &topPlaneSel);
		status = ProSelectionAlloc(NULL, &sidePlaneItem, &sidePlaneSel);

		bondpadUDFsels.push_back(topPlaneSel);
		bondpadUDFsels.push_back(sidePlaneSel);
		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);
	wcscpy(wNode1Name, L"NODE_");
	wcscat(wNode1Name, wPoint1Name);
	wcscpy(wNode2Name, L"NODE_");
	wcscat(wNode2Name, wPoint2Name);
	status = ProModelitemNameSet(&feats[3], wNode1Name);
	status = ProModelitemNameSet(&feats[4], wNode2Name);

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);

	return status;

}

ProError placeBondpadPairPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	double fanoutHeightVal, circleRadiusVal, bondpadSideVal;
	status = ProUIInputpanelDoubleGet(dialog, ip_fanout_height, &fanoutHeightVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_circle_radius, &circleRadiusVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_side, &bondpadSideVal);

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(dialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProName wPoint1Name, wPoint2Name, wSurfName, wNode1Name, wNode2Name;
	ProCharName cPoint1Name, cPoint2Name;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPoint1Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPoint2Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

	ProWstringToString(cPoint1Name, wPoint1Name);
	ProWstringToString(cPoint2Name, wPoint2Name);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	status = ProSelectionModelitemGet(pntPlacementSel[0], &placementRefItem);

	// IF SELECTED REF IS SURFACE
	if (placementRefItem.type == PRO_SURFACE)
	{
		ProModelitem topPlaneItem, sidePlaneItem;
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"TOP", &topPlaneItem);
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);

		ProSelection topPlaneSel, sidePlaneSel;
		status = ProSelectionAlloc(NULL, &topPlaneItem, &topPlaneSel);
		status = ProSelectionAlloc(NULL, &sidePlaneItem, &sidePlaneSel);

		bondpadUDFsels.push_back(topPlaneSel);
		bondpadUDFsels.push_back(sidePlaneSel);
		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);
	wcscpy(wNode1Name, L"NODE_");
	wcscat(wNode1Name, wPoint1Name);
	wcscpy(wNode2Name, L"NODE_");
	wcscat(wNode2Name, wPoint2Name);
	status = ProModelitemNameSet(&feats[3], wNode1Name );
	status = ProModelitemNameSet(&pntItem, wNode2Name);

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);

	return status;
}

ProError placeLargeBondpadPairPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	double fanoutHeightVal, circleRadiusVal, bondpadSideVal;
	status = ProUIInputpanelDoubleGet(dialog, ip_fanout_height, &fanoutHeightVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_circle_radius, &circleRadiusVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_side, &bondpadSideVal);

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(dialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProName wPoint1Name, wPoint2Name, wSurfName, wNode1Name, wNode2Name;
	ProCharName cPoint1Name, cPoint2Name;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPoint1Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPoint2Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

	ProWstringToString(cPoint1Name, wPoint1Name);
	ProWstringToString(cPoint2Name, wPoint2Name);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	status = ProSelectionModelitemGet(pntPlacementSel[0], &placementRefItem);

	// IF SELECTED REF IS SURFACE
	if (placementRefItem.type == PRO_SURFACE)
	{
		ProModelitem topPlaneItem, sidePlaneItem;
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"TOP", &topPlaneItem);
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);

		ProSelection topPlaneSel, sidePlaneSel;
		status = ProSelectionAlloc(NULL, &topPlaneItem, &topPlaneSel);
		status = ProSelectionAlloc(NULL, &sidePlaneItem, &sidePlaneSel);

		bondpadUDFsels.push_back(topPlaneSel);
		bondpadUDFsels.push_back(sidePlaneSel);
		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);
	wcscpy(wNode1Name, L"NODE_");
	wcscat(wNode1Name, wPoint1Name);
	wcscpy(wNode2Name, L"NODE_");
	wcscat(wNode2Name, wPoint2Name);
	status = ProModelitemNameSet(&feats[3], wNode1Name);
	status = ProModelitemNameSet(&feats[4], wNode2Name);

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);

	return status;
}

ProError placeResistorNetwork1PbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	/*static char* ip_bondpad_side = "ip_bondpad_side_len", * ip_overall_len = "ip_overall_len", * ip_width = "ip_width", * ip_bondpad_to_res_dist = "ip_bondpad_to_resistor_dist";

	double bondpadSideVal, overallLenVal, widthVal, bondpadToResistorDistVal;
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_side, &bondpadSideVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_overall_len, &overallLenVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_width, &widthVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_to_res_dist, &bondpadToResistorDistVal);*/

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(dialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProName wPoint1Name, wPoint2Name, wSurfName, wNode1Name, wNode2Name;
	ProCharName cPoint1Name, cPoint2Name;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPoint1Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPoint2Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

	ProWstringToString(cPoint1Name, wPoint1Name);
	ProWstringToString(cPoint2Name, wPoint2Name);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	status = ProSelectionModelitemGet(pntPlacementSel[0], &placementRefItem);

	// IF SELECTED REF IS SURFACE
	if (placementRefItem.type == PRO_SURFACE)
	{
		ProModelitem frontPlaneItem, sidePlaneItem;
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"FRONT", &frontPlaneItem);
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);

		ProSelection frontPlaneSel, sidePlaneSel;
		status = ProSelectionAlloc(NULL, &frontPlaneItem, &frontPlaneSel);
		status = ProSelectionAlloc(NULL, &sidePlaneItem, &sidePlaneSel);

		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		bondpadUDFsels.push_back(frontPlaneSel);
		bondpadUDFsels.push_back(sidePlaneSel);
		udfSuccess = LoadResistorUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);
	wcscpy(wNode1Name, L"NODE_");
	wcscat(wNode1Name, wPoint1Name);
	wcscpy(wNode2Name, L"NODE_");
	wcscat(wNode2Name, wPoint2Name);
	status = ProModelitemNameSet(&feats[83], wNode1Name);
	status = ProModelitemNameSet(&feats[84], wNode2Name);

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);

	return status;
}

ProError placeResistorNetwork2PbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	static char* ip_bondpad_side = "ip_bondpad_side_len", *ip_overall_len = "ip_overall_len", *ip_width = "ip_width", *ip_bondpad_to_res_dist = "ip_bondpad_to_resistor_dist";

	double bondpadSideVal, overallLenVal, widthVal, bondpadToResistorDistVal;
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_side, &bondpadSideVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_overall_len, &overallLenVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_width, &widthVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_to_res_dist, &bondpadToResistorDistVal);

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(dialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProName wPoint1Name, wPoint2Name, wSurfName, wNode1Name, wNode2Name;
	ProCharName cPoint1Name, cPoint2Name;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPoint1Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPoint2Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

	ProWstringToString(cPoint1Name, wPoint1Name);
	ProWstringToString(cPoint2Name, wPoint2Name);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	status = ProSelectionModelitemGet(pntPlacementSel[0], &placementRefItem);

	// IF SELECTED REF IS SURFACE
	if (placementRefItem.type == PRO_SURFACE)
	{
		ProModelitem frontPlaneItem, sidePlaneItem;
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"FRONT", &frontPlaneItem);
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);

		ProSelection frontPlaneSel, sidePlaneSel;
		status = ProSelectionAlloc(NULL, &frontPlaneItem, &frontPlaneSel);
		status = ProSelectionAlloc(NULL, &sidePlaneItem, &sidePlaneSel);

		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		bondpadUDFsels.push_back(sidePlaneSel);
		bondpadUDFsels.push_back(frontPlaneSel);
		udfSuccess = LoadResistorUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);
	wcscpy(wNode1Name, L"NODE_");
	wcscat(wNode1Name, wPoint1Name);
	wcscpy(wNode2Name, L"NODE_");
	wcscat(wNode2Name, wPoint2Name);
	status = ProModelitemNameSet(&feats[80], wNode1Name);
	status = ProModelitemNameSet(&feats[81], wNode2Name);

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);

	return status;
}

ProError placeInterdigitatedCapacitorPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	/*static char* ip_bondpad_side = "ip_bondpad_side_len", * ip_overall_len = "ip_overall_len", * ip_width = "ip_width", * ip_bondpad_to_cap_dist = "ip_bondpad_to_capacitor_dist";

	double bondpadSideVal, overallLenVal, widthVal, bondpadToCapacitorDistVal;
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_side, &bondpadSideVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_overall_len, &overallLenVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_width, &widthVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_to_cap_dist, &bondpadToCapacitorDistVal);*/

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(dialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProName wPoint1Name, wPoint2Name, wSurfName, wNode1Name, wNode2Name;
	ProCharName cPoint1Name, cPoint2Name;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPoint1Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPoint2Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

	ProWstringToString(cPoint1Name, wPoint1Name);
	ProWstringToString(cPoint2Name, wPoint2Name);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

	status = ProSelectionModelitemGet(pntPlacementSel[0], &placementRefItem);

	// IF SELECTED REF IS SURFACE
	if (placementRefItem.type == PRO_SURFACE)
	{
		ProModelitem frontPlaneItem, sidePlaneItem;
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"FRONT", &frontPlaneItem);
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);

		ProSelection frontPlaneSel, sidePlaneSel;
		status = ProSelectionAlloc(NULL, &frontPlaneItem, &frontPlaneSel);
		status = ProSelectionAlloc(NULL, &sidePlaneItem, &sidePlaneSel);

		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		bondpadUDFsels.push_back(frontPlaneSel);
		bondpadUDFsels.push_back(sidePlaneSel);
		udfSuccess = LoadCapacitorUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);
	wcscpy(wNode1Name, L"NODE_");
	wcscat(wNode1Name, wPoint1Name);
	wcscpy(wNode2Name, L"NODE_");
	wcscat(wNode2Name, wPoint2Name);
	status = ProModelitemNameSet(&feats[97], wNode1Name);
	status = ProModelitemNameSet(&feats[98], wNode2Name);

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);

	return status;
}

ProError placeSerpentineResistorPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	ProMdl CurMdl1;
	bool udfSuccess = false;

	double fanoutHeightVal, circleRadiusVal, bondpadSideVal;
	status = ProUIInputpanelDoubleGet(dialog, ip_fanout_height, &fanoutHeightVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_circle_radius, &circleRadiusVal);
	status = ProUIInputpanelDoubleGet(dialog, ip_bondpad_side, &bondpadSideVal);

	status = ProUIDialogHide(mainDialog);
	status = ProUIDialogHide(dialog);

	status = ProMdlCurrentGet(&CurMdl1);

	char* bondpadUdfpath = (char*)appdata;

	vector<ProSelection> bondpadUDFsels;
	ProFeature pntFeat, bondpadUDFFeat;
	ProModelitem placementRefItem, pntItem, placementSurfItem, dirSurfItem, dirEdgeItem;
	ProSelection pntSel = NULL, placementSurfSel = NULL, dirSurfSel = NULL, dirEdgeSel = NULL;
	ProSelection* pntPlacementSel;
	int nSel;

	status = ProSelect("surface", 1, NULL, NULL, NULL, NULL, &pntPlacementSel, &nSel);
	if (status != PRO_TK_NO_ERROR || nSel < 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Invalid selection", L"Please select proper surfaces/edges.", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProName wPoint1Name, wPoint2Name, wSurfName, wNode1Name, wNode2Name;
	ProCharName cPoint1Name, cPoint2Name;
	wchar_t* msgfil = L"Message.txt";
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 1 name:");
	status = ProMessageStringRead(10, wPoint1Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;
	status = ProMessageDisplay(msgfil, (char*)"Enter the Node 2 name:");
	status = ProMessageStringRead(10, wPoint2Name);
	if (status == PRO_TK_MSG_USER_QUIT)
		return status;

	ProWstringToString(cPoint1Name, wPoint1Name);
	ProWstringToString(cPoint2Name, wPoint2Name);
	status = ProDemoFieldPointCreate(pntPlacementSel[0], &pntFeat);

	status = ProFeatureGeomitemVisit(&pntFeat, PRO_POINT, (ProGeomitemAction)featPointVisitAction, NULL, (ProAppData)&pntItem);
	status = ProSelectionAlloc(NULL, &pntItem, &pntSel);	// point selection

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
			/*bondpadUDFsels.push_back(pntPlacementSel[0]);
			bondpadUDFsels.push_back(pntSel);
			bondpadUDFsels.push_back(placementSurfSel);
			bondpadUDFsels.push_back(dirSurfSel);*/
			bondpadUDFsels.push_back(pntSel);
			bondpadUDFsels.push_back(placementSurfSel);
			bondpadUDFsels.push_back(pntPlacementSel[0]);
			bondpadUDFsels.push_back(dirSurfSel);
			//udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
			udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
		}
		bondpadUDFsels.clear();
	}

	// IF SELECTED REF IS SURFACE
	else if (placementRefItem.type == PRO_SURFACE)
	{
		ProModelitem topPlaneItem, sidePlaneItem;
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"TOP", &topPlaneItem);
		status = ProModelitemByNameInit(CurMdl1, PRO_SURFACE, L"SIDE", &sidePlaneItem);

		ProSelection topPlaneSel, sidePlaneSel;
		status = ProSelectionAlloc(NULL, &topPlaneItem, &topPlaneSel);
		status = ProSelectionAlloc(NULL, &sidePlaneItem, &sidePlaneSel);

		bondpadUDFsels.push_back(topPlaneSel);
		bondpadUDFsels.push_back(sidePlaneSel);
		bondpadUDFsels.push_back(pntSel);
		bondpadUDFsels.push_back(pntPlacementSel[0]);
		udfSuccess = LoadBondpadUDF(CurMdl1, bondpadUdfpath, bondpadUDFsels, bondpadUDFFeat, NULL, PRO_B_FALSE, PRO_B_TRUE);
	}

	if (!udfSuccess) {
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"UDF creation failed", L"UDF fails to create based on the provided inputs", buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);
		return status;
	}

	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&bondpadUDFFeat, &feats);
	wcscpy(wNode1Name, L"NODE_");
	wcscat(wNode1Name, wPoint1Name);
	wcscpy(wNode2Name, L"NODE_");
	wcscat(wNode2Name, wPoint2Name);
	status = ProModelitemNameSet(&feats[3], wNode2Name );
	status = ProModelitemNameSet(&pntItem, wNode1Name);

	status = ProUIDialogShow(mainDialog);
	status = ProUIDialogExit(dialog, PRO_TK_NO_ERROR);

	return status;

}

ProError executeLcrNetworkPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	status = ProUIDialogCreate(paramsDialogLCR, paramsDialogLCR);
	if (status != PRO_TK_NO_ERROR)
		return status;

	double defaultHeight = 0, defaultWidth = 0;
	ProErr lErr;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	lErr = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\lcr_network_udf1.gph");

	status = ProUIDialogAboveactivewindowSet(paramsDialogLCR, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialogLCR, okbtn, (ProUIAction)placeLCRnetworkAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(paramsDialogLCR, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialogLCR, closeMYdialog, NULL);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	int stat = -1;
	status = ProUIDialogActivate(paramsDialogLCR, &stat);
	status = ProUIDialogDestroy(paramsDialogLCR);

	return PRO_TK_NO_ERROR;
}

ProError executeSerpentineResistorPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	status = ProUIDialogCreate(resistorParamsDialog, resistorParamsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\serpentine_resistor_udf.gph");

	status = ProUIDialogAboveactivewindowSet(resistorParamsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, okbtn, (ProUIAction)placeResistorNetwork1PbAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(resistorParamsDialog, closeMYdialog, NULL);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
	ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	int stat = -1;
	status = ProUIDialogActivate(resistorParamsDialog, &stat);
	status = ProUIDialogDestroy(resistorParamsDialog);
}

ProError executeBondpadPairPbAction(char* dialog, char* component, ProAppData appdata)

{
	ProError status;
	/*status = ProUIDialogCreate(paramsDialogLCR, paramsDialogLCR);
	if (status != PRO_TK_NO_ERROR)
		return status;*/

	double defaultHeight = 0, defaultWidth = 0;
	ProErr lErr;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	lErr = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\bondpad_pair_udf.gph");

	/*status = ProUIDialogAboveactivewindowSet(paramsDialogLCR, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialogLCR, okbtn, (ProUIAction)placeLCRnetworkAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(paramsDialogLCR, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialogLCR, closeMYdialog, NULL);*/

	status = placeBondpadPairPbAction(paramsDialogLCR, okbtn, (ProAppData)bondpadUdfpath);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	/*int stat = -1;
	status = ProUIDialogActivate(paramsDialogLCR, &stat);
	status = ProUIDialogDestroy(paramsDialogLCR);*/

	return PRO_TK_NO_ERROR;
}

ProError executeInterdigitatedCapacitorPbAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	status = ProUIDialogCreate(capacitorParamsDialog, capacitorParamsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\interdigitated_capacitor_dim.gph");

	status = ProUIDialogAboveactivewindowSet(capacitorParamsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(capacitorParamsDialog, okbtn, (ProUIAction)placeInterdigitatedCapacitorPbAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(capacitorParamsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(capacitorParamsDialog, closeMYdialog, NULL);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
	ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	int stat = -1;
	status = ProUIDialogActivate(capacitorParamsDialog, &stat);
	status = ProUIDialogDestroy(capacitorParamsDialog);

	return PRO_TK_NO_ERROR;
}

ProError executeLargeBondpadPairPbAction(char* dialog, char* component, ProAppData appdata)

{
	ProError status;
	/*status = ProUIDialogCreate(paramsDialogLCR, paramsDialogLCR);
	if (status != PRO_TK_NO_ERROR)
		return status;*/

	double defaultHeight = 0, defaultWidth = 0;
	ProErr lErr;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	lErr = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\bondpad_large_udf.gph");

	/*status = ProUIDialogAboveactivewindowSet(paramsDialogLCR, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(paramsDialogLCR, okbtn, (ProUIAction)placeLCRnetworkAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(paramsDialogLCR, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(paramsDialogLCR, closeMYdialog, NULL);*/

	status = placeLargeBondpadPairPbAction(paramsDialogLCR, okbtn, (ProAppData)bondpadUdfpath);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	/*int stat = -1;
	status = ProUIDialogActivate(paramsDialogLCR, &stat);
	status = ProUIDialogDestroy(paramsDialogLCR);*/

	return PRO_TK_NO_ERROR;
}

ProError executeResistorNetwork1Action(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	status = ProUIDialogCreate(resistorParamsDialog, resistorParamsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\resistor_network_1.gph");

	status = ProUIDialogAboveactivewindowSet(resistorParamsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, okbtn, (ProUIAction)placeResistorNetwork1PbAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(resistorParamsDialog, closeMYdialog, NULL);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
	ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	int stat = -1;
	status = ProUIDialogActivate(resistorParamsDialog, &stat);
	status = ProUIDialogDestroy(resistorParamsDialog);

	return PRO_TK_NO_ERROR;
}

ProError executeResistorNetwork2Action(char* dialog, char* component, ProAppData appdata)

{
	ProError status;
	status = ProUIDialogCreate(resistorParamsDialog, resistorParamsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\resistor_network_2.gph");

	status = ProUIDialogAboveactivewindowSet(resistorParamsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, okbtn, (ProUIAction)placeResistorNetwork2PbAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(resistorParamsDialog, closeMYdialog, NULL);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
	ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	int stat = -1;
	status = ProUIDialogActivate(resistorParamsDialog, &stat);
	status = ProUIDialogDestroy(resistorParamsDialog);

	return PRO_TK_NO_ERROR;
}

ProError executeResistorNetwork2InvAction(char* dialog, char* component, ProAppData appdata)
{
	ProError status;
	status = ProUIDialogCreate(resistorParamsDialog, resistorParamsDialog);
	if (status != PRO_TK_NO_ERROR)
		return status;

	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;
	char bondpadUdfpath[PRO_PATH_SIZE] = "";
	strcpy(bondpadUdfpath, cTextDirPath);
	strcat(bondpadUdfpath, "\\text\\UDF_LATEST\\resistor_network_2_inv.gph");

	status = ProUIDialogAboveactivewindowSet(resistorParamsDialog, PRO_B_FALSE);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, okbtn, (ProUIAction)placeResistorNetwork2PbAction, (ProAppData)bondpadUdfpath);
	status = ProUIPushbuttonActivateActionSet(resistorParamsDialog, closebtn, closeMYdialog, NULL);
	status = ProUIDialogCloseActionSet(resistorParamsDialog, closeMYdialog, NULL);

	/*string sBondpadUdfpath = string(bondpadUdfpath);
	ProStringToWstring(UdfFilePath, (char*)sBondpadUdfpath.c_str());
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
	status = ProUIInputpanelDoubleSet(paramsDialog, ip_width, defaultWidth);*/

	int stat = -1;
	status = ProUIDialogActivate(resistorParamsDialog, &stat);
	status = ProUIDialogDestroy(resistorParamsDialog);

	return PRO_TK_NO_ERROR;
}

// ----------------------------------------------   SUPPORTING FUNCTIONS   ----------------------------------------------
int createFanOut(string point1Name, string point2Name, ProMdl CurMdl1, vector<PointCoOrd> &fanoutMidpointsCoords, double widthDimVal, ProAsmcomppath &comp_path, vector<PointData> &vectPointData)
{
	ProError status;
	ProFeature _csoFeat, _csoFeat1;
	std::vector<ProSelection> _UdfInputSel;
	PointData _curPointData = {};
	/*string _point1Name = nodePairs[i][0].ref;
	string _point2Name = nodePairs[i][1].ref;*/
	string _point1Name = point1Name;
	string _point2Name = point2Name;

	int _surfaceid = 41245;
	ProFeature grpCurveFeat;
	int create_curve_sts = createCurveBw2Points(CurMdl1, _point1Name, _csoFeat, _curPointData, _point2Name, _csoFeat1, status, _surfaceid, comp_path, vectPointData, _UdfInputSel, grpCurveFeat);
	if (create_curve_sts != 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		wchar_t wErrMsg[500];
		char cErrMsg[500];
		ProTKSprintf(cErrMsg, "Unable to create a curve between %s and %s. \nPlease check if these nodes are available in the model.", _point1Name.c_str(), _point2Name.c_str());
		ProStringToWstring(wErrMsg, cErrMsg);
		//status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Curve creation failed",wErrMsg, buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);

		return -1;
		//continue;
	}

	vectPointData.clear();

	bool isFeatFound = false;
	ProFeature _point1Feat, _point2Feat;
	vector<ProFeature> geomItems;
	ProParameter fanoutParam;
	ProParamvalue fanoutParamVal;
	double p1XoffsetVal = 0, p1YoffsetVal = 0, p2XoffsetVal = 0, p2YoffsetVal = 0, finalXoffsetVal = 0, finalYoffsetVal = 0, traceMarginVal = 0;

	isFeatFound = GetFeatureByName(CurMdl1, _point1Name, _point1Feat);
	if (!isFeatFound) {
		return -1;
	}
	/*GetGeomItems(CurMdl1, _point1Feat, geomItems)*/;
	status = ProParameterInit(&_point1Feat, L"X_OFFSET_CLEARANCE", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		p1XoffsetVal = fanoutParamVal.value.d_val;
	}
	status = ProParameterInit(&_point1Feat, L"Y_OFFSET_CLEARANCE", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		p1YoffsetVal = fanoutParamVal.value.d_val;
	}
	status = ProParameterInit(&_point1Feat, L"FANOUT_TRACE_MARGIN", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		traceMarginVal = fanoutParamVal.value.d_val;
	}
	finalXoffsetVal = p1XoffsetVal;
	finalYoffsetVal = p1YoffsetVal;


	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&grpCurveFeat, &feats);//Give only non suppressed entities

	//double bendPntCoOrd[3] = { 0,0,0 };
	vector<PointCoOrd> reqBendPointCoOrds;
	status = ProFeatureGeomitemVisit(&feats[1], PRO_CURVE, (ProGeomitemAction)featCurveGeomitemVisit, NULL, (ProAppData*)&reqBendPointCoOrds);

	bool isBendPointFound = false;
	double epsilon = 0.0001;
	/*if (fabs(bendPntCoOrd[0]) > epsilon && fabs(bendPntCoOrd[1]) > epsilon && fabs(bendPntCoOrd[2]) > epsilon){
		isBendPointFound = true;
	}*/

	if (reqBendPointCoOrds.size() > 0)
		isBendPointFound = true;

	if (isBendPointFound)
	{
		for (int k = 0; k < reqBendPointCoOrds.size(); ++k)
		{
			ProPoint3d pnt3D, nearPnt3d;
			pnt3D[0] = reqBendPointCoOrds[k].x;
			pnt3D[1] = reqBendPointCoOrds[k].y;
			pnt3D[2] = reqBendPointCoOrds[k].z;

			// check if the same obstacle is hit twice
			/*bool isSameObstacle = false;
			for (auto& point : fanoutMidpointsCoords)
			{
				if ((fabs(point.x - pnt3D[0]) < epsilon) && (fabs(point.y - pnt3D[1]) < epsilon) && (fabs(point.z - pnt3D[2]) < epsilon)) {
					pnt3D[0] += 5;
					pnt3D[1] += 5;
					pnt3D[2] += 5;
					isSameObstacle = true;
				}
			}*/

			ProSurface onSurface;
			ProUvParam surfaceUV, updatedSurfUV;
			status = ProSolidProjectPoint((ProSolid)CurMdl1, pnt3D, 3, &onSurface, surfaceUV);
			int surfId;
			status = ProSurfaceIdGet(onSurface, &surfId);
			ProModelitem surfItem;
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, onSurface, &surfItem);
			ProSelection surfSel;
			status = ProSelectionAlloc(NULL, &surfItem, &surfSel);
			/*surfaceUV[0] = surfaceUV[0] + 0.05;
			surfaceUV[1] = surfaceUV[1] + 3;*/

			updatedSurfUV[0] = surfaceUV[0] + ((finalXoffsetVal + traceMarginVal + widthDimVal) * 0.001);
			updatedSurfUV[1] = surfaceUV[1] + finalYoffsetVal + traceMarginVal + widthDimVal - 10;

			/*if (isSameObstacle) {
				surfaceUV[0] -= 5;
				surfaceUV[1] -=	5;
			}*/

			status = ProSelectionUvParamSet(surfSel, updatedSurfUV);
			ProVector projectedPoint;
			status = ProSurfaceXyzdataEval(onSurface, updatedSurfUV, projectedPoint, NULL, NULL, NULL);

			ProBoolean isOnSurf = PRO_B_FALSE;

			status = ProPoint3dOnsurfaceFind(projectedPoint, onSurface, &isOnSurf, NULL);
			if (!isOnSurf) {
				updatedSurfUV[0] = surfaceUV[0] - ((finalXoffsetVal + traceMarginVal + widthDimVal ) * 0.001);
				updatedSurfUV[1] = surfaceUV[1] - (finalYoffsetVal + traceMarginVal + widthDimVal);
				status = ProSelectionUvParamSet(surfSel, updatedSurfUV);

				status = ProSurfaceXyzdataEval(onSurface, updatedSurfUV, projectedPoint, NULL, NULL, NULL);
				status = ProPoint3dOnsurfaceFind(projectedPoint, onSurface, &isOnSurf, NULL);
			}


			ProFeature pointFeature;
			status = ProDemoFieldPointCreate(surfSel, &pointFeature);
			ProCharName cFoMpntName;
			ProName wFoMpntName;
			sprintf(cFoMpntName, "FO_MP_%s_%s_%d", _point1Name.c_str(), _point2Name.c_str(), k+1);
			ProStringToWstring(wFoMpntName, cFoMpntName);
			status = ProModelitemNameSet(&pointFeature, wFoMpntName);

			fanoutMidpointsCoords.push_back({ pnt3D[0], pnt3D[1], pnt3D[2] });

			// check if any other point lies within the specified radius
			vector<ProFeature> fanoutMidpoints;
			status = ProSolidFeatVisit((ProSolid)CurMdl1, (ProFeatureVisitAction)solidFeatCollectAction, (ProFeatureFilterAction)fanoutMidpointFilter, (ProAppData)&fanoutMidpoints);

			bool fanoutInterferenceFound = false;
			//for (int j = 0; j < fanoutMidpoints.size(); ++j)
			//{
			//	ProName wName; ProCharName cName;
			//	status = ProModelitemNameGet(&fanoutMidpoints[j], wName);
			//	ProWstringToString(cName, wName);
			//	if (string(cName).find(cFoMpntName) == string::npos)
			//	{
			//		ProSelection pnt1Sel, pnt2Sel;
			//		vector<ProGeomitem> vecGeomItems;

			//		GetGeomItems(CurMdl1, pointFeature, vecGeomItems);
			//		status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt1Sel);
			//		vecGeomItems.clear();

			//		GetGeomItems(CurMdl1, fanoutMidpoints[j], vecGeomItems);
			//		status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt2Sel);
			//		vecGeomItems.clear();

			//		Pro2dPnt param1, param2;
			//		Pro3dPnt pnt_1, pnt_2;
			//		double distBwPnts;
			//		status = ProSelectionWithOptionsDistanceEval(pnt1Sel, PRO_B_FALSE, pnt2Sel, PRO_B_FALSE, param1, param1, pnt_1, pnt_1, &distBwPnts);

			//		if (distBwPnts < traceMarginVal)
			//			fanoutInterferenceFound = true;
			//	}
			//}
			//fanoutMidpoints.clear();

			//ProName wBendPointName;
			//status = ProModelitemNameGet(&pointFeature, wBendPointName);
			//ProCharName cBendPointName;
			//ProWstringToString(cBendPointName, wBendPointName);

			//status = ProModelitemHide(&feats[1]);

			//if (fanoutInterferenceFound) {
			//	ProUIMessageButton* buttons = nullptr;
			//	ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			//	buttons[0] = PRO_UI_MESSAGE_OK;
			//	ProUIMessageButton resultBtn;
			//	status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Fanout intereference found",
			//		L"Alert: Fanouts detected within the specified boundary. Application will now exit.",
			//		buttons, buttons[0], &resultBtn);
			//	status = ProArrayFree((ProArray*)&buttons);
			//	status = ProUIDialogShow(mainDialog);
			//	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);
			//	return -1;
			//}

			//ProFeature* surf1GrpFeats = NULL, * surf2GrpFeats = NULL;
			//ProParamvalue nodeNameParamVal;
			//ProParameter nodeNameParam;

			//ProFeature csoFeat, csoFeat1;
			//std::vector<ProSelection> UdfInputSel;
			//PointData curPointData = {};
			//string point1 = _point1Name;
			////string point2 = sMiddleCurveName;
			//string point2 = string(cBendPointName);
			//int surfaceid = 41245;
			//ProFeature curveFeat1, offsetCurve1Feat1, offsetCurve2Feat1, surfUDFFeat1, curveFeat2, offsetCurve1Feat2, offsetCurve2Feat2, surfUDFFeat2;
			//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat1);
			//status = ProGroupFeaturesCollect(&curveFeat1, &feats);//Give only non suppressed entities
			//status = ProModelitemHide(&feats[1]);

			//createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat1, offsetCurve1Feat1, offsetCurve2Feat1);
			//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat1, offsetCurve2Feat1, surfUDFFeat1);

			//status = ProGroupFeaturesCollect(&surfUDFFeat1, &surf1GrpFeats);//Give only non suppressed entities

			//status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
			//status = ProParameterInit(&surf1GrpFeats[0], L"WIDTH", &nodeNameParam);
			//if (status != PRO_TK_NO_ERROR) {
			//	status = ProParameterWithUnitsCreate(&surf1GrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
			//}
			//else {
			//	status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
			//}


			////point1 = sMiddleCurveName;
			//point1 = string(cBendPointName);
			//point2 = _point2Name;
			//UdfInputSel.clear();
			//vectPointData.clear();
			//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat2);
			//status = ProGroupFeaturesCollect(&curveFeat2, &feats);//Give only non suppressed entities
			//status = ProModelitemHide(&feats[1]);

			//createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat2, offsetCurve1Feat2, offsetCurve2Feat2);
			//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat2, offsetCurve2Feat2, surfUDFFeat2);

			//status = ProGroupFeaturesCollect(&surfUDFFeat2, &surf2GrpFeats);//Give only non suppressed entities

			//status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
			//status = ProParameterInit(&surf2GrpFeats[0], L"WIDTH", &nodeNameParam);
			//if (status != PRO_TK_NO_ERROR) {
			//	status = ProParameterWithUnitsCreate(&surf2GrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
			//}
			//else {
			//	status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
			//}


			//// check for overlapping
			//ProAsmcomppath fanoutComppath;
			//ProIdTable c_id_table;
			//vector<ProGeomitem> vecGeomitems;
			//vector<int> fanoutSurf1Ids;
			//c_id_table[0] = -1;
			//status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &fanoutComppath);
			//GetGeomItems(CurMdl1, surf1GrpFeats[1], vecGeomitems);
			//fanoutSurf1Ids.push_back(vecGeomitems[0].id);

			//vecGeomitems.clear();
			//GetGeomItems(CurMdl1, surf2GrpFeats[1], vecGeomitems);
			//fanoutSurf1Ids.push_back(vecGeomitems[0].id);

			//isFanoutOverlap = false;
			//status = ProSolidGroupVisit((ProSolid)CurMdl1, (ProGroupVisitAction)visitFanoutSurfsForOverlap, NULL, (ProAppData)&fanoutSurf1Ids);

			//if (isFanoutOverlap)
			//{
			//	ProUIMessageButton* buttons;
			//	ProUIMessageButton result = PRO_UI_MESSAGE_YES;
			//	ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			//	buttons[0] = PRO_UI_MESSAGE_YES;
			//	buttons[1] = PRO_UI_MESSAGE_NO;

			//	status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"Fanout Overlap found",
			//		L"This fanout overlaps with other fanouts. Would you like to create like this?\n\nClick \"Yes\" to create or \"No\" to remove it.",
			//		buttons, buttons[0], &result);

			//	if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
			//	{
			//		int* featList;
			//		ProFeatureDeleteOptions* delete_opts = 0;
			//		status = ProArrayAlloc(1, sizeof(ProFeatureDeleteOptions), 1, (ProArray*)&delete_opts);
			//		delete_opts[0] = PRO_FEAT_DELETE_CLIP;
			//		status = ProArrayAlloc(1, sizeof(int), 1, (ProArray*)&featList);

			//		featList[0] = surf1GrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		ProFeature* deleteGrpFeats;

			//		status = ProGroupFeaturesCollect(&offsetCurve2Feat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&offsetCurve2Feat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&offsetCurve1Feat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&offsetCurve1Feat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&curveFeat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&curveFeat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&grpCurveFeat, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//	}
			//}
		}
		
	}
	//else
	//{
	//	ProFeature* surfGrpFeats = NULL;
	//	ProParamvalue nodeNameParamVal;
	//	ProParameter nodeNameParam;

	//	ProFeature csoFeat, csoFeat1;
	//	std::vector<ProSelection> UdfInputSel;
	//	PointData curPointData = {};
	//	string point1 = _point1Name;
	//	//string point2 = sMiddleCurveName;
	//	string point2 = _point2Name;
	//	int surfaceid = 41245;
	//	ProFeature curveFeat, offsetCurve1Feat, offsetCurve2Feat, surfUDFFeat;
	//	createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat);
	//	status = ProGroupFeaturesCollect(&curveFeat, &feats);//Give only non suppressed entities
	//	status = ProModelitemHide(&feats[1]);

	//	createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat, offsetCurve1Feat, offsetCurve2Feat);
	//	CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat, offsetCurve2Feat, surfUDFFeat);

	//	status = ProGroupFeaturesCollect(&surfUDFFeat, &surfGrpFeats);//Give only non suppressed entities

	//	status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
	//	status = ProParameterInit(&surfGrpFeats[0], L"WIDTH", &nodeNameParam);
	//	if (status != PRO_TK_NO_ERROR) {
	//		status = ProParameterWithUnitsCreate(&surfGrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
	//	}
	//	else {
	//		status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);	
	//	}

	//	// check for overlapping
	//	ProAsmcomppath fanoutComppath;
	//	ProIdTable c_id_table;
	//	ProSelection fanout1Sel, fanout2sel;
	//	vector<ProGeomitem> vecGeomitems;
	//	c_id_table[0] = -1;
	//	status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &fanoutComppath);
	//	GetGeomItems(CurMdl1, surfGrpFeats[1], vecGeomitems);
	//	vector<int> fanoutSurf1Ids;
	//	fanoutSurf1Ids.push_back(vecGeomitems[0].id);

	//	isFanoutOverlap = false;
	//	status = ProSolidGroupVisit((ProSolid)CurMdl1, (ProGroupVisitAction)visitFanoutSurfsForOverlap, NULL, (ProAppData)&fanoutSurf1Ids);

	//	if (isFanoutOverlap)
	//	{
	//		ProUIMessageButton* buttons;
	//		ProUIMessageButton result = PRO_UI_MESSAGE_YES;
	//		ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
	//		buttons[0] = PRO_UI_MESSAGE_YES;
	//		buttons[1] = PRO_UI_MESSAGE_NO;

	//		status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"Fanout Overlap found",
	//			L"This fanout overlaps with other fanouts. Would you like to create like this?\n\nClick \"Yes\" to create or \"No\" to remove it.",
	//			buttons, buttons[0], &result);

	//		if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
	//		{
	//			int* featList;
	//			ProFeatureDeleteOptions* delete_opts = 0;
	//			status = ProArrayAlloc(1, sizeof(ProFeatureDeleteOptions),1, (ProArray*)&delete_opts);
	//			delete_opts[0] = PRO_FEAT_DELETE_CLIP;
	//			status = ProArrayAlloc(1, sizeof(int), 1, (ProArray*)&featList);

	//			featList[0] = surfGrpFeats[0].id;
	//			status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

	//			ProFeature* deleteGrpFeats;
	//			status = ProGroupFeaturesCollect(&offsetCurve2Feat, &deleteGrpFeats);
	//			featList[0] = deleteGrpFeats[0].id;
	//			status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

	//			status = ProGroupFeaturesCollect(&offsetCurve1Feat, &deleteGrpFeats);
	//			featList[0] = deleteGrpFeats[0].id;
	//			status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
	//			
	//			status = ProGroupFeaturesCollect(&curveFeat, &deleteGrpFeats);
	//			featList[0] = deleteGrpFeats[0].id;
	//			status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
	//			
	//			status = ProGroupFeaturesCollect(&grpCurveFeat, &deleteGrpFeats);
	//			featList[0] = deleteGrpFeats[0].id;
	//			status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
	//		}
	//	}
	//}

	return 0;
}

int createFanOutLCR(string point1Name, string point2Name, ProMdl CurMdl1, vector<PointCoOrd>& fanoutMidpointsCoords, double widthDimVal, ProAsmcomppath& comp_path, vector<PointData>& vectPointData)
{
	ProError status;
	ProFeature _csoFeat, _csoFeat1;
	std::vector<ProSelection> _UdfInputSel;
	PointData _curPointData = {};
	/*string _point1Name = nodePairs[i][0].ref;
	string _point2Name = nodePairs[i][1].ref;*/
	string _point1Name = point1Name;
	string _point2Name = point2Name;

	int _surfaceid = 41245;
	ProFeature grpCurveFeat;
	int create_curve_sts = createCurveBw2Points(CurMdl1, _point1Name, _csoFeat, _curPointData, _point2Name, _csoFeat1, status, _surfaceid, comp_path, vectPointData, _UdfInputSel, grpCurveFeat);
	if (create_curve_sts != 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		wchar_t wErrMsg[500];
		char cErrMsg[500];
		ProTKSprintf(cErrMsg, "Unable to create a curve between %s and %s. \nPlease check if these nodes are available in the model.", _point1Name.c_str(), _point2Name.c_str());
		ProStringToWstring(wErrMsg, cErrMsg);
		//status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Curve creation failed",wErrMsg, buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);

		return -1;
		//continue;
	}
	/*if (create_curve_sts == 0)
		return -1;*/

	vectPointData.clear();

	bool isFeatFound = false;
	ProFeature _point1Feat, _point2Feat;
	vector<ProFeature> geomItems;
	ProParameter fanoutParam;
	ProParamvalue fanoutParamVal;
	double p1XoffsetVal = 0, p1YoffsetVal = 0, p2XoffsetVal = 0, p2YoffsetVal = 0, finalXoffsetVal = 0, finalYoffsetVal = 0, traceMarginVal = 0;

	isFeatFound = GetFeatureByName(CurMdl1, _point1Name, _point1Feat);
	if (!isFeatFound) {
		return -1;
	}
	/*GetGeomItems(CurMdl1, _point1Feat, geomItems)*/;
	status = ProParameterInit(&_point1Feat, L"X_OFFSET_CLEARANCE", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		p1XoffsetVal = fanoutParamVal.value.d_val;
	}
	status = ProParameterInit(&_point1Feat, L"Y_OFFSET_CLEARANCE", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		p1YoffsetVal = fanoutParamVal.value.d_val;
	}
	status = ProParameterInit(&_point1Feat, L"FANOUT_TRACE_MARGIN", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		traceMarginVal = fanoutParamVal.value.d_val;
	}
	finalXoffsetVal = p1XoffsetVal;
	finalYoffsetVal = p1YoffsetVal;


	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&grpCurveFeat, &feats);//Give only non suppressed entities

	//double bendPntCoOrd[3] = { 0,0,0 };
	vector<PointCoOrd> reqBendPointCoOrds;
	status = ProFeatureGeomitemVisit(&feats[1], PRO_CURVE, (ProGeomitemAction)featCurveGeomitemVisit, NULL, (ProAppData*)&reqBendPointCoOrds);

	bool isBendPointFound = false;
	double epsilon = 0.0001;
	/*if (fabs(bendPntCoOrd[0]) > epsilon && fabs(bendPntCoOrd[1]) > epsilon && fabs(bendPntCoOrd[2]) > epsilon){
		isBendPointFound = true;
	}*/

	/*if (reqBendPointCoOrds.size() > 0)
		isBendPointFound = true;*/

	if (isBendPointFound)
	{
		for (int k = 0; k < reqBendPointCoOrds.size(); ++k)
		{
			ProPoint3d pnt3D, nearPnt3d;
			pnt3D[0] = reqBendPointCoOrds[k].x;
			pnt3D[1] = reqBendPointCoOrds[k].y;
			pnt3D[2] = reqBendPointCoOrds[k].z;

			// check if the same obstacle is hit twice
			/*bool isSameObstacle = false;
			for (auto& point : fanoutMidpointsCoords)
			{
				if ((fabs(point.x - pnt3D[0]) < epsilon) && (fabs(point.y - pnt3D[1]) < epsilon) && (fabs(point.z - pnt3D[2]) < epsilon)) {
					pnt3D[0] += 5;
					pnt3D[1] += 5;
					pnt3D[2] += 5;
					isSameObstacle = true;
				}
			}*/

			ProSurface onSurface;
			ProUvParam surfaceUV, updatedSurfUV;
			status = ProSolidProjectPoint((ProSolid)CurMdl1, pnt3D, 3, &onSurface, surfaceUV);
			int surfId;
			status = ProSurfaceIdGet(onSurface, &surfId);
			ProModelitem surfItem;
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, onSurface, &surfItem);
			ProSelection surfSel;
			status = ProSelectionAlloc(NULL, &surfItem, &surfSel);
			/*surfaceUV[0] = surfaceUV[0] + 0.05;
			surfaceUV[1] = surfaceUV[1] + 3;*/

			updatedSurfUV[0] = surfaceUV[0] + ((finalXoffsetVal + traceMarginVal + widthDimVal) * 0.001);
			updatedSurfUV[1] = surfaceUV[1] + finalYoffsetVal + traceMarginVal + widthDimVal - 10;

			/*if (isSameObstacle) {
				surfaceUV[0] -= 5;
				surfaceUV[1] -=	5;
			}*/

			status = ProSelectionUvParamSet(surfSel, updatedSurfUV);
			ProVector projectedPoint;
			status = ProSurfaceXyzdataEval(onSurface, updatedSurfUV, projectedPoint, NULL, NULL, NULL);

			ProBoolean isOnSurf = PRO_B_FALSE;

			status = ProPoint3dOnsurfaceFind(projectedPoint, onSurface, &isOnSurf, NULL);
			if (!isOnSurf) {
				updatedSurfUV[0] = surfaceUV[0] - ((finalXoffsetVal + traceMarginVal + widthDimVal) * 0.001);
				updatedSurfUV[1] = surfaceUV[1] - (finalYoffsetVal + traceMarginVal + widthDimVal);
				status = ProSelectionUvParamSet(surfSel, updatedSurfUV);

				status = ProSurfaceXyzdataEval(onSurface, updatedSurfUV, projectedPoint, NULL, NULL, NULL);
				status = ProPoint3dOnsurfaceFind(projectedPoint, onSurface, &isOnSurf, NULL);
			}


			ProFeature pointFeature;
			status = ProDemoFieldPointCreate(surfSel, &pointFeature);
			ProCharName cFoMpntName;
			ProName wFoMpntName;
			sprintf(cFoMpntName, "FO_MP_%s_%s_%d", _point1Name.c_str(), _point2Name.c_str(), k + 1);
			ProStringToWstring(wFoMpntName, cFoMpntName);
			status = ProModelitemNameSet(&pointFeature, wFoMpntName);

			fanoutMidpointsCoords.push_back({ pnt3D[0], pnt3D[1], pnt3D[2] });

			// check if any other point lies within the specified radius
			vector<ProFeature> fanoutMidpoints;
			status = ProSolidFeatVisit((ProSolid)CurMdl1, (ProFeatureVisitAction)solidFeatCollectAction, (ProFeatureFilterAction)fanoutMidpointFilter, (ProAppData)&fanoutMidpoints);

			bool fanoutInterferenceFound = false;
			//for (int j = 0; j < fanoutMidpoints.size(); ++j)
			//{
			//	ProName wName; ProCharName cName;
			//	status = ProModelitemNameGet(&fanoutMidpoints[j], wName);
			//	ProWstringToString(cName, wName);
			//	if (string(cName).find(cFoMpntName) == string::npos)
			//	{
			//		ProSelection pnt1Sel, pnt2Sel;
			//		vector<ProGeomitem> vecGeomItems;

			//		GetGeomItems(CurMdl1, pointFeature, vecGeomItems);
			//		status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt1Sel);
			//		vecGeomItems.clear();

			//		GetGeomItems(CurMdl1, fanoutMidpoints[j], vecGeomItems);
			//		status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt2Sel);
			//		vecGeomItems.clear();

			//		Pro2dPnt param1, param2;
			//		Pro3dPnt pnt_1, pnt_2;
			//		double distBwPnts;
			//		status = ProSelectionWithOptionsDistanceEval(pnt1Sel, PRO_B_FALSE, pnt2Sel, PRO_B_FALSE, param1, param1, pnt_1, pnt_1, &distBwPnts);

			//		if (distBwPnts < traceMarginVal)
			//			fanoutInterferenceFound = true;
			//	}
			//}
			//fanoutMidpoints.clear();

			//ProName wBendPointName;
			//status = ProModelitemNameGet(&pointFeature, wBendPointName);
			//ProCharName cBendPointName;
			//ProWstringToString(cBendPointName, wBendPointName);

			//status = ProModelitemHide(&feats[1]);

			//if (fanoutInterferenceFound) {
			//	ProUIMessageButton* buttons = nullptr;
			//	ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			//	buttons[0] = PRO_UI_MESSAGE_OK;
			//	ProUIMessageButton resultBtn;
			//	status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Fanout intereference found",
			//		L"Alert: Fanouts detected within the specified boundary. Application will now exit.",
			//		buttons, buttons[0], &resultBtn);
			//	status = ProArrayFree((ProArray*)&buttons);
			//	status = ProUIDialogShow(mainDialog);
			//	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);
			//	return -1;
			//}

			//ProFeature* surf1GrpFeats = NULL, * surf2GrpFeats = NULL;
			//ProParamvalue nodeNameParamVal;
			//ProParameter nodeNameParam;

			//ProFeature csoFeat, csoFeat1;
			//std::vector<ProSelection> UdfInputSel;
			//PointData curPointData = {};
			//string point1 = _point1Name;
			////string point2 = sMiddleCurveName;
			//string point2 = string(cBendPointName);
			//int surfaceid = 41245;
			//ProFeature curveFeat1, offsetCurve1Feat1, offsetCurve2Feat1, surfUDFFeat1, curveFeat2, offsetCurve1Feat2, offsetCurve2Feat2, surfUDFFeat2;
			//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat1);
			//status = ProGroupFeaturesCollect(&curveFeat1, &feats);//Give only non suppressed entities
			//status = ProModelitemHide(&feats[1]);

			//createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat1, offsetCurve1Feat1, offsetCurve2Feat1);
			//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat1, offsetCurve2Feat1, surfUDFFeat1);

			//status = ProGroupFeaturesCollect(&surfUDFFeat1, &surf1GrpFeats);//Give only non suppressed entities

			//status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
			//status = ProParameterInit(&surf1GrpFeats[0], L"WIDTH", &nodeNameParam);
			//if (status != PRO_TK_NO_ERROR) {
			//	status = ProParameterWithUnitsCreate(&surf1GrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
			//}
			//else {
			//	status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
			//}


			////point1 = sMiddleCurveName;
			//point1 = string(cBendPointName);
			//point2 = _point2Name;
			//UdfInputSel.clear();
			//vectPointData.clear();
			//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat2);
			//status = ProGroupFeaturesCollect(&curveFeat2, &feats);//Give only non suppressed entities
			//status = ProModelitemHide(&feats[1]);

			//createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat2, offsetCurve1Feat2, offsetCurve2Feat2);
			//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat2, offsetCurve2Feat2, surfUDFFeat2);

			//status = ProGroupFeaturesCollect(&surfUDFFeat2, &surf2GrpFeats);//Give only non suppressed entities

			//status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
			//status = ProParameterInit(&surf2GrpFeats[0], L"WIDTH", &nodeNameParam);
			//if (status != PRO_TK_NO_ERROR) {
			//	status = ProParameterWithUnitsCreate(&surf2GrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
			//}
			//else {
			//	status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
			//}


			//// check for overlapping
			//ProAsmcomppath fanoutComppath;
			//ProIdTable c_id_table;
			//vector<ProGeomitem> vecGeomitems;
			//vector<int> fanoutSurf1Ids;
			//c_id_table[0] = -1;
			//status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &fanoutComppath);
			//GetGeomItems(CurMdl1, surf1GrpFeats[1], vecGeomitems);
			//fanoutSurf1Ids.push_back(vecGeomitems[0].id);

			//vecGeomitems.clear();
			//GetGeomItems(CurMdl1, surf2GrpFeats[1], vecGeomitems);
			//fanoutSurf1Ids.push_back(vecGeomitems[0].id);

			//isFanoutOverlap = false;
			//status = ProSolidGroupVisit((ProSolid)CurMdl1, (ProGroupVisitAction)visitFanoutSurfsForOverlap, NULL, (ProAppData)&fanoutSurf1Ids);

			//if (isFanoutOverlap)
			//{
			//	ProUIMessageButton* buttons;
			//	ProUIMessageButton result = PRO_UI_MESSAGE_YES;
			//	ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			//	buttons[0] = PRO_UI_MESSAGE_YES;
			//	buttons[1] = PRO_UI_MESSAGE_NO;

			//	status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"Fanout Overlap found",
			//		L"This fanout overlaps with other fanouts. Would you like to create like this?\n\nClick \"Yes\" to create or \"No\" to remove it.",
			//		buttons, buttons[0], &result);

			//	if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
			//	{
			//		int* featList;
			//		ProFeatureDeleteOptions* delete_opts = 0;
			//		status = ProArrayAlloc(1, sizeof(ProFeatureDeleteOptions), 1, (ProArray*)&delete_opts);
			//		delete_opts[0] = PRO_FEAT_DELETE_CLIP;
			//		status = ProArrayAlloc(1, sizeof(int), 1, (ProArray*)&featList);

			//		featList[0] = surf1GrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		ProFeature* deleteGrpFeats;

			//		status = ProGroupFeaturesCollect(&offsetCurve2Feat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&offsetCurve2Feat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&offsetCurve1Feat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&offsetCurve1Feat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&curveFeat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&curveFeat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&grpCurveFeat, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//	}
			//}
		}

	}
	else
	{
		ProFeature* surfGrpFeats = NULL;
		ProParamvalue nodeNameParamVal;
		ProParameter nodeNameParam;

		ProFeature csoFeat, csoFeat1;
		std::vector<ProSelection> UdfInputSel;
		PointData curPointData = {};
		string point1 = _point1Name;
		//string point2 = sMiddleCurveName;
		string point2 = _point2Name;
		int surfaceid = 41245;
		ProFeature curveFeat, offsetCurve1Feat, offsetCurve2Feat, surfUDFFeat;
		createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat);
		status = ProGroupFeaturesCollect(&curveFeat, &feats);//Give only non suppressed entities
		status = ProModelitemHide(&feats[1]);

		createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat, offsetCurve1Feat, offsetCurve2Feat);
		CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat, offsetCurve2Feat, surfUDFFeat);

		status = ProGroupFeaturesCollect(&surfUDFFeat, &surfGrpFeats);//Give only non suppressed entities

		status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
		status = ProParameterInit(&surfGrpFeats[0], L"WIDTH", &nodeNameParam);
		if (status != PRO_TK_NO_ERROR) {
			status = ProParameterWithUnitsCreate(&surfGrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
		}
		else {
			status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);	
		}

		// check for overlapping
		ProAsmcomppath fanoutComppath;
		ProIdTable c_id_table;
		ProSelection fanout1Sel, fanout2sel;
		vector<ProGeomitem> vecGeomitems;
		c_id_table[0] = -1;
		status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &fanoutComppath);
		GetGeomItems(CurMdl1, surfGrpFeats[1], vecGeomitems);
		vector<int> fanoutSurf1Ids;
		fanoutSurf1Ids.push_back(vecGeomitems[0].id);

		isFanoutOverlap = false;
		status = ProSolidGroupVisit((ProSolid)CurMdl1, (ProGroupVisitAction)visitFanoutSurfsForOverlap, NULL, (ProAppData)&fanoutSurf1Ids);

		if (isFanoutOverlap)
		{
			ProUIMessageButton* buttons;
			ProUIMessageButton result = PRO_UI_MESSAGE_YES;
			ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			buttons[0] = PRO_UI_MESSAGE_YES;
			buttons[1] = PRO_UI_MESSAGE_NO;

			status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"Fanout Overlap found",
				L"This fanout overlaps with other fanouts. Would you like to create like this?\n\nClick \"Yes\" to create or \"No\" to remove it.",
				buttons, buttons[0], &result);

			if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
			{
				int* featList;
				ProFeatureDeleteOptions* delete_opts = 0;
				status = ProArrayAlloc(1, sizeof(ProFeatureDeleteOptions),1, (ProArray*)&delete_opts);
				delete_opts[0] = PRO_FEAT_DELETE_CLIP;
				status = ProArrayAlloc(1, sizeof(int), 1, (ProArray*)&featList);

				featList[0] = surfGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

				ProFeature* deleteGrpFeats;
				status = ProGroupFeaturesCollect(&offsetCurve2Feat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

				status = ProGroupFeaturesCollect(&offsetCurve1Feat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
				
				status = ProGroupFeaturesCollect(&curveFeat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
				
				status = ProGroupFeaturesCollect(&grpCurveFeat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			}
		}
	}

	return 0;
}

int createFanOutStraight(string point1Name, string point2Name, ProMdl CurMdl1, vector<PointCoOrd>& fanoutMidpointsCoords, double widthDimVal, ProAsmcomppath& comp_path, vector<PointData>& vectPointData)
{
	ProError status;
	ProFeature _csoFeat, _csoFeat1;
	std::vector<ProSelection> _UdfInputSel;
	PointData _curPointData = {};
	/*string _point1Name = nodePairs[i][0].ref;
	string _point2Name = nodePairs[i][1].ref;*/
	string _point1Name = point1Name;
	string _point2Name = point2Name;

	int _surfaceid = 41245;
	ProFeature grpCurveFeat;
	int create_curve_sts = createCurveBw2Points(CurMdl1, _point1Name, _csoFeat, _curPointData, _point2Name, _csoFeat1, status, _surfaceid, comp_path, vectPointData, _UdfInputSel, grpCurveFeat);
	if (create_curve_sts != 0)
	{
		ProUIMessageButton* buttons = nullptr;
		ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
		buttons[0] = PRO_UI_MESSAGE_OK;
		ProUIMessageButton resultBtn;
		wchar_t wErrMsg[500];
		char cErrMsg[500];
		ProTKSprintf(cErrMsg, "Unable to create a curve between %s and %s. \nPlease check if these nodes are available in the model.", _point1Name.c_str(), _point2Name.c_str());
		ProStringToWstring(wErrMsg, cErrMsg);
		//status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Curve creation failed",wErrMsg, buttons, buttons[0], &resultBtn);
		status = ProArrayFree((ProArray*)&buttons);

		return -1;
		//continue;
	}
	/*if (create_curve_sts == 0)
		return -1;*/

	vectPointData.clear();

	bool isFeatFound = false;
	ProFeature _point1Feat, _point2Feat;
	vector<ProFeature> geomItems;
	ProParameter fanoutParam;
	ProParamvalue fanoutParamVal;
	double p1XoffsetVal = 0, p1YoffsetVal = 0, p2XoffsetVal = 0, p2YoffsetVal = 0, finalXoffsetVal = 0, finalYoffsetVal = 0, traceMarginVal = 0;

	isFeatFound = GetFeatureByName(CurMdl1, _point1Name, _point1Feat);
	if (!isFeatFound) {
		return -1;
	}
	/*GetGeomItems(CurMdl1, _point1Feat, geomItems)*/;
	status = ProParameterInit(&_point1Feat, L"X_OFFSET_CLEARANCE", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		p1XoffsetVal = fanoutParamVal.value.d_val;
	}
	status = ProParameterInit(&_point1Feat, L"Y_OFFSET_CLEARANCE", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		p1YoffsetVal = fanoutParamVal.value.d_val;
	}
	status = ProParameterInit(&_point1Feat, L"FANOUT_TRACE_MARGIN", &fanoutParam);
	if (status == PRO_TK_NO_ERROR) {
		status = ProParameterValueWithUnitsGet(&fanoutParam, &fanoutParamVal, NULL);
		traceMarginVal = fanoutParamVal.value.d_val;
	}
	finalXoffsetVal = p1XoffsetVal;
	finalYoffsetVal = p1YoffsetVal;


	ProFeature* feats = NULL;
	status = ProGroupFeaturesCollect(&grpCurveFeat, &feats);//Give only non suppressed entities

	//double bendPntCoOrd[3] = { 0,0,0 };
	vector<PointCoOrd> reqBendPointCoOrds;
	status = ProFeatureGeomitemVisit(&feats[1], PRO_CURVE, (ProGeomitemAction)featCurveGeomitemVisit, NULL, (ProAppData*)&reqBendPointCoOrds);

	bool isBendPointFound = false;
	double epsilon = 0.0001;
	/*if (fabs(bendPntCoOrd[0]) > epsilon && fabs(bendPntCoOrd[1]) > epsilon && fabs(bendPntCoOrd[2]) > epsilon){
		isBendPointFound = true;
	}*/

	/*if (reqBendPointCoOrds.size() > 0)
		isBendPointFound = true;*/

	if (isBendPointFound)
	{
		for (int k = 0; k < reqBendPointCoOrds.size(); ++k)
		{
			ProPoint3d pnt3D, nearPnt3d;
			pnt3D[0] = reqBendPointCoOrds[k].x;
			pnt3D[1] = reqBendPointCoOrds[k].y;
			pnt3D[2] = reqBendPointCoOrds[k].z;

			// check if the same obstacle is hit twice
			/*bool isSameObstacle = false;
			for (auto& point : fanoutMidpointsCoords)
			{
				if ((fabs(point.x - pnt3D[0]) < epsilon) && (fabs(point.y - pnt3D[1]) < epsilon) && (fabs(point.z - pnt3D[2]) < epsilon)) {
					pnt3D[0] += 5;
					pnt3D[1] += 5;
					pnt3D[2] += 5;
					isSameObstacle = true;
				}
			}*/

			ProSurface onSurface;
			ProUvParam surfaceUV, updatedSurfUV;
			status = ProSolidProjectPoint((ProSolid)CurMdl1, pnt3D, 3, &onSurface, surfaceUV);
			int surfId;
			status = ProSurfaceIdGet(onSurface, &surfId);
			ProModelitem surfItem;
			status = ProSurfaceToGeomitem((ProSolid)CurMdl1, onSurface, &surfItem);
			ProSelection surfSel;
			status = ProSelectionAlloc(NULL, &surfItem, &surfSel);
			/*surfaceUV[0] = surfaceUV[0] + 0.05;
			surfaceUV[1] = surfaceUV[1] + 3;*/

			updatedSurfUV[0] = surfaceUV[0] + ((finalXoffsetVal + traceMarginVal + widthDimVal) * 0.001);
			updatedSurfUV[1] = surfaceUV[1] + finalYoffsetVal + traceMarginVal + widthDimVal - 10;

			/*if (isSameObstacle) {
				surfaceUV[0] -= 5;
				surfaceUV[1] -=	5;
			}*/

			status = ProSelectionUvParamSet(surfSel, updatedSurfUV);
			ProVector projectedPoint;
			status = ProSurfaceXyzdataEval(onSurface, updatedSurfUV, projectedPoint, NULL, NULL, NULL);

			ProBoolean isOnSurf = PRO_B_FALSE;

			status = ProPoint3dOnsurfaceFind(projectedPoint, onSurface, &isOnSurf, NULL);
			if (!isOnSurf) {
				updatedSurfUV[0] = surfaceUV[0] - ((finalXoffsetVal + traceMarginVal + widthDimVal) * 0.001);
				updatedSurfUV[1] = surfaceUV[1] - (finalYoffsetVal + traceMarginVal + widthDimVal);
				status = ProSelectionUvParamSet(surfSel, updatedSurfUV);

				status = ProSurfaceXyzdataEval(onSurface, updatedSurfUV, projectedPoint, NULL, NULL, NULL);
				status = ProPoint3dOnsurfaceFind(projectedPoint, onSurface, &isOnSurf, NULL);
			}


			ProFeature pointFeature;
			status = ProDemoFieldPointCreate(surfSel, &pointFeature);
			ProCharName cFoMpntName;
			ProName wFoMpntName;
			sprintf(cFoMpntName, "FO_MP_%s_%s_%d", _point1Name.c_str(), _point2Name.c_str(), k + 1);
			ProStringToWstring(wFoMpntName, cFoMpntName);
			status = ProModelitemNameSet(&pointFeature, wFoMpntName);

			fanoutMidpointsCoords.push_back({ pnt3D[0], pnt3D[1], pnt3D[2] });

			// check if any other point lies within the specified radius
			vector<ProFeature> fanoutMidpoints;
			status = ProSolidFeatVisit((ProSolid)CurMdl1, (ProFeatureVisitAction)solidFeatCollectAction, (ProFeatureFilterAction)fanoutMidpointFilter, (ProAppData)&fanoutMidpoints);

			bool fanoutInterferenceFound = false;
			//for (int j = 0; j < fanoutMidpoints.size(); ++j)
			//{
			//	ProName wName; ProCharName cName;
			//	status = ProModelitemNameGet(&fanoutMidpoints[j], wName);
			//	ProWstringToString(cName, wName);
			//	if (string(cName).find(cFoMpntName) == string::npos)
			//	{
			//		ProSelection pnt1Sel, pnt2Sel;
			//		vector<ProGeomitem> vecGeomItems;

			//		GetGeomItems(CurMdl1, pointFeature, vecGeomItems);
			//		status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt1Sel);
			//		vecGeomItems.clear();

			//		GetGeomItems(CurMdl1, fanoutMidpoints[j], vecGeomItems);
			//		status = ProSelectionAlloc(&comp_path, &vecGeomItems[0], &pnt2Sel);
			//		vecGeomItems.clear();

			//		Pro2dPnt param1, param2;
			//		Pro3dPnt pnt_1, pnt_2;
			//		double distBwPnts;
			//		status = ProSelectionWithOptionsDistanceEval(pnt1Sel, PRO_B_FALSE, pnt2Sel, PRO_B_FALSE, param1, param1, pnt_1, pnt_1, &distBwPnts);

			//		if (distBwPnts < traceMarginVal)
			//			fanoutInterferenceFound = true;
			//	}
			//}
			//fanoutMidpoints.clear();

			//ProName wBendPointName;
			//status = ProModelitemNameGet(&pointFeature, wBendPointName);
			//ProCharName cBendPointName;
			//ProWstringToString(cBendPointName, wBendPointName);

			//status = ProModelitemHide(&feats[1]);

			//if (fanoutInterferenceFound) {
			//	ProUIMessageButton* buttons = nullptr;
			//	ProArrayAlloc(1, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			//	buttons[0] = PRO_UI_MESSAGE_OK;
			//	ProUIMessageButton resultBtn;
			//	status = ProUIMessageDialogDisplay(PROUIMESSAGE_ERROR, L"Fanout intereference found",
			//		L"Alert: Fanouts detected within the specified boundary. Application will now exit.",
			//		buttons, buttons[0], &resultBtn);
			//	status = ProArrayFree((ProArray*)&buttons);
			//	status = ProUIDialogShow(mainDialog);
			//	status = ProUIDialogExit(paramsDialog, PRO_TK_NO_ERROR);
			//	return -1;
			//}

			//ProFeature* surf1GrpFeats = NULL, * surf2GrpFeats = NULL;
			//ProParamvalue nodeNameParamVal;
			//ProParameter nodeNameParam;

			//ProFeature csoFeat, csoFeat1;
			//std::vector<ProSelection> UdfInputSel;
			//PointData curPointData = {};
			//string point1 = _point1Name;
			////string point2 = sMiddleCurveName;
			//string point2 = string(cBendPointName);
			//int surfaceid = 41245;
			//ProFeature curveFeat1, offsetCurve1Feat1, offsetCurve2Feat1, surfUDFFeat1, curveFeat2, offsetCurve1Feat2, offsetCurve2Feat2, surfUDFFeat2;
			//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat1);
			//status = ProGroupFeaturesCollect(&curveFeat1, &feats);//Give only non suppressed entities
			//status = ProModelitemHide(&feats[1]);

			//createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat1, offsetCurve1Feat1, offsetCurve2Feat1);
			//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat1, offsetCurve2Feat1, surfUDFFeat1);

			//status = ProGroupFeaturesCollect(&surfUDFFeat1, &surf1GrpFeats);//Give only non suppressed entities

			//status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
			//status = ProParameterInit(&surf1GrpFeats[0], L"WIDTH", &nodeNameParam);
			//if (status != PRO_TK_NO_ERROR) {
			//	status = ProParameterWithUnitsCreate(&surf1GrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
			//}
			//else {
			//	status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
			//}


			////point1 = sMiddleCurveName;
			//point1 = string(cBendPointName);
			//point2 = _point2Name;
			//UdfInputSel.clear();
			//vectPointData.clear();
			//createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat2);
			//status = ProGroupFeaturesCollect(&curveFeat2, &feats);//Give only non suppressed entities
			//status = ProModelitemHide(&feats[1]);

			//createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat2, offsetCurve1Feat2, offsetCurve2Feat2);
			//CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat2, offsetCurve2Feat2, surfUDFFeat2);

			//status = ProGroupFeaturesCollect(&surfUDFFeat2, &surf2GrpFeats);//Give only non suppressed entities

			//status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
			//status = ProParameterInit(&surf2GrpFeats[0], L"WIDTH", &nodeNameParam);
			//if (status != PRO_TK_NO_ERROR) {
			//	status = ProParameterWithUnitsCreate(&surf2GrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
			//}
			//else {
			//	status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
			//}


			//// check for overlapping
			//ProAsmcomppath fanoutComppath;
			//ProIdTable c_id_table;
			//vector<ProGeomitem> vecGeomitems;
			//vector<int> fanoutSurf1Ids;
			//c_id_table[0] = -1;
			//status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &fanoutComppath);
			//GetGeomItems(CurMdl1, surf1GrpFeats[1], vecGeomitems);
			//fanoutSurf1Ids.push_back(vecGeomitems[0].id);

			//vecGeomitems.clear();
			//GetGeomItems(CurMdl1, surf2GrpFeats[1], vecGeomitems);
			//fanoutSurf1Ids.push_back(vecGeomitems[0].id);

			//isFanoutOverlap = false;
			//status = ProSolidGroupVisit((ProSolid)CurMdl1, (ProGroupVisitAction)visitFanoutSurfsForOverlap, NULL, (ProAppData)&fanoutSurf1Ids);

			//if (isFanoutOverlap)
			//{
			//	ProUIMessageButton* buttons;
			//	ProUIMessageButton result = PRO_UI_MESSAGE_YES;
			//	ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			//	buttons[0] = PRO_UI_MESSAGE_YES;
			//	buttons[1] = PRO_UI_MESSAGE_NO;

			//	status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"Fanout Overlap found",
			//		L"This fanout overlaps with other fanouts. Would you like to create like this?\n\nClick \"Yes\" to create or \"No\" to remove it.",
			//		buttons, buttons[0], &result);

			//	if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
			//	{
			//		int* featList;
			//		ProFeatureDeleteOptions* delete_opts = 0;
			//		status = ProArrayAlloc(1, sizeof(ProFeatureDeleteOptions), 1, (ProArray*)&delete_opts);
			//		delete_opts[0] = PRO_FEAT_DELETE_CLIP;
			//		status = ProArrayAlloc(1, sizeof(int), 1, (ProArray*)&featList);

			//		featList[0] = surf1GrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		ProFeature* deleteGrpFeats;

			//		status = ProGroupFeaturesCollect(&offsetCurve2Feat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&offsetCurve2Feat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&offsetCurve1Feat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&offsetCurve1Feat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&curveFeat1, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//		status = ProGroupFeaturesCollect(&curveFeat2, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

			//		status = ProGroupFeaturesCollect(&grpCurveFeat, &deleteGrpFeats);
			//		featList[0] = deleteGrpFeats[0].id;
			//		status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			//	}
			//}
		}

	}
	else
	{
		ProFeature* surfGrpFeats = NULL;
		ProParamvalue nodeNameParamVal;
		ProParameter nodeNameParam;

		ProFeature csoFeat, csoFeat1;
		std::vector<ProSelection> UdfInputSel;
		PointData curPointData = {};
		string point1 = _point1Name;
		//string point2 = sMiddleCurveName;
		string point2 = _point2Name;
		int surfaceid = 41245;
		ProFeature curveFeat, offsetCurve1Feat, offsetCurve2Feat, surfUDFFeat;
		createCurveBw2Points(CurMdl1, point1, csoFeat, curPointData, point2, csoFeat1, status, surfaceid, comp_path, vectPointData, UdfInputSel, curveFeat);
		status = ProGroupFeaturesCollect(&curveFeat, &feats);//Give only non suppressed entities
		status = ProModelitemHide(&feats[1]);

		createTwoOffsetCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, curveFeat, offsetCurve1Feat, offsetCurve2Feat);
		CreateSurfaceBwCurve(CurMdl1, status, surfaceid, comp_path, UdfInputSel, offsetCurve1Feat, offsetCurve2Feat, surfUDFFeat);

		status = ProGroupFeaturesCollect(&surfUDFFeat, &surfGrpFeats);//Give only non suppressed entities

		status = ProParamvalueSet(&nodeNameParamVal, (void*)&widthDimVal, PRO_PARAM_DOUBLE);
		status = ProParameterInit(&surfGrpFeats[0], L"WIDTH", &nodeNameParam);
		if (status != PRO_TK_NO_ERROR) {
			status = ProParameterWithUnitsCreate(&surfGrpFeats[0], L"WIDTH", &nodeNameParamVal, NULL, &nodeNameParam);
		}
		else {
			status = ProParameterValueWithUnitsSet(&nodeNameParam, &nodeNameParamVal, NULL);
		}

		// check for overlapping
		ProAsmcomppath fanoutComppath;
		ProIdTable c_id_table;
		ProSelection fanout1Sel, fanout2sel;
		vector<ProGeomitem> vecGeomitems;
		c_id_table[0] = -1;
		status = ProAsmcomppathInit((ProSolid)CurMdl1, c_id_table, 0, &fanoutComppath);
		GetGeomItems(CurMdl1, surfGrpFeats[1], vecGeomitems);
		vector<int> fanoutSurf1Ids;
		fanoutSurf1Ids.push_back(vecGeomitems[0].id);

		isFanoutOverlap = false;
		status = ProSolidGroupVisit((ProSolid)CurMdl1, (ProGroupVisitAction)visitFanoutSurfsForOverlap, NULL, (ProAppData)&fanoutSurf1Ids);

		if (isFanoutOverlap)
		{
			ProUIMessageButton* buttons;
			ProUIMessageButton result = PRO_UI_MESSAGE_YES;
			ProArrayAlloc(2, sizeof(ProUIMessageButton), 1, (ProArray*)&buttons);
			buttons[0] = PRO_UI_MESSAGE_YES;
			buttons[1] = PRO_UI_MESSAGE_NO;

			status = ProUIMessageDialogDisplay(PROUIMESSAGE_QUESTION, L"Fanout Overlap found",
				L"This fanout overlaps with other fanouts. Would you like to create like this?\n\nClick \"Yes\" to create or \"No\" to remove it.",
				buttons, buttons[0], &result);

			if (status == PRO_TK_NO_ERROR && result == PRO_UI_MESSAGE_NO)
			{
				int* featList;
				ProFeatureDeleteOptions* delete_opts = 0;
				status = ProArrayAlloc(1, sizeof(ProFeatureDeleteOptions), 1, (ProArray*)&delete_opts);
				delete_opts[0] = PRO_FEAT_DELETE_CLIP;
				status = ProArrayAlloc(1, sizeof(int), 1, (ProArray*)&featList);

				featList[0] = surfGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

				ProFeature* deleteGrpFeats;
				status = ProGroupFeaturesCollect(&offsetCurve2Feat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

				status = ProGroupFeaturesCollect(&offsetCurve1Feat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

				status = ProGroupFeaturesCollect(&curveFeat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);

				status = ProGroupFeaturesCollect(&grpCurveFeat, &deleteGrpFeats);
				featList[0] = deleteGrpFeats[0].id;
				status = ProFeatureWithoptionsDelete((ProSolid)CurMdl1, featList, delete_opts, PRO_REGEN_NO_FLAGS);
			}
		}
	}

	return 0;
}


void writeRemarkTxtFile(string remarkFp, vector<string> missingPoints)
{
	FILE *fp = NULL;
	fp = fopen(remarkFp.c_str(), "w");

	fprintf(fp, "Please note that the following node pairs are not available in the model:\n\n");
	for (int i = 0; i < missingPoints.size(); ++i)
	{
		fprintf(fp, "%d. %s\n", i + 1, missingPoints[i].c_str());
	}
	fclose(fp);
}

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
					if (!nodes.empty())
					{
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

	char Udfpath[PRO_PATH_SIZE] = "";
	strcpy(Udfpath, cTextDirPath);
	strcat(Udfpath, "\\text\\UDF_LATEST\\create_curve.gph.1");
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
		double heightDimVal, widthDimVal, angleDimVal{};
		status = ProUIInputpanelDoubleGet(paramsDialog, ip_height, &heightDimVal);
		status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);
		status = ProUIInputpanelDoubleGet(paramsDialog, ip_angle, &angleDimVal);

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

				//if (strcmp(cPrompt, "width1") == 0) {
				if (strcmp(cPrompt, "width") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal));	// user entered width/2
				}
				//else if (strcmp(cPrompt, "width2") == 0) {
				//else if (strcmp(cPrompt, "WIDTH2") == 0) {
				//	status = ProUdfvardimValueSet(var_dim_array[i], (widthDimVal / 2));	// user entered width/2
				//}
				//else if (strcmp(cPrompt, "height1") == 0) {
				else if (strcmp(cPrompt, "height1") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal));	// user entered height
				}
				//else if (strcmp(cPrompt, "height2") == 0) {
				else if (strcmp(cPrompt, "height2") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal));	// user entered height
				}
				//else if (strcmp(cPrompt, "mid_height") == 0) {
				else if (strcmp(cPrompt, "mid_height") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (heightDimVal - (0.3 * heightDimVal)));	// user entered height - certain value
				}
				else if (strcmp(cPrompt, "main_angle") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], angleDimVal);	// user entered height - certain value
				}
				else if (strcmp(cPrompt, "anlge1") == 0 || strcmp(cPrompt, "angle2") == 0 || strcmp(cPrompt, "angle3") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (angleDimVal));
				}
				// LCR network udf
				else if (strcmp(cPrompt, "inner_circle_radius") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], 120);
				}

				// Serpentine resistor
				else if (strcmp(cPrompt, "dim1") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], 7.5);
				}

				// bondpad_pair
				else if (strcmp(cPrompt, "dimmm") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], 48);
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
	ProUdfCreateOption option[] = { PROUDFOPT_FIX_MODEL_UI_OFF };

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

bool LoadResistorUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/, ProBoolean modifyDim)
{
	ProError status = PRO_TK_NO_ERROR;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;

	ProStringToWstring(UdfFilePath, (char*)UDFFileName.c_str());

	status = ProUdfdataPathSet(UdfData, UdfFilePath);

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

	if (modifyDim)
	{
		// UPDATE DIMS
		static char* ip_bondpad_side = "ip_bondpad_side_len", * ip_overall_len = "ip_overall_len", * ip_width = "ip_width", * ip_bondpad_to_res_dist = "ip_bondpad_to_resistor_dist", *ip_pitch = "ip_pitch";
		double bondpadSideVal, overallLenVal, widthVal, bondpadToResistorDistVal, pitchVal;
		status = ProUIInputpanelDoubleGet(resistorParamsDialog, ip_bondpad_side, &bondpadSideVal);
		status = ProUIInputpanelDoubleGet(resistorParamsDialog, ip_overall_len, &overallLenVal);
		status = ProUIInputpanelDoubleGet(resistorParamsDialog, ip_width, &widthVal);
		status = ProUIInputpanelDoubleGet(resistorParamsDialog, ip_bondpad_to_res_dist, &bondpadToResistorDistVal);
		status = ProUIInputpanelDoubleGet(resistorParamsDialog, ip_pitch, &pitchVal);

		// calculated dims
		double bondpadShortSideVal, bondpadToStemShortMax, bondpadToStemLargeMax, bondpadToStemLargeMin, shortStemLen, bendStemLenMax, bendStemLenMin;
		bondpadShortSideVal = (bondpadSideVal / 2) - (widthVal / 2);
		bondpadToStemShortMax = bondpadToResistorDistVal + widthVal;
		bondpadToStemLargeMax = overallLenVal + bondpadToResistorDistVal + widthVal;
		bondpadToStemLargeMin = overallLenVal + bondpadToResistorDistVal;
		shortStemLen = overallLenVal / 2;
		bendStemLenMin = pitchVal;
		bendStemLenMax = bendStemLenMin + (widthVal * 2);

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

				if (strcmp(cPrompt, "bondpad_side_len") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadSideVal);	
				}
				else if (strcmp(cPrompt, "overall_len") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], overallLenVal);	
				}
				else if (strcmp(cPrompt, "bondpad_short_side") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadShortSideVal);
				}
				else if (strcmp(cPrompt, "bondpad_to_stem_short_max") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadToStemShortMax);
				}
				else if (strcmp(cPrompt, "bondpad_to_stem_short_min") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadToResistorDistVal);
				}
				else if (strcmp(cPrompt, "bend_stem_len_max") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bendStemLenMax);
				}
				else if (strcmp(cPrompt, "bend_stem_len_min") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bendStemLenMin);
				}
				else if (strcmp(cPrompt, "short_stem_len") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], shortStemLen);
				}
				else if (strcmp(cPrompt, "bondpad_to_stem_large_max") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadToStemLargeMax);
				}
				else if (strcmp(cPrompt, "bondpad_to_stem_large_min") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadToStemLargeMin);
				}
				// serpentine resistor
				else if (strcmp(cPrompt, "bondpad_to_stem_short") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadToResistorDistVal);
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
	ProUdfCreateOption option[] = { PROUDFOPT_FIX_MODEL_UI_OFF };

	ProFeature createdFeature;
	status = ProUdfCreate((ProSolid)Mdl, UdfData, NULL, option, 1, &createdFeature);
	if (status == PRO_TK_NO_ERROR)
		isCreatedUDF = true;
	UDFGrpFeat = createdFeature;

	status = ProTreetoolRefresh((ProSolid)Mdl);

	return isCreatedUDF;
}

bool LoadCapacitorUDF(ProMdl Mdl, string UDFFileName, vector<ProSelection>& UDFInputs, ProFeature& UDFGrpFeat, ProAsmcomppath* asmCompPath, ProBoolean RefAsm /*For Asm it is TRUE*/, ProBoolean modifyDim)
{
	ProError status = PRO_TK_NO_ERROR;
	bool isCreatedUDF = false;
	ProUdfdata UdfData = NULL;
	status = ProUdfdataAlloc(&UdfData);
	ProPath UdfFilePath;

	ProStringToWstring(UdfFilePath, (char*)UDFFileName.c_str());

	status = ProUdfdataPathSet(UdfData, UdfFilePath);

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

	if (modifyDim)
	{
		// UPDATE DIMS
		static char* ip_contact_width = "ip_contact_width", * ip_contact_height = "ip_contact_height", * ip_feature_size = "ip_feature_size", 
			* ip_current_collector_width = "ip_current_collector_width", *ip_current_collector_height = "ip_current_collector_height" ,
			* ip_capacitor_len = "ip_capacitor_len", * ip_spacing = "ip_spacing";

		double contactWidthVal, contactHeightVal, featureSizeVal, capacitorLenVal, currentCollectorWidthVal, currentCollectorHeightVal, spacingVal;
		status = ProUIInputpanelDoubleGet(capacitorParamsDialog, ip_feature_size, &featureSizeVal);	// (F)eature size
		status = ProUIInputpanelDoubleGet(capacitorParamsDialog, ip_capacitor_len, &capacitorLenVal);	// (L)ength
		status = ProUIInputpanelDoubleGet(capacitorParamsDialog, ip_spacing, &spacingVal);	// (S)pacing
		status = ProUIInputpanelDoubleGet(capacitorParamsDialog, ip_current_collector_width, &currentCollectorWidthVal);	// current collector width (ccw)
		status = ProUIInputpanelDoubleGet(capacitorParamsDialog, ip_current_collector_height, &currentCollectorHeightVal);	// current collector height (cch)
		status = ProUIInputpanelDoubleGet(capacitorParamsDialog, ip_contact_width, &contactWidthVal);	// contact width
		status = ProUIInputpanelDoubleGet(capacitorParamsDialog, ip_contact_height, &contactHeightVal);	// contact height
		
		// calculated dims
		double overallLenVal, stem_length_by_2, bondpadShortSideVal, point11, point12, point21, point22, point31, offsetDistVal;
		overallLenVal = ((featureSizeVal * 7.0) + (spacingVal * 12.0) + (featureSizeVal * 6.0));
		stem_length_by_2 = (featureSizeVal * 6) + (spacingVal * 6.0);
		bondpadShortSideVal = (contactHeightVal / 2.0) - (currentCollectorHeightVal / 2.0);
		offsetDistVal = spacingVal + featureSizeVal;

		point11 = (spacingVal * 2.0) + (featureSizeVal * 2.0);
		point12 = (spacingVal * 2.0) + (featureSizeVal * 3.0);
		point21 = (spacingVal * 4.0) + (featureSizeVal * 4.0);
		point22 = (spacingVal * 4.0) + (featureSizeVal * 5.0);
		point31 = (overallLenVal - featureSizeVal /*middle capacitor*/ )/2.0;

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

				if (strcmp(cPrompt, "contact_height") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], contactHeightVal);
				}
				else if (strcmp(cPrompt, "contact_width") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], contactWidthVal);
				}
				else if (strcmp(cPrompt, "stem_length_by_2") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], stem_length_by_2);
				}
				else if (strcmp(cPrompt, "bondpad_short_side") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], bondpadShortSideVal);
				}
				else if (strcmp(cPrompt, "current_collector_width") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], currentCollectorWidthVal);
				}
				else if (strcmp(cPrompt, "capacitor_len") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], capacitorLenVal);
				}
				else if (strcmp(cPrompt, "feature_size") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], featureSizeVal);
				}
				else if (strcmp(cPrompt, "point11_dist") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], point11);
				}
				else if (strcmp(cPrompt, "point12_dist") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], point12);
				}
				else if (strcmp(cPrompt, "point21_dist") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], point21);
				}
				else if (strcmp(cPrompt, "point22_dist") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], point22);
				}
				else if (strcmp(cPrompt, "point31_dist") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], point31);
				}
				else if (strcmp(cPrompt, "offset_dist") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], offsetDistVal);
				}
				else if (strcmp(cPrompt, "overall_len_by_2") == 0) {
					status = ProUdfvardimValueSet(var_dim_array[i], (overallLenVal - currentCollectorHeightVal)/2.0);
				}
				status = ProUdfdataUdfvardimAdd(UdfData, var_dim_array[i]);	// adding the vardim to UdfData
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
		ProUdfCreateOption option[] = { PROUDFOPT_FIX_MODEL_UI_OFF };

		ProFeature createdFeature;
		status = ProUdfCreate((ProSolid)Mdl, UdfData, NULL, option, 1, &createdFeature);
		if (status == PRO_TK_NO_ERROR)
			isCreatedUDF = true;
		UDFGrpFeat = createdFeature;

		status = ProTreetoolRefresh((ProSolid)Mdl);

		return isCreatedUDF;
	}
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

void CreateSurfaceBwCurve(const ProMdl& CurMdl1, ProError status, int surfaceid, ProAsmcomppath& comp_path, std::vector<ProSelection>& UdfInputSel, ProFeature& udfOffsetCurve1Feat, ProFeature& udfOffsetCurve2Feat, ProFeature &surfFeature)
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
	char Udfpath[PRO_PATH_SIZE] = "";
	strcpy(Udfpath, cTextDirPath);
	strcat(Udfpath, "\\text\\UDF_LATEST\\create_surface.gph.1");
	
	LoadUDFwithoutDims(CurMdl1, Udfpath, UdfInputSel, surfFeature, NULL, PRO_B_TRUE);

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
	char Udfpath[PRO_PATH_SIZE] = "";
	strcpy(Udfpath, cTextDirPath);
	strcat(Udfpath, "\\text\\UDF_LATEST\\end_2_offset_curve.gph.1");

	double widthDimVal;
	status = ProUIInputpanelDoubleGet(paramsDialog, ip_width, &widthDimVal);

	LoadCurveUDF(CurMdl1, Udfpath, UdfInputSel, udfOffsetCurve1Feat, NULL, PRO_B_TRUE, widthDimVal);
	widthDimVal *= -1;
	LoadCurveUDF(CurMdl1, Udfpath, UdfInputSel, udfOffsetCurve2Feat, NULL, PRO_B_TRUE, widthDimVal);

}


extern "C" void user_terminate()
{
}
