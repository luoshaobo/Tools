#include "CeRegDump.h"


const std::wstring		CeRegDump::WSTR_HKEY_CLASSES_ROOT(L"HKEY_CLASSES_ROOT");
const std::wstring		CeRegDump::WSTR_HKEY_CURRENT_USER(L"HKEY_CURRENT_USER");
const std::wstring		CeRegDump::WSTR_HKEY_LOCAL_MACHINE(L"HKEY_LOCAL_MACHINE");
const std::wstring		CeRegDump::WSTR_HKEY_USERS(L"HKEY_USERS");


bool CeRegDump::traverse() {

	bool ret = true;
	LONG nRet = 0;
	DWORD dwCnt = 0;
	
	if (m_init) {
		// create root node to initialise the stack
		m_rootNode = createNode();
		// DFS
		std::list< KeyNode* > stack;
		if (m_rootNode) {
		
			m_rootNode->m_ancester = 0;
			if ( !m_subKeyName.empty() ) {
				// we don't store the name of the predefined key
				m_rootNode->m_keyName = m_subKeyName;
				stack.push_front(m_rootNode);
				CRD_LOG_PRINT(LOG_INFO, "m_rootNode->m_keyName: [%s]", m_rootNode->m_keyName.c_str() );
			}
			else {
#ifndef UNDER_CE			
				m_rootNode->m_keyName.clear();
#else
				m_rootNode->m_keyName.assign(L"");
#endif			
				handleNode(m_hPredefinedKey, m_rootNode, stack);
			}
			
			// start iteration, root key is already opened
			do {
				CRD_LOG_PRINT( LOG_INFO, "loop count: %d", dwCnt++ );
				// all things needed are in elem
				KeyNode* elem = stack.front();
				stack.pop_front();
				// open the key
				HKEY hSubKey;
				nRet = ::RegOpenKeyEx( m_hPredefinedKey, elem->m_keyName.c_str(), 0, 0, &hSubKey );
				if ( ERROR_SUCCESS == nRet ) {
				
					CRD_LOG_PRINT( LOG_INFO, "key: [%s][%s] opened", m_predefinedKeyName.c_str(), elem->m_keyName.c_str() );
					handleNode(hSubKey, elem, stack);
					// remember to close the current key !!!
					nRet = ::RegCloseKey(hSubKey);
					if ( ERROR_SUCCESS == nRet ) {
						CRD_LOG_PRINT( LOG_INFO, "key: [%s][%s] closed", m_predefinedKeyName.c_str(), elem->m_keyName.c_str() );
					}
					else {
						CRD_LOG_PRINT( LOG_ERROR, "::RegCloseKey() failed, errorcode: %d", ::GetLastError() );
					}
				}
				else {
					CRD_LOG_PRINT( LOG_ERROR, "::RegOpenKeyEx() failed, errorcode: %d", ::GetLastError() );
					ret = false;
				}

			} while ( !stack.empty() );
		}
	}
	else {
		ret = false;
	}
	
	return ret;
}

// call ::RegQueryInfoKey()
bool CeRegDump::extractKeyInfo(HKEY hKey, KeyInfo& info) {

	bool ret = true;
	enum { BUFF_SIZE = 500 };
	WCHAR buff[ BUFF_SIZE +1 ];
	::memset( buff, 0, sizeof(buff) );
	DWORD dwSize = BUFF_SIZE;
	
	LONG nRet = ::RegQueryInfoKey( hKey 
									, buff, &dwSize, 0
									, &(info.m_subKeyNum), &(info.m_maxSubKeyLen)
									, &(info.m_maxClassLen), &(info.m_valueNum)
									, &(info.m_maxValueNameLen), &(info.m_maxValueDataLen)
									, 0, 0 );
										
	CRD_LOG_PRINT( LOG_INFO, "::RegQueryInfoKey(): \n\t\
m_subKeyNum: %d,\n\t\
m_maxSubKeyLen: %d\n\t\
m_valueNum: %d,\n\t\
m_maxValueNameLen: %d,\n\t\
m_maxValueDataLen: %d\n\n", info.m_subKeyNum, info.m_maxSubKeyLen, info.m_valueNum, info.m_maxValueNameLen, info.m_maxValueDataLen );
	
	if (ERROR_SUCCESS == nRet) {
		info.m_class.assign(buff, dwSize);
	}
	else {
		m_dwQIKErrorCnt++;
		ret = false;
	}
	
	return ret;
}


// call ::RegEnumValue()
bool CeRegDump::extractKeyValue(HKEY hKey, KeyInfo& info, std::list<KeyValue*>& list) {

	bool ret = true;
	DWORD dwIndex = info.m_valueNum;
	const DWORD NAME_BUFF_SIZE = info.m_maxValueNameLen + SPARED_TAIL_SIZE;
	const DWORD DATA_BUFF_SIZE = info.m_maxValueDataLen + SPARED_TAIL_SIZE;
	DWORD dwNameSize = NAME_BUFF_SIZE;
	DWORD dwDataSize = DATA_BUFF_SIZE;
	DWORD dwType;
	WCHAR* nameBuff = new WCHAR [ NAME_BUFF_SIZE + 1 ];
	BYTE* dataBuff = new BYTE [ DATA_BUFF_SIZE + 1 ];
	
	if (nameBuff && dataBuff) {
	
		::memset( nameBuff, 0, sizeof(WCHAR) * (NAME_BUFF_SIZE + 1) );
		::memset( dataBuff, 0, sizeof(BYTE) * (DATA_BUFF_SIZE + 1) );
		
		while (dwIndex) {
		
			CRD_LOG_PRINT(LOG_INFO, "CeRegDump::extractKeyValue(),\
m_maxValueNameLen: %d,\n\t\
m_maxValueDataLen: %d\n\t\
dwNameSize: %d,\n\t\
dwDataSize: %d,\n\n", info.m_maxValueNameLen, info.m_maxValueDataLen, dwNameSize, dwDataSize);

			LONG nRet = ::RegEnumValue( hKey, --dwIndex 
										, nameBuff, &dwNameSize
										, 0, &dwType
										, dataBuff, &dwDataSize );
										
			if ( ERROR_SUCCESS == nRet ) {
			
				KeyValue* item = KeyNode::createKeyValueOnHeap();
				if (item) {
					item->m_valueName.assign(nameBuff);
					item->m_valueType = dwType;
					item->m_valueData.assign(dataBuff, dataBuff + dwDataSize);
					
					list.push_back(item);
				}
			}
			else if ( ERROR_NO_MORE_ITEMS == nRet ) {
				break;
			}
			else {
				m_dwEVErrorCnt++;
				CRD_LOG_PRINT( LOG_ERROR, "::RegEnumValue() failed, index: %d, nRet: %d", dwIndex, nRet );
				// ret = false;
			}
			
			::memset( nameBuff, 0, sizeof(WCHAR) * (NAME_BUFF_SIZE + 1) );
			::memset( dataBuff, 0, sizeof(BYTE) * (DATA_BUFF_SIZE + 1) );
			dwNameSize = NAME_BUFF_SIZE;
			dwDataSize = DATA_BUFF_SIZE;
		}
		
		delete[] nameBuff;
		delete[] dataBuff;
	}
	else {
		ret = false;
	}
	
	return ret;
}


// call ::RegEnumKeyEx()
bool CeRegDump::extractSubKeysName(HKEY hKey, KeyInfo& info, std::list<std::wstring>& children) {

	bool ret = true;
	DWORD dwIndex = info.m_subKeyNum;
	const DWORD NAME_BUFF_SIZE = info.m_maxSubKeyLen + SPARED_TAIL_SIZE;
	DWORD dwNameSize = NAME_BUFF_SIZE;
	WCHAR* nameBuff = new WCHAR [ NAME_BUFF_SIZE + 1 ];
	
	if (nameBuff) {
	
		::memset( nameBuff, 0, sizeof(WCHAR) * (NAME_BUFF_SIZE + 1) );

		CRD_LOG_PRINT( LOG_INFO, "CeRegDump::extractSubKeysName(), dwIndex: %d", dwIndex );
		while (dwIndex) {
		
			LONG nRet = ::RegEnumKeyEx( hKey, --dwIndex
										, nameBuff, &dwNameSize
										, 0, 0, 0, 0 );
										
			if ( ERROR_SUCCESS == nRet ) {
			
				children.push_back(nameBuff);
				// CRD_LOG_PRINT( LOG_INFO, "sub-key extracted: [%s]", nameBuff );
			}
			else if ( ERROR_NO_MORE_ITEMS == nRet ) {
				break;
			}
			else {
				m_dwEKErrorCnt++;
				CRD_LOG_PRINT( LOG_ERROR, "::RegEnumKeyEx() failed, index: %d, nRet: %d", dwIndex, nRet );
				// ret = false;
			}
			
			::memset( nameBuff, 0, sizeof(WCHAR) * (NAME_BUFF_SIZE + 1) );
			dwNameSize = NAME_BUFF_SIZE;
		}
		
		delete[] nameBuff;
	}
	else {
		ret = false;
	}
	
	return ret;
}


bool CeRegDump::extractInfoToNode(HKEY hKey, KeyNode* tar, std::list<std::wstring>& children) {

	bool ret = true;
	
	if ( extractKeyInfo( hKey, tar->m_info ) ) {
	
		extractKeyValue(hKey, tar->m_info, tar->m_values);
		m_dwValueCnt += tar->m_values.size();
		extractSubKeysName(hKey, tar->m_info, children);
	}
	else {
		ret = false;
	}
	
	return ret;
}


bool CeRegDump::extractFromFullName(const std::wstring& fullName, std::wstring& predefinedName, std::wstring& subKeyName) {

	bool ret = true;
	size_t itr = fullName.find(L'\\', 0);
	if (std::wstring::npos != itr) {
	
		predefinedName.assign(fullName, 0, itr);
		if ( fullName.length() == itr + 1 ) {
#ifndef UNDER_CE			
			subKeyName.clear();
#else
			subKeyName.assign(L"");
#endif						
		}
		else {
		// subkey name does not include the slash
			subKeyName.assign( fullName, itr + 1, std::wstring::npos );
		}
	}
	else {
		if (WSTR_HKEY_CLASSES_ROOT == fullName) {
			predefinedName.assign(fullName);
#ifndef UNDER_CE			
			subKeyName.clear();
#else
			subKeyName.assign(L"");
#endif			
		}
		else if (WSTR_HKEY_CURRENT_USER == fullName) {
			predefinedName.assign(fullName);
#ifndef UNDER_CE			
			subKeyName.clear();
#else
			subKeyName.assign(L"");
#endif						
		}
		else if (WSTR_HKEY_LOCAL_MACHINE == fullName) {
			predefinedName.assign(fullName);
#ifndef UNDER_CE			
			subKeyName.clear();
#else
			subKeyName.assign(L"");
#endif						
		}
		else if (WSTR_HKEY_USERS == fullName) {
			predefinedName.assign(fullName);
#ifndef UNDER_CE			
			subKeyName.clear();
#else
			subKeyName.assign(L"");
#endif						
		}
		else {
			ret = false;
		}
	}
	
	return ret;
}


bool CeRegDump::mapToPredefinedHKEY(const std::wstring& name, HKEY& key) {

	bool ret = true;
	
	if (WSTR_HKEY_CLASSES_ROOT == name) {
		key = HKEY_CLASSES_ROOT;
	}
	else if (WSTR_HKEY_CURRENT_USER == name) {
		key = HKEY_CURRENT_USER;
	}
	else if (WSTR_HKEY_LOCAL_MACHINE == name) {
		key = HKEY_LOCAL_MACHINE;
	}
	else if (WSTR_HKEY_USERS == name) {
		key = HKEY_USERS;
	}
	else {
		ret = false;
	}
	
	return ret;
}


void CeRegDump::handleNode(HKEY hSubKey, KeyNode* elem, std::list< KeyNode* >& stack) {

	// extract the info and fulfill the node, fill the implementation of the following function
	std::list<std::wstring> childrenName;
	extractInfoToNode(hSubKey, elem, childrenName);	
	
	// for each child, create the node, link to the ancster, then push to the stack
	std::list<std::wstring>::iterator itr;
	for ( itr = childrenName.begin(); itr != childrenName.end(); itr++ ) {
		KeyNode* node = createNode();
		if (node) {
			node->m_ancester = elem;
			node->m_keyName = elem->m_keyName + L'\\' + (*itr);
			CRD_LOG_PRINT(LOG_INFO, "new child node: [%s]", node->m_keyName.c_str() );
			
			elem->m_childList.push_back(node);
			stack.push_front(node);
		}
	}
}


void CeRegDump::releaseTree(KeyNode* start) {

	std::list< KeyNode* > stack;
	if (start) {
	
		stack.push_front(start);
		do {
		
			KeyNode* node = stack.front();
			stack.pop_front();
			std::list<KeyNode*>::iterator itr;
			for ( itr = node->m_childList.begin(); itr != node->m_childList.end(); itr++ ) {
			
				stack.push_front(*itr);
			}
			destoryNode(node);
		
		} while ( !stack.empty() );		
	}
}


bool CeRegDump::dump(const std::wstring& tar) {

	bool ret = true;
	
	FILE* p = ::_wfopen( tar.c_str(), L"wb+" );
	
	if (p) {
	
		std::list< KeyNode* > stack;
		stack.push_front(m_rootNode);
		
		std::wstring summary(L"******************** CeRegDump SUMMARY ********************\n\n");
		std::wstring start(L"\n\n******************** CeRegDump START ********************\n\n"); 
		std::wstring end(L"\n\n******************** CeRegDump END ********************\n\n");
		
		::fwrite( summary.c_str(), sizeof(WCHAR), summary.length(), p );
		
		enum { BUFF_SIZE = 100 };
		WCHAR buff[ BUFF_SIZE + 1 ];
		
		::memset( buff, 0, sizeof(buff) );
		::swprintf(buff, L"Number of Nodes: %d\n", m_dwNodeCnt);
		::fwrite( buff, sizeof(WCHAR), ::wcslen(buff), p );
		
		::memset( buff, 0, sizeof(buff) );
		::swprintf(buff, L"Number of Values: %d\n", m_dwValueCnt);
		::fwrite( buff, sizeof(WCHAR), ::wcslen(buff), p );
		
		::memset( buff, 0, sizeof(buff) );
		::swprintf(buff, L"Error when calling ::RegQueryInfoKey(): %d\n", m_dwQIKErrorCnt);
		::fwrite( buff, sizeof(WCHAR), ::wcslen(buff), p );	
	
		::memset( buff, 0, sizeof(buff) );
		::swprintf(buff, L"Error when calling ::RegEnumValue(): %d\n", m_dwEVErrorCnt);
		::fwrite( buff, sizeof(WCHAR), ::wcslen(buff), p );		
		
		::memset( buff, 0, sizeof(buff) );
		::swprintf(buff, L"Error when calling ::RegEnumKeyEx(): %d\n", m_dwEKErrorCnt);
		::fwrite( buff, sizeof(WCHAR), ::wcslen(buff), p );			
		
		::fwrite( start.c_str(), sizeof(WCHAR), start.length(), p );
		do {
		
			KeyNode* node = stack.front();
			stack.pop_front();
			std::list<KeyNode*>::iterator itr;
			for ( itr = node->m_childList.begin(); itr != node->m_childList.end(); itr++ ) {
				
				stack.push_front(*itr);
			}
			
			std::list<KeyValue*>::iterator valItr;
			for ( valItr = node->m_values.begin(); valItr != node->m_values.end(); valItr++ ) {
				
				std::wstring result( L"[" + m_predefinedKeyName + L"\\" + node->m_keyName + L"\\" + (*valItr)->m_valueName + L"]\n\t" );		
				std::wstring value;
				
				(*valItr)->format(value);
				result.append(value);
				
				if ( !result.empty() ) {
					::fwrite( result.c_str(), sizeof(WCHAR), result.length(), p );
				}
			}
		
		} while ( !stack.empty() );
		::fwrite( end.c_str(), sizeof(WCHAR), end.length(), p );
		
		::fclose(p);
	}
	else {
		ret = false;
	}
	
	return ret;
}


bool CeRegDump::mapToPredefinedName(HKEY key) {

	bool ret = true;
	
	if ( HKEY_CLASSES_ROOT == key ) {
		m_predefinedKeyName = WSTR_HKEY_CLASSES_ROOT;
	}
	else if ( HKEY_CURRENT_USER == key ) {
		m_predefinedKeyName = WSTR_HKEY_CURRENT_USER;
	}
	else if ( HKEY_LOCAL_MACHINE == key ) {
		m_predefinedKeyName = WSTR_HKEY_LOCAL_MACHINE;
	}
	else if ( HKEY_USERS == key ) {
		m_predefinedKeyName = WSTR_HKEY_USERS;
	}
	else {
#ifndef UNDER_CE			
			m_predefinedKeyName.clear();
#else
			m_predefinedKeyName.assign(L"");
#endif						
		ret = false;
	}
	
	return ret;
}
