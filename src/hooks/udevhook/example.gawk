#!/usr/bin/env -S gawk --csv -f

{
    delete kv
    for (i = 1; i <= NF; i += 2) {
        kv[$i] = $(i + 1)
    }

    delete props
    for (i = 0; i < kv["len_properties"]; i++) {
        prop_name_k = "property_name_" i
        prop_value_k = "property_value_" i

        name = kv[prop_name_k]
        value = kv[prop_value_k]

        props[name] = value
    }
}

kv["subsystem"] == "block" && kv["devtype"] == "disk" && kv["action"] == "add" {
    # print "new drive connected"
    # print "debug_kv:"
    # debug_kv()
    # print "debug_props:"
    # debug_props()
    
    print $0

    print "---"
    print "Total Records:", NF
    print "---"

    for (i = 1; i <= NF; i++) {
        printf("Position %d: %s\n", i, $i)
    }
}

function has_usb_properties() {
    if (!("ID_BUS" in props && props["ID_BUS"] == "usb")) {
        return 0
    }

    if (!("ID_TYPE" in props && props["ID_TYPE"] == "disk")) {
        return 0
    }

    if (!("ID_BUS_DRIVER" in props && props["ID_BUS_DRIVER"] == "usb-storage")) {
        return 0
    }

    return 1
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
