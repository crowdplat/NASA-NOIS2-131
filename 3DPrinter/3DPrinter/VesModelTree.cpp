
//#include "..\..\LibHeaders\Header\stdafx.h"
#include "VesModelTree.h"
//#include "..\Header\VesMacro.h"
//#include "..\Header\VesCreoWindow.h"
//#include "..\Header\VesParameter.h"
//#include "..\Header\VesSymbol.h"
//#include "..\..\LibGeneralUtility\Header\VesUtility.h"
#include<algorithm>
VesModelTree::VesModelTree(ProMdl currMdl) : m_currMdl(currMdl)
{
}
void WcharToString_(wchar_t* xWString, std::string& xString)
{
	// Converts wchar to string
	std::wstring lStr(xWString);

	std::string test(lStr.begin(), lStr.end());
	xString = test;

}
void stringToWString_(string schar,  wchar_t* wcharout)
{
	wstring ch(schar.begin(), schar.end());
	wcscpy_s(wcharout, ch.size() + 1, ch.c_str());
}

bool CompareStringNoCase(string source, string destination)
{
	std::transform(source.begin(), source.end(), source.begin(), ::tolower);
	std::transform(destination.begin(), destination.end(), destination.begin(), ::tolower);
	bool retVal;
	(source == destination) ? retVal = true : retVal = false;

	return retVal;
}
/*===========================================================================*\
Function:
Purpose:
\*===========================================================================*/
ProError VesModelTree::CollectModelTreeComponents(ProAsmcomppath* xPath, ProSolid xSolid, ProBoolean down, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	std::vector<AsmTreeData>* lpVecfeature = (std::vector<AsmTreeData>*)(app_data);
	AsmTreeData lData;
	if (down)
	{
		ProMdlType p_type;
		lErr = ProMdlTypeGet((ProMdl)xSolid, &p_type);
		ProMdlName MdlName;
		lErr = ProMdlMdlnameGet((ProMdl)xSolid, MdlName);
		char pName[PRO_PATH_SIZE];
		ProWstringToString(pName, MdlName);

		if (PRO_MDL_PART == p_type || PRO_MDL_ASSEMBLY == p_type)
		{
			lData.lMdlAssm = xSolid;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				lData.lAsmPath.comp_id_table[i] = xPath->comp_id_table[i];
			}
			lData.lAsmPath.owner = xPath->owner;
			lData.lAsmPath.table_num = xPath->table_num;

			lData.lAsmName = pName;
			lData.MdlType = p_type;
			lpVecfeature->push_back(lData);
		}
	}
	return lErr;
}

ProError VesModelTree::CollectModelTreeComponentsuptoNLevel(ProAsmcomppath* xPath, ProSolid xSolid, ProBoolean down, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	std::map<int, std::vector<VesModelTree::AsmTreeData>>* lpMapfeature = (std::map<int, std::vector<VesModelTree::AsmTreeData>>*)(app_data);
	int complevel;
	std::vector<VesModelTree::AsmTreeData> lpVecfeature;
	for (auto lpMap : *lpMapfeature) {
		complevel = lpMap.first;
		lpVecfeature = lpMap.second;
	}
	AsmTreeData lData;
	if (down)
	{
		ProMdlType p_type;
		lErr = ProMdlTypeGet((ProMdl)xSolid, &p_type);
		ProMdlName MdlName;
		lErr = ProMdlMdlnameGet((ProMdl)xSolid, MdlName);
		char pName[PRO_PATH_SIZE];
		ProWstringToString(pName, MdlName);

		if (PRO_MDL_PART == p_type || PRO_MDL_ASSEMBLY == p_type)
		{
			lData.lMdlAssm = xSolid;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				lData.lAsmPath.comp_id_table[i] = xPath->comp_id_table[i];
			}
			lData.lAsmPath.owner = xPath->owner;
			lData.lAsmPath.table_num = xPath->table_num;

			lData.lAsmName = pName;
			lData.MdlType = p_type;
			lpVecfeature.push_back(lData);
		}
	}
	(*lpMapfeature)[complevel] = lpVecfeature;
	return lErr;
}

ProError VesModelTree::CollectModelTreeComponentsFilter(ProAsmcomppath* xPath, ProSolid xSolid, ProAppData app_data)
{
	ProError lErr = PRO_TK_NO_ERROR;
	std::map<int, std::vector<VesModelTree::AsmTreeData>>* lpVecfeature = (std::map<int, std::vector<VesModelTree::AsmTreeData>>*)(app_data);
	int complevel;
	for (auto lpVec : *lpVecfeature) {
		complevel = lpVec.first;
	}
	if (xPath->table_num > complevel-1)
		return PRO_TK_CONTINUE;

	return lErr;
}

void VesModelTree::GetModelTreeComponents(ProMdl Mdl, std::vector<VesModelTree::AsmTreeData> &ComponentVector)
{
	ProError status;
	ComponentVector.clear();
	status = ProSolidDispCompVisit((ProSolid)Mdl, CollectModelTreeComponents, NULL, (void*)&ComponentVector);
}

void VesModelTree::GetModelTreeComponentsinNLevel(ProMdl Mdl,int nLevel, std::vector<VesModelTree::AsmTreeData>& ComponentVector)
{
	ProError status;
	ComponentVector.clear();
	std::map<int, std::vector<VesModelTree::AsmTreeData>> mapVect;
	mapVect.insert({ nLevel,ComponentVector });
	status = ProSolidDispCompVisit((ProSolid)Mdl, CollectModelTreeComponentsuptoNLevel, CollectModelTreeComponentsFilter, (ProAppData*)&mapVect);
	ComponentVector = mapVect.at(nLevel);
}

ProError VesModelTree::GetParentCompPath(ProAsmcomppath inPath, ProAsmcomppath& parentPath) {
	parentPath = inPath;
	parentPath.table_num = inPath.table_num - 1;
	for (int pp = 0; pp < 25; pp++) { 
		if (inPath.comp_id_table[pp] == -1) {
			parentPath.comp_id_table[pp - 1] = -1;
			return PRO_TK_NO_ERROR;
		}
	}
}

void VesModelTree::GetAsmCompTreeData(  map<string, VesModelTree::AsmTreeData>& MembMdlMap)
{
	vector<VesModelTree::AsmTreeData> TreeVec;
	VesModelTree::GetModelTreeComponents(m_currMdl, TreeVec);
	for (auto currTreeComp : TreeVec)
	{
		//if (currTreeComp.lAsmName == compName)
			VesModelTree::AsmTreeData MembMdl;
			MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
			}
			MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
			MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

			MembMdl.lAsmName = currTreeComp.lAsmName;
			MembMdl.MdlType = currTreeComp.MdlType;

			MembMdl = currTreeComp;
			MembMdlMap[currTreeComp.lAsmName] = MembMdl;
	}
}

void VesModelTree::GetAsmCompTreeData( vector<VesModelTree::AsmTreeData>& TreeVec)
{
	//vector<VesModelTree::AsmTreeData> TreeVec;
	VesModelTree::GetModelTreeComponents(m_currMdl, TreeVec);
	//for (auto currTreeComp : TreeVec)
	//{
	//	//if (currTreeComp.lAsmName == compName)
	//	VesModelTree::AsmTreeData MembMdl;
	//	MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
	//	for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
	//	{
	//		MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
	//	}
	//	MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
	//	MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

	//	MembMdl.lAsmName = currTreeComp.lAsmName;
	//	MembMdl.MdlType = currTreeComp.MdlType;

	//	MembMdl = currTreeComp;
	//	TreeVec.push_back(MembMdl);
	//}
}

bool VesModelTree::GetAsmCompTreeData( vector<VesModelTree::AsmTreeData>& TreeVec,  string compName,  VesModelTree::AsmTreeData& MembMdl)
{
	//vector<VesModelTree::AsmTreeData> TreeVec;
	//VesModelTree::GetModelTreeComponents(m_currMdl, TreeVec);
	for (auto currTreeComp : TreeVec)
	{
		if (CompareStringNoCase(currTreeComp.lAsmName, compName))
		{
			MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
			}
			MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
			MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

			MembMdl.lAsmName = currTreeComp.lAsmName;
			MembMdl.MdlType = currTreeComp.MdlType;

			MembMdl = currTreeComp;
			return true;
		}
	}
	return false;
}

void VesModelTree::LinkChildAsmCompTreeDataWithParentAsmCompTreeData(VesModelTree::AsmTreeData _parentAsmCmp, VesModelTree::AsmTreeData _childAsmCmp,
	VesModelTree::AsmTreeData& linkedchildAsmComp)
{
	linkedchildAsmComp.lAsmPath.owner = _parentAsmCmp.lAsmPath.owner;
	linkedchildAsmComp.lAsmPath.table_num = _parentAsmCmp.lAsmPath.table_num + _childAsmCmp.lAsmPath.table_num;
	for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
	{
		linkedchildAsmComp.lAsmPath.comp_id_table[i] = _parentAsmCmp.lAsmPath.comp_id_table[i];
	}
	int cntr = 0;
	for (int i = _parentAsmCmp.lAsmPath.table_num; i < PRO_MAX_ASSEM_LEVEL; i++)
	{
		linkedchildAsmComp.lAsmPath.comp_id_table[i] = _childAsmCmp.lAsmPath.comp_id_table[cntr];
		cntr++;
	}
	linkedchildAsmComp.lAsmName = _childAsmCmp.lAsmName;
	linkedchildAsmComp.MdlType = _childAsmCmp.MdlType;
	linkedchildAsmComp.lMdlAssm = _childAsmCmp.lMdlAssm;
}

bool VesModelTree::GetAsmCompTreeData(string compName, vector<VesModelTree::AsmTreeData>& MembMdlVec)
{
	bool isSuccess = false;
	vector<VesModelTree::AsmTreeData> TreeVec;
	VesModelTree::GetModelTreeComponents(m_currMdl, TreeVec);
	for (auto currTreeComp : TreeVec)
	{
		if (currTreeComp.lAsmName == compName)
		{
			VesModelTree::AsmTreeData MembMdl;
			MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
			}
			MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
			MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

			MembMdl.lAsmName = currTreeComp.lAsmName;
			MembMdl.MdlType = currTreeComp.MdlType;

			MembMdl = currTreeComp;
			MembMdlVec.push_back(MembMdl);
			isSuccess = true;
		}
	}
	return isSuccess;
}
/*====================================================================*\
Function : GetAsmCompTreeData
Purpose :
Step1 :
Step2 :
Input :
Output :
Return : ProError
Created :
History :
\*====================================================================*/
bool VesModelTree::GetAsmCompTreeData(string compName, VesModelTree::AsmTreeData& MembMdl)
{
	vector<VesModelTree::AsmTreeData> TreeVec;
	ProName mdlName;
	ProMdlNameGet(m_currMdl, mdlName);
	VesModelTree::GetModelTreeComponents(m_currMdl, TreeVec);
	for (auto currTreeComp : TreeVec)
	{
		if (CompareStringNoCase(currTreeComp.lAsmName, compName))
		{
			MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
			}
			MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
			MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

			MembMdl.lAsmName = currTreeComp.lAsmName;
			MembMdl.MdlType = currTreeComp.MdlType;

			MembMdl = currTreeComp;
			return true;
		}
	}
	return false;
}

bool VesModelTree::GetAsmCompinNLevelTreeData(string compName,int nLevel, VesModelTree::AsmTreeData& MembMdl)
{
	vector<VesModelTree::AsmTreeData> TreeVec;
	VesModelTree::GetModelTreeComponentsinNLevel(m_currMdl, nLevel, TreeVec);
	for (auto currTreeComp : TreeVec)
	{
		if (CompareStringNoCase(currTreeComp.lAsmName, compName))
		{
			MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
			}
			MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
			MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

			MembMdl.lAsmName = currTreeComp.lAsmName;
			MembMdl.MdlType = currTreeComp.MdlType;

			MembMdl = currTreeComp;
			return true;
		}
	}
	return false;
}

bool VesModelTree::GetAsmCompTreeDatabyID(vector<int> compID, VesModelTree::AsmTreeData& MembMdl)
{
	vector<VesModelTree::AsmTreeData> TreeVec;
	VesModelTree::GetModelTreeComponentsinNLevel(m_currMdl, compID.size(), TreeVec);
	for (auto currTreeComp : TreeVec)
	{
		bool matched=false;
		for (int iii = 0; iii < compID.size(); iii++) {
			if (currTreeComp.lAsmPath.comp_id_table[iii] == compID.at(iii)) {
				matched = true;
			}
			else {
				matched = false;
				break;
			}
		}
		if (matched)
		{
			MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
			}
			MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
			MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

			MembMdl.lAsmName = currTreeComp.lAsmName;
			MembMdl.MdlType = currTreeComp.MdlType;

			MembMdl = currTreeComp;
			return true;
		}
	}
	return false;
}

bool VesModelTree::GetAsmCompTreeDatabyID(vector<VesModelTree::AsmTreeData>& TreeVec, vector<int> compID, VesModelTree::AsmTreeData& MembMdl)
{
	/*vector<VesModelTree::AsmTreeData> TreeVec;
	VesModelTree::GetModelTreeComponentsinNLevel(m_currMdl, compID.size(), TreeVec);*/
	for (auto currTreeComp : TreeVec)
	{
		bool matched = false;
		for (int iii = 0; iii < compID.size(); iii++) {
			if (currTreeComp.lAsmPath.comp_id_table[iii] == compID.at(iii)) {
				matched = true;
			}
			else {
				matched = false;
				break;
			}
		}
		if (matched)
		{
			MembMdl.lMdlAssm = currTreeComp.lMdlAssm;
			for (int i = 0; i < PRO_MAX_ASSEM_LEVEL; i++)
			{
				MembMdl.lAsmPath.comp_id_table[i] = currTreeComp.lAsmPath.comp_id_table[i];
			}
			MembMdl.lAsmPath.owner = currTreeComp.lAsmPath.owner;
			MembMdl.lAsmPath.table_num = currTreeComp.lAsmPath.table_num;

			MembMdl.lAsmName = currTreeComp.lAsmName;
			MembMdl.MdlType = currTreeComp.MdlType;

			MembMdl = currTreeComp;
			return true;
		}
	}
	return false;
}

//TODO Move the below utility file to respective class/project lib
void trim_left(std::string& str)
{
	string::size_type pos = str.find_first_not_of(' ');
	if (pos != string::npos)
		str.erase(0, pos);
	else
		str.erase(str.begin(), str.end());
}

void trim_right(string& str)
{
	string::size_type pos = str.find_last_not_of(' ');
	if (pos != string::npos) {
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if (pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
}
/*====================================================================*\
Function : GetParamValue
Purpose : Get ParamValue
Step1 :
Step2 :
Input :
Output :
Return : ProError
Created : 
History :
\*====================================================================*/
ProError GetParamValue(ProMdl lMdl, ProName lName, std::string& xParamValue)
{
	ProError lErr = PRO_TK_NO_ERROR;
	ProModelitem lModelItem;
	ProParameter lParameter;

	lErr = ProMdlToModelitem(lMdl, &lModelItem);
	lErr = ProParameterInit(&lModelItem, lName, &lParameter);
	if (lErr == PRO_TK_NO_ERROR)
	{
		ProParamvalue lValue;
		char lTempname[100];
		//lErr = ProParameterValueGet(&lParameter, &lValue);  //" "
		lErr = ProParameterValueWithUnitsGet(&lParameter, &lValue,NULL);  //" " " "
		xParamValue = ProWstringToString(lTempname, lValue.value.s_val);
		trim_left(xParamValue);
		trim_right(xParamValue);
	}
	return  lErr;
}

ProErr VesModelTree::utils::RenameModel(ProMdl mdl, string NewName)
{
	ProErr status;
	ProMdlName wNewName;
	string mdlName;
	string currmdlName;
	bool stat;
	string DllPath;
	DllPath = "tb_w_frame_and_prop1_1";
	/*VesSymbol::UpdateProSymbol(mdl, DllPath);
	DllPath = "tb_w_frame_and_prop1";
	VesSymbol::UpdateProSymbol(mdl, DllPath);
	VesUtility::_String::stringToWString(NewName, wNewName);
	currmdlName = VesCreoWindow::GetActiveWindowMdlName();
	stat = GetModelName(mdl, mdlName);
	if (!VesUtility::_String::CompareStringNoCase(mdlName, currmdlName)) {
		int windID = VesCreoWindow::CreateNewWindow(mdl);
	}
	status = ProMdlnameRename(mdl, wNewName);
	status = ProMdlCommonnameSet(mdl, wNewName);
	VesParameter::ParameterUpdate(mdl, NewName);
	VesMacro::Utils::RunMC(mdl);
	if (!VesUtility::_String::CompareStringNoCase(mdlName, currmdlName)) {
		VesCreoWindow::CloseCurrentWindow();
	}*/

	return status;
}

ProErr VesModelTree::utils::RenameModel(VesModelTree::AsmTreeData& mdlTree, string NewName)
{
	ProErr status;
	status = RenameModel(mdlTree.lMdlAssm, NewName);
	if (status == PRO_TK_NO_ERROR) mdlTree.lAsmName = NewName;
	return status;
}


ProMdl VesModelTree::utils::LoadMdlInSession(string MdlName)
{
	ProMdl mdl;
	ProName wName;
	ProErr status;
	stringToWString_(MdlName, wName);

	//status = ProMdlLoad(wName, PRO_MDL_ASSEMBLY, PRO_B_FALSE, &mdl);       //" "
	status = ProMdlFiletypeLoad(wName, PRO_MDLFILE_ASSEMBLY, PRO_B_FALSE, &mdl);    //" " " "

	//std::ofstream log_file("c:\\Temp\\Windchill_Names.txt", std::ios_base::out | std::ios_base::app);
	//log_file << MdlName << endl;

	if (status != PRO_TK_NO_ERROR)
	{
		//status = ProMdlRetrieve(wName, PRO_MDL_ASSEMBLY, &mdl);     //" "
		status = ProMdlnameRetrieve(wName, PRO_MDLFILE_ASSEMBLY, &mdl);    //" " " "
		if (status != PRO_TK_NO_ERROR)
			status = ProMdlFiletypeLoad(wName, PRO_MDLFILE_PART, PRO_B_FALSE, &mdl);     //" " " "
		if (status != PRO_TK_NO_ERROR)
			status = ProMdlnameRetrieve(wName, PRO_MDLFILE_PART, &mdl);      //" " " "
		if (status != PRO_TK_NO_ERROR)
			return NULL;
	}

	return mdl;
}

ProSolid VesModelTree::utils::LoadMdlRepInSession(string MdlName, string RepMdlName)
{
	ProMdl mdl;
	ProSolid mdlSol;
	ProAssembly mdlAssm;
	ProSimprepdata simpData;
	ProName wName,repName;
	ProErr status;
	stringToWString_(MdlName, wName);
	stringToWString_(RepMdlName, repName);
	
	//ProAssemblySimprepMdlnameRetrieve()
	//ProSimprepMdlnameRetrieve()
	//status = ProMdlLoad(wName, PRO_MDL_ASSEMBLY, PRO_B_FALSE, &mdl);
	//if (status != PRO_TK_NO_ERROR)
	//status = ProMdlRepresentationFiletypeLoad(wName, ProMdlfileType::PRO_MDLFILE_ASSEMBLY, repName,NULL, &mdl);
	status = ProSimprepMdlnameRetrieve(wName, PRO_MDLFILE_ASSEMBLY, PRO_SIMPREP_USER_DEFINED, repName, &mdlAssm);
	// ProAssemblySimprepMdlnameRetrieve(wName, ProMdlfileType::PRO_MDLFILE_ASSEMBLY, repName,NULL, &mdlAssm);
	if (status != PRO_TK_NO_ERROR)
	{
		return NULL;
	}
	mdlSol = (ProSolid)mdlAssm;
	return mdlSol;
}

bool VesModelTree::utils::IsMdlIsPart(ProMdl currMdl)
{
	ProErr lErr;
	ProMdlType p_type;
	lErr = ProMdlTypeGet(currMdl, &p_type);
	if (lErr == PRO_TK_NO_ERROR && PRO_MDL_PART == p_type)
	{
		return true;
	}
	else
		return false;
}

bool VesModelTree::utils::IsMdlIsAssembly(ProMdl currMdl)
{
	ProErr lErr;
	ProMdlType p_type;
	lErr = ProMdlTypeGet(currMdl, &p_type);
	if (lErr == PRO_TK_NO_ERROR && PRO_MDL_ASSEMBLY == p_type)
	{
		return true;
	}
	else
		return false;
}

bool VesModelTree::utils::GetModelName(ProMdl mdl, string& currMdlName)
{
	ProMdlName name;
	ProError status;

	status = ProMdlMdlnameGet(mdl, name);

	WcharToString_(name, currMdlName);
	if (PRO_TK_NO_ERROR == status)
	{
		return true;
	}
	else
		return false;
	//TODO ProWstringFree is required?
}

bool VesModelTree::utils::IsModelsSkeleton(ProMdl mdl)
{
	ProBoolean isSkel = PRO_B_FALSE;
	ProErr status;
	status = ProMdlIsSkeleton(mdl, &isSkel);
	if (status == PRO_TK_NO_ERROR && isSkel == PRO_B_TRUE)
	{
		return true;
	}
	else
		return false;
}