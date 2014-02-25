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
//	単純形状を表すためのベースクラス

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

	// 単純形状の型（球、シリンダー、箱）を返す
	virtual SHAPE_TYPE getType() = 0;

	// 歪度
	float hizumi() { return m_hizumi; }
	void hizumi(float hizumi) { m_hizumi = hizumi; }

protected:
	// 歪度
	float m_hizumi;
};


//////////////////////////////////////////////////////////////////
//
//	球

class CSimplifiedSphere : public CSimplifiedShape
{
public:
	CSimplifiedSphere() {}
	virtual ~CSimplifiedSphere() {}

	SHAPE_TYPE getType() { return SPHERE; }

	// 球の半径
	float radius() { return m_r; }
	void radius(float r) { m_r = r; }

	// 球の中心の位置
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
//	シリンダー

class CSimplifiedCylinder : public CSimplifiedShape
{
public:
	CSimplifiedCylinder() {}
	virtual ~CSimplifiedCylinder() {}

	SHAPE_TYPE getType() { return CYLINDER; }

	// 円の半径
	float radius() { return m_r; }
	void radius(float r) { m_r = r; }

	// 高さ
	float height() { return m_h; }
	void height(float h) { m_h = h; }

	// 中心位置
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
	// （底面はx, z軸で張られる）
	float m_x, m_y, m_z;
};


//////////////////////////////////////////////////////////////////
//
//	箱

class CSimplifiedBox : public CSimplifiedShape
{
public:
	CSimplifiedBox() {}
	virtual ~CSimplifiedBox() {}

	SHAPE_TYPE getType() { return BOX; }

	// 直方体の隅の座標(1)
	float x1() { return m_x1; }
	void x1(float x) { m_x1 = x; }

	float y1() { return m_y1; }
	void y1(float y) { m_y1 = y; }

	float z1() { return m_z1; }
	void z1(float z) { m_z1 = z; }

	// 直方体の隅の座標(2)
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
//	IndexedFaceSetを含むShapeから単純形状を計算する

class CSimplifiedShapeFactory
{
public:
	// ---------------------------------------------------------
	//	TransformノードとShapeノードからなるツリー状のデータから
	//	単純形状を生成する
	//
	//	引数のMFNodeには以下の構造のデータが入っていることを期待している
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
	//	(1) treeには0個以上のTransformノード(記号T)が含まれている．
	//	(2) それらのTransformノードには、0個以上のShapeノード(記号S)と、
	//		0個以上のTransformノードが含まれている．
	//	(3) Shapeノードの下には何も含まれない．
	//	(4) Transformノードは再帰的に（ツリー状に）なりうる．
	// ---------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromTree(MFNode *tree);
	static CSimplifiedCylinder *calcCylinderFromTree(MFNode *tree);
	static CSimplifiedBox *calcBoxFromTree(MFNode *tree);
	static CSimplifiedShape *calcAutoFromTree(MFNode *tree);
	static CSimplifiedShape *calcFromTree(MFNode *tree, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	//	複数のShapeから1つの単純形状を作る
	//
	//	第2引数のhintを与えない場合、最適な単純形状を自動的に選んで返す
	//	第2引数のhintに求めたい形状を与えた場合、その単純形状を返す
	//
	//	返り値のオブジェクトの開放責任は呼び出し側にある
	//	使い終わったら必ずdeleteすること
	//
	//	(ex)
	//	CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromShapeNodes(shapes);
	//	if (ss) {
	//	   ....
	//	   delete ss;	// <--- これが必要
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromShapeNodes(MFNode *shapes);
	static CSimplifiedCylinder *calcCylinderFromShapeNodes(MFNode *shapes);
	static CSimplifiedBox *calcBoxFromShapeNodes(MFNode *shapes);
	static CSimplifiedShape *calcAutoFromShapeNodes(MFNode *shapes);
	static CSimplifiedShape *calcFromShapeNodes(MFNode *shapes, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	//	1つのShapeから1つの単純形状を作る
	//
	//	返り値のオブジェクトの開放責任は呼び出し側にある
	//	使い終わったら必ずdeleteすること
	//
	//	(ex)
	//	CSimplifiedShape *ss = CSimplifiedShapeFactory::calcAutoFromShapeNode(pShape);
	//	if (ss) {
	//	   ....
	//	   delete ss;	// <--- これが必要
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphereFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedCylinder *calcCylinderFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedBox *calcBoxFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedShape *calcAutoFromShapeNode(CX3DShapeNode *pShape);
	static CSimplifiedShape *calcFromShapeNode(CX3DShapeNode *pShape, CSimplifiedShape::SHAPE_TYPE hint=CSimplifiedShape::NONE);

	// --------------------------------------------------------
	//	1つのIndexedFaceSetから1つの単純形状を作る
	//
	//	返り値のオブジェクトの開放責任は呼び出し側にある
	//	使い終わったら必ずdeleteすること
	//
	//	(ex)
	//	CSimplifiedSphere *ss = CSimplifiedShapeFactory::calcSphere(pFaceSet);
	//	if (ss) {
	//	   ....
	//	   delete ss;	// <--- これが必要
	//	}
	// --------------------------------------------------------
	static CSimplifiedSphere *calcSphere(CX3DIndexedFaceSetNode *pFaceSet);
	static CSimplifiedCylinder *calcCylinder(CX3DIndexedFaceSetNode *pFaceSet);
	static CSimplifiedBox *calcBox(CX3DIndexedFaceSetNode *pFaceSet);

private:
	// ツリー状のTransformノードの集合から点を抽出する
	static void extractPointsFromTree(CX3DTransformNode *pTrans, SFVec3f t, SFVec4f q, std::vector<SFVec3f>& vecPos);

	// Shapeノードの集合から点抽出する
	static void extractPointsFromShapeNodes(MFNode *shapeNodes, std::vector<SFVec3f>& vecPos);

	// Shapeノードから点抽出する
	static void extractPointsFromShapeNode(CX3DShapeNode *pShape, std::vector<SFVec3f>& vecPos);

	// IndexedFaceSetノードから点抽出する
	static void extractPointsFromIndexedFaceSetNode(CX3DIndexedFaceSetNode *pFaceSet, std::vector<SFVec3f>& vecPos);

	// 単純形状計算のコア部分
	static CSimplifiedSphere *calcSphere(std::vector<SFVec3f>& vecPos);
	static CSimplifiedCylinder *calcCylinder(std::vector<SFVec3f>& vecPos);
	static CSimplifiedBox *calcBox(std::vector<SFVec3f>& vecPos);

	// 各形状ごとの「ひずみ度」を計算する
	static double calcHizumiWithSphere(float r, float gx, float gy, float gz, std::vector<SFVec3f>& vecPos);
	static double calcHizumiWithCylinder(float r, float cx, float cz, std::vector<SFVec3f>& vecPos);
	static double calcHizumiWithBox(float x1, float y1, float z1, float x2, float y2, float z2, std::vector<SFVec3f>& vecPos);

public:
	static CSimplifiedCylinder *calcCylinder(CX3DCylinderNode *pCylinderNode);

	/**
	 * 指定形状をシリンダとみなしても問題ないかどうかの判定
	 * @param pNodeData
	 * @param radiusEstimated シリンダ形状ならば、半径が格納される
	 * @param heightEstimated シリンダ形状ならば、高さが格納される
	 * @param xAxis[]		  シリンダの軸の座標が格納される(xAxis[0]:表,xAxis[1]:裏)
	 * @param yAxis[]		  シリンダの軸の座標が格納される(yAxis[0]:表,yAxis[1]:裏)
	 * @param zAxis[]		  シリンダの軸の座標が格納される(zAxis[0]:表,zAxis[1]:裏)
	 * @return シリンダ形状とみなしても問題なければtrueを返す
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
	// 6つのfloatの最小値を返す
	// f[0]～f[5]に有効な値が入っていなければならない
	static float min6Float(float *f);

	// 2点間の距離の二乗を返す
	static float sqrDist(float px, float py, float pz, float qx, float qy, float qz);

	/**
	 * 2点間の距離を返す
	 */
	static float getDistance(float px, float py, float pz, float qx, float qy, float qz);

	/**
	 * 重心の座標を求めます
	 * @param coords 頂点座標リスト
	 * @param gx 重心のx座標が格納される
	 * @param gy 重心のy座標が格納される
	 * @param gz 重心のz座標が格納される
	 */
	static void getCenter(MFVec3f *coords,float &gx,float &gy,float &gz);

	/**
	 * (gx,gy,gz)から近い点を探して格納します。
	 * @param coords  頂点座標リスト
	 * @param gx	  指定点のx座標
	 * @param gy	  指定点のy座標
	 * @param gz	  指定点のz座標
	 * @param destX[] 指定点に近い点の座標が配列で格納される
	 * @param destY[] 指定点に近い点の座標が配列で格納される
	 * @param destZ[] 指定点に近い点の座標が配列で格納される
	 * @param num	  (gx,gy,gz)に近い点をいくつまで探すか
	 */
	static void getNearPoint(MFVec3f *coords,float gx,float gy,float gz,float destX[],float destY[],float destZ[],int num);
};

#endif
