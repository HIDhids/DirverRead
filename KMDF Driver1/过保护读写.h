#pragma once

int ReadProcessMemoryForPid(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize);

int ReadProcessMemoryForPid2(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize);
