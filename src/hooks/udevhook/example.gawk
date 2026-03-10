#!/usr/bin/env -S gawk --csv -f

{
    # map csv record to a map.
    delete kv
    for (i = 1; i <= NF; i += 2) {
        kv[$i] = $(i + 1)
    }

    # map properties to a map.
    delete props
    for (i = 0; i < kv["len_properties"]; i++) {
        prop_name_k = "property_name_" i
        prop_value_k = "property_value_" i

        name = kv[prop_name_k]
        value = kv[prop_value_k]

        props[name] = value
    }
}

kv["subsystem"] == "block" && kv["devtype"] == "disk" && kv["action"] == "add" && props["ID_BUS"] == "usb" {
    vendor = props["ID_VENDOR"]
    model = props["ID_USB_MODEL"]
    devnode = kv["devnode"]

    title = "USB Device Connected"
    description = sprintf("%s %s\n%s", vendor, model, devnode)
    icon = "/usr/share/icons/AdwaitaLegacy/48x48/devices/media-removable.png"

    cmd = sprintf("notify-send -i '%s' '%s' '%s'", icon, title, description)
    system(cmd)
}

kv["subsystem"] == "block" && kv["action"] == "remove" && props["ID_BUS"] == "usb" {
    vendor = props["ID_VENDOR"]
    model = props["ID_USB_MODEL"]
    devnode = kv["devnode"]

    title = "USB Device Disconnected"
    description = sprintf("%s %s\n%s", vendor, model, devnode)
    icon = "/usr/share/icons/AdwaitaLegacy/48x48/devices/media-removable.png"

    cmd = sprintf("notify-send -u low -i '%s' '%s' '%s'", icon, title, description)
    system(cmd)
}

kv["subsystem"] == "power_supply" && kv["action"] == "change" {
    name = props["POWER_SUPPLY_NAME"]
    online = props["POWER_SUPPLY_ONLINE"]
    status = props["POWER_SUPPLY_STATUS"]
    capacity = props["POWER_SUPPLY_CAPACITY"]

    if (name ~ /^(AC|ACAD|ADP[0-9]+|ACPI[0-9A-F]+(:[0-9A-F]+)?)$/) {
        description = sprintf("AC Adapter %s: %s", name, online == 1 ? "Plugged in" : "Unplugged")
        urgency = online == 1 ? "normal" : "low"
        icon = "/usr/share/icons/AdwaitaLegacy/48x48/legacy/ac-adapter.png"
        cmd = sprintf("notify-send -u '%s' -i '%s' '%s' '%s'", urgency, icon, "Power supply", description)

        system(cmd)
    } else if (name ~ /^(BAT[0-9]+|BATT[0-9]+)$/) {
        cmd = sprintf("notify-send '%s' '%s'", "Battery", capacity " " status)
        system(cmd)
    } else {
    }
}

function debug_kv() {
    for (k in kv) {
        printf("key='%s' value='%s'\n", k, kv[k])
    }
}

function debug_props() {
    for (k in props) {
        printf("name='%s' value='%s'\n", k, props[k])
    }
}

function debug_fields() {
    print "---"
    print $0

    print "---"
    print "Total Records:", NF
    print "---"

    for (i = 1; i <= NF; i++) {
        printf("Position %d: %s\n", i, $i)
    }

    print "---"
}
