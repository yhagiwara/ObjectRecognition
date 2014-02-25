#ifndef _CX3DNODE_H_
#define _CX3DNODE_H_

#include "CX3DField.h"
#include "CVRMLFieldData.h"
#include <jni.h>

// �z�Q�Ɓi�H�j�̂��ߒP���ɃC���N���[�h����ƃG���[�ɂȂ�
//#include "CX3DField.h"
class CVRMLFieldData;


/////////////////////////////////////////////////////////////
//
//  Node ID

enum CX3DNodeType
{
	BASE_NODE,	// �ȉ��̃m�[�h�ł͂Ȃ��A�����̃m�[�h
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
	//  �m�[�h�̌^�𓾂�
	// ------------------------------------------
	virtual CX3DNodeType getNodeType() { return BASE_NODE; }

	// ------------------------------------------
	//  �m�[�h�^�𕶎���\���ɂ���
	// ------------------------------------------
	static char *getNodeTypeString(CX3DNodeType ntype);

	// ------------------------------------------
	//  �m�[�h���𓾂�
	// ------------------------------------------
	char *getNodeName();

	// ------------------------------------------
	//  ���̃m�[�h�̎��t�B�[���h�̐���Ԃ�
	// ------------------------------------------
	int countFields();

	// ------------------------------------------
	//  iField�Ԗڂ̃t�B�[���h���𓾂�
	//
	//  iField��0����J�E���g����
	// ------------------------------------------
	char *getFieldName(int iField);
	
	// ------------------------------------------
	//  iField�Ԗڂ̃t�B�[���h�̌^�𓾂�
	//  �Ԃ�l��CX3DFieldType�Ɠ����l
	//
	//  iField��0����J�E���g����
	// ------------------------------------------
	int getFieldType(int iField);
	
	// ------------------------------------------
	//  iField�Ԗڂ̃t�B�[���h�̌^���𓾂�
	//  �Ԃ�l��"SFInt32", "SFString", "MFNode"��
	//
	//  iField��0����J�E���g����
	// ------------------------------------------
	char *getFieldTypeString(int iField);

	// ------------------------------------------
	//  �t�B�[���h���ɑΉ�����
	//  �t�B�[���h�C���f�b�N�X(iField)��Ԃ�
	// ------------------------------------------
	int getFieldIndex(char *fieldName);

	// --------------------------------------------------------------
	// �w��t�B�[���h�̃f�[�^��Ԃ�
	// �Ăяo�����͕Ԃ�l��CVRMLFieldData��delete���Ȃ���΂Ȃ�Ȃ��D
	// --------------------------------------------------------------
	CVRMLFieldData *createFieldValue(int iField);
	CVRMLFieldData *createFieldValue(char *fieldName);

	// --------------------------------------------------------
	//  ���̃m�[�h�̎��q�m�[�h�������̖��O�̃m�[�h��T��
	// --------------------------------------------------------
	// �����̎q�̂�
	MFNode *searchNodesFromDirectChildren(char *nodeName);

	// �ċA�I�ɂ��ׂĂ̎q��T��
	MFNode *searchNodesFromAllChildren(char *nodeName);
	
	// ------------------------------------------
	//  ���̃m�[�h�̓��e�����O�ɏo�͂���
	// ------------------------------------------
	virtual void print(int indent=0);

	// ------------------------------------------
	// �m�[�h�̃g���o�[�X�̘g�g�݃R�[�h
	// �i�Q�l�p�F�������Ȃ��j
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

