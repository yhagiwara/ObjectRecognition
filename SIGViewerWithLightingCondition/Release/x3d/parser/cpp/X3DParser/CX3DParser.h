#ifndef _CX3D_PARSER_H_
#define _CX3D_PARSER_H_

#include "CX3DNode.h"
#include "CX3DBoxNode.h"
#include "CX3DColorNode.h"
#include "CX3DConeNode.h"
#include "CX3DCoordinateNode.h"
#include "CX3DCylinderNode.h"
#include "CX3DDirectionalLightNode.h"
#include "CX3DGroupNode.h"
#include "CX3DHAnimDisplacerNode.h"
#include "CX3DHAnimHumanoidNode.h"
#include "CX3DHAnimJointNode.h"
#include "CX3DHAnimSegmentNode.h"
#include "CX3DHAnimSiteNode.h"
#include "CX3DImageTextureNode.h"
#include "CX3DIndexedFaceSetNode.h"
#include "CX3DIndexedLineSetNode.h"
#include "CX3DMaterialNode.h"
#include "CX3DNormalNode.h"
#include "CX3DOpenHRPHumanoidNode.h"
#include "CX3DOpenHRPJointNode.h"
#include "CX3DOpenHRPSegmentNode.h"
#include "CX3DPointLightNode.h"
#include "CX3DShapeNode.h"
#include "CX3DSphereNode.h"
#include "CX3DSpotLightNode.h"
#include "CX3DTextureCoordinateNode.h"
#include "CX3DTextureTransformNode.h"
#include "CX3DTransformNode.h"
#include "CX3DViewpointNode.h"
#include "CJNIUtil.h"

#include <jni.h>
#include <vector>

class CX3DParser
{
public:
	CX3DParser();
	virtual ~CX3DParser();

	// =================================================
	//	X3D Parse Methods
	// =================================================

	// ---------------------------------------------
	//  X3D/VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	bool parse(char *fname);

	// ---------------------------------------------
	//  �p�[�X���ʂ����O�ɏ����o��
	// ---------------------------------------------
	void print();

	/**
	 * �m�[�h�^�C�v�̃`�F�b�N
	 */
	void printNodeTypeList();

	// ---------------------------------------------
	//  ���[�g�̎q�m�[�h��Ԃ�
	// ---------------------------------------------
	MFNode *getChildrenOfRootNode();

	// ---------------------------------------------
	//  ���[�g�̎q�m�[�h����
	//  �w��m�[�h���������̂�T���D
	//
	//  ���m�[�h�ȍ~�͌������Ȃ�
	// ---------------------------------------------
	MFNode *searchNodesFromDirectChildrenOfRoot(char *nodeName);

	// ---------------------------------------------
	//  ���[�g�̂��ׂĂ̎q�m�[�h����A
	//  �w��m�[�h���������ׂẴm�[�h���ċA�I�ɒT��
	// ---------------------------------------------
	MFNode *searchNodesFromAllChildrenOfRoot(char *nodeName);

	// ---------------------------------------------
	//  ���ׂĂ�def�m�[�h���𓾂�
	// ---------------------------------------------
	std::vector<std::string> getDefNames();

	// ---------------------------------------------
	//  def�m�[�h��Ԃ�
	// ---------------------------------------------
	CX3DNode *getDefNode(char *defName);


	// =================================================
	//	Debug Log�֘A
	// =================================================

	// ---------------------------------------------
	//  ���O���J��
	//
	//  ���O�̏o�͐�t�@�C����fname�ɂȂ�D
	//  ��2������true�ɂ���ƃt�@�C����ǉ����[�h��
	//  �J��
	// ---------------------------------------------
	static void openDebugLog(char *fname, bool bAppend=false);

	// ---------------------------------------------
	//  ���O�����
	//
	//  ���̃��\�b�h���Ă񂾌�́A���O�͂ǂ��ɂ�
	//  �o�Ȃ��Ȃ�
	// ---------------------------------------------
	static void closeDebugLog();

	// ---------------------------------------------
	//  ���O�̏o�͐��fp�ɐݒ肷��
	//
	//  ���̃��\�b�h���ĂԈȑO�Ƀ��O���J���Ă����ꍇ
	//  ���̃��O�͕�����
	//
	//  fp��NULL�ɐݒ肷��ƁA���O�͂ǂ��ɂ��o�͂���Ȃ�
	// ---------------------------------------------
	static void setDebugLogFp(FILE *fp);

	// ---------------------------------------------
	//  ���O�̏o�͐��stderr�ɖ߂�
	//
	//  ���̃��\�b�h���ĂԈȑO�Ƀ��O���J���Ă����ꍇ
	//  ���̃��O�͕�����
	// ---------------------------------------------
	static void resetDebugLogFp();

	// ---------------------------------------------
	//  ���݂̃��O�̏o�͐�(=FILE *)��Ԃ�
	// ---------------------------------------------
	static FILE *getDebugLogFp();

	// ---------------------------------------------
	//  �C���f���g���o�͂���
	// ---------------------------------------------
	static void printIndent(int indent);

	// -----------------------------------------------------
	//  ���O���o�͂���
	//
	//  �f�t�H���g�ł�stderr�ɏo��D
	//  openDebugLog�܂���setDebugLog�ŏo�͐��ς����ꍇ�́A
	//  �����ɏo��
	// -----------------------------------------------------
	static void printLog(char *format, ...);

	// ---------------------------------------------
	//  �C���f���g�����O�o��
	// ---------------------------------------------
	static void printIndentLog(int indentLevel, char *format, ...);

	// ---------------------------------------------
	//  �o�b�t�@�ɗ��܂��Ă��郍�O���t���b�V������
	// ---------------------------------------------
	static void flushLog();

	// ----------------------------------------------------------
	//  MF�t�B�[���h�̕\������擪n�܂łɐ�������D
	//
	//  ���Ƃ��΁A�v�f��1000��MFInt32���������ꍇ�A
	//  n=5�Ƃ��Ă��̃��\�b�h���R�[�����Ă����΁A
	//  print()�ŕ\�������̂�1000�̂����A�擪��5�����ɂȂ�D
	//  �i������MFNode�Ɋւ��Ă͂��ׂĕ\������j
	// 
	//  �t�B�[���h��\������ہAMF�t�B�[���h�̗v�f����
	//  ��������ƁA�S�̂̍\�����킩��ɂ����Ȃ�̂�
	//  �����������邽�߂̑΍�
	//
	//  0�ȉ��̐���^�����ꍇ�A���ׂĂ̗v�f��\������
	//  �i�f�t�H���g�ł͂��ׂĕ\���j
	// ----------------------------------------------------------
	static void setMaxPrintElemsForMFField(int n);

	static int getMaxPrintElemsForMFField();

private:
	std::string m_fname;
	jobject m_X3DParser;
};

#endif
