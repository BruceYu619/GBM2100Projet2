/*******************************************************************************
* File Name: cyfitter_sysint_cfg.c
* 
* PSoC Creator  4.4
*
* Description:
* 
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2020 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "cyfitter_sysint.h"
#include "cyfitter_sysint_cfg.h"

/* ARM CM4 */
#if (((__CORTEX_M == 4) && (CY_CORE_ID == 0)))

    /* Bouton_isr */
    const cy_stc_sysint_t Bouton_isr_cfg = {
        .intrSrc = (IRQn_Type)Bouton_isr__INTC_NUMBER,
        .intrPriority = Bouton_isr__INTC_CORTEXM4_PRIORITY
    };

    /* dataCapteur */
    const cy_stc_sysint_t dataCapteur_cfg = {
        .intrSrc = (IRQn_Type)dataCapteur__INTC_NUMBER,
        .intrPriority = dataCapteur__INTC_CORTEXM4_PRIORITY
    };

    /* I2C_SCB_IRQ */
    const cy_stc_sysint_t I2C_SCB_IRQ_cfg = {
        .intrSrc = (IRQn_Type)I2C_SCB_IRQ__INTC_NUMBER,
        .intrPriority = I2C_SCB_IRQ__INTC_CORTEXM4_PRIORITY
    };

    /* I2C_1_SCB_IRQ */
    const cy_stc_sysint_t I2C_1_SCB_IRQ_cfg = {
        .intrSrc = (IRQn_Type)I2C_1_SCB_IRQ__INTC_NUMBER,
        .intrPriority = I2C_1_SCB_IRQ__INTC_CORTEXM4_PRIORITY
    };

    /* UART_1_SCB_IRQ */
    const cy_stc_sysint_t UART_1_SCB_IRQ_cfg = {
        .intrSrc = (IRQn_Type)UART_1_SCB_IRQ__INTC_NUMBER,
        .intrPriority = UART_1_SCB_IRQ__INTC_CORTEXM4_PRIORITY
    };

    /* CY_EINK_SPIM_SCB_IRQ */
    const cy_stc_sysint_t CY_EINK_SPIM_SCB_IRQ_cfg = {
        .intrSrc = (IRQn_Type)CY_EINK_SPIM_SCB_IRQ__INTC_NUMBER,
        .intrPriority = CY_EINK_SPIM_SCB_IRQ__INTC_CORTEXM4_PRIORITY
    };

    /* CapSense_ISR */
    const cy_stc_sysint_t CapSense_ISR_cfg = {
        .intrSrc = (IRQn_Type)CapSense_ISR__INTC_NUMBER,
        .intrPriority = CapSense_ISR__INTC_CORTEXM4_PRIORITY
    };

#endif /* ((__CORTEX_M == 4) && (CY_CORE_ID == 0)) */

