#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  ノード名を表示する
// ==========================================

void Test_printNodeName(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRMLファイルをパースする
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Node names of (%s) ****\n", vrmlFile);

	// ---------------------------------------------
	//  ルート直下のノードを得る
	// ---------------------------------------------
	MFNode *nodes = parser.getChildrenOfRootNode();

	if (nodes)
	{
		// ---------------------------------------------
		//  ノード名を表示
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
		//  後始末
		// ---------------------------------------------
		delete nodes;
		nodes = NULL;
	}
}
