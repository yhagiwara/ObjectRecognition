/* $Id: CSimplifiedShape.cpp,v 1.3 2011-05-12 08:33:56 msi Exp $ */

#include "CSimplifiedShape.h"
#include "CX3DParser.h"
#include "CX3DField.h"
#include "CX3DNode.h"
#include "CX3DTransformNode.h"
#include "CX3DShapeNode.h"
#include "CX3DIndexedFaceSetNode.h"
#include "CX3DCoordinateNode.h"
#include <math.h>
#include <vector>

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
//	�P���`��v�Z
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

// ---------------------------------------------------------
//	������Shape��v�f�Ɏ���Transform�m�[�h�̃c���[����1�̒P���`������
// ---------------------------------------------------------
CSimplifiedSphere *CSimplifiedShapeFactory::calcSphereFromTree(MFNode *tree)
{
	return (CSimplifiedSphere *)calcFromTree(tree, CSimplifiedShape::SPHERE);
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinderFromTree(MFNode *tree)
{
	return (CSimplifiedCylinder *)calcFromTree(tree, CSimplifiedShape::CYLINDER);
}

CSimplifiedBox *CSimplifiedShapeFactory::calcBoxFromTree(MFNode *tree)
{
	return (CSimplifiedBox *)calcFromTree(tree, CSimplifiedShape::BOX);
}

CSimplifiedShape *CSimplifiedShapeFactory::calcAutoFromTree(MFNode *tree)
{
	// �P���`��̎����I��
	// 3�̒P���`��ɂ��Čv�Z���A�ł��c�x���������`���Ԃ�

	CSimplifiedShape *retValue = NULL;
	int i=-1;
	CSimplifiedSphere *sp = NULL;
	CSimplifiedCylinder *cy = NULL;
	CSimplifiedBox *bx = NULL;

	// �ł��c�x�̏������`�����肷�� (--> i)
	sp = (CSimplifiedSphere *)CSimplifiedShapeFactory::calcFromTree(tree, CSimplifiedShape::SPHERE);
	if (sp)
	{
		cy = (CSimplifiedCylinder *)CSimplifiedShapeFactory::calcFromTree(tree, CSimplifiedShape::CYLINDER);
		if (cy)
		{
			bx = (CSimplifiedBox *)CSimplifiedShapeFactory::calcFromTree(tree, CSimplifiedShape::BOX);
			if (bx)
			{
				float minHizumi = sp->hizumi();
				i = 0;

				if (cy->hizumi() < minHizumi)
				{
					minHizumi = cy->hizumi();
					i = 1;
				}

				if (bx->hizumi() < minHizumi)
				{
					i = 2;
				}
			}
		}
	}

	// �Ԃ�l�̏���
	switch (i)
	{
	case 0:
		retValue = (CSimplifiedShape *)sp;
		sp = NULL;
		break;

	case 1:
		retValue = (CSimplifiedShape *)cy;
		cy = NULL;
		break;

	case 2:
		retValue = (CSimplifiedShape *)bx;
		bx = NULL;
		break;
	}

	// �s�v�ɂȂ����I�u�W�F�N�g�͏���
	if (sp) delete sp;
	if (cy) delete cy;
	if (bx) delete bx;

	return retValue;
}

CSimplifiedShape *CSimplifiedShapeFactory::calcFromTree(MFNode *tree, CSimplifiedShape::SHAPE_TYPE hint)
{
	if (!tree || tree->count() == 0) return NULL;

	if (hint == CSimplifiedShape::NONE)
	{
		// �q���g���Ȃ�(=�����I��)�̏ꍇ
		return calcAutoFromTree(tree);
	}

	// �q���g������ꍇ
	std::vector<SFVec3f> vecPos;
	SFVec3f t;
	SFVec4f q;

	// tree�ȉ��̒��_�����ׂĎ��o��vecPos�Ɋi�[����
	vecPos.clear();	// �����Ɍ��ʂ�����
	t.setValue(0, 0, 0);
	q.setValue(1, 0, 0, 0);
	int nTrans = tree->count();
	for (int i=0; i<nTrans; i++)
	{
		CX3DTransformNode *pTrans = (CX3DTransformNode *)(tree->getNode(i));
		if (!pTrans) continue;

		extractPointsFromTree(pTrans, t, q, vecPos);
	}

	// �P���`��𐶐�����
	switch (hint)
	{
	case CSimplifiedShape::SPHERE:
		return (CSimplifiedShape *)calcSphere(vecPos);

	case CSimplifiedShape::CYLINDER:
		return (CSimplifiedShape *)calcCylinder(vecPos);

	case CSimplifiedShape::BOX:
		return (CSimplifiedShape *)calcBox(vecPos);

	default:
		return NULL;
	}
}

// ------------------------------------------
//	 ������Shape����P���`������߂�
// ------------------------------------------

CSimplifiedSphere *CSimplifiedShapeFactory::calcSphereFromShapeNodes(MFNode *shapes)
{
	return (CSimplifiedSphere *)calcFromShapeNodes(shapes, CSimplifiedShape::SPHERE);
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinderFromShapeNodes(MFNode *shapes)
{
	return (CSimplifiedCylinder *)calcFromShapeNodes(shapes, CSimplifiedShape::CYLINDER);
}

CSimplifiedBox *CSimplifiedShapeFactory::calcBoxFromShapeNodes(MFNode *shapes)
{
	return (CSimplifiedBox *)calcFromShapeNodes(shapes, CSimplifiedShape::BOX);
}

CSimplifiedShape *CSimplifiedShapeFactory::calcAutoFromShapeNodes(MFNode *shapes)
{
	// �P���`��̎����I��
	// 3�̒P���`��ɂ��Čv�Z���A�ł��c�x���������`���Ԃ�

	CSimplifiedShape *retValue = NULL;
	int i=-1;
	CSimplifiedSphere *sp = NULL;
	CSimplifiedCylinder *cy = NULL;
	CSimplifiedBox *bx = NULL;

	// �ł��c�x�̏������`�����肷�� (--> i)
	sp = (CSimplifiedSphere *)CSimplifiedShapeFactory::calcFromShapeNodes(shapes, CSimplifiedShape::BOX);
	if (sp)
	{
		cy = (CSimplifiedCylinder *)CSimplifiedShapeFactory::calcFromShapeNodes(shapes, CSimplifiedShape::CYLINDER);
		if (cy)
		{
			bx = (CSimplifiedBox *)CSimplifiedShapeFactory::calcFromShapeNodes(shapes, CSimplifiedShape::BOX);
			if (bx)
			{
				float minHizumi = sp->hizumi();
				i = 0;

				if (cy->hizumi() < minHizumi)
				{
					minHizumi = cy->hizumi();
					i = 1;
				}

				if (bx->hizumi() < minHizumi)
				{
					i = 2;
				}
			}
		}
	}

	// �Ԃ�l�̏���
	switch (i)
	{
	case 0:
		retValue = (CSimplifiedShape *)sp;
		sp = NULL;
		break;

	case 1:
		retValue = (CSimplifiedShape *)cy;
		cy = NULL;
		break;

	case 2:
		retValue = (CSimplifiedShape *)bx;
		bx = NULL;
		break;
	}

	// �s�v�ɂȂ����I�u�W�F�N�g�͏���
	if (sp) delete sp;
	if (cy) delete cy;
	if (bx) delete bx;

	return retValue;
}

CSimplifiedShape *CSimplifiedShapeFactory::calcFromShapeNodes(MFNode *shapes, CSimplifiedShape::SHAPE_TYPE hint)
{
	if (!shapes || shapes->count() == 0) return NULL;

	if (hint == CSimplifiedShape::NONE)
	{
		// �q���g���Ȃ�(=�����I��)�̏ꍇ
		return calcAutoFromShapeNodes(shapes);
	}

	// �q���g������ꍇ
	std::vector<SFVec3f> vecPos;

	extractPointsFromShapeNodes(shapes, vecPos);

	switch (hint)
	{
	case CSimplifiedShape::SPHERE:
		return calcSphere(vecPos);

	case CSimplifiedShape::CYLINDER:
		return calcCylinder(vecPos);

	case CSimplifiedShape::BOX:
		return calcBox(vecPos);

	default:
		return NULL;	// �����ɂ͗��Ȃ��͂�
	}
}

// ------------------------------------------
//	 1��Shape����P���`������߂�
// ------------------------------------------

CSimplifiedSphere *CSimplifiedShapeFactory::calcSphereFromShapeNode(CX3DShapeNode *pShape)
{
	return (CSimplifiedSphere *)calcFromShapeNode(pShape, CSimplifiedShape::SPHERE);
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinderFromShapeNode(CX3DShapeNode *pShape)
{
	return (CSimplifiedCylinder *)calcFromShapeNode(pShape, CSimplifiedShape::CYLINDER);
}

CSimplifiedBox *CSimplifiedShapeFactory::calcBoxFromShapeNode(CX3DShapeNode *pShape)
{
	return (CSimplifiedBox *)calcFromShapeNode(pShape, CSimplifiedShape::BOX);
}

CSimplifiedShape *CSimplifiedShapeFactory::calcAutoFromShapeNode(CX3DShapeNode *pShape)
{
	if (!pShape) return NULL;

	// �P���`��̎����I��
	// 3�̒P���`��ɂ��Čv�Z���A�ł��c�x���������`���Ԃ�

	CSimplifiedShape *retValue = NULL;
	int i=-1;
	CSimplifiedSphere *sp = NULL;
	CSimplifiedCylinder *cy = NULL;
	CSimplifiedBox *bx = NULL;

	// �ł��c�x�̏������`�����肷�� (--> i)
	sp = (CSimplifiedSphere *)CSimplifiedShapeFactory::calcFromShapeNode(pShape, CSimplifiedShape::BOX);
	if (sp)
	{
		cy = (CSimplifiedCylinder *)CSimplifiedShapeFactory::calcFromShapeNode(pShape, CSimplifiedShape::CYLINDER);
		if (cy)
		{
			bx = (CSimplifiedBox *)CSimplifiedShapeFactory::calcFromShapeNode(pShape, CSimplifiedShape::BOX);
			if (bx)
			{
				float minHizumi = sp->hizumi();
				i = 0;

				if (cy->hizumi() < minHizumi)
				{
					minHizumi = cy->hizumi();
					i = 1;
				}

				if (bx->hizumi() < minHizumi)
				{
					i = 2;
				}
			}
		}
	}

	// �Ԃ�l�̏���
	switch (i)
	{
	case 0:
		retValue = (CSimplifiedShape *)sp;
		sp = NULL;
		break;

	case 1:
		retValue = (CSimplifiedShape *)cy;
		cy = NULL;
		break;

	case 2:
		retValue = (CSimplifiedShape *)bx;
		bx = NULL;
		break;
	}

	// �s�v�ɂȂ����I�u�W�F�N�g�͏���
	if (sp) delete sp;
	if (cy) delete cy;
	if (bx) delete bx;

	return retValue;
}

CSimplifiedShape *CSimplifiedShapeFactory::calcFromShapeNode(CX3DShapeNode *pShape, CSimplifiedShape::SHAPE_TYPE hint)
{
	if (!pShape) return NULL;

	if (hint == CSimplifiedShape::NONE)
	{
		// �q���g���Ȃ�(=�����I��)�̏ꍇ
		return calcAutoFromShapeNode(pShape);
	}

	// �q���g������ꍇ
	std::vector<SFVec3f> vecPos;

	extractPointsFromShapeNode(pShape, vecPos);

	switch (hint)
	{
	case CSimplifiedShape::SPHERE:
		return calcSphere(vecPos);

	case CSimplifiedShape::CYLINDER:
		return calcCylinder(vecPos);

	case CSimplifiedShape::BOX:
		return calcBox(vecPos);

	default:
		return NULL;	// �����ɂ͗��Ȃ��͂�
	}
}

// -----------------------------------------------
//	 1��IndexedFaceSetNode����P���`������߂�
// -----------------------------------------------

CSimplifiedSphere *CSimplifiedShapeFactory::calcSphere(CX3DIndexedFaceSetNode *pFaceSet)
{
	std::vector<SFVec3f> vecPos;

	extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);

	return calcSphere(vecPos);
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinder(CX3DIndexedFaceSetNode *pFaceSet)
{
	std::vector<SFVec3f> vecPos;

	extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);

	return calcCylinder(vecPos);
}

CSimplifiedBox *CSimplifiedShapeFactory::calcBox(CX3DIndexedFaceSetNode *pFaceSet)
{
	std::vector<SFVec3f> vecPos;

	extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);

	return calcBox(vecPos);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
//	�m�[�h����_�̏W���𒊏o����
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

void CSimplifiedShapeFactory::extractPointsFromTree(CX3DTransformNode *pTrans, SFVec3f t, SFVec4f q, std::vector<SFVec3f>& vecPos)
{
	if (!pTrans) return;

	MFNode *transNodes = pTrans->searchNodesFromDirectChildren("Transform");
	if (transNodes)
	{
		int nTrans = transNodes->count();
		for (int i=0; i<nTrans; i++)
		{
			CX3DTransformNode *pChildTrans = (CX3DTransformNode *)(transNodes->getNode(i));
			if (!pChildTrans) continue;

			// *********************
			//	������Œ���
			// *********************
			SFVec3f newT = t;
			SFVec4f newQ = q;

			extractPointsFromTree(pChildTrans, newT, newQ, vecPos);
		}

		delete transNodes;	// ���ꂪ�K�v
		transNodes = NULL;
	}

	MFNode *shapeNodes = pTrans->searchNodesFromDirectChildren("Shape");
	if (shapeNodes)
	{
		int nShapes = shapeNodes->count();
		for (int i=0; i<nShapes; i++)
		{
			CX3DShapeNode *pChildShape = (CX3DShapeNode *)(shapeNodes->getNode(i));
			if (!pChildShape) continue;

			extractPointsFromShapeNode(pChildShape, vecPos);
		}

		delete shapeNodes;	// ���ꂪ�K�v
		shapeNodes = NULL;
	}

	return;
}

void CSimplifiedShapeFactory::extractPointsFromShapeNodes(MFNode *shapeNodes, std::vector<SFVec3f>& vecPos)
{
	if (!shapeNodes) return;

	// MFNode�̎��m�[�h�̐�(=Shape�m�[�h�̐��j
	int nShape = shapeNodes->count();
	if (nShape <= 0) return;

	for (int iShape=0; iShape<nShape; iShape++)
	{
		CX3DShapeNode *pShape = (CX3DShapeNode *)(shapeNodes->getNode(iShape));
		if (!pShape) continue;

		extractPointsFromShapeNode(pShape, vecPos);
	}
}

void CSimplifiedShapeFactory::extractPointsFromShapeNode(CX3DShapeNode *pShape, std::vector<SFVec3f>& vecPos)
{
	if (!pShape) return;

	// -------------------------------------------
	//	IndexedFaceSet�m�[�h�����o��
	// -------------------------------------------
	CX3DNode *pNode = (CX3DIndexedFaceSetNode *)(pShape->getGeometry()->getNode());
	if (pNode && (pNode->getNodeType() == INDEXED_FACE_SET_NODE))
	{
		CX3DIndexedFaceSetNode *pFaceSet = (CX3DIndexedFaceSetNode *)pNode;

		extractPointsFromIndexedFaceSetNode(pFaceSet, vecPos);
	}
}

void CSimplifiedShapeFactory::extractPointsFromIndexedFaceSetNode(CX3DIndexedFaceSetNode *pFaceSet, std::vector<SFVec3f>& vecPos)
{
	if (!pFaceSet) return;

	CX3DCoordinateNode *pCoord = (CX3DCoordinateNode *)(pFaceSet->getCoord()->getNode());
	if (pCoord)
	{
		// ���_���W
		MFVec3f *coords = pCoord->getPoint();

		// ���_�C���f�b�N�X
		MFInt32 *coordIndex = pFaceSet->getCoordIndex();
		bool bHasCoordIndex = (coordIndex->count() > 0) ? true : false;

		int n = bHasCoordIndex ? coordIndex->count() : coords->count();

		// ���_�����o��
		for (int i=0; i<n; i++)
		{
			SFVec3f pos;

			// ���_���W --> pos
			if (bHasCoordIndex)
			{
				int ind = coordIndex->getValue(i);
				if (ind < 0) continue;

				pos = coords->getValue(ind);
			}
			else
			{
				pos = coords->getValue(i);
			}

			vecPos.push_back(pos);
		}
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
//	�P���`��v�Z�̍�������
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

// ------------------------------------------
//	��
// ------------------------------------------

CSimplifiedSphere *CSimplifiedShapeFactory::calcSphere(std::vector<SFVec3f>& vecPos)
{
	CSimplifiedSphere *retValue = new CSimplifiedSphere();
	if (!retValue) return NULL;

	int n = (int)(vecPos.size());
	if (n <= 0) return NULL;

	// �d�S���W�Z�o
	float gx = 0;
	float gy = 0;
	float gz = 0;

	for (int i=0; i<n; i++)
	{
		SFVec3f pos = vecPos[i];

		gx += pos.x();
		gy += pos.y();
		gz += pos.z();
	}

	gx /= n;
	gy /= n;
	gz /= n;

	// �d�S����̋������e�_�ɂ��ĎZ�o
	// �ł���������(=fr)�ƁA�ł��߂�����(=nr)�A���ϋ���(=ar)�����߂�
	float fr = 0;
	float nr = 0;
	float ar = 0;
	bool bFirst = true;

	for (int i=0; i<n; i++)
	{
		SFVec3f pos = vecPos[i];

		if (bFirst)
		{
			fr = nr = sqrDist(gx, gy, gz, pos.x(), pos.y(), pos.z());
			bFirst = false;
		}
		else
		{
			float d = sqrDist(gx, gy, gz, pos.x(), pos.y(), pos.z());

			if (d > fr) fr = d;
			if (d < nr) nr = d;
		}
	}

	fr = sqrt(fr);
	nr = sqrt(nr);
	ar = (nr + fr)/2;

	// r�ɑ΂���u�Ђ��݋�v���v�Z����
	// �Ђ��݋�Ƃ́A�`��̊e�_�ɂ�����d�S����̋���pr��
	// r�Ƃ̍��̓��a�̂��Ƃł���D
	// ���ar�̋��ɋ߂��`�قǒl���������Ȃ�A������`�����������قǑ傫���Ȃ�
	float hizumi = CSimplifiedShapeFactory::calcHizumiWithSphere(ar, gx, gy, gz, vecPos);

	// �Ԃ�l�ɃZ�b�g
	retValue->x(gx);
	retValue->y(gy);
	retValue->z(gz);
	retValue->radius(ar);
	retValue->hizumi(hizumi);

	return retValue;
}

double CSimplifiedShapeFactory::calcHizumiWithSphere(float r, float gx, float gy, float gz, std::vector<SFVec3f>& vecPos)
{
	int n = (int)(vecPos.size());
	if (n>0)
	{
		double hizumiSum = 0;

		for (int i=0; i<n; i++)
		{
			SFVec3f pos = vecPos[i];

			// �d�S����pos�܂ł̋���
			float d = sqrt(sqrDist(gx, gy, gz, pos.x(), pos.y(), pos.z()));

			// ���a�Ƃ̍��̓��a
			hizumiSum = hizumiSum + (d-r)*(d-r);
		}

		return hizumiSum/n;
	}
	else
	{
		return 0;
	}
}

// ------------------------------------------
//	�V�����_�[
//
//	*** ���� ***
//	�ǂ����faceSet�̒��̍��W�n�́A
//
//	��ʂ�(x, z)
//	��i����j������y
//
//	�ɂȂ��Ă���炵���D
//	�iCG�̐��E�ł̕��ʂ̉E��n���W�j
//
//	��W�搶�̌����Ă����A�u�V�~�����[�V�����̐��E�ł�z������������v�Ƃ͈قȂ�̂Œ���
// ------------------------------------------

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinder(std::vector<SFVec3f>& vecPos)
{
	CSimplifiedCylinder *retValue = new CSimplifiedCylinder();
	if (!retValue) return NULL;

	int n = (int)(vecPos.size());
	if (n <= 0) return NULL;

	// �d�S���W�Z�o
	float gx = 0;
	float gy = 0;
	float gz = 0;

	for (int i=0; i<n; i++)
	{
		SFVec3f pos = vecPos[i];

		gx += pos.x();
		gy += pos.y();
		gz += pos.z();
	}

	gx /= n;
	gy /= n;
	gz /= n;

	// ���S������ł����ꂽ�_�ւ̋���(=fr)�A�ł��߂��_�ւ̋���(=nr)
	// ����т��̕���(=ar)�����߂�
	// ����ɃV�����_�[�̔��a��ar�ɂ���
	float fr = 0;
	float nr = 0;
	float ar = 0;
	bool bFirst = true;

	for (int i=0; i<n; i++)
	{
		SFVec3f pos = vecPos[i];

		if (bFirst)
		{
			fr = nr = sqrDist(gx, 0, gz, pos.x(), 0, pos.z());
			bFirst = false;
		}
		else
		{
			float d = sqrDist(gx, 0, gz, pos.x(), 0, pos.z());

			if (d > fr) fr = d;
			if (d < nr) nr = d;
		}
	}

	fr = sqrt(fr);
	nr = sqrt(nr);
	ar = (nr + fr)/2;

	// �V�����_�[�̍���=y���W�̍ő�l - y���W�̍ŏ��l
	float maxy, miny;
	bFirst = true;
	for (int i=0; i<n; i++)
	{
		SFVec3f pos = vecPos[i];

		if (bFirst)
		{
			maxy = miny = pos.y();
			bFirst = false;
		}
		else
		{
			float d = pos.y();

			if (d > maxy) maxy = d;
			if (d < miny) miny = d;
		}
	}

	// r�ɑ΂���u�Ђ��݋�v���v�Z����
	// �Ђ��݋�Ƃ́A�`��̊e�_�ɂ�����d�S����̋���pr��
	// r�Ƃ̍��̓��a�̂��Ƃł���D
	// ���ar�̋��ɋ߂��`�قǒl���������Ȃ�A������`�����������قǑ傫���Ȃ�
	float hizumi = calcHizumiWithCylinder(ar, gx, gz, vecPos);

	// �Ԃ�l�ɃZ�b�g
	retValue->x(gx);
	retValue->y(gy);
	retValue->z(gz);
	retValue->radius(ar);
	retValue->height(maxy - miny);
	retValue->hizumi(hizumi);

	return retValue;
}

CSimplifiedCylinder *CSimplifiedShapeFactory::calcCylinder(CX3DCylinderNode *pCylinderNode)
{
	int i;
	printf("\t\t[Start calcCylinder]\n");

	if (!pCylinderNode) return NULL;

	CSimplifiedCylinder *retValue = new CSimplifiedCylinder();
	if (!retValue) return NULL;

	//	�d�S�̍��W
	float gx,gy,gz;

	//	�d�S�����_�ɐݒ�
	gx = 0.0f;
	gy = 0.0f;
	gz = 0.0f;

	//	���a
	float radius = pCylinderNode->getRadius()->getValue();
	//	����
	float height = pCylinderNode->getHeight()->getValue();

	//	�Ђ��݂͖������̂Ƃ���
	float hizumi = 0.0f;

	// �Ԃ�l�ɃZ�b�g
	retValue->x(gx);
	retValue->y(gy);
	retValue->z(gz);
	retValue->radius(radius);
	retValue->height(height);
	retValue->hizumi(hizumi);

	return retValue;
}

/**
 * �w��`����V�����_�Ƃ݂Ȃ��Ă����Ȃ����ǂ����̔���
 * @param pNodeData
 * @param radiusEstimated �V�����_�`��Ȃ�΁A���a���i�[�����
 * @param heightEstimated �V�����_�`��Ȃ�΁A�������i�[�����
 * @param xAxis[]		  �V�����_�̎��̍��W���i�[�����(xAxis[0]:�\,xAxis[1]:��)
 * @param yAxis[]		  �V�����_�̎��̍��W���i�[�����(yAxis[0]:�\,yAxis[1]:��)
 * @param zAxis[]		  �V�����_�̎��̍��W���i�[�����(zAxis[0]:�\,zAxis[1]:��)
 * @return �V�����_�`��Ƃ݂Ȃ��Ă����Ȃ����true��Ԃ�
 */
bool CSimplifiedShapeFactory::isCylinder(
		CX3DIndexedFaceSetNode *pNodeData,
		float &radiusEstimated,
		float &heightEstimated,
		float xAxis[],
		float yAxis[],
		float zAxis[]
	)
{
	int i, n;
	bool result = false;

// FIX20110422(sekikawa)
//	if (!pNodeData) return NULL;
	if (!pNodeData) return false;

	CX3DCoordinateNode *pCoord = (CX3DCoordinateNode *)(pNodeData->getCoord()->getNode());

// FIX20110422(sekikawa)
//	if (!pCoord) return NULL;
	if (!pCoord) return false;

	// ���_���W
	MFVec3f *coords = pCoord->getPoint();

	//	�d�S���W
	float gx,gy,gz;
	//	���_��
	n = coords->count();

	//	���_�������Ȃ��ꍇ�́A�L���[�u�`��̕������߂��Ƃ݂Ȃ�
	if(n < 14) return false;

	getCenter(coords,gx,gy,gz);	//	�d�S�����߂�

	//
	//	�V�����_�[�̎������߂�B
	//	�����V�����_�[�Ȃ�΁A�d�S��������Ƃ��߂�2�_�̒��_�����ɂȂ�͂�
	//
	float x_a[2],y_a[2],z_a[2];	//	�d�S����߂�2�_�̍��W
	getNearPoint(coords,gx,gy,gz,x_a,y_a,z_a,2);

	//	������̋��������߂�
	//	(x[0],y[0],z[0]) or (x[1],y[1],z[1])�̂ǂ��炩�߂��������ɂȂ��Ă���͂�
	float distance = 0.0f;
	int vertex = 0;	//	���ȊO�̒��_��
	//printf("Vertex Num : %d\n",n);
	for (i=0; i<n; i++) {
		SFVec3f pos;
		// ���_���W --> pos
		pos = coords->getValue(i);

		float d1,d2,d;

		d1 = getDistance(pos.x(),pos.y(),pos.z(),x_a[0],y_a[0],z_a[0]);
		d2 = getDistance(pos.x(),pos.y(),pos.z(),x_a[1],y_a[1],z_a[1]);
		d = ( (d1 > d2) ? d2 : d1 );

		distance += d;
		//printf("%f,%f,%f,%f\n",pos.x(),pos.y(),pos.z(),d);
		if(d > SMALL_VALUE) {
			vertex++;
		}
	}

	//	�d�S����̋����̊T�Z�l
	if(vertex > 0) {
		distance /= (float)vertex;
	}
	//printf("distance = %f\n",distance);

	//	�V�����_�[�̔��a�̐���l
	float radius = distance;
	result = true;
	for (i=0; i<n; i++) {
		SFVec3f pos;
		// ���_���W --> pos
		pos = coords->getValue(i);

		float d1,d2,d;

		d1 = getDistance(pos.x(),pos.y(),pos.z(),x_a[0],y_a[0],z_a[0]);
		d2 = getDistance(pos.x(),pos.y(),pos.z(),x_a[1],y_a[1],z_a[1]);
		d = ( (d1 > d2) ? d2 : d1 );

		//	���a����傫������Ă���f�[�^������΃V�����_�[�ł͂Ȃ��͂�
		if(d > SMALL_VALUE && (d - radius > SLIGHT_SMALL_VALUE)) {	//	�����̂��͔̂��肩�珜�O����
			return false;
		}
	}

	//	�V�����_�[�`��ł���Ɣ��肳�ꂽ
	radiusEstimated = radius;
	heightEstimated = getDistance(x_a[0],y_a[0],z_a[0],x_a[1],y_a[1],z_a[1]);

	xAxis[0] = x_a[0];
	xAxis[1] = x_a[1];
	yAxis[0] = y_a[0];
	yAxis[1] = y_a[1];
	zAxis[0] = z_a[0];
	zAxis[1] = z_a[1];

	return result;
}

double CSimplifiedShapeFactory::calcHizumiWithCylinder(float r, float cx, float cz, std::vector<SFVec3f>& vecPos)
{
	int n = (int)(vecPos.size());

	if (n>0)
	{
		double hizumiSum = 0;

		for (int i=0; i<n; i++)
		{
			SFVec3f pos = vecPos[i];

			// �V�����_�[�̎�����pos�܂ł̋���
			float d = sqrt(sqrDist(cx, 0, cz, pos.x(), 0, pos.z()));

			// ���a�Ƃ̍��̓��a
			hizumiSum = hizumiSum + (d-r)*(d-r);
		}

		return hizumiSum/n;
	}
	else
	{
		return 0;
	}
}

// ------------------------------------------
//	��
// ------------------------------------------

CSimplifiedBox *CSimplifiedShapeFactory::calcBox(std::vector<SFVec3f>& vecPos)
{
	CSimplifiedBox *retValue = new CSimplifiedBox();
	if (!retValue) return NULL;

	int n = (int)(vecPos.size());
	if (n <= 0) return NULL;

	// (x, y, z)�̊e���ɂ����āA�ŏ��l�ƍő�l�����߂�D
	// �ŏ��l�̓_=(x1, y1, z1)
	// �ő�l�̓_=(x2, y2, z2)
	// �Ƃ���D
	// ����ɂ��A(x1, y1, z1)-(x2, y2, z2)��Ίp���Ƃ��钼���̂�
	// �o�E���f�B���O�{�b�N�X�ƂȂ�D
	float x1, y1, z1, x2, y2, z2;

	bool bFirst = true;
	for (int i=0; i<n; i++)
	{
		SFVec3f pos = vecPos[i];

		if (bFirst)
		{
			x1 = x2 = pos.x();
			y1 = y2 = pos.y();
			z1 = z2 = pos.z();

			bFirst = false;
		}
		else
		{
			float x = pos.x();
			float y = pos.y();
			float z = pos.z();

			if (x < x1) x1 = x;
			if (x > x2) x2 = x;

			if (y < y1) y1 = y;
			if (y > y2) y2 = y;

			if (z < z1) z1 = z;
			if (z > z2) z2 = z;
		}
	}

	float hizumi = calcHizumiWithBox(x1, y1, z1, x2, y2, z2, vecPos);

	// �Ԃ�l�ɃZ�b�g
	retValue->x1(x1);
	retValue->y1(y1);
	retValue->z1(z1);
	retValue->x2(x2);
	retValue->y2(y2);
	retValue->z2(z2);

	retValue->hizumi(hizumi);

	return retValue;
}

double CSimplifiedShapeFactory::calcHizumiWithBox(float x1, float y1, float z1, float x2, float y2, float z2, std::vector<SFVec3f>& vecPos)
{
	float dist[6];
	float d;

	int n = (int)(vecPos.size());
	if (n>0)
	{
		double hizumiSum = 0;

		for (int i=0; i<n; i++)
		{
			SFVec3f pos = vecPos[i];

			// �o�E���f�B���O�{�b�N�X��6�̖ʂւ̋���
			dist[0] = pos.x() - x1;
			dist[1] = x2 - pos.x();
			dist[2] = pos.y() - y1;
			dist[3] = y2 - pos.y();
			dist[4] = pos.z() - z1;
			dist[5] = z2 - pos.z();

			// �ʂ܂ł̋����̍ŏ��l�𓾂�
			d = min6Float(dist);

			// �ŏ��l�̓��a���v�Z
			hizumiSum = hizumiSum + d*d;
		}

		return hizumiSum/n;
	}
	else
	{
		return 0;
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++
//	�w���p�[�֐�
// ++++++++++++++++++++++++++++++++++++++++++++++++++++

float CSimplifiedShapeFactory::min6Float(float *f)
{
	float minValue;

	for (int i=0; i<6; i++)
	{
		float x = f[i];

		if (i==0)
		{
			minValue = x;
		}
		else
		{
			if (x < minValue) minValue = x;
		}
	}

	return minValue;
}

float CSimplifiedShapeFactory::sqrDist(float px, float py, float pz, float qx, float qy, float qz)
{
	return (px-qx)*(px-qx) + (py-qy)*(py-qy) + (pz-qz)*(pz-qz);
}

/**
 * 2�_�Ԃ̋�����Ԃ�
 */
float CSimplifiedShapeFactory::getDistance(float px, float py, float pz, float qx, float qy, float qz){
	return (float)sqrt(sqrDist(px,py,pz,qx,qy,qz));
}


/**
 * �d�S�̍��W�����߂܂�
 * @param coords ���_���W�Z�b�g
 * @param gx �d�S��x���W���i�[�����
 * @param gy �d�S��y���W���i�[�����
 * @param gz �d�S��z���W���i�[�����
 */
void CSimplifiedShapeFactory::getCenter(MFVec3f *coords,float &gx,float &gy,float &gz){

	int i, n;
	//	���_��
	n = coords->count();
	gx = 0.0f;
	gy = 0.0f;
	gz = 0.0f;

	bool bFirst = true;
	for (i=0; i<n; i++)
	{
		SFVec3f pos;
		// ���_���W --> pos
		pos = coords->getValue(i);

		gx += pos.x();
		gy += pos.y();
		gz += pos.z();
	}

	if(n > 0) {
		gx /= (float)n;
		gy /= (float)n;
		gz /= (float)n;
	}
	//printf("gx = %f,gy = %f,gz = %f\n",gx,gy,gz);

}



/**
 * (gx,gy,gz)����߂��_��T���Ċi�[���܂��B
 * @param coords  ���_���W���X�g
 * @param gx	  �w��_��x���W
 * @param gy	  �w��_��y���W
 * @param gz	  �w��_��z���W
 * @param destX[] �w��_�ɋ߂��_�̍��W���z��Ŋi�[�����
 * @param destY[] �w��_�ɋ߂��_�̍��W���z��Ŋi�[�����
 * @param destZ[] �w��_�ɋ߂��_�̍��W���z��Ŋi�[�����
 * @param num	  (gx,gy,gz)�ɋ߂��_�������܂ŒT����
 */
void CSimplifiedShapeFactory::getNearPoint(MFVec3f *coords,float gx,float gy,float gz,float destX[],float destY[],float destZ[],int num){


	//
	//	�V�����_�[�̎������߂�B
	//	�����V�����_�[�Ȃ�΁A�d�S��������Ƃ��߂�2�_�̒��_�����ɂȂ�͂�
	//
	float distMin = INVALID_VALUE;

	for(int n=0;n<num;n++) {
		destX[n] = INVALID_VALUE;
		destY[n] = INVALID_VALUE;
		destZ[n] = INVALID_VALUE;
	}

	for (int i=0; i < coords->count() ; i++) {
		SFVec3f pos;
		// ���_���W --> pos
		pos = coords->getValue(i);

		float d = getDistance(pos.x(),pos.y(),pos.z(),gx,gy,gz);
		if( distMin >= d ) {
			distMin = d;
			for(int n=num-1;n>0;n--) {
				destX[n] = destX[n-1];
				destY[n] = destY[n-1];
				destZ[n] = destZ[n-1];
			}
			destX[0] = pos.x();
			destY[0] = pos.y();
			destZ[0] = pos.z();
		}
	}
	//printf("///\n");
	//printf("%f,%f,%f,,,,,\n",destX[0],destY[0],destZ[0]);
	//printf("%f,%f,%f,,,,,\n",destX[1],destY[1],destZ[1]);

}
