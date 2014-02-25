#include "CX3DParser.h"
#include "Test.h"

// ===============================================================
//  OpenHRP������VRML�t�@�C����ǂ݁A����̒l�����o���\������
// ===============================================================

void printOpenHRPStructure(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** OpenHRP field value of (%s) ****\n", vrmlFile);

	// -----------------------------------------
	//  Humanoid�m�[�h��T��
	//
	//  ���������m�[�h�͕����ɂȂ蓾��
	// -----------------------------------------
	MFNode *pHumanoidNodes = parser.searchNodesFromAllChildrenOfRoot("Humanoid");
	if (pHumanoidNodes)
	{
		for (int i=0; i<pHumanoidNodes->count(); i++)
		{
			CX3DOpenHRPHumanoidNode *pHumanoid = (CX3DOpenHRPHumanoidNode *)(pHumanoidNodes->getNode(i));

			if (pHumanoid)
			{
				// -----------------------------------------
				//  HumanoidBody�t�B�[���h�̒l�����o��
				// -----------------------------------------
				MFNode *humanoidBody = pHumanoid->getHumanoidBody();

				// -----------------------------------------
				//  �t�B�[���h���܂ރm�[�h�̌��𓾂�
				// -----------------------------------------
				int nBody = humanoidBody->count();

				for (int i=0; i<nBody; i++)
				{
					// -----------------------------------------
					//  i�Ԗڂ̃m�[�h�����o��
					// -----------------------------------------
					CX3DNode *pNode = humanoidBody->getNode(i);
					if (pNode)
					{
						// -----------------------------------------
						//  �m�[�h�̌^�ŕ���
						// -----------------------------------------
						switch (pNode->getNodeType())
						{
						case OPENHRP_JOINT_NODE:
							{
								// -----------------------------------------
								//  �m�[�h��Joint�������ꍇ
								// -----------------------------------------
								CX3DOpenHRPJointNode *pJointNode = (CX3DOpenHRPJointNode *)pNode;
								printOpenHRPJointNode(pJointNode, 0);
							}
							break;
						case OPENHRP_SEGMENT_NODE:
							{
								// -----------------------------------------
								//  �m�[�h��Segment�������ꍇ
								// -----------------------------------------
								CX3DOpenHRPSegmentNode *pSegNode = (CX3DOpenHRPSegmentNode *)pNode;
								printOpenHRPSegmentNode(pSegNode, 0);
							}
							break;
						}
					}
				}
			}
		}

		delete pHumanoidNodes;
		pHumanoidNodes = NULL;
	}
}

// ===============================================================
//  Joint�m�[�h�̕\��
// ===============================================================
void printOpenHRPJointNode(CX3DOpenHRPJointNode *pJointNode, int indent)
{
	// -----------------------------------------
	//  Joint��
	// -----------------------------------------
	SFString *name = pJointNode->getName();

	// -----------------------------------------
	//  translation�t�B�[���h
	// -----------------------------------------
	SFVec3f *trans = pJointNode->getTranslation();

	// -----------------------------------------
	//  rotation�t�B�[���h
	// -----------------------------------------
	SFRotation *rot = pJointNode->getRotation();

	// -----------------------------------------
	//  jointType�t�B�[���h
	// -----------------------------------------
	SFString *jointType = pJointNode->getJointType();

	// -----------------------------------------
	//  children�t�B�[���h
	//
	//  �����ɁA
	//  0�ȏ��Joint�m�[�h
	//  ���X1��Segment�m�[�h
	//  ���܂܂��
	// -----------------------------------------
	MFNode *children = pJointNode->getChildren();

	CX3DParser::printIndent(indent);
	CX3DParser::printLog("name: (%s)\n", name->getValue());

	CX3DParser::printIndent(indent);
	CX3DParser::printLog("type: (%s)\n", jointType->getValue());

	CX3DParser::printIndent(indent);
	CX3DParser::printLog("translation: (%f, %f, %f)\n", trans->x(), trans->y(), trans->z());

	CX3DParser::printIndent(indent);
	CX3DParser::printLog("rotation:    (%f, %f, %f, %f)\n", rot->x(), rot->y(), rot->z(), rot->rot());

	// -------------------------------------------
	//  children�t�B�[���h���܂ރm�[�h�̐��𓾂�
	// -------------------------------------------
	int n = children->count();

	for (int i=0; i<n; i++)
	{
		// -------------------------------------------
		//  i�Ԗڂ̃m�[�h�����o��
		// -------------------------------------------
		CX3DNode *pNode = children->getNode(i);

		// -------------------------------------------
		//  �m�[�h�̌^�ŕ���
		// -------------------------------------------
		switch (pNode->getNodeType())
		{
		case OPENHRP_JOINT_NODE:
			{
				// -------------------------------------------
				//  Joint�m�[�h�̏ꍇ
				// -------------------------------------------
				CX3DOpenHRPJointNode *pJointNode = (CX3DOpenHRPJointNode *)pNode;
				printOpenHRPJointNode(pJointNode, indent+1);
			}
			break;

		case OPENHRP_SEGMENT_NODE:
			{
				// -------------------------------------------
				//  Segment�m�[�h�̏ꍇ
				// -------------------------------------------
				CX3DOpenHRPSegmentNode *pSegNode = (CX3DOpenHRPSegmentNode *)pNode;
				printOpenHRPSegmentNode(pSegNode, indent+1);
			}
			break;
		}
	}
}

// ===============================================================
//  Segment�m�[�h�̕\��
// ===============================================================
void printOpenHRPSegmentNode(CX3DOpenHRPSegmentNode *pSegmentNode, int indent)
{
	// -----------------------------------------
	//  Joint��
	// -----------------------------------------
	SFString *name = pSegmentNode->getName();

	CX3DParser::printIndent(indent);
	CX3DParser::printLog("name: (%s)\n", name->getValue());

	// -----------------------------------------
	//  children�t�B�[���h
	//
	//  �����ɁA���ۂ�Segment�̌`�󂪂���
	//
	//  �ʏ�ABox�ASphere�ACylinder��
	//  �����ꂩ�̃m�[�h���܂�Shape�m�[�h������
	// -----------------------------------------
	MFNode *children = pSegmentNode->getChildren();

	int n = children->count();
	for (int i=0; i<n; i++)
	{
		CX3DNode *pNode = children->getNode(i);
		switch (pNode->getNodeType())
		{
		case SHAPE_NODE:
			{
				CX3DShapeNode *pShape = (CX3DShapeNode *)pNode;
				pShape->print(indent);
			}
			break;
		}
	}
}
