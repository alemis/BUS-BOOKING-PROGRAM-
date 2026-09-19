/*
 * BUS BOOKING SYSTEM
 * Build: gcc -std=c11 -Wall -Wextra -pedantic bus_booking.c -o bus_booking
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUSES 4
#define MAX_SEATS 40
#define MAX_BOOKINGS 100
#define NAME_LEN 80
#define PHONE_LEN 24
#define CARD_LEN 32

struct Bus {
    int id;
    char registration[16];
    char route[80];
    char departure[16];
    char arrival[16];
    double fare;
    int seats;
};

struct Booking {
    int booking_id;
    int bus_id;
    int seat;
    char passenger[NAME_LEN];
    char phone[PHONE_LEN];
    char payment_reference[32];
    double amount;
    int active;
};

static struct Bus buses[MAX_BUSES] = {
    {1, "KDA 101A", "Nairobi - Mombasa", "07:00", "14:30", 1500.00, MAX_SEATS},
    {2, "KDB 202B", "Nairobi - Kisumu", "08:30", "15:00", 1200.00, MAX_SEATS},
    {3, "KDC 303C", "Nairobi - Nakuru", "10:00", "13:00", 700.00, MAX_SEATS},
    {4, "KDD 404D", "Mombasa - Malindi", "09:00", "12:00", 600.00, MAX_SEATS}
};
static struct Booking bookings[MAX_BOOKINGS];
static int booking_count = 0;
static int next_booking_id = 1001;

static void read_line(const char *prompt, char *buffer, size_t size) {
    int c;
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        putchar('\n');
        exit(EXIT_SUCCESS);
    }
    if (strchr(buffer, '\n') == NULL) {
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

static int read_int(const char *prompt, int min, int max) {
    char input[64], extra;
    int value;
    for (;;) {
        read_line(prompt, input, sizeof input);
        if (sscanf(input, " %d %c", &value, &extra) == 1 && value >= min && value <= max)
            return value;
        printf("Please enter a number from %d to %d.\n", min, max);
    }
}

static double read_amount(const char *prompt, double expected) {
    char input[64], extra;
    double value;
    for (;;) {
        read_line(prompt, input, sizeof input);
        if (sscanf(input, " %lf %c", &value, &extra) == 1 && value >= expected)
            return value;
        printf("Payment must be at least %.2f.\n", expected);
    }
}

static struct Bus *find_bus(int id) {
    for (int i = 0; i < MAX_BUSES; i++)
        if (buses[i].id == id) return &buses[i];
    return NULL;
}

static int seat_taken(int bus_id, int seat) {
    for (int i = 0; i < booking_count; i++)
        if (bookings[i].active && bookings[i].bus_id == bus_id && bookings[i].seat == seat)
            return 1;
    return 0;
}

static void list_buses(void) {
    printf("\n%-4s %-12s %-25s %-8s %-8s %-10s %s\n", "ID", "Bus", "Route", "Departs", "Arrives", "Fare", "Available");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < MAX_BUSES; i++) {
        int available = 0;
        for (int s = 1; s <= buses[i].seats; s++) available += !seat_taken(buses[i].id, s);
        printf("%-4d %-12s %-25s %-8s %-8s %-10.2f %d/%d\n", buses[i].id, buses[i].registration,
               buses[i].route, buses[i].departure, buses[i].arrival, buses[i].fare, available, buses[i].seats);
    }
}

static void show_seats(void) {
    list_buses();
    int bus_id = read_int("Enter bus ID (0 to return): ", 0, MAX_BUSES);
    if (bus_id == 0) return;
    struct Bus *bus = find_bus(bus_id);
    printf("\nSeats for %s (%s):\n", bus->registration, bus->route);
    for (int s = 1; s <= bus->seats; s++)
        printf("[%2d:%c] %s", s, seat_taken(bus_id, s) ? 'X' : 'O', s % 5 == 0 ? "\n" : "  ");
    if (bus->seats % 5 != 0) putchar('\n');
    printf("O = available, X = booked\n");
}

static void make_booking(void) {
    if (booking_count >= MAX_BOOKINGS) { puts("Booking capacity reached."); return; }
    list_buses();
    int bus_id = read_int("Enter bus ID (0 to return): ", 0, MAX_BUSES);
    if (bus_id == 0) return;
    struct Bus *bus = find_bus(bus_id);
    int seat;
    do {
        seat = read_int("Choose a seat number (0 to return): ", 0, bus->seats);
        if (seat == 0) return;
        if (seat_taken(bus_id, seat)) puts("That seat is already booked.");
    } while (seat_taken(bus_id, seat));

    struct Booking *b = &bookings[booking_count];
    read_line("Passenger name: ", b->passenger, sizeof b->passenger);
    read_line("Phone number: ", b->phone, sizeof b->phone);
    if (b->passenger[0] == '\0' || b->phone[0] == '\0') { puts("Name and phone are required."); return; }

    printf("Amount due: %.2f\n", bus->fare);
    double paid = read_amount("Enter payment amount (simulated mobile/card payment): ", bus->fare);
    char method[20];
    read_line("Payment method (MOBILE/CARD): ", method, sizeof method);
    for (size_t i = 0; method[i]; i++) method[i] = (char)toupper((unsigned char)method[i]);
    if (strcmp(method, "MOBILE") != 0 && strcmp(method, "CARD") != 0) {
        puts("Unsupported payment method. Booking cancelled."); return;
    }
    snprintf(b->payment_reference, sizeof b->payment_reference, "%s-%d", method, next_booking_id);
    b->booking_id = next_booking_id++;
    b->bus_id = bus_id; b->seat = seat; b->amount = paid; b->active = 1;
    booking_count++;
    printf("\nBooking confirmed! ID: %d | Change: %.2f | Payment ref: %s\n", b->booking_id, paid - bus->fare, b->payment_reference);
}

static void view_bookings(void) {
    int found = 0;
    puts("\nACTIVE BOOKINGS");
    for (int i = 0; i < booking_count; i++) {
        if (!bookings[i].active) continue;
        struct Bus *bus = find_bus(bookings[i].bus_id);
        printf("ID %d | %s | %s | Seat %d | %s | %.2f | Ref %s\n", bookings[i].booking_id,
               bookings[i].passenger, bus->route, bookings[i].seat, bookings[i].phone,
               bookings[i].amount, bookings[i].payment_reference);
        found = 1;
    }
    if (!found) puts("No active bookings.");
}

static void cancel_booking(void) {
    int id = read_int("Enter booking ID (0 to return): ", 0, 999999999);
    if (id == 0) return;
    for (int i = 0; i < booking_count; i++) {
        if (bookings[i].active && bookings[i].booking_id == id) {
            bookings[i].active = 0;
            printf("Booking %d cancelled. A refund of %.2f should be processed by the payment provider.\n", id, bookings[i].amount);
            return;
        }
    }
    puts("Booking not found.");
}

int main(void) {
    int choice;
    puts("========================================");
    puts("       BUS BOOKING SYSTEM (C)           ");
    puts("========================================");
    do {
        puts("\n1. View buses and routes\n2. View seat map\n3. Book a seat and pay\n4. View my bookings\n5. Cancel a booking\n0. Exit");
        choice = read_int("Select an option: ", 0, 5);
        switch (choice) {
            case 1: list_buses(); break;
            case 2: show_seats(); break;
            case 3: make_booking(); break;
            case 4: view_bookings(); break;
            case 5: cancel_booking(); break;
            case 0: puts("Thank you for using the bus booking system."); break;
        }
    } while (choice != 0);
    return 0;
}
