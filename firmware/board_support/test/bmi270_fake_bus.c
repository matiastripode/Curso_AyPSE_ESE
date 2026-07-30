/**
 * @file bmi270_fake_bus.c
 * @brief Implementación del doble de prueba de @ref bmi270_bus_t.
 * @author Matias Tripode
 */

#include "bmi270_fake_bus.h"

#include <string.h>

#define BMI270_FAKE_CHIP_ID_REG         0x00U
#define BMI270_FAKE_CHIP_ID_VALUE       0x24U
#define BMI270_FAKE_INTERNAL_STATUS_REG 0x21U
#define BMI270_FAKE_INTERNAL_STATUS_OK  0x01U
#define BMI270_FAKE_INIT_DATA_REG       0x5EU

/**
 * @brief Contexto del bus fake creado más recientemente.
 *
 * @ref bmi270_bus_t::delay_us no recibe un puntero de contexto, por lo que
 * este puntero es la única forma de que @ref fake_delay_us encuentre el
 * contexto sobre el cual registrar sus llamadas.
 */
static bmi270_fake_bus_ctx_t *active_ctx;

static bmi270_bus_error_t fake_read(void *context, uint8_t reg, uint8_t *data, size_t len) {
    if (context == NULL || data == NULL || len == 0U) {
        return BMI270_BUS_ERR_INVALID_ARG;
    }

    bmi270_fake_bus_ctx_t *ctx = context;
    ctx->read_count++;

    if (ctx->has_fail_read_reg && ctx->fail_read_reg == reg) {
        return BMI270_BUS_FAIL;
    }

    for (size_t i = 0; i < len; i++) {
        size_t index = ((size_t)reg + i) % BMI270_FAKE_BUS_REG_COUNT;
        data[i] = ctx->regs[index];
    }

    if (ctx->has_read_override_reg && ctx->read_override_reg == reg) {
        data[0] = ctx->read_override_value;
    }

    return BMI270_BUS_OK;
}

static bmi270_bus_error_t fake_write(void *context, uint8_t reg, const uint8_t *data, size_t len) {
    if (context == NULL || data == NULL || len == 0U) {
        return BMI270_BUS_ERR_INVALID_ARG;
    }

    bmi270_fake_bus_ctx_t *ctx = context;
    ctx->write_count++;
    ctx->last_write_reg = reg;
    ctx->last_write_len = len;
    ctx->last_write_len_by_reg[reg] = len;

    if (ctx->has_fail_write_reg && ctx->fail_write_reg == reg) {
        return BMI270_BUS_FAIL;
    }

    if (reg == BMI270_FAKE_INIT_DATA_REG) {
        /* INIT_DATA es una ventana de carga hacia la RAM interna del firmware
         * del sensor: un burst de hasta 32 bytes no incrementa la dirección
         * sobre el mapa de registros "normal" (a diferencia de otros
         * registros contiguos como INIT_ADDR_0/INIT_ADDR_1). Modelarlo como
         * un auto-incremento genérico corrompería registros no relacionados
         * (p.ej. PWR_CTRL, que cae 31 bytes más adelante). Solo se registra
         * el byte final del bloque para poder inspeccionarlo si hiciera falta. */
        ctx->regs[reg] = data[len - 1U];
        return BMI270_BUS_OK;
    }

    for (size_t i = 0; i < len; i++) {
        size_t index = ((size_t)reg + i) % BMI270_FAKE_BUS_REG_COUNT;
        ctx->regs[index] = data[i];
    }

    return BMI270_BUS_OK;
}

static void fake_delay_us(uint32_t us) {
    if (active_ctx == NULL) {
        return;
    }
    if (active_ctx->delay_count < BMI270_FAKE_BUS_DELAY_LOG) {
        active_ctx->delay_log[active_ctx->delay_count] = us;
    }
    active_ctx->delay_count++;
}

void BMI270FakeBusReset(bmi270_fake_bus_ctx_t *ctx) {
    if (ctx == NULL) {
        return;
    }

    memset(ctx, 0, sizeof(*ctx));
    ctx->regs[BMI270_FAKE_CHIP_ID_REG] = BMI270_FAKE_CHIP_ID_VALUE;
    ctx->regs[BMI270_FAKE_INTERNAL_STATUS_REG] = BMI270_FAKE_INTERNAL_STATUS_OK;
}

bmi270_bus_error_t BMI270FakeBusCreate(bmi270_bus_t *bus, bmi270_fake_bus_ctx_t *ctx) {
    if (bus == NULL || ctx == NULL) {
        return BMI270_BUS_ERR_INVALID_ARG;
    }

    bus->context = ctx;
    bus->read = fake_read;
    bus->write = fake_write;
    bus->delay_us = fake_delay_us;

    active_ctx = ctx;

    return BMI270_BUS_OK;
}
