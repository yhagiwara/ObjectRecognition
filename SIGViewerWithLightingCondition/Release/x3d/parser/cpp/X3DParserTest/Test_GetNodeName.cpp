#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  �m�[�h����\������
// ==========================================

void Test_printNodeName(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Node names of (%s) ****\n", vrmlFile);

	// ---------------------------------------------
	//  ���[�g�����̃m�[�h�𓾂�
	// ---------------------------------------------
	MFNode *nodes = parser.getChildrenOfRootNode();

	if (nodes)
	{
		// ---------------------------------------------
		//  �m�[�h����\��
		// ---------------------------------------------
		for (int i=0; i<nodes->count(); i++)
		{
			CX3DNode *node = nodes->getNode(i);

			if (node)
			{
				printf("%s\n", node->getNodeName());
			}
		}

		// ---------------------------------------------
		//  ��n��
		// ---------------------------------------------
		delete nodes;
		nodes = NULL;
	}
}
