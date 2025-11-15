#include "error_handling.h"

#ifdef PLATFORM_DS
#include <nitro.h>
#else
#include "platform/platform_types.h"
#endif
#include <string.h>

#include "error_message_reset.h"
#include "unk_020366A0.h"

void ErrorHandling_AssertFail(void);

void ErrorHandling_AssertFail(void)
{
    if (CommMan_IsInitialized() && (OS_GetProcMode() != OS_PROCMODE_IRQ)) {
        ErrorMessageReset_PrintErrorAndReset();
    }
}
