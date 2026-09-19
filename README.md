# BUS BOOKING SYSTEM

A command-line bus booking system written in C. It enables clients to:

- View available buses, routes, departure times, fares, and seat availability.
- View a seat map for each bus.
- Book a seat and enter passenger details.
- Simulate payment by mobile money or card and receive a payment reference.
- View active bookings and cancel a booking.

## Compile and run

```bash
gcc -std=c11 -Wall -Wextra -pedantic bus_booking.c -o bus_booking
./bus_booking
```

Payments are simulated for demonstration purposes; connect `make_booking()` to a real payment gateway before using this in production.
