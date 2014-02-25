#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  ���[�g�����̃m�[�h��񋓂���
// ==========================================

void Test_printChildrenOfRoot(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Children of Root Node ****\n");

	// ---------------------------------------------
	//  ���[�g�����̃m�[�h�𓾂�
	// ---------------------------------------------
	MFNode *nodes = parser.getChildrenOfRootNode();

	if (nodes)
	{
		// ---------------------------------------------
		//  �m�[�h���̂ݕ\��
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
