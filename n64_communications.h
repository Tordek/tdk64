/*
 * File:   n64_communications.h
 * Author: Tordek
 *
 * Created on 24 de abril de 2017, 02:15
 */

#ifndef N64_COMMUNICATIONS_H
#define	N64_COMMUNICATIONS_H

/* Formatted to match the shape of the N64 response to avoid having to reorder
 * data for the USB message.
 */
struct n64_poll_response {
    char d_right : 1;
    char d_left : 1;
    char d_down : 1;
    char d_up : 1;
    char start : 1;
    char z : 1;
    char b : 1;
    char a : 1;

    char c_right : 1;
    char c_left : 1;
    char c_down : 1;
    char c_up : 1;
    char r : 1;
    char l : 1;
    char : 2; // Padding

    signed char analog_x;

    signed char analog_y;
};

enum n64_command_code {
    n64_id, // 0x00
    n64_poll,
    n64_read,
    n64_write,
};

char tdk64_command(char port, char *send_buffer, char len, char wide_stop_bit, char *recv_buffer, char recv_len);

#endif	/* N64_COMMUNICATIONS_H */

