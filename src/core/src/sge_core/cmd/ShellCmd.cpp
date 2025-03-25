#include "ShellCmd.h"
#include <sge_core/string/UtfUtil.h>
#include "CommandLine.h"

namespace sge {
#if 0
#pragma mark ========= Windows ============
#endif
#if SGE_OS_WINDOWS

void ShellCmd::onExec(Param* params, size_t n) {
	TempStringW tmpExecFileName;
	UtfUtil::convert(tmpExecFileName, _execFileName);
	tmpExecFileName.replaceChars(L'/', L'\\'); // Windows OS path separator need \ instead of /

	TempStringW tmpParams;
	TempStringW tmpW;
	TempStringW tmpParamAssignment;

	for (int i = 0; i < n; ++i) {
		const auto& param = params[i];
		if (param.empty()) continue;

		if (!tmpParams.empty())
			tmpParams.append(L" ");

		param.opNameW(tmpW); tmpParams.append(tmpW);

		param.opValueW(tmpW);
		if (!tmpW.empty()) {
			param.opAssignmentW(tmpParamAssignment); tmpParams.append(tmpParamAssignment);
			tmpParams.append(tmpW);
		}
	}

#if _DEBUG
	wprintf(L"[ShellCmd]:%ls %ls\n", tmpExecFileName.c_str(), tmpParams.c_str());
#endif

	::SHELLEXECUTEINFO ShExecInfo = {};
	ShExecInfo.cbSize			  = sizeof(ShExecInfo);
	ShExecInfo.fMask			  = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd				  = nullptr;
	ShExecInfo.lpVerb			  = L"open";
	ShExecInfo.lpFile			  = tmpExecFileName.c_str();
	ShExecInfo.lpParameters		  = tmpParams.c_str();
	ShExecInfo.lpDirectory		  = nullptr;
	ShExecInfo.nShow			  = SW_SHOW; // SW_HIDE
	ShExecInfo.hInstApp			  = nullptr;
	::ShellExecuteEx(&ShExecInfo);
	::WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	::CloseHandle(ShExecInfo.hProcess);
}

#endif // SGE_OS_WINDOWS

} // namespace sge