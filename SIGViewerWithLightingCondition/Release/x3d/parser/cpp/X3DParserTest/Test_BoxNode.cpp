#include "CX3DParser.h"
#include "Test.h"

// ==========================================
//  Box�m�[�h�̃t�B�[���h�𓾂�
// ==========================================

void Test_printFieldsOfBoxNode(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Fields of BoxNode ****\n");

	// ---------------------------------------------
	//  Shape�m�[�h��T��
	// ---------------------------------------------
	MFNode *pShapeNodes = parser.searchNodesFromAllChildrenOfRoot("Shape");
	if (pShapeNodes)
	{
		for (int i=0; i<pShapeNodes->count(); i++)
		{
			CX3DShapeNode *pShape = (CX3DShapeNode *)(pShapeNodes->getNode(i));

			if (pShape)
			{
				// --------------------------------------------------------
				//  Shape�m�[�h��geometry�t�B�[���h�̒l�𓾂�
				//
				// �����Ɏ��ۂ̌`��f�[�^
				// (Box, Cylinder, Sphere, IndexedFaceSet���j�������Ă���
				// --------------------------------------------------------
				SFNode *geometry = pShape->getGeometry();
				CX3DNode *pNode = geometry->getNode();

				// ---------------------------------------------
				//  �m�[�h��Box�m�[�h���H
				// ---------------------------------------------
				if (pNode && pNode->getNodeType() == BOX_NODE)
				{
					CX3DBoxNode *pBoxNode = (CX3DBoxNode *)pNode;

					// ---------------------------------------------
					//  Box�m�[�h�̃t�B�[���h�̒l�𓾂�
					// ---------------------------------------------

					// �T�C�Y
					SFVec3f *sz = pBoxNode->getSize();
					printf("size : (%f, %f, %f)\n", sz->x(), sz->y(), sz->z());

					// solid
					SFBool *solid = pBoxNode->getSolid();
					printf("solid : %s\n", solid->getValue() ? "TRUE" : "FALSE");
				}
			}
		}

		delete pShapeNodes;
		pShapeNodes = NULL;
	}
}
