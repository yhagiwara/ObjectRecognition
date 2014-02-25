/* $Id: CSimplifiedShape.h,v 1.3 2011-05-12 08:33:56 msi Exp $ */

#ifndef _CSIMPLIFIED_SHAPE_H_
#define _CSIMPLIFIED_SHAPE_H_

#include "CX3DNode.h"
#include "CX3DIndexedFaceSetNode.h"
#include "CX3DTransformNode.h"
#include "CX3DShapeNode.h"
#include "CX3DCylinderNode.h"
#include <float.h>
#include <vector>

//////////////////////////////////////////////////////////////////
//
//	�P���`���\�����߂̃x�[�X�N���X

class CSimplifiedShape
{
public:
	enum SHAPE_TYPE {
		NONE,
		SPHERE,
		CYLINDER,
		BOX
	};

	CSimplifiedShape() {}
	virtual ~CSimplifiedShape() {}

	// �P���`��̌^�i���A�V�����_�[�A���j��Ԃ�
	virtual SHAPE_TYPE getType() = 0;

	// �c�x
	float hizumi() { return m_hizumi; }
	void hizumi(float hizumi) { m_hizumi = hizumi; }

protected:
	// �c�x
	float m_hizumi;
};


//////////////////////////////////////////////////////////////////
//
//	��

class CSimplifiedSphere : public CSimplifiedShape
{
public:
	CSimplifiedSphere() {}
	virtual ~CSimplifiedSphere() {}

	SHAPE_TYPE getType() { return SPHERE; }

	// ���̔��a
	float radius() { return m_r; }
	void radius(float r) { m_r = r; }

	// ���̒��S�̈ʒu
	float x() { return m_x; }
	void x(float x) { m_x = x; }

	float y() { return m_y; }
	void y(float y) { m_y = y; }

	float z() { return m_z; }
	void z(float z) { m_z = z; }

private:
	// radius
	float m_r;

	// center position
	float m_x, m_y, m_z;
};


//////////////////////////////////////////////////////////////////
//
//	�V�����_�[

class CSimplifiedCylinder : public CSimplifiedShape
{
public:
	CSimplifiedCylinder() {}
	virtual ~CSimplifiedCylinder() {}

	SHAPE_TYPE getType() { return CYLINDER; }

	// �~�̔��a
	float radius() { return m_r; }
	void radius(float r) { m_r = r; }

	// ����
	float height() { return m_h; }
	void height(float h) { m_h = h; }

	// ���S�ʒu
	float x() { return m_x; }
	void x(float x) { m_x = x; }

	float y() { return m_y; }
	void y(float y) { m_y = y; }

	float z() { return m_z; }
	void z(float z) { m_z = z; }

private:
	// radius
	float m_r;

	// height
	float m_h;

	// center of bottom circle
	// �i��ʂ�x, z���Œ�����j
	float m_x, m_y, m_z;
};


//////////////////////////////////////////////////////////////////
//
//	��

class CSimplifiedBox : public CSimplifiedShape
{
public:
	CSimplifiedBox() {}
	virtual ~CSimplifiedBox() {}

	SHAPE_TYPE getType() { return BOX; }

	// �����̂̋��̍��W(1)
	float x1() { return m_x1; }
	void x1(float x) { m_x1 = x; }

	float y1() { return m_y1; }
	void y1(float y) { m_y1 = y; }

	float z1() { return m_z1; }
	void z1(float z) { m_z1 = z; }

	// �����̂̋��̍��W(2)
	float x2() { return m_x2; }
	void x2(float x) { m_x2 = x; }

	float y2() { return m_y2; }
	void y2(float y) { m_y2 = y; }

	float z2() { return m_z2; }
	void z2(float z) { m_z2 = z; }

private:
	// positions
	float m_x1, m_y1, m_z1;
	float m_x2, m_y2, m_z2;
};


//////////////////////////////////////////////////////////////////
//
//	IndexedFaceSet���܂�Shape����P���`����v�Z����

class CSimplifiedShapeFactory
{
public:
	// ---------------------------------------------------------
	//	Transform�m�[�h��Shape�m�[�h����Ȃ�c���[��̃f�[�^����
	//	�P���`��𐶐�����
	//
	//	������MFNode�ɂ͈ȉ��̍\���̃f�[�^�������Ă��邱�Ƃ����҂��Ă���
	//
	//	+--T
	//	|  +--S
	//	|  :
	//	|  +--S
	//	|  +--T
	//	|  :
	//	|  +--T
	//	|	  +--S
	//	|	  :
	//	|	  +--S
	//	|	  +--T
	//	|	  :
	//	|	  +--T
	//	|  :
	//	+--T
	//
	//	(1) tree�ɂ�0�ȏ��Transform�m�[�h(�L��T)���܂܂�Ă���D
	//	(2) ������Transform�m�[�h�ɂ́A0�ȏ��Shape�m�[�h(�L��S)�ƁA
	//		0�ȏ��Transform�m�[�h���܂܂�Ă���D
	//	(3) Shape�m�[�h�̉��ɂ͉����܂܂�Ȃ��D
	//	(4) Transform�m�[�h�͍ċA�I�Ɂi�c���[��Ɂj�Ȃ肤��D
	// ---------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromTree(MFNode *tree);
	static CSimplifiedCylinder *calcCylinderFromTree(MFNode *tree);
	static CSimplifiedBox *calcBoxFromTree(MFNode *tree);
	static CSimplifiedShape *calcAutoFromTree(MFNode *tree);
	static CSimplifiedShape *calcFromTree(MFNode *tree, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	//	������Shape����1�̒P���`������
	//
	//	��2������hint��^���Ȃ��ꍇ�A�œK�ȒP���`��������I�ɑI��ŕԂ�
	//	��2������hint�ɋ��߂����`���^�����ꍇ�A���̒P���`���Ԃ�
	//
	//	�Ԃ�l�̃I�u�W�F�N�g�̊J���ӔC�͌Ăяo�����ɂ���
	//	�g���I�������K��delete���邱��
	//
	//	(ex)
	//	CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromShapeNodes(shapes);
	//	if (ss) {
	//	   ....
	//	   delete ss;	// <--- ���ꂪ�K�v
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromShapeNodes(MFNode *shapes);
	static CSimplifiedCylinder *calcCylinderFromShapeNodes(MFNode *shapes);
	static CSimplifiedBox *calcBoxFromShapeNodes(MFNode *shapes);
	static CSimplifiedShape *calcAutoFromShapeNodes(MFNode *shapes);
	static CSimplifiedShape *calcFromShapeNodes(MFNode *shapes, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	//	1��Shape����1�̒P���`������
	//
	//	�Ԃ�l�̃I�u�W�F�N�g�̊J���ӔC�͌Ăяo�����ɂ���
	//	�g���I�������K��delete���邱��
	//
	//	(ex)
	//	CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromShapeNode(pShape);
	//	if (ss) {
	//	   ....
	//	   delete ss;	// <--- ���ꂪ�K�v
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedCylinder *calcCylinderFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedBox *calcBoxFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedShape *calcAutoFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedShape *calcFromShapeNode(CX3DShapeNode *pShape, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	//	1��IndexedFaceSet����1�̒P���`������
	//
	//	�Ԃ�l�̃I�u�W�F�N�g�̊J���ӔC�͌Ăяo�����ɂ���
	//	�g���I�������K��delete���邱��
	//
	//	(ex)
	//	CSimplifiedSphere *ss = CSimplifiedShapeFactory::calcSphere(pFaceSet);
	//	if (ss) {
	//	   ....
	//	   delete ss;	// <--- ���ꂪ�K�v
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphere(CX3DIndexedFaceSetNode *pFaceSet);
	static CSimplifiedCylinder *calcCylinder(CX3DIndexedFaceSetNode *pFaceSet);
	static CSimplifiedBox *calcBox(CX3DIndexedFaceSetNode *pFaceSet);

private:
	// �c���[���Transform�m�[�h�̏W������_�𒊏o����
	static void extractPointsFromTree(CX3DTransformNode *pTrans, SFVec3f t, SFVec4f q, std::vector<SFVec3f>& vecPos);

	// Shape�m�[�h�̏W������_���o����
	static void extractPointsFromShapeNodes(MFNode *shapeNodes, std::vector<SFVec3f>& vecPos);

	// Shape�m�[�h����_���o����
	static void extractPointsFromShapeNode(CX3DShapeNode *pShape, std::vector<SFVec3f>& vecPos);

	// IndexedFaceSet�m�[�h����_���o����
	static void extractPointsFromIndexedFaceSetNode(CX3DIndexedFaceSetNode *pFaceSet, std::vector<SFVec3f>& vecPos);

	// �P���`��v�Z�̃R�A����
	static CSimplifiedSphere *calcSphere(std::vector<SFVec3f>& vecPos);
	static CSimplifiedCylinder *calcCylinder(std::vector<SFVec3f>& vecPos);
	static CSimplifiedBox *calcBox(std::vector<SFVec3f>& vecPos);

	// �e�`�󂲂Ƃ́u�Ђ��ݓx�v���v�Z����
	static double calcHizumiWithSphere(float r, float gx, float gy, float gz, std::vector<SFVec3f>& vecPos);
	static double calcHizumiWithCylinder(float r, float cx, float cz, std::vector<SFVec3f>& vecPos);
	static double calcHizumiWithBox(float x1, float y1, float z1, float x2, float y2, float z2, std::vector<SFVec3f>& vecPos);

public:
	static CSimplifiedCylinder *calcCylinder(CX3DCylinderNode *pCylinderNode);

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
	static bool isCylinder(
			CX3DIndexedFaceSetNode *pNodeData,
			float &radiusEstimated,
			float &heightEstimated,
			float xAxis[],
			float yAxis[],
			float zAxis[]
		);

private:
	#define INVALID_VALUE (FLT_MAX)
	#define SMALL_VALUE (0.001f)
	#define SLIGHT_SMALL_VALUE (0.01f)

public:
	// 6��float�̍ŏ��l��Ԃ�
	// f[0]�`f[5]�ɗL���Ȓl�������Ă��Ȃ���΂Ȃ�Ȃ�
	static float min6Float(float *f);

	// 2�_�Ԃ̋����̓���Ԃ�
	static float sqrDist(float px, float py, float pz, float qx, float qy, float qz);

	/**
	 * 2�_�Ԃ̋�����Ԃ�
	 */
	static float getDistance(float px, float py, float pz, float qx, float qy, float qz);

	/**
	 * �d�S�̍��W�����߂܂�
	 * @param coords ���_���W���X�g
	 * @param gx �d�S��x���W���i�[�����
	 * @param gy �d�S��y���W���i�[�����
	 * @param gz �d�S��z���W���i�[�����
	 */
	static void getCenter(MFVec3f *coords,float &gx,float &gy,float &gz);

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
	static void getNearPoint(MFVec3f *coords,float gx,float gy,float gz,float destX[],float destY[],float destZ[],int num);
};

#endif
