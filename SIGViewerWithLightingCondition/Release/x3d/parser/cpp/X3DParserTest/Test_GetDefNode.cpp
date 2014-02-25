#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  指定defノードの内容を表示する
// ==========================================

void Test_printDefNode(CX3DParser& parser, char *vrmlFile, char *defName)
{
	// ---------------------------------------------
	//  VRMLファイルをパースする
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Node contents of defName(%s) ****\n", defName);

	// ---------------------------------------------
	//  defノードを得る
	// ---------------------------------------------
	CX3DNode *node = parser.getDefNode(defName);
	if (node)
	{
		node->print();

		delete node;
		node = NULL;
	}
}
