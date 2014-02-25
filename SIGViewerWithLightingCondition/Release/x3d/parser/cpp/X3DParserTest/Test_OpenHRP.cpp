#include "CX3DParser.h"
#include "Test.h"

// ===============================================================
//  OpenHRP準拠のVRMLファイルを読み、特定の値を取り出し表示する
// ===============================================================

void printOpenHRPStructure(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRMLファイルをパースする
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** OpenHRP field value of (%s) ****\n", vrmlFile);

	// -----------------------------------------
	//  Humanoidノードを探す
	//
	//  発見されるノードは複数個になり得る
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
				//  HumanoidBodyフィールドの値を取り出す
				// -----------------------------------------
				MFNode *humanoidBody = pHumanoid->getHumanoidBody();

				// -----------------------------------------
				//  フィールドが含むノードの個数を得る
				// -----------------------------------------
				int nBody = humanoidBody->count();

				for (int i=0; i<nBody; i++)
				{
					// -----------------------------------------
					//  i番目のノードを取り出す
					// -----------------------------------------
					CX3DNode *pNode = humanoidBody->getNode(i);
					if (pNode)
					{
						// -----------------------------------------
						//  ノードの型で分岐
						// -----------------------------------------
						switch (pNode->getNodeType())
						{
						case OPENHRP_JOINT_NODE:
							{
								// -----------------------------------------
								//  ノードがJointだった場合
								// -----------------------------------------
								CX3DOpenHRPJointNode *pJointNode = (CX3DOpenHRPJointNode *)pNode;
								printOpenHRPJointNode(pJointNode, 0);
							}
							break;
						case OPENHRP_SEGMENT_NODE:
							{
								// -----------------------------------------
								//  ノードがSegmentだった場合
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
//  Jointノードの表示
// ===============================================================
void printOpenHRPJointNode(CX3DOpenHRPJointNode *pJointNode, int indent)
{
	// -----------------------------------------
	//  Joint名
	// -----------------------------------------
	SFString *name = pJointNode->getName();

	// -----------------------------------------
	//  translationフィールド
	// -----------------------------------------
	SFVec3f *trans = pJointNode->getTranslation();

	// -----------------------------------------
	//  rotationフィールド
	// -----------------------------------------
	SFRotation *rot = pJointNode->getRotation();

	// -----------------------------------------
	//  jointTypeフィールド
	// -----------------------------------------
	SFString *jointType = pJointNode->getJointType();

	// -----------------------------------------
	//  childrenフィールド
	//
	//  ここに、
	//  0個以上のJointノード
	//  高々1個のSegmentノード
	//  が含まれる
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
	//  childrenフィールドが含むノードの数を得る
	// -------------------------------------------
	int n = children->count();

	for (int i=0; i<n; i++)
	{
		// -------------------------------------------
		//  i番目のノードを取り出す
		// -------------------------------------------
		CX3DNode *pNode = children->getNode(i);

		// -------------------------------------------
		//  ノードの型で分岐
		// -------------------------------------------
		switch (pNode->getNodeType())
		{
		case OPENHRP_JOINT_NODE:
			{
				// -------------------------------------------
				//  Jointノードの場合
				// -------------------------------------------
				CX3DOpenHRPJointNode *pJointNode = (CX3DOpenHRPJointNode *)pNode;
				printOpenHRPJointNode(pJointNode, indent+1);
			}
			break;

		case OPENHRP_SEGMENT_NODE:
			{
				// -------------------------------------------
				//  Segmentノードの場合
				// -------------------------------------------
				CX3DOpenHRPSegmentNode *pSegNode = (CX3DOpenHRPSegmentNode *)pNode;
				printOpenHRPSegmentNode(pSegNode, indent+1);
			}
			break;
		}
	}
}

// ===============================================================
//  Segmentノードの表示
// ===============================================================
void printOpenHRPSegmentNode(CX3DOpenHRPSegmentNode *pSegmentNode, int indent)
{
	// -----------------------------------------
	//  Joint名
	// -----------------------------------------
	SFString *name = pSegmentNode->getName();

	CX3DParser::printIndent(indent);
	CX3DParser::printLog("name: (%s)\n", name->getValue());

	// -----------------------------------------
	//  childrenフィールド
	//
	//  ここに、実際のSegmentの形状がくる
	//
	//  通常、Box、Sphere、Cylinderの
	//  いずれかのノードを含むShapeノードが来る
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
