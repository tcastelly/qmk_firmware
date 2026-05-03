#include <stdbool.h>
#include "matrix.h"

void ps2_stm32_init(void);
void ps2_stm32_scan(void);

void mcp_matrix_init_custom(void);
bool mcp_matrix_scan_custom(matrix_row_t current_matrix[]);

void matrix_init_custom(void) {
    // --- Initialize PS/2 Trackpoint ---
#ifdef PS2_CUSTOM_ENABLE
    ps2_stm32_init();
#endif

    mcp_matrix_init_custom();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    // --- Run PS/2 Receiver Logic ---
#ifdef PS2_CUSTOM_ENABLE
    ps2_stm32_scan();
#endif

  return mcp_matrix_scan_custom(current_matrix);
}
