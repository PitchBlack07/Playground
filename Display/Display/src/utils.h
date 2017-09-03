#pragma once
#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#ifdef __cplusplus
template<typename T>
void SAFE_RELEASE(T*& x) { if (x) { x->Release(); x = nullptr; } }
#else

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; }
#endif

#endif

#ifndef SET_DEBUG_NAME
#ifdef _DEBUG
#define SET_DEBUG_NAME(x, name)  x->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
#else
#define SET_DEBUG_NAME(x, name)
#endif
#endif

#endif
