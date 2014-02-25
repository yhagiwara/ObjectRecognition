#include "CX3DSpotLightNode.h"
#include "CX3DParserUtil.h"
#include "CX3DParser.h"
#include "CJNIUtil.h"

/////////////////////////////////////////////////////////
//
//  CX3DSpotLightNode

// vrmlNode����SpotLight�I�u�W�F�N�g�����
CX3DSpotLightNode::CX3DSpotLightNode(jobject vrmlNode)
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

		if (nodeName && strcmp(nodeName, "SpotLight")==0)
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
						if (strcmp(fieldName.c_str(), "ambientIntensity") == 0)
						{
							m_ambientIntensity.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "color") == 0)
						{
							m_color.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "intensity") == 0)
						{
							m_intensity.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "direction") == 0)
						{
							m_direction.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "location") == 0)
						{
							m_location.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "radius") == 0)
						{
							m_radius.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "attenuation") == 0)
						{
							m_attenuation.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "beamWidth") == 0)
						{
							m_beamWidth.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "cutOffAngle") == 0)
						{
							m_cutOffAngle.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "on") == 0)
						{
							m_on.setValue(data);
						}
						else if (strcmp(fieldName.c_str(), "global") == 0)
						{
							m_global.setValue(data);
						}

						delete data;
					}
				}
			}
		}
		delete tmpNode;
	}
}

void CX3DSpotLightNode::print(int indent)
{
	FILE *fp = CX3DParser::getDebugLogFp();

	char *nodeName = getNodeName();
	if (nodeName)
	{
		float r, g, b;
		float x, y, z;

		CX3DParser::printIndent(indent);
		fprintf(fp, "%s (%s)\n", nodeName, CX3DNode::getNodeTypeString(getNodeType()));

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "ambientIntensity : (%f)\n", getAmbientIntensity()->getValue());

		getColor()->getValue(r, g, b);
		CX3DParser::printIndent(indent+1);
		fprintf(fp, "color : (%f %f %f)\n", r, g, b);

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "intensity : (%f)\n", getIntensity()->getValue());

		getDirection()->getValue(x, y, z);
		CX3DParser::printIndent(indent+1);
		fprintf(fp, "direction : (%f %f %f)\n", x, y, z);

		getLocation()->getValue(x, y, z);
		CX3DParser::printIndent(indent+1);
		fprintf(fp, "location : (%f %f %f)\n", x, y, z);

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "radius : (%f)\n", getRadius()->getValue());

		getAttenuation()->getValue(x, y, z);
		CX3DParser::printIndent(indent+1);
		fprintf(fp, "attenuation : (%f %f %f)\n", x, y, z);

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "beamWidth : (%f)\n", getBeamWidth()->getValue());

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "cutOffAngle : (%f)\n", getCutOffAngle()->getValue());

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "on : %s\n", getOn()->getValue() ? "TRUE" : "FALSE");

		CX3DParser::printIndent(indent+1);
		fprintf(fp, "global : %s\n", getGlobal()->getValue() ? "TRUE" : "FALSE");
	}
}
