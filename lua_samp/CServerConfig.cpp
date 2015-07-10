#include "CServerConfig.h"
#include "CUtility.h"

CServerConfig::CServerConfig(std::string fileName)
{
	configFile = fileName;
	ParseFile();
}

CServerConfig::~CServerConfig()
{
	
}

void CServerConfig::ParseFile()
{
	configSettings.clear();

	std::ifstream confFile(configFile);
	std::string readLine;

	while (std::getline(confFile, readLine))
	{
		std::string key, value;
		int delimiterPos = readLine.find(" ");
		key = readLine.substr(0, delimiterPos);

		if (delimiterPos != std::string::npos)
			value = readLine.substr(delimiterPos + 1);

		if (value.find(" ") != std::string::npos)
		{
			std::stringstream valueStream(value);
			std::string values;
			while (std::getline(valueStream, values, ' '))
			{
				configSettings[key].push_back(values);
			}
		}
		else
		{
			configSettings[key].push_back(value);
		}
	}
}

std::string CServerConfig::GetSetting(std::string key, const int index)
{
	if (IsValidSetting(key))
	{
		if (index == -1)
		{
			std::stringstream ss;
			for (size_t i = 0; i < GetSettingSize(key); ++i)
			{
				if (i != 0)
					ss << " ";

				ss << configSettings[key][i];
			}

			return ss.str();
		}

		return configSettings[key][index];
	}

	return std::string();
}