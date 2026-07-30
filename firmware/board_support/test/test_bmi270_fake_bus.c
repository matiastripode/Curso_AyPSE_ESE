#include <string.h>

#include "unity.h"
#include "bmi270.h"
#include "bmi270_bus.h"
#include "bmi270_config.h"
#include "bmi270_fake_bus.h"

/* Direcciones de registro del BMI270, replicadas de bmi270.c (son estáticas
 * allí y no forman parte de la interfaz pública). */
#define REG_CMD             0x7EU
#define REG_CHIP_ID         0x00U
#define REG_PWR_CONF        0x7CU
#define REG_INIT_CTRL       0x59U
#define REG_INIT_ADDR_0     0x5BU
#define REG_INIT_DATA       0x5EU
#define REG_INTERNAL_STATUS 0x21U
#define REG_ACC_CONF        0x40U
#define REG_ACC_RANGE       0x41U
#define REG_PWR_CTRL        0x7DU
#define REG_STATUS          0x03U
#define REG_DATA_8          0x0CU

#define BMI270_STATUS_DRDY_ACC 0x80U
#define BMI270_ACC_CONF_EXPECTED 0xA8U
#define BMI270_PWR_CTRL_ACC_EN   0x04U

static void MakeValidBus(bmi270_bus_t *bus, bmi270_fake_bus_ctx_t *ctx) {
    BMI270FakeBusReset(ctx);
    bmi270_bus_error_t result = BMI270FakeBusCreate(bus, ctx);
    TEST_ASSERT_TRUE(result == BMI270_BUS_OK);
}

/* ---- A. Validación de argumentos ---------------------------------------- */

TEST_CASE("BMI270Init null bus", "[board_support][bmi270][fake_bus]")
{
    TEST_ASSERT_TRUE(BMI270Init(NULL) == BMI270_ERR_INVALID_ARG);
}

TEST_CASE("BMI270Init null context", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    bus.context = NULL;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_ARG);
}

TEST_CASE("BMI270Init null read callback", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    bus.read = NULL;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_ARG);
}

TEST_CASE("BMI270Init null write callback", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    bus.write = NULL;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_ARG);
}

TEST_CASE("BMI270Init null delay_us callback", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    bus.delay_us = NULL;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_ARG);
}

/* ---- B. Fallas de comunicación / verificación ---------------------------- */

TEST_CASE("BMI270Init soft reset write fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_write_reg = true;
    ctx.fail_write_reg = REG_CMD;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_STATE);
}

TEST_CASE("BMI270Init chip id read fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_read_reg = true;
    ctx.fail_read_reg = REG_CHIP_ID;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_STATE);
}

TEST_CASE("BMI270Init wrong chip id", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.regs[REG_CHIP_ID] = 0x00U;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);
}

TEST_CASE("BMI270Init fails when PWR_CONF write fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_write_reg = true;
    ctx.fail_write_reg = REG_PWR_CONF;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);
}

TEST_CASE("BMI270Init fails when config block write fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_write_reg = true;
    ctx.fail_write_reg = REG_INIT_DATA;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);
}

TEST_CASE("BMI270Init fails when INTERNAL_STATUS read fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_read_reg = true;
    ctx.fail_read_reg = REG_INTERNAL_STATUS;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);
}

TEST_CASE("BMI270Init fails when INTERNAL_STATUS message is not OK", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.regs[REG_INTERNAL_STATUS] = 0x00U;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_STATE);
}

TEST_CASE("BMI270Init fails when ACC_CONF write fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_write_reg = true;
    ctx.fail_write_reg = REG_ACC_CONF;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);
}

TEST_CASE("BMI270Init fails when ACC_RANGE write fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_write_reg = true;
    ctx.fail_write_reg = REG_ACC_RANGE;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);
}

TEST_CASE("BMI270Init fails when PWR_CTRL read fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_fail_read_reg = true;
    ctx.fail_read_reg = REG_PWR_CTRL;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);
}

TEST_CASE("BMI270Init fails when final ACC_CONF verification mismatches", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_read_override_reg = true;
    ctx.read_override_reg = REG_ACC_CONF;
    ctx.read_override_value = 0x00U;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_STATE);
}

TEST_CASE("BMI270Init fails when final ACC_RANGE verification mismatches", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_read_override_reg = true;
    ctx.read_override_reg = REG_ACC_RANGE;
    ctx.read_override_value = 0x01U;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_STATE);
}

TEST_CASE("BMI270Init fails when accel enable bit is not set", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    ctx.has_read_override_reg = true;
    ctx.read_override_reg = REG_PWR_CTRL;
    ctx.read_override_value = 0x00U;

    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_ERR_INVALID_STATE);
}

/* ---- C. Happy path observable solo a través del fake ------------------- */

TEST_CASE("BMI270Init happy path succeeds and computes expected register values", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);

    bmi270_error_t result = BMI270Init(&bus);

    TEST_ASSERT_TRUE(result == BMI270_OK);
    TEST_ASSERT_EQUAL_UINT8(BMI270_ACC_CONF_EXPECTED, ctx.regs[REG_ACC_CONF]);
    TEST_ASSERT_EQUAL_UINT8(0x00U, ctx.regs[REG_ACC_RANGE] & 0x03U);
    TEST_ASSERT_TRUE((ctx.regs[REG_PWR_CTRL] & BMI270_PWR_CTRL_ACC_EN) != 0U);
}

TEST_CASE("BMI270Init writes config file in 32 byte blocks", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);

    bmi270_error_t result = BMI270Init(&bus);
    TEST_ASSERT_TRUE(result == BMI270_OK);

    size_t remainder = bmi270_config_file_size % 32U;
    size_t expected_last_block_len = (remainder == 0U) ? 32U : remainder;

    /* La última escritura sobre INIT_DATA debe llevar el tamaño del bloque
     * final calculado a partir de bmi270_config_file_size, aun cuando el
     * driver siga escribiendo otros registros después (ACC_CONF, etc.). */
    TEST_ASSERT_EQUAL_UINT32(expected_last_block_len, ctx.last_write_len_by_reg[REG_INIT_DATA]);
}

TEST_CASE("BMI270Init preserves unrelated PWR_CTRL bits", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    /* Simula que el giróscopo ya estaba habilitado antes de inicializar el
     * acelerómetro (bit 1 = gyr_en). */
    ctx.regs[REG_PWR_CTRL] = 0x02U;

    bmi270_error_t result = BMI270Init(&bus);

    TEST_ASSERT_TRUE(result == BMI270_OK);
    TEST_ASSERT_EQUAL_UINT8(0x02U | BMI270_PWR_CTRL_ACC_EN, ctx.regs[REG_PWR_CTRL]);
}

TEST_CASE("BMI270Init issues the documented delays", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);

    bmi270_error_t result = BMI270Init(&bus);
    TEST_ASSERT_TRUE(result == BMI270_OK);

    TEST_ASSERT_EQUAL_UINT32(4U, ctx.delay_count);
    TEST_ASSERT_EQUAL_UINT32(2000U, ctx.delay_log[0]);   /* post soft-reset */
    TEST_ASSERT_EQUAL_UINT32(450U, ctx.delay_log[1]);    /* advanced power save */
    TEST_ASSERT_EQUAL_UINT32(150000U, ctx.delay_log[2]); /* procesamiento del blob */
    TEST_ASSERT_EQUAL_UINT32(2000U, ctx.delay_log[3]);   /* arranque del acelerómetro */
}

/* ---- D. BMI270AccelTryRead ------------------------------------------------ */

TEST_CASE("BMI270AccelTryRead null accel", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_OK);

    TEST_ASSERT_TRUE(BMI270AccelTryRead(NULL) == BMI270_ERR_INVALID_ARG);
}

TEST_CASE("BMI270AccelTryRead before init returns invalid state", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    /* Fuerza que BMI270Init falle y deje initialized=false, sin necesitar un
     * hook de reset dedicado: BMI270Init siempre limpia su estado estático al
     * entrar, incluso cuando termina en error. */
    ctx.regs[REG_CHIP_ID] = 0x00U;
    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_FAIL);

    bmi270_vector_t accel = {0};
    TEST_ASSERT_TRUE(BMI270AccelTryRead(&accel) == BMI270_ERR_INVALID_STATE);
}

TEST_CASE("BMI270AccelTryRead fails when STATUS read fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_OK);

    ctx.has_fail_read_reg = true;
    ctx.fail_read_reg = REG_STATUS;

    bmi270_vector_t accel = {0};
    TEST_ASSERT_TRUE(BMI270AccelTryRead(&accel) == BMI270_FAIL);
}

TEST_CASE("BMI270AccelTryRead returns not ready when DRDY is clear", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_OK);

    ctx.regs[REG_STATUS] = 0x00U;

    bmi270_vector_t accel = { .x = 111, .y = 222, .z = 333 };
    TEST_ASSERT_TRUE(BMI270AccelTryRead(&accel) == BMI270_NOT_READY);
    /* El vector de salida no debe modificarse cuando no hay muestra nueva. */
    TEST_ASSERT_EQUAL_INT16(111, accel.x);
    TEST_ASSERT_EQUAL_INT16(222, accel.y);
    TEST_ASSERT_EQUAL_INT16(333, accel.z);
}

TEST_CASE("BMI270AccelTryRead fails when DATA_8 read fails", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_OK);

    ctx.regs[REG_STATUS] = BMI270_STATUS_DRDY_ACC;
    ctx.has_fail_read_reg = true;
    ctx.fail_read_reg = REG_DATA_8;

    bmi270_vector_t accel = {0};
    TEST_ASSERT_TRUE(BMI270AccelTryRead(&accel) == BMI270_FAIL);
}

static void SetAccelRaw(bmi270_fake_bus_ctx_t *ctx, int16_t x, int16_t y, int16_t z) {
    uint16_t ux = (uint16_t)x;
    uint16_t uy = (uint16_t)y;
    uint16_t uz = (uint16_t)z;

    ctx->regs[REG_DATA_8 + 0] = (uint8_t)(ux & 0xFFU);
    ctx->regs[REG_DATA_8 + 1] = (uint8_t)(ux >> 8);
    ctx->regs[REG_DATA_8 + 2] = (uint8_t)(uy & 0xFFU);
    ctx->regs[REG_DATA_8 + 3] = (uint8_t)(uy >> 8);
    ctx->regs[REG_DATA_8 + 4] = (uint8_t)(uz & 0xFFU);
    ctx->regs[REG_DATA_8 + 5] = (uint8_t)(uz >> 8);
}

TEST_CASE("BMI270AccelTryRead decodes signed samples correctly", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_OK);

    ctx.regs[REG_STATUS] = BMI270_STATUS_DRDY_ACC;
    SetAccelRaw(&ctx, 0, 32767, -32768);
    /* La cuarta muestra usa -1 para verificar la extensión de signo de forma
     * independiente del resto de los ejes. */

    bmi270_vector_t accel = {0};
    TEST_ASSERT_TRUE(BMI270AccelTryRead(&accel) == BMI270_OK);
    TEST_ASSERT_EQUAL_INT16(0, accel.x);
    TEST_ASSERT_EQUAL_INT16(32767, accel.y);
    TEST_ASSERT_EQUAL_INT16(-32768, accel.z);

    SetAccelRaw(&ctx, -1, -1, -1);
    TEST_ASSERT_TRUE(BMI270AccelTryRead(&accel) == BMI270_OK);
    TEST_ASSERT_EQUAL_INT16(-1, accel.x);
    TEST_ASSERT_EQUAL_INT16(-1, accel.y);
    TEST_ASSERT_EQUAL_INT16(-1, accel.z);
}

TEST_CASE("BMI270AccelTryRead decodes several sequential samples", "[board_support][bmi270][fake_bus]")
{
    bmi270_bus_t bus;
    bmi270_fake_bus_ctx_t ctx;
    MakeValidBus(&bus, &ctx);
    TEST_ASSERT_TRUE(BMI270Init(&bus) == BMI270_OK);

    ctx.regs[REG_STATUS] = BMI270_STATUS_DRDY_ACC;

    for (int16_t step = 0; step < 5; step++) {
        int16_t value = (int16_t)(step * 1000);
        SetAccelRaw(&ctx, value, (int16_t)(-value), (int16_t)(value / 2));

        bmi270_vector_t accel = {0};
        TEST_ASSERT_TRUE(BMI270AccelTryRead(&accel) == BMI270_OK);
        TEST_ASSERT_EQUAL_INT16(value, accel.x);
        TEST_ASSERT_EQUAL_INT16((int16_t)(-value), accel.y);
        TEST_ASSERT_EQUAL_INT16((int16_t)(value / 2), accel.z);
    }
}
