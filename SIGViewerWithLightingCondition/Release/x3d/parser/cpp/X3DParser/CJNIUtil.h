#ifndef _CJNIUTIL_H_
#define _CJNIUTIL_H_

#include <jni.h>

class CJNIUtil
{
public:
	// ========================================================
	//  JNIの一般的使用に関するメソッド
	// ========================================================

	// -----------------------------------------------
	//  Java VMをconfFileに従って起動し、
	//  唯一のCJNIUtilオブジェクトを生成する
	// -----------------------------------------------
	static bool init(char *confFile);

	// -----------------------------------------------
	//  Java VMを終了し、CJNIUtilオブジェクトを破棄する
	// -----------------------------------------------
	static void destroy();

	// -----------------------------------------------
	//  CJNIUtilオブジェクトを返す
	// -----------------------------------------------
	static CJNIUtil *getUtil();

	// -----------------------------------------------
	//  JNIEnvへのポインタを返す
	//  CJNIUtilが初期化されていない場合はNULLが返る
	// -----------------------------------------------
	static JNIEnv *getEnv();

	// -----------------------------------------------
	//  Java VMを起動する
	// -----------------------------------------------
	bool createJavaVM(char *confFile);

	// -----------------------------------------------
	//  Java VMを終了する
	// -----------------------------------------------
	void destroyJavaVM();

	JNIEnv *env() { return m_env; }
	JavaVM *jvm() { return m_jvm; }

	// -----------------------------------------------
	//  クラスを得る
	// -----------------------------------------------
	jclass getClass(char *className);

	// -----------------------------------------------
	//  オブジェクトを生成(new)する
	// -----------------------------------------------
	jobject newInstance(char *className);
	jobject newInstance(jclass c);

	// -----------------------------------------------
	//  メソッドIDを得る
	// -----------------------------------------------
	jmethodID getMethodID(char *className, char *methodName, char *methodSig);
	jmethodID getMethodID(jclass c, char *methodName, char *methodSig);


	// ========================================================
	//  JavaのX3DParserおよび関連クラスに特化したメソッド
	// ========================================================

	// -----------------------------------------------
	//  よく使うクラスのクラスidを得る
	// -----------------------------------------------
	jclass getClassOfVRMLNode();

	// -----------------------------------------------
	//  型チェック
	// -----------------------------------------------
	bool isInstanceOfVRMLNode(jobject obj);

	// -----------------------------------------------
	//  X3DParserクラスのメソッドのラッパー
	// -----------------------------------------------
	bool X3DParser_parse(jobject x3dParser, char *fname);
	jobjectArray X3DParser_getChildrenOfRootNode(jobject x3dParser);
	jobjectArray X3DParser_getDefNames(jobject x3dParser);
	jobject X3DParser_getDefNode(jobject x3dParser, char *defName);

	// -----------------------------------------------
	//  VRMLNodeクラスのメソッドのラッパー
	// -----------------------------------------------
	char *VRMLNode_getNodeName(jobject vrmlNode);
	int VRMLNode_getNumFields(jobject vrmlNode);
	jobject VRMLNode_getFieldDeclaration(jobject vrmlNode, int i);
	int VRMLNode_getFieldIndex(jobject vrmlNode, char *fieldName);

	// -----------------------------------------------
	//  VRMLNodeTypeクラスのメソッドのラッパー
	// -----------------------------------------------
	jobject VRMLNodeType_getFieldValue(jobject vrmlNode, int i);

	// -----------------------------------------------
	// VRMLFieldDeclarationクラスのメソッドのラッパー
	// -----------------------------------------------
	char *VRMLFieldDeclaration_getName(jobject vrmlDecl);
	int VRMLFieldDeclaration_getFieldType(jobject vrmlDecl);
	char *VRMLFieldDeclaration_getFieldTypeString(jobject vrmlDecl);

private:
	JNIEnv *m_env;
	JavaVM *m_jvm;

	jclass m_class_VRMLNode;

	// ------------------------------------------------
	//  このクラスはコンストラクタでは作成しない
	//  CJNIUtil::init()スタティックメソッドで生成する
	// ------------------------------------------------
	CJNIUtil();
	virtual ~CJNIUtil();
};

#endif
