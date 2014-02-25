#include "CX3DParser.h"
#include "CX3DParserUtil.h"
#include "CVRMLFieldData.h"
#include <stdio.h>
#include <stdarg.h>		// for va_list, etc
#include <jni.h>

FILE *g_debugLogFp = stderr;
int g_maxPrintElemsForMFField = 0;

#define INDENT_LEVEL_STRING	"  "

/////////////////////////////////////////////////////////////////
//
//	static methods

void CX3DParser::openDebugLog(char *fname, bool bAppend)
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	CX3DParser::closeDebugLog();

	g_debugLogFp = fopen(fname, bAppend ? "a" : "w");
#endif
}

void CX3DParser::closeDebugLog()
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	if (g_debugLogFp && g_debugLogFp != stderr)
	{
		fclose(g_debugLogFp);
		g_debugLogFp = NULL;
	}
#endif
}

void CX3DParser::setDebugLogFp(FILE *fp)
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	CX3DParser::closeDebugLog();

	g_debugLogFp = fp;
#endif
}

void CX3DParser::resetDebugLogFp()
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	CX3DParser::closeDebugLog();

	g_debugLogFp = stderr;
#endif
}

FILE *CX3DParser::getDebugLogFp()
{
	return g_debugLogFp;
}

void CX3DParser::printIndent(int indent)
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	FILE *fp = CX3DParser::getDebugLogFp();
	
	if (fp)
	{
		for (int i=0; i<indent; i++)
		{
			fprintf(fp, INDENT_LEVEL_STRING);
		}
	}
#endif
}

void CX3DParser::printLog(char *format, ...)
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	FILE *fp = CX3DParser::getDebugLogFp();
	
	if (fp)
	{
		char msg[1024];
		va_list arg;

		va_start(arg, format);
		vsprintf(msg, format, arg);

		fprintf(CX3DParser::getDebugLogFp(), msg);

		va_end(arg);
	}
#endif
}

void CX3DParser::printIndentLog(int indentLevel, char *format, ...)
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	FILE *fp = CX3DParser::getDebugLogFp();
	
	if (fp)
	{
		char msg[1024];
		va_list arg;

		va_start(arg, format);
		vsprintf(msg, format, arg);

		CX3DParser::printIndent(indentLevel);
		fprintf(CX3DParser::getDebugLogFp(), msg);

		va_end(arg);
	}
#endif
}

void CX3DParser::flushLog()
{
#ifndef X3DPARSER_DISABLE_DEBUG_LOG
	FILE *fp = getDebugLogFp();

	if (fp)
	{
		fflush(fp);
	}
#endif
}

void CX3DParser::setMaxPrintElemsForMFField(int n)
{
	g_maxPrintElemsForMFField = n;
}

int CX3DParser::getMaxPrintElemsForMFField()
{
	return g_maxPrintElemsForMFField;
}


/////////////////////////////////////////////////////////////////
//
//	CX3DParser member functions

CX3DParser::CX3DParser()
{
	/*
	  090311 yoshi
	  
	  ���N���X�C���X�^���X��1��쐬�Adelete��ēx�쐬����ƃG���[���N����B
	  CJNIUtil::init ��X3D���C�u�������g�p����Ƃ��ɊO���Ŗ����I�ɌĂԁB
	  CJNIUtil::destroy �������B
	  
	if (!CJNIUtil::init("X3DParser.cfg"))
	{
		CX3DParser::printLog("*** Failed to initialize Java VM ***\n");
		exit(1);
	}
	*/

	CVRMLFieldDataFactory::init();

	// -----------------------------------------------
	//	create X3DParser java class instance
	// -----------------------------------------------
	CJNIUtil *ju = CJNIUtil::getUtil();

	char *className = "X3DParser";

	m_X3DParser = ju->newInstance(className);
	if (!m_X3DParser)
	{
		CX3DParser::printLog("cannot create object of %s.class\n", className);
		exit(1);
	}

	ju->env()->NewGlobalRef(m_X3DParser);
}

CX3DParser::~CX3DParser()
{
	if (m_X3DParser)
	{
		CJNIUtil *ju = CJNIUtil::getUtil();

		ju->env()->DeleteGlobalRef(m_X3DParser);
		m_X3DParser = NULL;
	}

	CVRMLFieldDataFactory::destroy();
	// 090311 yoshi �����ł͌Ă΂Ȃ�
	//CJNIUtil::destroy();
}

// -----------------------------------------------------
//	VRML�t�@�C�����p�[�X����
// -----------------------------------------------------
bool CX3DParser::parse(char *fname)
{
	CJNIUtil *ju = CJNIUtil::getUtil();

	if (ju->X3DParser_parse(m_X3DParser, fname))
	{
		m_fname = fname;
		return true;
	}
	else
	{
		m_fname = "";
		return false;
	}
}

// -----------------------------------------------------
//	VRML�t�@�C����ǂݍ���Œ��g��\��
// -----------------------------------------------------
void CX3DParser::print()
{
	CX3DParser::printLog("*****  content of %s  *****\n", m_fname.c_str());

	MFNode *nodes = getChildrenOfRootNode();
	if (nodes)
	{
		for (int i=0; i<nodes->count(); i++)
		{
			CX3DNode *node = nodes->getNode(i);

			if (node) node->print();
		}

		delete nodes;
	}
}


/**
 * �m�[�h�^�C�v�̃`�F�b�N
 */
void CX3DParser::printNodeTypeList(){
	printf("\t\t\t///CX3DParser#printNodeTypeList()///\n");
	CX3DParser::printLog("\t\t\t*****  content of %s  *****\n", m_fname.c_str());

	MFNode *nodes = getChildrenOfRootNode();
	if (nodes)
	{
		for (int i=0; i < nodes->count(); i++)
		{
			CX3DNode *node = nodes->getNode(i);

			if (node) {
				int nodetype = node->getNodeType();
				printf("\t\t\t\t\t[NodeType %d] = %d\n",i,nodetype);
				printf("\t\t\t\t\t\tL[Name] = %s\n",node->getNodeName());

				// �q�m�[�h���T��
				MFNode *children = node->searchNodesFromAllChildren(node->getNodeName());
				if (children) {
					int m = children->count();
					for (int j=0; j<m; j++) {
						CX3DNode *child = children->releaseNode(j);
						int childNodeType = child->getNodeType();
						printf("\t\t\t\t\t[ChildNodeType %d] = %d\n",j,childNodeType);
						printf("\t\t\t\t\t\tL[ChildName] = %s\n",child->getNodeName());
					}

					delete children;
					children = NULL;
				}

			}
		}

		delete nodes;
	}
}


// -----------------------------------------------------
//	���[�g�����̃m�[�h�i�����j�𓾂�
// -----------------------------------------------------
MFNode *CX3DParser::getChildrenOfRootNode()
{
	CJNIUtil *ju = CJNIUtil::getUtil();

	jobjectArray vrmlNodeArray = ju->X3DParser_getChildrenOfRootNode(m_X3DParser);
	jint len = ju->env()->GetArrayLength(vrmlNodeArray);

	MFNode *nodes = new MFNode();
	if (!nodes)
	{
		CX3DParser::printLog("out of memory [%s:%d]\n", __FILE__, __LINE__);
		return NULL;
	}

	for (int i=0; i<len; i++)
	{
		jobject vrmlNode = ju->env()->GetObjectArrayElement(vrmlNodeArray, i);

		CX3DNode *node = CX3DNodeFactory::createNode(vrmlNode);
		if (node)
		{
			nodes->addValue(node);
		}
	}

	return nodes;
}

// -----------------------------------------------------
//	�m�[�h������m�[�h������ (1)
// -----------------------------------------------------
MFNode *CX3DParser::searchNodesFromDirectChildrenOfRoot(char *nodeName)
{
	MFNode *ret = new MFNode();
	if (!ret)
	{
		CX3DParser::printLog("out of memory [%s:%d]\n", __FILE__, __LINE__);
		return NULL;
	}

	MFNode *nodes = getChildrenOfRootNode();
	if (!nodes) return NULL;

	int n = nodes->count();
	for (int i=0; i<n; i++)
	{
		CX3DNode *node = nodes->releaseNode(i);
			// releaseNode()�̓m�[�h�̏��L��(=delete�ӔC)��
			// �Ăяo�����Ɉڂ�

		if (node)
		{
			if (strcmp(node->getNodeName(), nodeName) == 0)
			{
				ret->addValue(node);
			}
			else
			{
				delete node;
				node = NULL;
			}
		}
	}

	delete nodes;
	nodes = NULL;

	return ret;
}

// -----------------------------------------------------
//	�m�[�h������m�[�h������ (2)
// -----------------------------------------------------
MFNode *CX3DParser::searchNodesFromAllChildrenOfRoot(char *nodeName)
{
	MFNode *ret = new MFNode();
	if (!ret)
	{
		CX3DParser::printLog("out of memory [%s:%d]\n", __FILE__, __LINE__);
		return NULL;
	}

	MFNode *nodes = getChildrenOfRootNode();
	if (!nodes) return NULL;

	int n = nodes->count();
	for (int i=0; i<n; i++)
	{
		CX3DNode *node = nodes->releaseNode(i);
			// releaseNode()�̓m�[�h�̏��L��(=delete�ӔC)��
			// �Ăяo�����Ɉڂ�

		if (node)
		{
			bool bDeleteNode;

			if (strcmp(node->getNodeName(), nodeName) == 0)
			{
				ret->addValue(node);
				bDeleteNode = false;
					// ���̃m�[�h�͕Ԃ�l�ɒǉ������̂�delete���Ă͂����Ȃ�
			}
			else
			{
				bDeleteNode = true;
			}

			// �q�m�[�h���T��
			MFNode *children = node->searchNodesFromAllChildren(nodeName);
			if (children)
			{
				int m = children->count();
				for (int j=0; j<m; j++)
				{
					CX3DNode *child = children->releaseNode(j);

					if (child)
					{
						ret->addValue(child);
					}
				}

				delete children;
				children = NULL;
			}

			if (bDeleteNode)
			{
				delete node;
				node = NULL;
			}
		}
	}

	delete nodes;
	nodes = NULL;

	return ret;
}

// -----------------------------------------------------
//	���ׂĂ�def���𓾂�
// -----------------------------------------------------
std::vector<std::string> CX3DParser::getDefNames()
{
	std::vector<std::string> defNames;

	CJNIUtil *ju = CJNIUtil::getUtil();

	jobjectArray defNameArray = ju->X3DParser_getDefNames(m_X3DParser);
	jint len = ju->env()->GetArrayLength(defNameArray);

	for (int i=0; i<len; i++)
	{
		jstring str = (jstring)(ju->env()->GetObjectArrayElement(defNameArray, i));

		if (str)
		{
			jboolean isCopy;
			const char *pstr = ju->env()->GetStringUTFChars(str, &isCopy);

			defNames.push_back(pstr);

			if (isCopy == JNI_TRUE)
			{
				ju->env()->ReleaseStringUTFChars(str, pstr);
			}
		}
	}

	return defNames;
}

// -----------------------------------------------------
//	def������def�m�[�h������
// -----------------------------------------------------
CX3DNode *CX3DParser::getDefNode(char *defName)
{
	CJNIUtil *ju = CJNIUtil::getUtil();

	jobject vrmlNode = ju->X3DParser_getDefNode(m_X3DParser, defName);
	if (!vrmlNode) return NULL;

	CX3DNode *node = CX3DNodeFactory::createNode(vrmlNode);
	return node;
}
