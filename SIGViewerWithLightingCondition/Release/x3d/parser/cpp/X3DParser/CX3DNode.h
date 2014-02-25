#ifndef _CX3DNODE_H_
#define _CX3DNODE_H_

#include "CX3DField.h"
#include "CVRMLFieldData.h"
#include <jni.h>

// 循環参照（？）のため単純にインクルードするとエラーになる
//#include "CX3DField.h"
class CVRMLFieldData;


/////////////////////////////////////////////////////////////
//
//  Node ID

enum CX3DNodeType
{
	BASE_NODE,	// 以下のノードではない、何かのノード
	APPEARANCE_NODE,
	BOX_NODE,
	COLOR_NODE,
	CONE_NODE,
	COORDINATE_NODE,
	CYLINDER_NODE,
	DIRECTIONAL_LIGHT_NODE,
	GROUP_NODE,
	HANIM_DISPLACER_NODE,
	HANIM_HUMANOID_NODE,
	HANIM_JOINT_NODE,
	HANIM_SEGMENT_NODE,
	HANIM_SITE_NODE,
	IMAGE_TEXTURE_NODE,
	INDEXED_FACE_SET_NODE,
	INDEXED_LINE_SET_NODE,
	MATERIAL_NODE,
	NORMAL_NODE,
	OPENHRP_HUMANOID_NODE,
	OPENHRP_JOINT_NODE,
	OPENHRP_SEGMENT_NODE,
	POINT_LIGHT_NODE,
	SCENE_NODE,
	SHAPE_NODE,
	SPHERE_NODE,
	SPOT_LIGHT_NODE,
	TEXTURE_COORDINATE_NODE,
	TEXTURE_TRANSFORM_NODE,
	TRANSFORM_NODE,
	VIEWPOINT_NODE
};


/////////////////////////////////////////////////////////////
//
//  Base class

class CX3DNode
{
public:
	CX3DNode();
	CX3DNode(jobject vrmlNode);
	virtual ~CX3DNode();

	// ------------------------------------------
	//  ノードの型を得る
	// ------------------------------------------
	virtual CX3DNodeType getNodeType() { return BASE_NODE; }

	// ------------------------------------------
	//  ノード型を文字列表現にする
	// ------------------------------------------
	static char *getNodeTypeString(CX3DNodeType ntype);

	// ------------------------------------------
	//  ノード名を得る
	// ------------------------------------------
	char *getNodeName();

	// ------------------------------------------
	//  このノードの持つフィールドの数を返す
	// ------------------------------------------
	int countFields();

	// ------------------------------------------
	//  iField番目のフィールド名を得る
	//
	//  iFieldは0からカウントする
	// ------------------------------------------
	char *getFieldName(int iField);
	
	// ------------------------------------------
	//  iField番目のフィールドの型を得る
	//  返り値はCX3DFieldTypeと同じ値
	//
	//  iFieldは0からカウントする
	// ------------------------------------------
	int getFieldType(int iField);
	
	// ------------------------------------------
	//  iField番目のフィールドの型名を得る
	//  返り値は"SFInt32", "SFString", "MFNode"等
	//
	//  iFieldは0からカウントする
	// ------------------------------------------
	char *getFieldTypeString(int iField);

	// ------------------------------------------
	//  フィールド名に対応する
	//  フィールドインデックス(iField)を返す
	// ------------------------------------------
	int getFieldIndex(char *fieldName);

	// --------------------------------------------------------------
	// 指定フィールドのデータを返す
	// 呼び出し側は返り値のCVRMLFieldDataをdeleteしなければならない．
	// --------------------------------------------------------------
	CVRMLFieldData *createFieldValue(int iField);
	CVRMLFieldData *createFieldValue(char *fieldName);

	// --------------------------------------------------------
	//  このノードの持つ子ノードから特定の名前のノードを探す
	// --------------------------------------------------------
	// 直下の子のみ
	MFNode *searchNodesFromDirectChildren(char *nodeName);

	// 再帰的にすべての子を探す
	MFNode *searchNodesFromAllChildren(char *nodeName);
	
	// ------------------------------------------
	//  このノードの内容をログに出力する
	// ------------------------------------------
	virtual void print(int indent=0);

	// ------------------------------------------
	// ノードのトラバースの枠組みコード
	// （参考用：何もしない）
	// ------------------------------------------
	void traverse();

protected:
	jobject m_vrmlNode;
};


/////////////////////////////////////////////////////////////
//
//  Node Factory

class CX3DNodeFactory
{
public:
	static CX3DNode *createNode(jobject vrmlNode);
};

#endif

