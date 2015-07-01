#include "CResourceManager.h"
#include "CGlobalContainer.h"

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
	CContainer::Resource::Delete(r_resourceName.c_str());
}

bool CResourceManager::IsValidFile(std::string szFileScript)
{
	std::string file("lua_scripts/" + r_resourceName + "/" + szFileScript);
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

void CResourceManager::LoadResource()
{
	std::string xmlFile("lua_scripts/" + r_resourceName + "/meta.xml");
	XMLDocument doc;
	XMLError eResult = doc.LoadFile(xmlFile.c_str());

	if (eResult == XML_SUCCESS)
	{
		XMLNode * pRoot = doc.FirstChild();
		if (pRoot == nullptr)
		{
			CUtility::printf("Unable to handle the XML file!");
			delete this;
			return;
		}

		XMLElement * pElement = pRoot->FirstChildElement("script");
		while (pElement != nullptr)
		{
			std::string srcFile(pElement->Attribute("src"));
			bool IsValid = IsValidFile(srcFile.c_str());

			if (IsValid)
			{
				CLuaManager *luaScript = new CLuaManager(r_resourceName, srcFile);
				//CUtility::printf("A: %s", luaScript->GetResourceName().c_str());
				r_validScripts.push_back(luaScript);
			}
			else
			{
				CUtility::printf("Unable to load the file: %s (file not exists)", srcFile.c_str());
				return;
			}

			pElement = pElement->NextSiblingElement("script");
		}

		r_isScriptsValid = true;
	}
	else
	{
		CUtility::printf("Unable to load '%s' resource! (unable to open XML file)", r_resourceName.c_str());
		delete this;
	}
}

void CResourceManager::StartResource(void)
{
	for (std::vector<CLuaManager *>::iterator it = r_validScripts.begin(); it != r_validScripts.end(); ++it)
	{
		(*it)->InitVM();
		(*it)->StartLua();

		CContainer::LuaResourceManager::Add((*it)->GetVirtualMachine(), this);
	}

	//r_isActive = true;
	r_isRunning = true;
}

void CResourceManager::StopResource(void)
{
	std::map<lua_State *, CResourceManager *> tempContainer = CContainer::LuaResourceManager::GetContainer();
	for (std::map<lua_State *, CResourceManager *>::iterator it = tempContainer.begin(); it != tempContainer.end(); ++it)
	{
		if (it->second == this)
		{
			CLuaManager *luaManager = CContainer::LuaManager::Get(it->first);
			//CUtility::printf("%d", luaManager->GetVirtualMachine());

			delete luaManager;
			CContainer::LuaResourceManager::Delete(it->first);
			CContainer::LuaReference::Delete(it->first);

			lua_close(luaManager->GetVirtualMachine());
		}
	}

	delete this;
}