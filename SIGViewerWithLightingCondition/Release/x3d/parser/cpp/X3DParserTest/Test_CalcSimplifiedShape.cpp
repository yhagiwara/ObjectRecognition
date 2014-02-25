#include "CX3DParser.h"
#include "CSimplifiedShape.h"
#include "Test.h"
#include <vector>
#include <string>

// ============================================
//  Shape�m�[�h����P���`����v�Z����
// ============================================

void Test_calcSimplifiedShape(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	// ---------------------------------------------
	//  Shape�m�[�h�����o��
	//
	//  VRML�t�@�C�����̃m�[�h�K�w��
	//  �ȉ��̂悤�ɂȂ��Ă��邱�Ƃ����肵�Ă���
	//
	//  Group
	//    Transform
	//      Shape
	//        IndexedFaceSet
	// ---------------------------------------------

	// Group�m�[�h��T��
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

	// Transform�m�[�h��T��
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

	// Shape�m�[�h��T��
	MFNode *shapeNodes = pTrans->searchNodesFromDirectChildren("Shape");
	if (!shapeNodes)
	{
		fprintf(stderr, "could not found Shape node\n");
		return;
	}

	// -------------------------------------------
	//  �P���`��̃p�����[�^���v�Z����
	// -------------------------------------------

	CSimplifiedShape *ss = CSimplifiedShapeFactory::calc(shapeNodes);
	if (!ss)
	{
		fprintf(stderr, "error in calculating simplified shape\n");
		return;
	}

	// �Ԃ�l�̒P���`��̌^�ɂ���ĕ���
	switch (ss->getType())
	{
	case CSimplifiedShape::SPHERE:
		{
			// ��
			CSimplifiedSphere *sp = (CSimplifiedSphere *)ss;

			printf("** sphere **\n");
			printf("\tradius  : %f\n", sp->radius());
			printf("\tcenter  : (%f, %f, %f)\n", sp->x(), sp->y(), sp->z());
			printf("\t�c�x : %f\n", sp->hizumi());
		}
		break;

	case CSimplifiedShape::CYLINDER:
		{
			// �V�����_�[
			CSimplifiedCylinder *cy = (CSimplifiedCylinder *)ss;

			printf("** cylinder **\n");
			printf("\tradius  : %f\n", cy->radius());
			printf("\theight  : %f\n", cy->height());
			printf("\tcenter  : (%f, %f, %f)\n", cy->x(), cy->y(), cy->z());
			printf("\t�c�x : %f\n", cy->hizumi());
		}
		break;
		
	case CSimplifiedShape::BOX:
		{
			// ��
			CSimplifiedBox *bx = (CSimplifiedBox *)ss;

			printf("** box **\n");
			printf("\tp1  : (%f, %f, %f)\n", bx->x1(), bx->y1(), bx->z1());
			printf("\tp2  : (%f, %f, %f)\n", bx->x2(), bx->y2(), bx->z2());
			printf("\t�c�x : %f\n", bx->hizumi());
		}
		break;
	}

	delete ss;

	// �e�P���`�󂻂ꂼ��̘c�x�̒l��\��

	printf("�e�P���`��̘c�x\n");
	CSimplifiedSphere *sp = CSimplifiedShapeFactory::calcSphere(shapeNodes);
	if (sp)
	{
		printf("\t��         : %f\n", sp->hizumi());

		delete sp;
	}

	CSimplifiedCylinder *cy = CSimplifiedShapeFactory::calcCylinder(shapeNodes);
	if (cy)
	{
		printf("\t�V�����_�[ : %f\n", cy->hizumi());

		delete cy;
	}

	CSimplifiedBox *bx = CSimplifiedShapeFactory::calcBox(shapeNodes);
	if (bx)
	{
		printf("\t��         : %f\n", bx->hizumi());

		delete bx;
	}

	delete shapeNodes;
	delete transformNodes;
	delete groupNodes;
}
