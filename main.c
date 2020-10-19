#include <msp430.h>
#define MAX_TIME  (1024) // intialize MAX_TIME equal 1024
unsigned int pointer[3]; // initializing three pointers array for the memory
volatile int rv, gv, bv; // Intializing variables rv, gv, bv
#define REDREG TA1CCR1   //making the REDREG connecting to timer 1
#define GREENREG TA0CCR1 // making the GREENREG connecting to timer 0
#define BLUEREG TA1CCR2  // making the BLUEREG connecting to timer 1



// Function
// sets up the values for the REDREG that will lead to the TIMER 1
// this will affect the brightness of the RED LED
void setRedChannel(int value){
    REDREG = value;                 // Duty-cycle
}

// Function
// sets up the values for the REDREG that will lead to the TIMER 0
// this will affect the brightness of the GREEN LED
void setGreenChannel(int value){
    GREENREG = value;                 // Duty-cycle
}

// Function
// sets up the values for the REDREG that will lead to the TIMER 1
// this will affect the brightness of the BLUE LED
void setBlueChannel(int value){
    BLUEREG = value;                 // Duty-cycle
}

int main(void) {
  // Stop the watchdog timer.
  WDTCTL  =  (WDTPW + WDTHOLD);

  // RED and BLUE outputs to 2.1 and 2.5
  P2DIR  |=  (BIT1 | BIT5);
  P2SEL  |=  (BIT1 | BIT5);
  TA1CCTL0 = (OUTMOD_7);
  TA1CCR0  = (MAX_TIME);
  TA1CCTL1 = (OUTMOD_7);
  TA1CCTL2 = (OUTMOD_7);

  // Start timer 1.
  TA1CTL  =  (TASSEL_2 | MC_1);
  // Use timer 0 for the green LED...
  P1DIR  |=  (BIT6); // GREEN outputs to 1.6
  P1SEL  |=  (BIT6);
  TA0CCR0  = (MAX_TIME);
  TA0CCTL1 = (OUTMOD_7);
  TA0CTL  |= (TAIE);

  // Start timer 0.
  TA0CTL  |= (TASSEL_2 | MC_1);

  // ADC config
  ADC10CTL1 = INCH_2 + CONSEQ_1 + CONSEQ_0;  // A2,A1,A0 repeat multi channel
  ADC10AE0 |= (BIT0 + BIT1 + BIT2);          // P1.0 (A0), P1.1 (A1), P1.2 (A2) ADC option select
  ADC10DTC1 = 3;                             // Creates an array for ADC10MEM
  ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + ADC10IE;


  while(1)
     {
         ADC10CTL0 &= ~ENC;
         ADC10SA = (unsigned int)&pointer;   // Data buffer start, index starts at 0 in the array for ADC10MEM
         ADC10CTL0 |= ENC + ADC10SC;         // Sampling and conversion ready
         while (ADC10CTL1 & BUSY);           // Wait if ADC10 core is active
         setRedChannel(rv);                  // Adjust REDLED brightness and it send the value to the setRedChannel function
         setGreenChannel(gv);                // Adjust GREENLED brightness and it send the value to the setGreenChannel function
         setBlueChannel(bv);                 // Adjust BLUELED brightness and it send the value to the setBlueChannel function
         // Turn off the CPU, and enable interrupts.
         __bis_SR_register(CPUOFF + GIE);    // LPM0, ADC10_ISR will force exit
     }
}


// ADC10 interrupt service routine
  #pragma vector=ADC10_VECTOR
  __interrupt void ADC10_ISR (void)
  {
      rv = pointer[0];                // Receives the value from the pointer and sends it to rv
      gv = pointer[1];                // Receives the value from the pointer and sends it to gv
      bv = pointer[2];                // Receives the value from the pointer and sends it to bv
      __bic_SR_register_on_exit(CPUOFF);  // Clear CPUOFF bit from 0(SR)
  }
