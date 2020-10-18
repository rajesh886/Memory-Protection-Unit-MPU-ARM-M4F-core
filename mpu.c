#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"

// PortF masks
#define GREEN_LED_MASK 8
#define RED_LED_MASK 2
#define PUSH_BUTTON_MASK 16

//Subroutines
extern void usermode();
extern void privilegedmode();
extern void setPSP();
extern void setASP();
extern uint32_t getPSP();
extern uint32_t getMSP();

// Blocking function that returns only when SW1 is pressed
void waitPbPress()
{
    while(GPIO_PORTF_DATA_R & PUSH_BUTTON_MASK);
}

//this function converts uint32_t value into hexadecimal value and prints the result.
void uint32_tToHex(uint32_t decimal){
    uint32_t quotient, remainder;
    int i,j = 0;
    char hex[100];

    quotient = decimal;

    while (quotient != 0)
    {
       remainder = quotient % 16;
       if (remainder < 10){
           hex[j++] = 48 + remainder;
       }

       else{
           hex[j++] = 55 + remainder;
       }

          quotient = quotient / 16;
     }

    if(quotient == 0 && j != 8){
        while(j!=8){
            hex[j++] = 48;
        }
    }

    for (i = j; i >= 0; i--)
           putcUart0(hex[i]);
}

void initMPU(){
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    //Initializing MPU Control register. It is disabled at first.
      NVIC_MPU_CTRL_R |= 0x00000000;

    //Enabling MPU Region for RAM, peripherals, and bitbanded addresses
      NVIC_MPU_NUMBER_R = 0x0;
    //Configuring base register
      NVIC_MPU_BASE_R |= 0x00000000;          //base address of the RAM, peripherals and bitbanded region.
      NVIC_MPU_BASE_R |= 0x00000000;          //disabling valid region number
      NVIC_MPU_BASE_R |= 0x0;                 //Region rule is 0.
    // Configuration of Attribute Register
      NVIC_MPU_ATTR_R |= 0x10000000;          // Instruction fetches are disabled.
      NVIC_MPU_ATTR_R |= 0x03000000;          // Full Access Mode
      NVIC_MPU_ATTR_R |= 0x00000000;          // Setting TEX field for Flash
      NVIC_MPU_ATTR_R |= 0x00040000;          // Shareable
      NVIC_MPU_ATTR_R |= 0x00000000;          // Not Cacheable
      NVIC_MPU_ATTR_R |= 0x00000000;          // Not Bufferable
      NVIC_MPU_ATTR_R |= 0x00000000;          // Sub region is enabled.
      NVIC_MPU_ATTR_R |= 0x0000003E;          // Region Size Mask for RAM, peripheral and bitbanded (Size = log2(4*1024*1024*1024)-1 )
      NVIC_MPU_ATTR_R |= 0x01;                // Region Enabled


      //Enabling MPU Region for 256 KiB flash
      NVIC_MPU_NUMBER_R = 0x1;
      //Configuring base register
      NVIC_MPU_BASE_R |= 0x00000000;      //base address of the Flash region.
      NVIC_MPU_BASE_R |= 0x00000000;      //disabling valid region number
      NVIC_MPU_BASE_R |= 0x1;             //Region rule is 1.
      //Configuration of Attribute Register
      NVIC_MPU_ATTR_R |= 0x00000000;      // Instruction fetches are enabled.
      NVIC_MPU_ATTR_R |= 0x03000000;      // Full Access Mode
      NVIC_MPU_ATTR_R |= 0x00000000;      // Setting TEX field for Flash
      NVIC_MPU_ATTR_R |= 0x00000000;      // Not Shareable
      NVIC_MPU_ATTR_R |= 0x00020000;      // Cacheable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Not Bufferable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Sub region is enabled.
      NVIC_MPU_ATTR_R |= 0x00000022;      // Region Size Mask for flash (Size = log2(256*1024)-1 = 17)
      NVIC_MPU_ATTR_R |= 0x01;            // Region Enabled


     //Enabling mutliple MPU Regions for 32 KiB RAM
     // NVIC_MPU_NUMBER_R &= 0x0;
      NVIC_MPU_NUMBER_R = 0x2;
     //Configuring base register
      NVIC_MPU_BASE_R |= 0x20000000;      //base address of the region.
      NVIC_MPU_BASE_R |= 0x00000000;      //disabling valid region number
      NVIC_MPU_BASE_R |= 0x2;             //Region rule is 2.
     // Configuration of Attribute Register
      NVIC_MPU_ATTR_R |= 0x10000000;      // Instruction fetches are disabled.
      NVIC_MPU_ATTR_R |= 0x01000000;      // RW - Priv / No RW for UnPriv Mode
      NVIC_MPU_ATTR_R |= 0x00000000;      // Setting TEX field for RAM
      NVIC_MPU_ATTR_R |= 0x00040000;      // Shareable
      NVIC_MPU_ATTR_R |= 0x00020000;      // Cacheable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Not Bufferable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Sub region is enabled.
      NVIC_MPU_ATTR_R |= (12 << 1);      // Region Size Mask for 8KiB
      NVIC_MPU_ATTR_R |= 0x01;            // Region Enabled


      NVIC_MPU_NUMBER_R = 0x3;
     //Configuring base register
      NVIC_MPU_BASE_R |= 0x20002000;      //base address of the MPU region.
      NVIC_MPU_BASE_R |= 0x00000000;      //disabling valid region number
      NVIC_MPU_BASE_R |= 0x3;             //Region rule is 3.
     // Configuration of Attribute Register
      NVIC_MPU_ATTR_R |= 0x10000000;      // Instruction fetches are disabled.
      NVIC_MPU_ATTR_R |= 0x01000000;      // RW - Priv / No RW for UnPriv Mode
      NVIC_MPU_ATTR_R |= 0x00000000;      // Setting TEX field for RAM
      NVIC_MPU_ATTR_R |= 0x00040000;      // Shareable
      NVIC_MPU_ATTR_R |= 0x00020000;      // Cacheable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Not Bufferable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Sub region is enabled.
      NVIC_MPU_ATTR_R |= (12 << 1);      // Region Size Mask for 8KiB
      NVIC_MPU_ATTR_R |= 0x01;            // Region Enabled


      NVIC_MPU_NUMBER_R = 0x4;
      //Configuring base register
      NVIC_MPU_BASE_R |= 0x20004000;      //base address of the MPU region.
      NVIC_MPU_BASE_R |= 0x00000000;      //disabling valid region number
      NVIC_MPU_BASE_R |= 0x4;             //Region rule is 4.
      // Configuration of Attribute Register
      NVIC_MPU_ATTR_R |= 0x10000000;      // Instruction fetches are disabled.
      NVIC_MPU_ATTR_R |= 0x01000000;      // RW - Priv / No RW for UnPriv Mode
      NVIC_MPU_ATTR_R |= 0x00000000;      // Setting TEX field for RAM
      NVIC_MPU_ATTR_R |= 0x00040000;      // Shareable
      NVIC_MPU_ATTR_R |= 0x00020000;      // Cacheable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Not Bufferable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Sub region is enabled.
      NVIC_MPU_ATTR_R |= (12 << 1);      // Region Size Mask for 8KiB
      NVIC_MPU_ATTR_R |= 0x01;            // Region Enabled


      NVIC_MPU_NUMBER_R = 0x5;
     //Configuring base register
      NVIC_MPU_BASE_R |= 0x20006000;      //base address of the MPU region.
      NVIC_MPU_BASE_R |= 0x00000000;      //disabling valid region number
      NVIC_MPU_BASE_R |= 0x5;             //Region rule is 5.
     // Configuration of Attribute Register
      NVIC_MPU_ATTR_R |= 0x10000000;      // Instruction fetches are disabled.
      NVIC_MPU_ATTR_R |= 0x01000000;      // RW - Priv / No RW for UnPriv Mode except for 1KiB
      NVIC_MPU_ATTR_R |= 0x00000000;      // Setting TEX field for RAM
      NVIC_MPU_ATTR_R |= 0x00040000;      // Shareable
      NVIC_MPU_ATTR_R |= 0x00020000;      // Cacheable
      NVIC_MPU_ATTR_R |= 0x00000000;      // Not Bufferable
      NVIC_MPU_ATTR_R |= 0x00008000;      // Sub region is enabled and disabled for the 1kiB region.
      NVIC_MPU_ATTR_R |= (12 << 1);      // Region Size Mask for 8KiB
      NVIC_MPU_ATTR_R |= 0x01;            // Region Enabled


      NVIC_MPU_CTRL_R |= 0x00000004 | 0x00000002 | 0x00000001;        // MPU Default Region | MPU Enabled During Faults | MPU enable

      NVIC_SYS_HND_CTRL_R |= 0x00040000; //Enabling usage fault
      NVIC_SYS_HND_CTRL_R |= 0x00020000; //Enabling bus fault
      NVIC_SYS_HND_CTRL_R |= 0x00010000; //Enabling memory management fault
 }

// Initialize Hardware
void initHw()
{
    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
    _delay_cycles(3);

    // Configure LED and pushbutton pins
    GPIO_PORTF_DIR_R |= GREEN_LED_MASK | RED_LED_MASK;   // bits 1 and 3 are outputs, other pins are inputs
    GPIO_PORTF_DIR_R &= ~PUSH_BUTTON_MASK;               // bit 4 is an input
    GPIO_PORTF_DR2R_R |= GREEN_LED_MASK | RED_LED_MASK;  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTF_DEN_R |= PUSH_BUTTON_MASK | GREEN_LED_MASK | RED_LED_MASK;
                                                         // enable LEDs and pushbuttons
    GPIO_PORTF_PUR_R |= PUSH_BUTTON_MASK;                // enable internal pull-up for push button
}

void busfaultISR(){
    putsUart0("Bus fault in process pid\r\n");
}

void usagefaultISR(){
    putsUart0("Usage fault in process pid\r\n");
}

void hardfaultISR(){
    putsUart0("Hardfault in process pid\r\n");

    uint32_t *PSP = (uint32_t*) getPSP();
    putsUart0("PSP = ");
    uint32_tToHex(PSP);
    putsUart0("\r\n");

    uint32_t *MSP = (uint32_t*) getMSP();
    putsUart0("MSP = ");
    uint32_tToHex(MSP);
    putsUart0("\r\n");

    uint32_t hflag = NVIC_HFAULT_STAT_R;
    putsUart0("HFLAG = ");
    uint32_tToHex(hflag);
    putsUart0("\r\n");
}

void mpufaultISR(){
    putsUart0("MPU fault in process pid\r\n");

    uint32_t *R0, *R1, *R2, *R3, *R12, *LR, *PC, *xPSR;

    uint32_t *PSP = (uint32_t*) getPSP();
    putsUart0("PSP = ");
    uint32_tToHex(PSP);
    putsUart0("\r\n");

    uint32_t *MSP = (uint32_t*) getMSP();
    putsUart0("MSP = ");
    uint32_tToHex(MSP);
    putsUart0("\r\n");

    putsUart0("MFAULTFLAG = ");
    uint32_tToHex(NVIC_FAULT_STAT_R);
    putsUart0("\r\n");

    putsUart0("Offending instruction at ");
    PC = *(PSP+6);
    uint32_tToHex(PC);
    putsUart0(" tried to access ");
    uint32_tToHex(NVIC_MM_ADDR_R);
    putsUart0("\r\n");

    putsUart0("Process stack dump:\r\n");
    R0=*PSP;
    putsUart0("R0 = ");
    uint32_tToHex(R0);
    putsUart0("\r\n");

    R1 = *(PSP+1);
    putsUart0("R1 = ");
    uint32_tToHex(R1);
    putsUart0("\r\n");

    R2 = *(PSP+2);
    putsUart0("R2 = ");
    uint32_tToHex(R2);
    putsUart0("\r\n");

    R3 = *(PSP+3);
    putsUart0("R3 = ");
    uint32_tToHex(R3);
    putsUart0("\r\n");

    R12 = *(PSP+4);
    putsUart0("R12 = ");
    uint32_tToHex(R12);
    putsUart0("\r\n");

    LR = *(PSP+5);
    putsUart0("LR = ");
    uint32_tToHex(LR);
    putsUart0("\r\n");

    PC = *(PSP+6);
    putsUart0("PC = ");
    uint32_tToHex(PC);
    putsUart0("\r\n");

    xPSR = *(PSP+7);
    putsUart0("xPSR = ");
    uint32_tToHex(xPSR);
    putsUart0("\r\n");

    NVIC_SYS_HND_CTRL_R &= ~NVIC_SYS_HND_CTRL_MEMP; //Clear the pending bit


    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV ; //turning on the pendSV exception

}

void pendsvISR(){
    putsUart0("pendSV in process N");
    putsUart0("\r\n");
    if((NVIC_FAULT_STAT_R | NVIC_FAULT_STAT_DERR) || (NVIC_FAULT_STAT_R | NVIC_FAULT_STAT_IERR))  //clearing the IERR and DERR bit
    {
        //if(NVIC_FAULT_STAT_R | NVIC_FAULT_STAT_DERR){
            NVIC_FAULT_STAT_R &= ~NVIC_FAULT_STAT_DERR;
        //}
        //else{
            NVIC_FAULT_STAT_R &= ~NVIC_FAULT_STAT_IERR;
        //}
        putsUart0("Called from MPU.\r\n");
        putsUart0("\r\n");
    }
}

//this thread test the region access in the user mode. The region size that is only accessible is 1kiB. If the instruction
//tries to access the region outside of 1kiB then it throws mpu fault error and prints the stack dump.
void thread(){
    char* p;

    p = (char*) 0x20007FFC;
    *p = 0;

    p = (char*) 0x20007EB8;
    *p = 0;

    p = (char*) 0x20007BFC;
    *p = 0;

    while(true);
}

/**
 * main.c
 */
int main(void)
{

    initMPU();

    initUart0();
    setUart0BaudRate(115200, 40e6);

    setPSP();
    putsUart0("Setting up the PSP\r\n");

    setASP();
    putsUart0("Setting up the ASP\r\n");

    usermode();
    putsUart0("Turning on user mode\r\n");

    //privilegedmode();
    //putsUart0("Turning on privileged mode\r\n");

     thread();

     return 0;
}
