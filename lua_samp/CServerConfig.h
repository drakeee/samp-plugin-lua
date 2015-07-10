#ifndef _SERVER_CONFIG_HEADER
#define _SERVER_CONFIG_HEADER

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>

class CServerConfig
{
public:
	CServerConfig(std::string fileName);
	~CServerConfig();
	void			ParseFile();
	std::string		GetSetting(std::string key, const int index = -1);
	inline int		GetSettingSize(std::string key)
	{
		if (IsValidSetting(key))
		{
			return configSettings[key].size();
		}

		return 0;
	}
	inline bool		IsValidSetting(std::string key) { return !!configSettings.count(key); }
private:
	std::string										configFile;
	std::map<std::string, std::vector<std::string>> configSettings;
};

#endif