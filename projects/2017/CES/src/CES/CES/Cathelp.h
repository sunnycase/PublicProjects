#pragma once
#include <comcat.h>

// Helper function to create a component category and associated
// description
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription);

// Helper function to register a CLSID as belonging to a component
// category
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid);

// HRESULT UnRegisterCLSIDInCategory - Remove entries from the registry 
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid);