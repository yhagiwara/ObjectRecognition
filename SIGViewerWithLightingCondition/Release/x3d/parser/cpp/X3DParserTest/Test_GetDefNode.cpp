#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  �w��def�m�[�h�̓��e��\������
// ==========================================

void Test_printDefNode(CX3DParser& parser, char *vrmlFile, char *defName)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	CX3DParser::printLog("**** Node contents of defName(%s) ****\n", defName);

	// ---------------------------------------------
	//  def�m�[�h�𓾂�
	// ---------------------------------------------
	CX3DNode *node = parser.getDefNode(defName);
	if (node)
	{
		node->print();

		delete node;
		node = NULL;
	}
}
