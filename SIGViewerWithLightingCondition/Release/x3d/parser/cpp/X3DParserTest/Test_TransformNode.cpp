#include "CX3DParser.h"
#include "Test.h"

// ==========================================
//  Transform�m�[�h�̃t�B�[���h�𓾂�
// ==========================================

void Test_printFieldsOfTransformNode(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Some Field values of Transform Node in (%s) ****\n", vrmlFile);

	// ---------------------------------------------
	//  Transform�m�[�h��T��
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
				//  �t�B�[���h�̒l�𓾂�
				// -------------------------

				// translation�t�B�[���h
				SFVec3f *trans = pTrans->getTranslation();
				printf("translation : (%f, %f, %f)\n", trans->x(), trans->y(), trans->z());

				// rotation�t�B�[���h
				SFRotation *rot = pTrans->getRotation();
				printf("rotation : (%f, %f, %f, %f)\n", rot->x(), rot->y(), rot->z(), rot->rot());

				// center�t�B�[���h
				SFVec3f *center = pTrans->getCenter();
				printf("center : (%f, %f, %f)\n", center->x(), center->y(), center->z());

				// scale�t�B�[���h
				SFVec3f *scale = pTrans->getScale();
				printf("scale : (%f, %f, %f)\n", scale->x(), scale->y(), scale->z());

				// scaleOrientation�t�B�[���h
				SFRotation *sc = pTrans->getScaleOrientation();
				printf("scaleOrientation : (%f, %f, %f, %f)\n", sc->x(), sc->y(), sc->z(), sc->rot());
			}
		}

		delete pTransformNodes;
		pTransformNodes = NULL;
	}
}
