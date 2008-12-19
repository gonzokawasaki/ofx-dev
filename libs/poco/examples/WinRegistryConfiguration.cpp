//
// WinRegistryConfiguration.cpp
//
// $Id: //poco/1.3/Util/src/WinRegistryConfiguration.cpp#3 $
//
// Library: Util
// Package: Windows
// Module:  WinRegistryConfiguration
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Util/WinRegistryConfiguration.h"
#include "Poco/Util/WinRegistryKey.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"


namespace Poco {
namespace Util {


WinRegistryConfiguration::WinRegistryConfiguration(const std::string& rootPath): _rootPath(rootPath)
{
	// rootPath must end with backslash
	std::string::iterator it = _rootPath.end();
	if (*(--it) != '\\')
		_rootPath.append("\\");
}


WinRegistryConfiguration::~WinRegistryConfiguration()
{
}


bool WinRegistryConfiguration::getRaw(const std::string& key, std::string& value) const
{
	std::string keyName;
	std::string fullPath = _rootPath + ConvertToRegFormat(key, keyName);
	WinRegistryKey aKey(fullPath);
	bool exists = aKey.exists(keyName);
	if (exists)
	{
		WinRegistryKey::Type type = aKey.type(keyName);

		switch (type)
		{
		case WinRegistryKey::REGT_STRING:
			value = aKey.getString(keyName);
			break;
		case WinRegistryKey::REGT_STRING_EXPAND:
			value = aKey.getStringExpand(keyName);
			break;
		case WinRegistryKey::REGT_DWORD:
			value = Poco::NumberFormatter::format(aKey.getInt(keyName));
			break;
		default:
			exists = false;
		}
	}
	return exists;
}


void WinRegistryConfiguration::setRaw(const std::string& key, const std::string& value)
{
	std::string keyName;
	std::string fullPath = _rootPath+ConvertToRegFormat(key, keyName);
	WinRegistryKey aKey(fullPath);
	aKey.setString(keyName, value);
}


void WinRegistryConfiguration::enumerate(const std::string& key, Keys& range) const
{
	if (key.empty())
	{
		// return all root level keys
		range.push_back("HKEY_CLASSES_ROOT");
		range.push_back("HKEY_CURRENT_CONFIG");
		range.push_back("HKEY_CURRENT_USER");
		range.push_back("HKEY_LOCAL_MACHINE");
		range.push_back("HKEY_PERFORMANCE_DATA");
		range.push_back("HKEY_USERS");
	}
	else
	{
		std::string keyName;
		std::string fullPath = _rootPath+ConvertToRegFormat(key, keyName);
		WinRegistryKey aKey(fullPath);
		aKey.values(range);
		aKey.subKeys(range);
	}
}


std::string WinRegistryConfiguration::ConvertToRegFormat(const std::string& key, std::string& value) const
{
	std::size_t pos = key.rfind('.');
	if (pos == std::string::npos)
	{
		value = key;
		return std::string();
	}
	std::string prefix(key.substr(0,pos));
	value = key.substr(pos+1);
	Poco::translateInPlace(prefix, ".", "\\");
	return prefix;
}


} } // namespace Poco::Util