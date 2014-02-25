#include "CX3DParser.h"
#include "Test.h"

// ==========================================
//  Transformノードのフィールドを得る
// ==========================================

void Test_printFieldsOfTransformNode(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRMLファイルをパースする
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Some Field values of Transform Node in (%s) ****\n", vrmlFile);

	// ---------------------------------------------
	//  Transformノードを探す
	// ---------------------------------------------
	MFNode *pTransformNodes = parser.searchNodesFromAllChildrenOfRoot("Transform");
	if (pTransformNodes)
	{
		int n = pTransformNodes->count();

		for (int i=0; i<n; i++)
		{
			printf("(%d/%d)\n", i+1, n);

			CX3DTransformNode *pTrans = (CX3DTransformNode *)(pTransformNodes->getNode(i));

			if (pTrans)
			{
				// -------------------------
				//  フィールドの値を得る
				// -------------------------

				// translationフィールド
				SFVec3f *trans = pTrans->getTranslation();
				printf("translation : (%f, %f, %f)\n", trans->x(), trans->y(), trans->z());

				// rotationフィールド
				SFRotation *rot = pTrans->getRotation();
				printf("rotation : (%f, %f, %f, %f)\n", rot->x(), rot->y(), rot->z(), rot->rot());

				// centerフィールド
				SFVec3f *center = pTrans->getCenter();
				printf("center : (%f, %f, %f)\n", center->x(), center->y(), center->z());

				// scaleフィールド
				SFVec3f *scale = pTrans->getScale();
				printf("scale : (%f, %f, %f)\n", scale->x(), scale->y(), scale->z());

				// scaleOrientationフィールド
				SFRotation *sc = pTrans->getScaleOrientation();
				printf("scaleOrientation : (%f, %f, %f, %f)\n", sc->x(), sc->y(), sc->z(), sc->rot());
			}
		}

		delete pTransformNodes;
		pTransformNodes = NULL;
	}
}
