#include "CX3DParser.h"
#include "CSimplifiedShape.h"
#include "Test.h"
#include <vector>
#include <string>

// ============================================
//  Shapeノードから単純形状を計算する
// ============================================

void Test_calcSimplifiedShape(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRMLファイルをパースする
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	// ---------------------------------------------
	//  Shapeノードを取り出す
	//
	//  VRMLファイル内のノード階層が
	//  以下のようになっていることを仮定している
	//
	//  Group
	//    Transform
	//      Shape
	//        IndexedFaceSet
	// ---------------------------------------------

	// Groupノードを探す
	MFNode *groupNodes = parser.searchNodesFromDirectChildrenOfRoot("Group");
	if (!groupNodes)
	{
		fprintf(stderr, "could not found root group node in %s\n", vrmlFile);
		return;
	}

	CX3DGroupNode *pGroup = (CX3DGroupNode *)(groupNodes->getNode(0));
	if (!pGroup)
	{
		fprintf(stderr, "root group is NULL\n");
		return;
	}

	// Transformノードを探す
	MFNode *transformNodes = pGroup->searchNodesFromDirectChildren("Transform");
	if (!transformNodes)
	{
		fprintf(stderr, "could not found Transform node\n");
		return;
	}

	CX3DTransformNode *pTrans = (CX3DTransformNode *)(transformNodes->getNode(0));
	if (!pTrans)
	{
		fprintf(stderr, "Transform node is NULL\n");
		return;
	}

	// Shapeノードを探す
	MFNode *shapeNodes = pTrans->searchNodesFromDirectChildren("Shape");
	if (!shapeNodes)
	{
		fprintf(stderr, "could not found Shape node\n");
		return;
	}

	// -------------------------------------------
	//  単純形状のパラメータを計算する
	// -------------------------------------------

	CSimplifiedShape *ss = CSimplifiedShapeFactory::calc(shapeNodes);
	if (!ss)
	{
		fprintf(stderr, "error in calculating simplified shape\n");
		return;
	}

	// 返り値の単純形状の型によって分岐
	switch (ss->getType())
	{
	case CSimplifiedShape::SPHERE:
		{
			// 球
			CSimplifiedSphere *sp = (CSimplifiedSphere *)ss;

			printf("** sphere **\n");
			printf("\tradius  : %f\n", sp->radius());
			printf("\tcenter  : (%f, %f, %f)\n", sp->x(), sp->y(), sp->z());
			printf("\t歪度 : %f\n", sp->hizumi());
		}
		break;

	case CSimplifiedShape::CYLINDER:
		{
			// シリンダー
			CSimplifiedCylinder *cy = (CSimplifiedCylinder *)ss;

			printf("** cylinder **\n");
			printf("\tradius  : %f\n", cy->radius());
			printf("\theight  : %f\n", cy->height());
			printf("\tcenter  : (%f, %f, %f)\n", cy->x(), cy->y(), cy->z());
			printf("\t歪度 : %f\n", cy->hizumi());
		}
		break;
		
	case CSimplifiedShape::BOX:
		{
			// 箱
			CSimplifiedBox *bx = (CSimplifiedBox *)ss;

			printf("** box **\n");
			printf("\tp1  : (%f, %f, %f)\n", bx->x1(), bx->y1(), bx->z1());
			printf("\tp2  : (%f, %f, %f)\n", bx->x2(), bx->y2(), bx->z2());
			printf("\t歪度 : %f\n", bx->hizumi());
		}
		break;
	}

	delete ss;

	// 各単純形状それぞれの歪度の値を表示

	printf("各単純形状の歪度\n");
	CSimplifiedSphere *sp = CSimplifiedShapeFactory::calcSphere(shapeNodes);
	if (sp)
	{
		printf("\t球         : %f\n", sp->hizumi());

		delete sp;
	}

	CSimplifiedCylinder *cy = CSimplifiedShapeFactory::calcCylinder(shapeNodes);
	if (cy)
	{
		printf("\tシリンダー : %f\n", cy->hizumi());

		delete cy;
	}

	CSimplifiedBox *bx = CSimplifiedShapeFactory::calcBox(shapeNodes);
	if (bx)
	{
		printf("\t箱         : %f\n", bx->hizumi());

		delete bx;
	}

	delete shapeNodes;
	delete transformNodes;
	delete groupNodes;
}
