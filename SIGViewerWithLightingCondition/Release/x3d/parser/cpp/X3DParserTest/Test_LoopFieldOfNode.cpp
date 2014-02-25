#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  ノードに属するフィールドをループする
// ==========================================

void Test_printFieldOfNode(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRMLファイルをパースする
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Fields of Node in (%s) ****\n", vrmlFile);

	// ---------------------------------------------
	//  ルート直下のノードを得る
	// ---------------------------------------------
	MFNode *nodes = parser.getChildrenOfRootNode();

	if (nodes)
	{
		// ---------------------------------------------
		//  ノードでループ
		// ---------------------------------------------
		for (int i=0; i<nodes->count(); i++)
		{
			CX3DNode *node = nodes->getNode(i);
			if (node)
			{
				// ------------------------------------
				//  ノード名表示
				// ------------------------------------
				char *nodeName = node->getNodeName();
				printf("%s\n", nodeName);

				// ------------------------------------
				//  ノードの持つフィールドをループ
				// ------------------------------------
				int nFields = node->countFields();
				for (int iField=0; iField<nFields; iField++)
				{
					// フィールド名
					char *fieldName = node->getFieldName(iField);

					// フィールドの型
					int fieldType = node->getFieldType(iField);

					// フィールドの型名(SFInt32, SFString, MFNode, ... etc)
					char *fieldTypeName = node->getFieldTypeString(iField);

					printf("\t%s [%d:%s]\n", fieldName, fieldType, fieldTypeName);
				}
			}
		}

		// ---------------------------------------------
		//  後始末
		// ---------------------------------------------
		delete nodes;
		nodes = NULL;
	}
}
