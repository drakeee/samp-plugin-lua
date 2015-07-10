#include "CResourceManager.h"
#include "CCore.h"

using namespace tinyxml2;

CResourceManager::CResourceManager(const char* rN)
{
	//CUtility::printf("Constructor");
	r_resourceName = rN;
	r_isRunning = false;
	//r_isActive = false;
	r_isScriptsValid = false;
}

CResourceManager::~CResourceManager()
{
	//CUtility::printf("Destructor");
	s_Core->DeleteResource(r_resourceName.c_str());
}

bool CResourceManager::IsValidFile(std::string szFileScript)
{
	std::string file(LUA_RESOURCES_FOLDER + r_resourceName + "/" + szFileScript);
	std::ifstream fileScript(file);
	if (fileScript.good())
	{
		fileScript.close();
		return true;
	}
	else
	{
		fileScript.close();
		return false;
	}
}

int CResourceManager::LoadResource()
{
	std::string xmlFile(LUA_RESOURCES_FOLDER + r_resourceName + "/meta.xml");
	tinyxml2::XMLDocument doc;
	XMLError eResult = doc.LoadFile(xmlFile.c_str());

	if (eResult == XML_SUCCESS)
	{
		XMLNode * pRoot = doc.FirstChild();
		if (pRoot == nullptr)
		{
			CUtility::printf("Unable to handle the XML file!");
			delete this;
			return 1;
		}

		XMLElement * pElement = pRoot->FirstChildElement("script");
		CLuaManager *luaScript = new CLuaManager(r_resourceName);

		while (pElement != nullptr)
		{
			std::string srcFile(pElement->Attribute("src"));
			bool IsValid = IsValidFile(srcFile.c_str());

			if (IsValid)
			{
				
				//CUtility::printf("A: %s", luaScript->GetResourceName().c_str());
				luaScript->AddFile(srcFile);
				//r_validScripts.push_back(luaScript);
			}
			else
			{
				CUtility::printf("Unable to load the file: %s (file not exists)", srcFile.c_str());
				delete luaScript;
				return 2;
			}

			pElement = pElement->NextSiblingElement("script");
		}

		r_validScript = luaScript;
		r_isScriptsValid = true;
	}
	else
	{
		CUtility::printf("Unable to load '%s' resource! (unable to open XML file)", r_resourceName.c_str());
		delete this;
		return 3;
	}

	return 4;
}

void CResourceManager::StartResource(void)
{
	r_validScript->StartLua();
	s_Core->RegisterLuaVM(r_validScript->GetVirtualMachine(), this);

	//r_isActive = true;
	r_isRunning = true;
}

void CResourceManager::StopResource(void)
{
	std::map<lua_State *, CResourceManager *> *tempContainer = s_Core->GetLuaResourceContainer();
	for (std::map<lua_State *, CResourceManager *>::iterator it = (*tempContainer).begin(); it != (*tempContainer).end(); ++it)
	{
		if (it->second == this)
		{
			CLuaManager *luaManager = s_Core->GetLuaManager(it->first);
			//CUtility::printf("%d", luaManager->GetVirtualMachine());

			auto *tempContainer = s_Core->GetModuleContainer();
			for (auto it = (*tempContainer).begin(); it != (*tempContainer).end(); ++it)
			{
				(*it)->_ResourceStopping(luaManager->GetVirtualMachine());
			}
			lua_close(luaManager->GetVirtualMachine());

			delete luaManager;
			s_Core->DeleteLuaVM(it->first);
			s_Core->DeleteReference(it->first);
			break;
		}
	}

	delete this;
}