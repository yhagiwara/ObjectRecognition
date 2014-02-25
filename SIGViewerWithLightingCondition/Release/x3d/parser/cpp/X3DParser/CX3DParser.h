#ifndef _CX3D_PARSER_H_
#define _CX3D_PARSER_H_

#include "CX3DNode.h"
#include "CX3DBoxNode.h"
#include "CX3DColorNode.h"
#include "CX3DConeNode.h"
#include "CX3DCoordinateNode.h"
#include "CX3DCylinderNode.h"
#include "CX3DDirectionalLightNode.h"
#include "CX3DGroupNode.h"
#include "CX3DHAnimDisplacerNode.h"
#include "CX3DHAnimHumanoidNode.h"
#include "CX3DHAnimJointNode.h"
#include "CX3DHAnimSegmentNode.h"
#include "CX3DHAnimSiteNode.h"
#include "CX3DImageTextureNode.h"
#include "CX3DIndexedFaceSetNode.h"
#include "CX3DIndexedLineSetNode.h"
#include "CX3DMaterialNode.h"
#include "CX3DNormalNode.h"
#include "CX3DOpenHRPHumanoidNode.h"
#include "CX3DOpenHRPJointNode.h"
#include "CX3DOpenHRPSegmentNode.h"
#include "CX3DPointLightNode.h"
#include "CX3DShapeNode.h"
#include "CX3DSphereNode.h"
#include "CX3DSpotLightNode.h"
#include "CX3DTextureCoordinateNode.h"
#include "CX3DTextureTransformNode.h"
#include "CX3DTransformNode.h"
#include "CX3DViewpointNode.h"
#include "CJNIUtil.h"

#include <jni.h>
#include <vector>

class CX3DParser
{
public:
	CX3DParser();
	virtual ~CX3DParser();

	// =================================================
	//	X3D Parse Methods
	// =================================================

	// ---------------------------------------------
	//  X3D/VRMLファイルをパースする
	// ---------------------------------------------
	bool parse(char *fname);

	// ---------------------------------------------
	//  パース結果をログに書き出す
	// ---------------------------------------------
	void print();

	/**
	 * ノードタイプのチェック
	 */
	void printNodeTypeList();

	// ---------------------------------------------
	//  ルートの子ノードを返す
	// ---------------------------------------------
	MFNode *getChildrenOfRootNode();

	// ---------------------------------------------
	//  ルートの子ノードから
	//  指定ノード名を持つものを探す．
	//
	//  孫ノード以降は検索しない
	// ---------------------------------------------
	MFNode *searchNodesFromDirectChildrenOfRoot(char *nodeName);

	// ---------------------------------------------
	//  ルートのすべての子ノードから、
	//  指定ノード名を持つすべてのノードを再帰的に探す
	// ---------------------------------------------
	MFNode *searchNodesFromAllChildrenOfRoot(char *nodeName);

	// ---------------------------------------------
	//  すべてのdefノード名を得る
	// ---------------------------------------------
	std::vector<std::string> getDefNames();

	// ---------------------------------------------
	//  defノードを返す
	// ---------------------------------------------
	CX3DNode *getDefNode(char *defName);


	// =================================================
	//	Debug Log関連
	// =================================================

	// ---------------------------------------------
	//  ログを開く
	//
	//  ログの出力先ファイルはfnameになる．
	//  第2引数をtrueにするとファイルを追加モードで
	//  開く
	// ---------------------------------------------
	static void openDebugLog(char *fname, bool bAppend=false);

	// ---------------------------------------------
	//  ログを閉じる
	//
	//  このメソッドを呼んだ後は、ログはどこにも
	//  出なくなる
	// ---------------------------------------------
	static void closeDebugLog();

	// ---------------------------------------------
	//  ログの出力先をfpに設定する
	//
	//  このメソッドを呼ぶ以前にログを開いていた場合
	//  そのログは閉じられる
	//
	//  fpをNULLに設定すると、ログはどこにも出力されない
	// ---------------------------------------------
	static void setDebugLogFp(FILE *fp);

	// ---------------------------------------------
	//  ログの出力先をstderrに戻す
	//
	//  このメソッドを呼ぶ以前にログを開いていた場合
	//  そのログは閉じられる
	// ---------------------------------------------
	static void resetDebugLogFp();

	// ---------------------------------------------
	//  現在のログの出力先(=FILE *)を返す
	// ---------------------------------------------
	static FILE *getDebugLogFp();

	// ---------------------------------------------
	//  インデントを出力する
	// ---------------------------------------------
	static void printIndent(int indent);

	// -----------------------------------------------------
	//  ログを出力する
	//
	//  デフォルトではstderrに出る．
	//  openDebugLogまたはsetDebugLogで出力先を変えた場合は、
	//  そこに出る
	// -----------------------------------------------------
	static void printLog(char *format, ...);

	// ---------------------------------------------
	//  インデントつきログ出力
	// ---------------------------------------------
	static void printIndentLog(int indentLevel, char *format, ...);

	// ---------------------------------------------
	//  バッファに溜まっているログをフラッシュする
	// ---------------------------------------------
	static void flushLog();

	// ----------------------------------------------------------
	//  MFフィールドの表示個数を先頭n個までに制限する．
	//
	//  たとえば、要素数1000個のMFInt32があった場合、
	//  n=5としてこのメソッドをコールしておけば、
	//  print()で表示されるのは1000個のうち、先頭の5個だけになる．
	//  （ただしMFNodeに関してはすべて表示する）
	// 
	//  フィールドを表示する際、MFフィールドの要素数が
	//  多すぎると、全体の構造がわかりにくくなるので
	//  それを回避するための対策
	//
	//  0以下の数を与えた場合、すべての要素を表示する
	//  （デフォルトではすべて表示）
	// ----------------------------------------------------------
	static void setMaxPrintElemsForMFField(int n);

	static int getMaxPrintElemsForMFField();

private:
	std::string m_fname;
	jobject m_X3DParser;
};

#endif
