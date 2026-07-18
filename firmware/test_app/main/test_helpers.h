#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

/**
 * @file test_helpers.h
 * @brief Utilidades compartidas por los tests de integración.
 */

/**
 * @brief Informa al runner que el test actual adquirió el bus I2C.
 *
 * El runner liberará el recurso desde `tearDown()`, incluso si una aserción
 * interrumpe anticipadamente el test.
 */
void TestMarkI2CBusAcquired(void);

#endif /* TEST_HELPERS_H */
