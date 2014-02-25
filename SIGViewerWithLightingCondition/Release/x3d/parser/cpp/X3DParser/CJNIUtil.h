#ifndef _CJNIUTIL_H_
#define _CJNIUTIL_H_

#include <jni.h>

class CJNIUtil
{
public:
	// ========================================================
	//  JNI�̈�ʓI�g�p�Ɋւ��郁�\�b�h
	// ========================================================

	// -----------------------------------------------
	//  Java VM��confFile�ɏ]���ċN�����A
	//  �B���CJNIUtil�I�u�W�F�N�g�𐶐�����
	// -----------------------------------------------
	static bool init(char *confFile);

	// -----------------------------------------------
	//  Java VM���I�����ACJNIUtil�I�u�W�F�N�g��j������
	// -----------------------------------------------
	static void destroy();

	// -----------------------------------------------
	//  CJNIUtil�I�u�W�F�N�g��Ԃ�
	// -----------------------------------------------
	static CJNIUtil *getUtil();

	// -----------------------------------------------
	//  JNIEnv�ւ̃|�C���^��Ԃ�
	//  CJNIUtil������������Ă��Ȃ��ꍇ��NULL���Ԃ�
	// -----------------------------------------------
	static JNIEnv *getEnv();

	// -----------------------------------------------
	//  Java VM���N������
	// -----------------------------------------------
	bool createJavaVM(char *confFile);

	// -----------------------------------------------
	//  Java VM���I������
	// -----------------------------------------------
	void destroyJavaVM();

	JNIEnv *env() { return m_env; }
	JavaVM *jvm() { return m_jvm; }

	// -----------------------------------------------
	//  �N���X�𓾂�
	// -----------------------------------------------
	jclass getClass(char *className);

	// -----------------------------------------------
	//  �I�u�W�F�N�g�𐶐�(new)����
	// -----------------------------------------------
	jobject newInstance(char *className);
	jobject newInstance(jclass c);

	// -----------------------------------------------
	//  ���\�b�hID�𓾂�
	// -----------------------------------------------
	jmethodID getMethodID(char *className, char *methodName, char *methodSig);
	jmethodID getMethodID(jclass c, char *methodName, char *methodSig);


	// ========================================================
	//  Java��X3DParser����ъ֘A�N���X�ɓ����������\�b�h
	// ========================================================

	// -----------------------------------------------
	//  �悭�g���N���X�̃N���Xid�𓾂�
	// -----------------------------------------------
	jclass getClassOfVRMLNode();

	// -----------------------------------------------
	//  �^�`�F�b�N
	// -----------------------------------------------
	bool isInstanceOfVRMLNode(jobject obj);

	// -----------------------------------------------
	//  X3DParser�N���X�̃��\�b�h�̃��b�p�[
	// -----------------------------------------------
	bool X3DParser_parse(jobject x3dParser, char *fname);
	jobjectArray X3DParser_getChildrenOfRootNode(jobject x3dParser);
	jobjectArray X3DParser_getDefNames(jobject x3dParser);
	jobject X3DParser_getDefNode(jobject x3dParser, char *defName);

	// -----------------------------------------------
	//  VRMLNode�N���X�̃��\�b�h�̃��b�p�[
	// -----------------------------------------------
	char *VRMLNode_getNodeName(jobject vrmlNode);
	int VRMLNode_getNumFields(jobject vrmlNode);
	jobject VRMLNode_getFieldDeclaration(jobject vrmlNode, int i);
	int VRMLNode_getFieldIndex(jobject vrmlNode, char *fieldName);

	// -----------------------------------------------
	//  VRMLNodeType�N���X�̃��\�b�h�̃��b�p�[
	// -----------------------------------------------
	jobject VRMLNodeType_getFieldValue(jobject vrmlNode, int i);

	// -----------------------------------------------
	// VRMLFieldDeclaration�N���X�̃��\�b�h�̃��b�p�[
	// -----------------------------------------------
	char *VRMLFieldDeclaration_getName(jobject vrmlDecl);
	int VRMLFieldDeclaration_getFieldType(jobject vrmlDecl);
	char *VRMLFieldDeclaration_getFieldTypeString(jobject vrmlDecl);

private:
	JNIEnv *m_env;
	JavaVM *m_jvm;

	jclass m_class_VRMLNode;

	// ------------------------------------------------
	//  ���̃N���X�̓R���X�g���N�^�ł͍쐬���Ȃ�
	//  CJNIUtil::init()�X�^�e�B�b�N���\�b�h�Ő�������
	// ------------------------------------------------
	CJNIUtil();
	virtual ~CJNIUtil();
};

#endif
