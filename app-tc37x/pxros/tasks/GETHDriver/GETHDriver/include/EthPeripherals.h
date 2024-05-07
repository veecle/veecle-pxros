/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#ifndef __ETH_PERIPHERALS_H__
#define __ETH_PERIPHERALS_H__


/* identification of the Ethernet MAC IP:
 * Ethernet MAC 10/100/1G Quality-of-Service */
#define GETH_DWC_ETHER_QOS


#define GPIO_BASE_P11   ((unsigned int)&MODULE_P11)
#define GPIO_END_P11    (GPIO_BASE_P11 + sizeof(Ifx_P))

#define GPIO_BASE_P21   ((unsigned int)&MODULE_P21)
#define GPIO_END_P21    (GPIO_BASE_P21 + sizeof(Ifx_P))

/* concat the GPIO port ranges (P11 - P21) to one range */
#define GPIO_MODULE_BASE    GPIO_BASE_P11
#define GPIO_MODULE_END     GPIO_END_P21

#define ETH_MODULE_BASE ((unsigned int)&MODULE_GETH)
#define ETH_MODULE_END  (ETH_MODULE_BASE + sizeof(Ifx_GETH))

#define SRC_MODULE_BASE ((unsigned int)&MODULE_SRC)
#define SRC_MODULE_END  (SRC_MODULE_BASE + sizeof(Ifx_SRC))

#define SCU_MODULE_BASE ((unsigned int)&MODULE_SCU)
#define SCU_MODULE_END  (SCU_MODULE_BASE + sizeof(Ifx_SCU))

#define TMR_MODULE_BASE ((unsigned int)&MODULE_STM0)
#define TMR_MODULE_END  (TMR_MODULE_BASE + sizeof(Ifx_STM))


#endif /* __ETH_PERIPHERALS_H__ */
