#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dbus/dbus.h>

void main() {
    DBusError err;
    dbus_error_init(&err);

    DBusConnection* conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection error (%s)\n", err.message);
        dbus_error_free(&err);
    }

    if (conn == NULL) {
        exit(1);
    }

    dbus_bus_add_match(conn, "interface='org.PulseAudio.Core1'", &err);
    dbus_connection_flush(conn);

    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Match error (%s)\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }

    while (1) {
        dbus_connection_read_write(conn, 0);
        DBusMessage* msg = dbus_connection_pop_message(conn);

        if (msg == NULL) {
            sleep(1);
            continue;
        }

        printf("Serial: %d", dbus_message_get_serial(msg));
        printf("Type: %d", dbus_message_get_type(msg));

        dbus_message_unref(msg);
    }
}
