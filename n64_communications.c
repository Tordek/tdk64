/*
 * Author: Guillermo O. «Tordek» Freschi
 * License: GNU GPL v3.0
 */

#include <xc.h>
#include <n64_communications.h>

static char tdk64_command_j1(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len);
static char tdk64_command_j2(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len);
static char tdk64_command_j3(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len);
static char tdk64_command_j4(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len);

/*
 * The N64 controller protocol sends all bits in a row, followed by a stop bit.
 * The signal is sent by changing the impedance of the pin, NOT by sending 5v.
 *
 * Each bit consists of some time of low signal, followed by some time of high
 * signal. If the bit is a 0, 3us of low are followed by 1 of high. If the bit
 * is a 1, 1us of low is followed by 3us of high.
 *
 * The controller responds quickly, so response has to be handled by the same
 * function.
 *
 * In order to pick the port, we need to do some preprocessor magic.
 */

char tdk64_command(char port, char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len)
{
    switch (port) {
    case 1:
        return tdk64_command_j1(send_buffer, send_len, wide_stop_bit, recv_buffer, recv_len);
    case 2:
        return tdk64_command_j2(send_buffer, send_len, wide_stop_bit, recv_buffer, recv_len);
    case 3:
        return tdk64_command_j3(send_buffer, send_len, wide_stop_bit, recv_buffer, recv_len);
    case 4:
        return tdk64_command_j4(send_buffer, send_len, wide_stop_bit, recv_buffer, recv_len);
    default:
        return 1;
    }
}

static char tdk64_command_j1(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len)
{
#define WRITE_PORT "TRISB, 0"
#define READ_PORT "PORTB, 0"
#define SCOPE "tdk64_command_j1"
#include <n64_command.inc>
#undef WRITE_PORT
#undef READ_PORT
#undef SCOPE
}

static char tdk64_command_j2(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len)
{
#define WRITE_PORT "TRISB, 1"
#define READ_PORT "PORTB, 1"
#define SCOPE "tdk64_command_j2"
#include <n64_command.inc>
#undef WRITE_PORT
#undef READ_PORT
#undef SCOPE
}

static char tdk64_command_j3(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len)
{
#define WRITE_PORT "TRISB, 2"
#define READ_PORT "PORTB, 2"
#define SCOPE "tdk64_command_j3"
#include <n64_command.inc>
#undef WRITE_PORT
#undef READ_PORT
#undef SCOPE
}

static char tdk64_command_j4(char *send_buffer, char send_len, char wide_stop_bit, char *recv_buffer, char recv_len)
{
#define WRITE_PORT "TRISB, 3"
#define READ_PORT "PORTB, 3"
#define SCOPE "tdk64_command_j4"
#include <n64_command.inc>
#undef WRITE_PORT
#undef READ_PORT
#undef SCOPE
}
