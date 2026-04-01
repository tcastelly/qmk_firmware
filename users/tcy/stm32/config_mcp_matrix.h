/* ============================================================
 * MCU: STM32F072CBT6 (Cortex-M0, 48MHz, 48-pin LQFP)
 * ============================================================ */

/* ── Matrix ─────────────────────────────────────────────────
 * Right half: 4 rows × 6 cols directly on MCU
 * Left half:  4 rows × 6 cols via MCP23017 over I2C2
 * ─────────────────────────────────────────────────────────── */
#define MATRIX_ROWS 8
#define MATRIX_COLS 12

#define MATRIX_ROW_PINS_MCU { A0, A1, A2, A3 }
#define MATRIX_COL_PINS_MCU { B0, B1, B2, B3, B4, B5 }

#define MATRIX_ROW_PINS_MCP { B0, B1, B2, B3 }
#define MATRIX_COL_PINS_MCP { A0, A1, A2, A3, A4, A5 }

#define MATRIX_ROW_PINS { B0, B1, B2, B3, A0, A1, A2, A3 }
#define MATRIX_COL_PINS { A0, A1, A2, A3, A4, A5, B0, B1, B2, B3, B4, B5 }
