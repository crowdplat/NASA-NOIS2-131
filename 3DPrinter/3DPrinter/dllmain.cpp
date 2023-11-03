// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include"CommonHeader.h"
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
static char* PushButton1 = "PushButton1";
static char* PushButton2 = "PushButton2";
static char* PushButton3 = "PushButton3";
static char* PushButton4 = "PushButton4";
static char* PushButton5 = "PushButton5";
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

void FillUDFDataWithReferenceForCopyFeat(ProUdfdata& UdfData, vector<ProSelection>& sel2, ProBoolean RefAsm)
{
	ProError status = PRO_TK_NO_ERROR;
	ProUdfRequiredRef* requiredRef;
	status = ProUdfdataRequiredreferencesGet(UdfData, &requiredRef);
	int sizeRef = 0;
	status = ProArraySizeGet(requiredRef, &sizeRef);
	ProUdfreference* udfReference = NULL;
	status = ProArrayAlloc(sizeRef, sizeof(ProUdfreference), 1, (ProArray*)&udfReference);
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
	status = ProUdfdataInstancenameSet(UdfData, udfNameOrginalName);
	ProFeature createdFeature;
	status = ProUdfCreate((ProSolid)Mdl, UdfData, asmCompPath, option, 1, &createdFeature);
	if (status == PRO_TK_NO_ERROR)
		isCreatedUDF = true;
	UDFGrpFeat = createdFeature;

	lErr = ProTreetoolRefresh((ProSolid)Mdl);

	return isCreatedUDF;
}

void FanOutCreation()
{

	ProError status;
	ProSelection* sels;
	int nSels = -1;
	status = ProSelect((char*)"part", 2, NULL, NULL, NULL, NULL, &sels, &nSels);
	//string Pointname;
	
	vector<ProFeature> vecpointFeatures;
	vector< PointData> vectPointData;
	for (size_t selcnt = 0; selcnt < nSels; selcnt++)
	{
		ProFeature feature;
		status = ProSelectionModelitemGet(sels[selcnt], &feature);
		ProAsmcomppath MdlCompPath;
		status = ProSelectionAsmcomppathGet(sels[selcnt], &MdlCompPath);
		ProMdl CurselectionMdl;
		status = ProModelitemMdlGet(&feature, &CurselectionMdl);
		PointData curPointData = {};
		curPointData.lAsmPath = MdlCompPath;
		curPointData.lMdlAssm = CurselectionMdl;
		ProMdl CapMdl;
		status = ProModelitemMdlGet(&feature, &CapMdl);

		ProName mdlName;

		status = ProMdlNameGet(CapMdl, mdlName);

		ProCharName CharMdlname;
		ProWstringToString(CharMdlname, mdlName);
		string strMdlName(CharMdlname);
		if (feature.type == PRO_PART)
		{
			

			ProFeature outFeat;
			if (strMdlName == "CAPACITOR_1" || strMdlName == "ANTENA_1")
			{
				GetFeatureByName(CapMdl, "PNT0", outFeat);
				ProFeattype type;
				status = ProFeatureTypeGet(&outFeat, &type);

				if (status == PRO_TK_NO_ERROR && type == PRO_FEAT_DATUM_POINT)
				{
					curPointData.lPointFeat = outFeat;
				}

			}
			else
			{
				ProName lCsysDefName1;
				string sFeatName="";

				if (strMdlName == "COMP_CAPACITOR")
					sFeatName = "PNT_CAPACITOR";
				else if (strMdlName == "COMP_ANTENNA")
					sFeatName = "PNT_ANTENNA";
				//status = ProModelitemByNameInit(CapMdl, PRO_CSYS, lCsysDefName, &comp_datum);
				GetFeatureByName(CapMdl, sFeatName, outFeat);
				ProFeattype type;
				status = ProFeatureTypeGet(&outFeat, &type);

				if (status == PRO_TK_NO_ERROR && type == PRO_FEAT_DATUM_POINT)
				{
					curPointData.lPointFeat = outFeat;
				}

			}


			vectPointData.push_back(curPointData);

		}
	}


	vector<ProSelection> UdfInputSel;
	for (size_t k = 0; k < vectPointData.size(); k++)
	{
		//Get Point Feature Selection
		vector<ProGeomitem> vecGeomItems;
		GetGeomItems(vectPointData[k].lMdlAssm, vectPointData[k].lPointFeat, vecGeomItems);
		ProSelection pointSelection;
		status = ProSelectionAlloc(&vectPointData[k].lAsmPath, (ProGeomitem*)&vecGeomItems[0], &pointSelection);
		UdfInputSel.push_back(pointSelection);
	}

	string Udfpath = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\curve.gph.1";
	ProMdl CurMdl;
	status = ProMdlCurrentGet(&CurMdl);
	ProFeature UDFGrpFeat;

	if (LoadUDF(CurMdl, Udfpath, UdfInputSel, UDFGrpFeat, NULL, PRO_B_TRUE))
	{
		//VecAxisUdfs.push_back(UDFGrpFeat);
		ProFeature* feats = NULL;
		status = ProGroupFeaturesCollect(&UDFGrpFeat, &feats);//Give only non suppressed entities
		int FeatSize;
		status = ProArraySizeGet(feats, &FeatSize);

	}

}

ProError UserAssembleByDatums(ProAssembly asm_model, ProSolid comp_model,string TemplateName, ProAsmcomp& asmcomp)
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


	status=ProAsmcompAssemble(asm_model, comp_model, identity_matrix, &asmcomp);
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
void AssembleModel(string modelPath, string mdlName)
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
	UserAssembleByDatums(ProAssembly(currModel), ProSolid(p_new_handle), mdlName, lAsmComp_Child);
	//ProSolidRegenerate(ProAssembly(asmMaster), PRO_REGEN_NO_RESOLVE_MODE);
	WindowFit();
	lErrCode = ProWindowActivate(iWindId);
}
void stringToWString(IN string schar, OUT  wchar_t* wcharout)
{
	wstring ch(schar.begin(), schar.end());
	wcscpy_s(wcharout, ch.size() + 1, ch.c_str());
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

	//lErr = ProParameterValueSet(&lParam, &lValue);  //Creo7
	lErr = ProParameterValueWithUnitsSet(&lParam, &lValue, NULL);  //creo9 navku
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
	//wchar_t* wview_Name;
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
	//			string strMdlPath = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\capacitor_1.prt";
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
	//			string strMdlPath1 = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\antena_1.prt";
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

void executeCapacitorAction(char* dialog, char* component, ProAppData appdata)
{
	ProMouseButton btn;
	ProVector loc;
	if (ProMousePickGet(PRO_ANY_BUTTON, &btn, loc) == PRO_TK_NO_ERROR)
	{
		/*double x = loc[0];
		double y = loc[1];
		double z = loc[2];*/
		string strMdlPath = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\capacitor_1.prt";
		AssembleModel(strMdlPath, "capacitor");

	}

}
void executeAntennaAction(char* dialog, char* component, ProAppData appdata)
{
	ProMouseButton btn1;
	ProVector loc1;
	if (ProMousePickGet(PRO_ANY_BUTTON, &btn1, loc1) == PRO_TK_NO_ERROR)
	{
		/*double x = loc[0];
		double y = loc[1];
		double z = loc[2];*/
		string strMdlPath1 = "C:\\Users\\kmsma\\OneDrive\\Desktop\\Creo_model\\3DPrinterModel\\antena_1.prt";
		AssembleModel(strMdlPath1, "antenna");

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
ProError CreateMenuItemAndAction()
{
	ProError lErr = PRO_TK_NO_ERROR;

	ProFileName FileName = L"resource.txt";

	uiCmdCmdId lCmd;
	uiCmdCmdId lCmd1;
	lErr = ProMenubarMenuAdd("TADDll_Menu", "TADDll_Menu", "Help", PRO_B_TRUE, FileName);

	lErr = ProCmdActionAdd("Action", (uiCmdCmdActFn)Create, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_FALSE, &lCmd);
	lErr = ProCmdActionAdd("Action1", (uiCmdCmdActFn)Create1, uiProeImmediate, NULL, PRO_B_FALSE, PRO_B_FALSE, &lCmd1);

	lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "TADDll_PushButton", "Template_PushButton", "Template_ToolTip", NULL, PRO_B_TRUE, lCmd, FileName);
	lErr = ProMenubarmenuPushbuttonAdd("TADDll_Menu", "TADll_PushButton", "Fanout_PushButton", "Template_ToolTip", NULL, PRO_B_TRUE, lCmd1, FileName);

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