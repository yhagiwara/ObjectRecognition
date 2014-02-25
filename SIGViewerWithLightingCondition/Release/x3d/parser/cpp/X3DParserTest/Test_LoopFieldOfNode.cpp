#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  �m�[�h�ɑ�����t�B�[���h�����[�v����
// ==========================================

void Test_printFieldOfNode(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Fields of Node in (%s) ****\n", vrmlFile);

	// ---------------------------------------------
	//  ���[�g�����̃m�[�h�𓾂�
	// ---------------------------------------------
	MFNode *nodes = parser.getChildrenOfRootNode();

	if (nodes)
	{
		// ---------------------------------------------
		//  �m�[�h�Ń��[�v
		// ---------------------------------------------
		for (int i=0; i<nodes->count(); i++)
		{
			CX3DNode *node = nodes->getNode(i);
			if (node)
			{
				// ------------------------------------
				//  �m�[�h���\��
				// ------------------------------------
				char *nodeName = node->getNodeName();
				printf("%s\n", nodeName);

				// ------------------------------------
				//  �m�[�h�̎��t�B�[���h�����[�v
				// ------------------------------------
				int nFields = node->countFields();
				for (int iField=0; iField<nFields; iField++)
				{
					// �t�B�[���h��
					char *fieldName = node->getFieldName(iField);

					// �t�B�[���h�̌^
					int fieldType = node->getFieldType(iField);

					// �t�B�[���h�̌^��(SFInt32, SFString, MFNode, ... etc)
					char *fieldTypeName = node->getFieldTypeString(iField);

					printf("\t%s [%d:%s]\n", fieldName, fieldType, fieldTypeName);
				}
			}
		}

		// ---------------------------------------------
		//  ��n��
		// ---------------------------------------------
		delete nodes;
		nodes = NULL;
	}
}
