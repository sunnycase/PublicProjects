#include "stdafx.h"
#include "Cathelp.h"
#include "strsafe.h"
#include "objsafe.h"

// HRESULT CreateComponentCategory - Used to register ActiveX control as safe 
HRESULT CreateComponentCategory(CATID catid, WCHAR *catDescription)
{
	ICatRegister *pcr = NULL;
	HRESULT hr = S_OK;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		return hr;

	// Make sure the HKCR\Component Categories\{..catid...}
	// key is registered.
	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409; // english
	size_t len;
	// Make sure the provided description is not too long.
	// Only copy the first 127 characters if it is.
	// The second parameter of StringCchLength is the maximum
	// number of characters that may be read into catDescription.
	// There must be room for a NULL-terminator. The third parameter
	// contains the number of characters excluding the NULL-terminator.
	hr = StringCchLength(catDescription, STRSAFE_MAX_CCH, &len);
	if (SUCCEEDED(hr))
	{
		if (len>127)
		{
			len = 127;
		}
	}
	else
	{
		// TODO: Write an error handler;
	}
	// The second parameter of StringCchCopy is 128 because you need 
	// room for a NULL-terminator.
	hr = StringCchCopy(catinfo.szDescription, len + 1, catDescription);
	// Make sure the description is null terminated.
	catinfo.szDescription[len + 1] = '\0';

	hr = pcr->RegisterCategories(1, &catinfo);
	pcr->Release();

	return hr;
}

// HRESULT RegisterCLSIDInCategory -
//      Register your component categories information 
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	// Register your component categories information.
	ICatRegister *pcr = NULL;
	HRESULT hr = S_OK;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Register this category as being "implemented" by the class.
		CATID rgcatid[1];
		rgcatid[0] = catid;
		hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}

// HRESULT UnRegisterCLSIDInCategory - Remove entries from the registry 
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	ICatRegister *pcr = NULL;
	HRESULT hr = S_OK;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Unregister this category as being "implemented" by the class.
		CATID rgcatid[1];
		rgcatid[0] = catid;
		hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}