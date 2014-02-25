#include "CX3DParser.h"
#include "Test.h"
#include <vector>
#include <string>

// ==========================================
//  ���ׂĂ�def�m�[�h����\������
// ==========================================

void Test_printDefNames(CX3DParser& parser, char *vrmlFile)
{
	// ---------------------------------------------
	//  VRML�t�@�C�����p�[�X����
	// ---------------------------------------------
	if (!parser.parse(vrmlFile))
	{
		fprintf(stderr, "%s parse failed\n", vrmlFile);
		return;
	}

	// ---------------------------------------------
	//  def�m�[�h���𓾂�
	// ---------------------------------------------
	std::vector<std::string> defNames = parser.getDefNames();
	std::vector<std::string>::iterator itr;

	printf("*** def names in %s ***\n", vrmlFile);

	for (itr=defNames.begin(); itr!=defNames.end(); itr++)
	{
		std::string defName = *itr;

		printf("%s\n", defName.c_str());
	}
}
