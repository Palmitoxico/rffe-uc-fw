#include "boot.h"
#include "chip.h"

void execute_user_code(void)
{
    USER_ENTRY_PFN user_entry;

    user_entry = (USER_ENTRY_PFN)*((uint32_t*)(USER_FLASH_START_ADDR +4));
    if (user_entry) {
        (user_entry)();
    }
}

int main (void)
{
    SystemCoreClockUpdate();

    /* user_fw_id = *(uint32_t*)USER_FLASH_ID_ADDR; */
    /* upgr_fw_id = *(uint32_t*)UPGRADE_FLASH_ID_ADDR; */

    execute_user_code();

    while (1);
}
