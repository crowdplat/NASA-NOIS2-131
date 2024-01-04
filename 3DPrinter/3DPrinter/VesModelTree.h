#pragma once

#include "CreoCommonHeader.h"
#include <iostream>
#include <ProToolkit.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProSelection.h>
#include <ProSelbuffer.h>
#include <ProIntfData.h>
#include <ProArray.h>
#include <ProNotify.h>
#include <ProDrawing.h>
#include <ProWindows.h>
#include <ProSolid.h>
#include <ProGraphic.h>
#include <ProCombstate.h>
#include<string>
#include<vector>
#include<map>
using namespace std; 

class VesModelTree
{
private:
	ProMdl m_currMdl;
public:
	VesModelTree(ProMdl currMdl);
	struct AsmTreeData
	{
		ProMdl lMdlAssm;
		ProAsmcomppath lAsmPath;
		std::string lAsmName;
		ProMdlType MdlType;
	};

	bool GetAsmCompTreeData( string compName, VesModelTree::AsmTreeData& MembMdl);
	bool GetAsmCompinNLevelTreeData(string compName, int nLevel, VesModelTree::AsmTreeData& MembMdl);
	bool GetAsmCompTreeDatabyID(vector<int> compID,  VesModelTree::AsmTreeData& MembMdl);
	bool GetAsmCompTreeDatabyID(vector<VesModelTree::AsmTreeData>& TreeVec,  vector<int> compID,  VesModelTree::AsmTreeData& MembMdl);
	bool GetAsmCompTreeData( string compName,  vector<VesModelTree::AsmTreeData>& MembMdlVec);
	void GetAsmCompTreeData(map<string, VesModelTree::AsmTreeData>& MembMdlMap);
	void GetAsmCompTreeData(vector<VesModelTree::AsmTreeData>& TreeVec);
	bool GetAsmCompTreeData(vector<VesModelTree::AsmTreeData>& TreeVec,  string compName,  VesModelTree::AsmTreeData& MembMdl);

	static ProError CollectModelTreeComponents(ProAsmcomppath* xPath, ProSolid xSolid, ProBoolean down, ProAppData app_data);
	static ProError CollectModelTreeComponentsuptoNLevel(ProAsmcomppath* xPath, ProSolid xSolid, ProBoolean down, ProAppData app_data);
	static ProError CollectModelTreeComponentsFilter(ProAsmcomppath* xPath, ProSolid xSolid, ProAppData app_data);
	static void GetModelTreeComponents(ProMdl Mdl, std::vector<AsmTreeData> &ComponentVector);
	static void GetModelTreeComponentsinNLevel(ProMdl Mdl,int nLevel,  std::vector<VesModelTree::AsmTreeData>& ComponentVector);
	static ProError GetParentCompPath(ProAsmcomppath inPath,ProAsmcomppath& parentPath);
	static void LinkChildAsmCompTreeDataWithParentAsmCompTreeData(VesModelTree::AsmTreeData _parentAsmCmp, VesModelTree::AsmTreeData _childAsmCmp,
		VesModelTree::AsmTreeData& linkedchildAsmComp);
	class utils
	{
	public:
		static ProErr RenameModel(ProMdl mdl, string NewName);
		static ProErr RenameModel(VesModelTree::AsmTreeData& mdlTree, string NewName);
		static ProMdl LoadMdlInSession(string MdlName);
		static ProSolid LoadMdlRepInSession(string MdlName, string RepMdlName);
		static bool IsMdlIsPart(ProMdl mdl);
		static bool IsMdlIsAssembly(ProMdl mdl);
		static bool IsModelsSkeleton(ProMdl mdl);

		static bool GetModelName(ProMdl mdl, string &currMdlName);

	};
};

ProError GetParamValue(ProMdl lMdl, ProName lName, std::string& xParamValue);