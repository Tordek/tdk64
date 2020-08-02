/*
 * Author: Guillermo O. «Tordek» Freschi
 * License: GNU GPL v3.0
 */

/*
 * In order to have some code reuse for this very time-critical part,
 * the body of the function is created through a set of macros.
 * The including code must #define the following constants before #include'ing
 * this file:
 * WRITE_PORT, READ_PORT "PORT, Bit" for the appropriate lead.
 * SCOPE: manually mangle variable names to prevent clashing
 */
    
static char bitsleft;
static char attempts;
    
// The line is low, someone is sending. Bail.
asm("btfss " READ_PORT);
asm("retlw 1");    
    
asm("clrf LATB"); // Safety: Keep READ_PORT low for when we set TRISB low.
    
/*
 * Part 1: Send command.
 *
 * With 6 ops per uS at 24MHz we gotta go down to ASM.
 */
asm("movlw 8");                            
asm("movwf " SCOPE "@bitsleft");           // bitsleft = 8

asm("movff " SCOPE "@send_buffer+1, FSR0H");      // FSR0 = (unsigned short) send_buffer;
asm("movff " SCOPE "@send_buffer, FSR0");

/**/ asm(SCOPE "transmission_loop:");
asm("bcf " WRITE_PORT);                    // [0]   Bit start (Low).
asm("nop");                                // [0+1]
asm("bra $+2");                            // [0+2]
asm("rlcf INDF0");                         // [0+4] Rotate right 1
asm("btfsc STATUS, 0");                    // [0+5] Check bit
asm("bsf " WRITE_PORT);                    // [1us] If enabled, raise signal (1 bit)
asm("dcfsnz " SCOPE "@bitsleft");          // [1+1]
asm("bra " SCOPE "last_bit_tx");           // [1+2]
asm("nop");                                // [1+3]
asm("bra $+2");                            // [1+4]
asm("bra $+2");                            // [2]
asm("bra $+2");                            // [2+2]
asm("bra $+2");                            // [2+4]
/**/ asm(SCOPE "finish_bit:");
asm("bsf " WRITE_PORT);                    // [3]   It's either a 0 or already high.
asm("nop");                                // [3+1]
asm("bra $+2");                            // [3+2]
asm("bra " SCOPE "transmission_loop");     // [3+4] New bit.

/**/ asm(SCOPE "last_bit_tx:");
asm("rlcf INDF0");                         // [1+4] A 9th rotation puts the byte back to its original value.
asm("dcfsnz " SCOPE "@send_len");          // [1+5]
asm("bra " SCOPE "end_of_message_tx");     // [2]
asm("movlw 8");                            // [2+1]
asm("movwf " SCOPE "@bitsleft");           // [2+2]
asm("incf FSR0L");                         // [2+3]
asm("bra " SCOPE "finish_bit");            // [2+4]

/**/ asm(SCOPE "end_of_message_tx:");
asm("bra $+2");                            // [2+2]
asm("bra $+2");                            // [2+4]
asm("bsf " WRITE_PORT);                    // [3]   End of the final bit. Send stop bit.
asm("nop");                                // [3+1]
asm("bra $+2");                            // [3+2]
asm("bra $+2");                            // [3+4]

asm("bcf " WRITE_PORT);                    // [0]   Stop bit start.
asm("nop");                                // [0+1]
asm("bra $+2");                            // [0+2]
asm("bra $+2");                            // [0+4]
asm("tstfsz " SCOPE "@wide_stop_bit");     // [0+5]
asm("bsf " WRITE_PORT);                    // [1us] Short stop bit end
asm("nop");                                // [1+1]
asm("bra $+2");                            // [1+2]
asm("bra $+2");                            // [1+4]
asm("bsf " WRITE_PORT);                    // [2us] Wide stop bit end

/* Part 2: Receive response.
 *
 * We can't afford another function for this because the response comes back
 * quickly (less than 3us/18 cycles).
 *
 * Timeout if there's too many unsuccessful read attempts.
 */
    
asm("movlw 10");                            
asm("movwf " SCOPE "@attempts");           // attempts = 10
asm("movlw 8");                            
asm("movwf " SCOPE "@bitsleft");           // bitsleft = 8

asm("movff " SCOPE "@recv_buffer+1, FSR0H");      // FSR0 = (unsigned short) recv_buffer;
asm("movff " SCOPE "@recv_buffer, FSR0");

/**/ asm(SCOPE "wait_for_bit_start:");
asm("dcfsnz " SCOPE "@attempts");
asm("bra " SCOPE "err_cond");
asm("btfsc " READ_PORT);
asm("bra " SCOPE "wait_for_bit_start");    // [0/0+3] Wait for bit start (0).

asm("movlw 10");                           // [0+1/0+4] Successful, restart attempt counter.
asm("movwf " SCOPE "@attempts");           // [0+2/0+5]
asm("rlncf INDF0");                        // [0+3/1]   Make room
asm("bcf INDF0, 0");                       // [0+4/1+1] Assume 0.
asm("dcfsnz " SCOPE "@bitsleft");          // [0+5/1+2] Last bit? We check early to take advantage of free cycles.
asm("bra " SCOPE "last_bit_rx");           // [1/1+3]
asm("nop");                                // [1+1/1+4]
asm("bra $+2");                            // [1+2/1+5]
asm("btfsc " READ_PORT);                   // [1+4/2+1]   Read the bit in the middle.
asm("bsf INDF0, 0");                       // [1+5/2+2] Set 1 if high bit
asm("bra $+2");                            // [2+2/2+5]
asm("bra $+2");                            // [2+4/3+1]
asm("bra $+2");                            // [3/3+3]
asm("bra " SCOPE "wait_for_bit_start");    // [3+2/3+5] The bit should be high by now.

/**/ asm(SCOPE "last_bit_rx:");
asm("dcfsnz " SCOPE "@recv_len");          // [1+2/1+5] // End of message?
asm("bra " SCOPE "end_of_message_rx");     // [1+3/2]
asm("btfsc " READ_PORT);                   // [1+4/2+1]   Read the bit in the middle.
asm("bsf INDF0, 0");                       // [1+5/2+2] Set 1 if high bit
asm("movlw 8");                            // [2/2+3]
asm("movwf " SCOPE "@bitsleft");           // [2+1/2+4] New byte
asm("incf FSR0L");                         // [2+2/2+5] Make room for the next byte.
asm("nop");                                // [2+3/3]
asm("bra $+2");                            // [2+4/3+1]
asm("bra " SCOPE "wait_for_bit_start");    // [3/3+3] The bit should be high by now.

/**/ asm(SCOPE "end_of_message_rx:");
asm("nop");                                // [1+5/2+2]
asm("btfsc " READ_PORT);                   // [2/2+3]   Read the bit in the middle.
asm("bsf INDF0, 0");                       // [2+1/2+4] Set 1 if high bit

asm("retlw 0");

/**/ asm(SCOPE "err_cond:");
asm("retlw 10");

    return -1; // Unreachable. Shuts up a warning.