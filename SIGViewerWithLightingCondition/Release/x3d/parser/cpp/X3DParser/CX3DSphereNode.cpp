#include "CX3DSphereNode.h"
#include "CX3DParserUtil.h"
#include "CX3DParser.h"
#include "CJNIUtil.h"

/////////////////////////////////////////////////////////
//
//  CX3DSphereNode

// vrmlNode����Sphere�I�u�W�F�N�g�����
CX3DSphereNode::CX3DSphereNode(jobject vrmlNode)
{
	if (!vrmlNode)
	{
		m_vrmlNode = NULL;
		return;
	}

	CJNIUtil *ju = CJNIUtil::getUtil();

	if (!ju->isInstanceOfVRMLNode(vrmlNode))
	{
		fprintf(stderr, "vrmlNode is not instance of VRMLNode [%s:%d]\n", __FILE__, __LINE__);
		exit(1);
	}

	m_vrmlNode = ju->env()->NewGlobalRef(vrmlNode);
	
	// �e���|�����m�[�h�i�t�B�[���h�T���p�j
	CX3DNode *tmpNode = new CX3DNode(vrmlNode);
	if (tmpNode)
	{
		char *nodeName = tmpNode->getNodeName();

		if (nodeName && strcmp(nodeName, "Sphere")==0)
		{
			int n = tmpNode->countFields();
			for (int i=0; i<n; i++)
			{
				char *pFieldName = tmpNode->getFieldName(i);
				if (pFieldName)
				{
					std::string fieldName = pFieldName;

					CVRMLFieldData *data = tmpNode->createFieldValue((char *)(fieldName.c_str()));
					if (data)
					{
						if (strcmp(fieldName.c_str(), "solid") == 0)
						{
							m_solid.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "radius") == 0)
						{
							m_radius.setValue(data);
						}

						delete data;
					}
				}
			}
		}
		delete tmpNode;
	}
}

void CX3DSphereNode::print(int indent)
{
	FILE *fp = CX3DParser::getDebugLogFp();

	char *nodeName = getNodeName();
	if (nodeName)
	{
		CX3DParser::printIndent(indent);
		fprintf(fp, "%s (%s)\n", nodeName, CX3DNode::getNodeTypeString(getNodeType()));

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "solid  : %s\n", getSolid()->getValue() ? "TRUE" : "FALSE");

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "radius : (%f)\n", getRadius()->getValue());
	}
}
